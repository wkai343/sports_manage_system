#include "judgewindow.h"
#include "ui_judgewindow.h"
#include "ui_dialogrecord.h"
#include <QMessageBox>

JudgeWindow::JudgeWindow(const char* dbName, QWidget* parent)
    : QMainWindow(parent),
    ui(new Ui::JudgeWindow),
    dbName(dbName)
{
    ui->setupUi(this);
    ui->eventView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->eventView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->eventView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->eventView->setAlternatingRowColors(true);
    openDB();
    connect(selectionModel, SIGNAL(currentRowChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(do_currentRowChanged(const QModelIndex&, const QModelIndex&)));
}

JudgeWindow::~JudgeWindow() { delete ui; }

void JudgeWindow::openDB() {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbName);
    if (!db.open()) {
        QMessageBox::critical(this, "Error", "Database not found.");
        return;
    }

    qryModel = new QSqlQueryModel(this);
    qryModel->setQuery("SELECT eventID, event.name, athleteID, athlete.name, record, score from entry join event on entry.eventID = event.id join athlete on entry.athleteID = athlete.id");
    if (qryModel->lastError().isValid()) {
        QMessageBox::critical(this, "Error", qryModel->lastError().text());
        return;
    }
    qryModel->setHeaderData(0, Qt::Horizontal, "项目编号");
    qryModel->setHeaderData(1, Qt::Horizontal, "项目名称");
    qryModel->setHeaderData(2, Qt::Horizontal, "运动员学号");
    qryModel->setHeaderData(3, Qt::Horizontal, "运动员姓名");
    qryModel->setHeaderData(4, Qt::Horizontal, "记录");
    qryModel->setHeaderData(5, Qt::Horizontal, "积分");
    ui->eventView->setModel(qryModel);

    selectionModel = new QItemSelectionModel(qryModel, this);
    ui->eventView->setSelectionModel(selectionModel);

    mapper = new QDataWidgetMapper(this);
    mapper->setModel(qryModel);
    mapper->addMapping(ui->eventID, 0);
    mapper->addMapping(ui->eventName, 1);
    mapper->addMapping(ui->athID, 2);
    mapper->addMapping(ui->athName, 3);
    mapper->addMapping(ui->record, 4);
    mapper->addMapping(ui->score, 5);
    mapper->toFirst();

    ui->statusbar->showMessage(QString("记录条数：%1").arg(qryModel->rowCount()));
}

void JudgeWindow::on_actRefresh_triggered() {
    qryModel->setQuery(qryModel->query().lastQuery());
    if (qryModel->lastError().isValid()) {
        QMessageBox::critical(this, "Error", qryModel->lastError().text());
        return;
    }
    ui->statusbar->showMessage(QString("记录条数：%1").arg(qryModel->rowCount()));
}

void JudgeWindow::do_currentRowChanged(const QModelIndex& current, const QModelIndex& previous) {
    mapper->setCurrentModelIndex(current);
    bool first = (current.row() == 0), last = (current.row() == qryModel->rowCount() - 1);
    ui->actFirst->setEnabled(!first);
    ui->actPrevious->setEnabled(!first);
    ui->actNext->setEnabled(!last);
    ui->actLast->setEnabled(!last);
}

void JudgeWindow::move() {
    selectionModel->clearSelection();
    selectionModel->setCurrentIndex(qryModel->index(mapper->currentIndex(), 0), QItemSelectionModel::Select);
}

void JudgeWindow::on_actFirst_triggered() {
    mapper->toFirst();
    move();
}
void JudgeWindow::on_actPrevious_triggered() {
    mapper->toPrevious();
    move();
}
void JudgeWindow::on_actNext_triggered() {
    mapper->toNext();
    move();
}
void JudgeWindow::on_actLast_triggered() {
    mapper->toLast();
    move();
}

void JudgeWindow::on_eventView_doubleClicked(const QModelIndex& index) {
    DialogRecord dialog;
    dialog.setRecord(qryModel->record(index.row()));
    if (dialog.exec() == QDialog::Accepted) {
        QSqlRecord record = dialog.getRecord();
        QSqlQuery query(db);
        query.prepare("UPDATE entry SET record = :record, score = :score WHERE eventID = :eventID AND athleteID = :athleteID");
        query.bindValue(":record", record.value("record"));
        query.bindValue(":score", record.value("score"));
        query.bindValue(":eventID", record.value("eventID"));
        query.bindValue(":athleteID", record.value("athleteID"));
        if (!query.exec()) {
            QMessageBox::critical(this, "Error", query.lastError().text());
            return;
        }
        on_actRefresh_triggered();
    }
}

DialogRecord::DialogRecord(QWidget* parent)
    : QDialog(parent),
    ui(new Ui::DialogRecord)
{ ui->setupUi(this); }

DialogRecord::~DialogRecord() { delete ui; }

void DialogRecord::setRecord(const QSqlRecord& record) {
    this->record = record;
    ui->eventID->setText(record.value("eventID").toString());
    ui->eventName->setText(record.value("event.name").toString());
    ui->athID->setText(record.value("athleteID").toString());
    ui->athName->setText(record.value("athlete.name").toString());
    ui->record->setText(record.value("record").toString());
    ui->score->setText(record.value("score").toString());
}

QSqlRecord DialogRecord::getRecord() {
    record.setValue("record", ui->record->text());
    record.setValue("score", ui->score->text());
    return record;
}