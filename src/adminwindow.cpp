#include "adminwindow.h"
#include "ui_adminwindow.h"
#include <QMessageBox>

AdminWindow::AdminWindow(const char* dbName, QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::AdminWindow),
    dbName(dbName)
{
    ui->setupUi(this);
    curTable = static_cast<Table>(ui->tabWidget->currentIndex()); // 注意初始化
    view[Event] = ui->eventView, view[Ath] = ui->athView, view[School] = ui->schoolView, view[Judge] = ui->judgeView, view[User] = ui->userView, view[Entry] = ui->entryView;
    openDB(); // 打开数据库
    initDelegate(); // 初始化代理
    ui->athSex->addItems({ "男", "女" }); // 添加性别选项
    ui->type->addItems({ "athlete", "judge" }); // 添加用户类型选项
    initMapper(); // 初始化映射
    initSignalSlots(); // 初始化信号槽
    on_actRefresh_triggered(); // 刷新数据
    QSqlQuery("PRAGMA foreign_keys = ON", db); // 开启外键约束
}

AdminWindow::~AdminWindow() { delete ui; }

void AdminWindow::initMapper() {
    for (int i{}; i < 6; ++i) {
        dataMapper[i] = new QDataWidgetMapper(this);
        dataMapper[i]->setModel(model[i]);
        dataMapper[i]->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
        dataMapper[i]->toFirst();
    }
    dataMapper[Event]->addMapping(ui->eventID, model[Event]->fieldIndex("id"));
    dataMapper[Event]->addMapping(ui->eventName, model[Event]->fieldIndex("name"));
    dataMapper[Ath]->addMapping(ui->athID, model[Ath]->fieldIndex("id"));
    dataMapper[Ath]->addMapping(ui->athName, model[Ath]->fieldIndex("name"));
    dataMapper[Ath]->addMapping(ui->athSex, model[Ath]->fieldIndex("sex"));
    dataMapper[Ath]->addMapping(ui->athSchool, model[Ath]->fieldIndex("schoolName"));
    dataMapper[School]->addMapping(ui->schoolName, model[School]->fieldIndex("name"));
    dataMapper[School]->addMapping(ui->leaderID, model[School]->fieldIndex("leaderID"));
    dataMapper[Judge]->addMapping(ui->judgeID, model[Judge]->fieldIndex("id"));
    dataMapper[Judge]->addMapping(ui->judgeName, model[Judge]->fieldIndex("name"));
    dataMapper[User]->addMapping(ui->id, model[User]->fieldIndex("id"));
    dataMapper[User]->addMapping(ui->password, model[User]->fieldIndex("password"));
    dataMapper[User]->addMapping(ui->type, model[User]->fieldIndex("type"));
    dataMapper[Entry]->addMapping(ui->entryAthlete, model[Entry]->fieldIndex("athleteID"));
    dataMapper[Entry]->addMapping(ui->entryEvent, model[Entry]->fieldIndex("eventID"));
}

void AdminWindow::initSignalSlots() {
    for (int i{}; i < 6; ++i) {
        connect(selModel[i], &QItemSelectionModel::currentChanged, this, &AdminWindow::do_currentChanged);
        connect(selModel[i], &QItemSelectionModel::currentRowChanged, this, &AdminWindow::do_currentRowChanged);
    }
}

void AdminWindow::showRecordCount() {
    ui->statusbar->showMessage(QString("记录数：%1").arg(model[curTable]->rowCount()));
}

void AdminWindow::initModels() {
    for (int i{}; i < 6; ++i)
        model[i] = new QSqlTableModel(this, db);
    // Event
    model[Event]->setTable("event");
    if (!model[Event]->select()) {
        QMessageBox::critical(this, "错误", "打开数据表失败！");
        return;
    }
    model[Event]->setHeaderData(model[Event]->fieldIndex("id"), Qt::Horizontal, "ID");
    model[Event]->setHeaderData(model[Event]->fieldIndex("name"), Qt::Horizontal, "项目名称");
    // Athlete
    model[Ath]->setTable("athlete");
    if (!model[Ath]->select()) {
        QMessageBox::critical(this, "错误", "打开数据表失败！");
        return;
    }
    model[Ath]->setHeaderData(model[Ath]->fieldIndex("id"), Qt::Horizontal, "ID");
    model[Ath]->setHeaderData(model[Ath]->fieldIndex("name"), Qt::Horizontal, "姓名");
    model[Ath]->setHeaderData(model[Ath]->fieldIndex("sex"), Qt::Horizontal, "性别");
    model[Ath]->setHeaderData(model[Ath]->fieldIndex("schoolName"), Qt::Horizontal, "学院");
    // School
    model[School]->setTable("school");
    if (!model[School]->select()) {
        QMessageBox::critical(this, "错误", "打开数据表失败！");
        return;
    }
    model[School]->setHeaderData(model[School]->fieldIndex("name"), Qt::Horizontal, "学院名称");
    model[School]->setHeaderData(model[School]->fieldIndex("leaderID"), Qt::Horizontal, "领队ID");
    // Judge
    model[Judge]->setTable("judge");
    if (!model[Judge]->select()) {
        QMessageBox::critical(this, "错误", "打开数据表失败！");
        return;
    }
    model[Judge]->setHeaderData(model[Judge]->fieldIndex("id"), Qt::Horizontal, "ID");
    model[Judge]->setHeaderData(model[Judge]->fieldIndex("name"), Qt::Horizontal, "裁判员姓名");
    // User
    model[User]->setTable("user");
    if (!model[User]->select()) {
        QMessageBox::critical(this, "错误", "打开数据表失败！");
        return;
    }
    model[User]->setHeaderData(model[User]->fieldIndex("id"), Qt::Horizontal, "ID");
    model[User]->setHeaderData(model[User]->fieldIndex("password"), Qt::Horizontal, "密码");
    model[User]->setHeaderData(model[User]->fieldIndex("type"), Qt::Horizontal, "类型");
    // Entry
    model[Entry]->setTable("entry");
    if (!model[Entry]->select()) {
        QMessageBox::critical(this, "错误", "打开数据表失败！");
        return;
    }
    model[Entry]->setHeaderData(model[Entry]->fieldIndex("eventID"), Qt::Horizontal, "项目ID");
    model[Entry]->setHeaderData(model[Entry]->fieldIndex("athleteID"), Qt::Horizontal, "运动员ID");
    // 设置外键
    // model[Ath]->setJoinMode(QSqlRelationalTableModel::LeftJoin);
    // model[School]->setJoinMode(QSqlRelationalTableModel::LeftJoin);
    // model[Entry]->setJoinMode(QSqlRelationalTableModel::LeftJoin);
    // model[Ath]->setRelation(model[Ath]->fieldIndex("schoolName"), QSqlRelation("school", "name", "name"));
    // model[School]->setRelation(model[School]->fieldIndex("leaderID"), QSqlRelation("athlete", "id", "id"));
    // view[Ath]->setItemDelegate(new QSqlRelationalDelegate(view[Ath]));
    // view[School]->setItemDelegate(new QSqlRelationalDelegate(view[School]));
    // model[Entry]->setRelation(model[Entry]->fieldIndex("eventID"), QSqlRelation("event", "id", "id"));
    // model[Entry]->setRelation(model[Entry]->fieldIndex("athleteID"), QSqlRelation("athlete", "id", "id"));
    // view[Entry]->setItemDelegate(new QSqlRelationalDelegate(view[Entry]));
}

void AdminWindow::initView(QTableView* view, QSqlTableModel* model, QItemSelectionModel* selModel) {
    model->setEditStrategy(QSqlTableModel::OnManualSubmit); // 手动提交修改
    view->setModel(model); // 设置模型
    view->setSelectionModel(selModel); // 设置选择模型
    view->setSelectionMode(QAbstractItemView::SingleSelection); // 单选
    view->setSelectionBehavior(QAbstractItemView::SelectRows); // 选中整行
    view->setAlternatingRowColors(true); // 交替行颜色
}

void AdminWindow::initViews() {
    for (int i{}; i < 6; ++i) {
        selModel[i] = new QItemSelectionModel(model[i], this);
        initView(view[i], model[i], selModel[i]); // 初始化视图
    }
    // 隐藏列
    // view[School]->setColumnHidden(model[School]->fieldIndex("totalScore"), true);
    view[Entry]->setColumnHidden(model[Entry]->fieldIndex("record"), true);
    view[Entry]->setColumnHidden(model[Entry]->fieldIndex("score"), true);
}

void AdminWindow::openDB() {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbName);
    if (!db.open()) {
        QMessageBox::critical(this, "错误", "打开数据库文件失败！");
    } else {
        initModels();
        initViews();
    }
}

void AdminWindow::initDelegate() {
    delegateSex.setItems({ "男", "女" });
    view[Ath]->setItemDelegateForColumn(model[Ath]->fieldIndex("sex"), &delegateSex);
    delegateType.setItems({ "athlete", "judge" });
    view[User]->setItemDelegateForColumn(model[User]->fieldIndex("type"), &delegateType);
    view[Ath]->setItemDelegateForColumn(model[Ath]->fieldIndex("schoolName"), &delegateSchool);
    view[School]->setItemDelegateForColumn(model[School]->fieldIndex("leaderID"), &delegateAth);
    view[Entry]->setItemDelegateForColumn(model[Entry]->fieldIndex("athleteID"), &delegateAth);
    view[Entry]->setItemDelegateForColumn(model[Entry]->fieldIndex("eventID"), &delegateEvent);
}

void AdminWindow::updateForeignKeyDelegate() {
    delegateSchool.setItems(getSchoolName());
    delegateAth.setItems(getAthID());
    delegateEvent.setItems(getEventID());
}

void AdminWindow::on_tabWidget_currentChanged(int index) {
    curTable = static_cast<Table>(index);
    ui->actInsert->setEnabled(false);
    ui->actRemove->setEnabled(false);
    if (model[curTable]->isDirty())
        return;
    on_actRefresh_triggered();
    showRecordCount();
}

void AdminWindow::on_actAppend_triggered() {
    model[curTable]->insertRecord(model[curTable]->rowCount(), model[curTable]->record());
    selModel[curTable]->setCurrentIndex(model[curTable]->index(model[curTable]->rowCount() - Ath, Event), QItemSelectionModel::Select);
    showRecordCount();
}

void AdminWindow::on_actInsert_triggered() {
    model[curTable]->insertRecord(view[curTable]->currentIndex().row(), model[curTable]->record());
    selModel[curTable]->setCurrentIndex(view[curTable]->currentIndex(), QItemSelectionModel::Select);
    showRecordCount();
}

void AdminWindow::on_actRemove_triggered() {
    model[curTable]->removeRow(view[curTable]->currentIndex().row());
    showRecordCount();
}

void AdminWindow::on_actSave_triggered() {
    if(!model[curTable]->submitAll())
        QMessageBox::critical(this, "错误", "保存数据失败！");
    else {
        ui->actSave->setEnabled(false);
        ui->actRevert->setEnabled(false);
    }
    showRecordCount();
}

void AdminWindow::on_actRevert_triggered() {
    model[curTable]->revertAll();
    ui->actSave->setEnabled(false);
    ui->actRevert->setEnabled(false);
    showRecordCount();
}

void AdminWindow::on_actRefresh_triggered() {
    model[curTable]->select();
    ui->actSave->setEnabled(false);
    ui->actRevert->setEnabled(false);
    showRecordCount();
    auto schoolName = getSchoolName(), athID = getAthID(), eventID = getEventID();
    if (curTable == Event) {
        ui->eventID->clear();
        ui->eventName->clear();
    }else if (curTable == Ath) {
        ui->athID->clear();
        ui->athName->clear();
        ui->athSex->setCurrentIndex(-1);
        ui->athSchool->clear();
        ui->athSchool->addItems(schoolName);
        ui->athSchool->setCurrentIndex(-1);
    } else if (curTable == School) {
        ui->schoolName->clear();
        ui->leaderID->clear();
        ui->leaderID->addItems(athID);
        ui->leaderID->setCurrentIndex(-1);
    } else if (curTable == Judge) {
        ui->judgeID->clear();
        ui->judgeName->clear();
    } else if (curTable == User) {
        ui->id->clear();
        ui->password->clear();
        ui->type->setCurrentIndex(-1);
    } else if (curTable == Entry) {
        ui->entryAthlete->clear();
        ui->entryAthlete->addItems(athID);
        ui->entryAthlete->setCurrentIndex(-1);
        ui->entryEvent->clear();
        ui->entryEvent->addItems(eventID);
        ui->entryEvent->setCurrentIndex(-1);
    }
    updateForeignKeyDelegate();
}

void AdminWindow::do_currentChanged(const QModelIndex& current, const QModelIndex& previous) {
    ui->actSave->setEnabled(model[curTable]->isDirty());
    ui->actRevert->setEnabled(model[curTable]->isDirty());
}

void AdminWindow::do_currentRowChanged(const QModelIndex& current, const QModelIndex& previous) {
    ui->actInsert->setEnabled(current.isValid());
    ui->actRemove->setEnabled(current.isValid());
    dataMapper[curTable]->setCurrentIndex(current.row());
}

QStringList AdminWindow::getEventID() {
    QSqlQuery query(db);
    query.exec("SELECT id FROM event");
    QStringList eventID;
    while (query.next())
        eventID << query.value(0).toString();
    return eventID;
}

QStringList AdminWindow::getAthID() {
    QSqlQuery query(db);
    query.exec("SELECT id FROM athlete");
    QStringList athID;
    while (query.next())
        athID << query.value(0).toString();
    return athID;
}

QStringList AdminWindow::getSchoolName() {
    QSqlQuery query(db);
    query.exec("SELECT name FROM school");
    QStringList schoolName;
    while (query.next())
        schoolName << query.value(0).toString();
    return schoolName;
}

