#include "athwindow.h"
#include "ui_athwindow.h"
#include <QMessageBox>

AthWindow::AthWindow(const char* dbName, int id, QWidget* parent)
    : QMainWindow(parent),
    ui(new Ui::AthWindow),
    dbName(dbName), id(id)
{
    ui->setupUi(this);
    ui->eventView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->eventView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->eventView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->eventView->setAlternatingRowColors(true);
    openDB();
}

AthWindow::~AthWindow() { delete ui; }

void AthWindow::openDB() {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbName);
    if (!db.open()) {
        QMessageBox::critical(this, "Error", "Database not found!");
        return;
    }

    qryModel = new QSqlQueryModel(this);
    qryModel->setQuery("SELECT eventID, name, record, score from entry join event on entry.eventID = event.id where entry.athleteID = " + QString::number(id));
    if (qryModel->lastError().isValid()) {
        QMessageBox::critical(this, "Error", qryModel->lastError().text());
        return;
    }
    qryModel->setHeaderData(0, Qt::Horizontal, "项目编号");
    qryModel->setHeaderData(1, Qt::Horizontal, "项目名称");
    qryModel->setHeaderData(2, Qt::Horizontal, "记录");
    qryModel->setHeaderData(3, Qt::Horizontal, "得分");
    ui->eventView->setModel(qryModel);

    selectionModel = new QItemSelectionModel(qryModel, this);
    ui->eventView->setSelectionModel(selectionModel);

    qry = QSqlQuery(db);
    qry.prepare("SELECT * from athlete where id = :id");
    qry.bindValue(":id", id);
    qry.exec();
    if (qry.next()) {
        ui->id->setText(qry.value(0).toString());
        ui->name->setText(qry.value(1).toString());
        ui->sex->setText(qry.value(2).toString());
        ui->school->setText(qry.value(3).toString());
    } else {
        QMessageBox::critical(this, "Error", "Athlete not found!");
        return;
    }

    ui->statusbar->showMessage(QString("记录条数：%1").arg(qryModel->rowCount()));
}