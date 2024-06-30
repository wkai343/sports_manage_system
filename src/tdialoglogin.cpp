#include "tdialoglogin.h"
#include "ui_tdialoglogin.h"
#include "ui_dialog.h"

#include <QMouseEvent>
#include <QMessageBox>
#include <QtSql>

void TDialogLogin::mousePressEvent(QMouseEvent *event)
{ //鼠标按键被按下
    if (event->button() == Qt::LeftButton)
    {
        m_moving = true;
        m_lastPos = event->globalPosition().toPoint() - this->pos();
    }
    return QDialog::mousePressEvent(event);
}

void TDialogLogin::mouseMoveEvent(QMouseEvent *event)
{ //鼠标按下左键移动
    QPoint eventPos=event->globalPosition().toPoint();

    if (m_moving && (event->buttons() & Qt::LeftButton)
        && (eventPos-m_lastPos).manhattanLength() > QApplication::startDragDistance())
    {
        move(eventPos-m_lastPos);
        m_lastPos = eventPos - this->pos();
    }
    return QDialog::mouseMoveEvent(event);
}

void TDialogLogin::mouseReleaseEvent(QMouseEvent *event)
{ //鼠标按键被释放
    m_moving = false;     //停止移动
    event->accept();
}

TDialogLogin::TDialogLogin(const char* dbName, QString& type, int& id, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TDialogLogin),
    dbName(dbName),
    type(type), id(id)
{
    ui->setupUi(this);

    ui->editPSWD->setEchoMode(QLineEdit::Password); //设置为密码输入模式
    this->setAttribute(Qt::WA_DeleteOnClose); //对话框关闭时自动删除
    this->setWindowFlags(Qt::SplashScreen); //设置为SplashScreen, 窗口无边框,不在任务栏显示
    // this->setWindowFlags(Qt::FramelessWindowHint); //无边框，但是在任务显示对话框标题
}

TDialogLogin::~TDialogLogin() {
    delete ui;
}

//"确定"按钮响应
void TDialogLogin::on_btnOK_clicked()
{
    QString user=ui->editUser->text().trimmed();    //输入的用户名
    QString pswd=ui->editPSWD->text().trimmed();    //输入的密码

    if ((user==adminID)&&(pswd==adminPWD)) {
        id = 0;
        type = "admin";
        pwd = adminPWD;
        this->accept();
    } else {
        bool ok;
        id = user.toInt(&ok);
        if (!ok || !getPWD(id) || pwd != pswd.trimmed())
            QMessageBox::warning(this, "错误提示", "用户名或密码错误");
        else
            this->accept();
    }
}

bool TDialogLogin::getPWD(int id) {
    auto db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbName);
    if (!db.open())
        QMessageBox::critical(this, "错误", "打开数据库文件失败！");
    QSqlQuery query(db);
    query.prepare("select * from user where id = :id");
    query.bindValue(":id", id);
    if (!query.exec()) {
        QMessageBox::critical(this, "错误", "查询失败！");
        return false;
    }
    if (query.next()) {
        pwd = query.value(1).toString();
        type = query.value(2).toString();
        return true;
    }
    return false;
}

QString TDialogLogin::getType() const {
    return type;
}

int TDialogLogin::getID() const {
    return id;
}

void TDialogLogin::on_btnSelect_clicked() {
    Dialog dlg(dbName, this);
    dlg.exec();
}

Dialog::Dialog(const char* dbName, QWidget* parent)
    : QDialog(parent),
    ui(new Ui::Dialog), dbName(dbName)
{
    ui->setupUi(this);
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbName);
    if (!db.open()) {
        QMessageBox::critical(this, "Error", "Database not found!");
        return;
    }

    qryModel = new QSqlQueryModel(this);
    qryModel->setQuery("SELECT schoolName, SUM(score) FROM Entry JOIN Athlete ON Entry.athleteId = Athlete.id GROUP BY Athlete.schoolName ORDER BY SUM(score) DESC");
    if (qryModel->lastError().isValid()) {
        QMessageBox::critical(this, "Error", qryModel->lastError().text());
        return;
    }
    qryModel->setHeaderData(0, Qt::Horizontal, "学校");
    qryModel->setHeaderData(1, Qt::Horizontal, "总分");
    ui->tableView->setModel(qryModel);

    selectionModel = new QItemSelectionModel(qryModel, this);
    ui->tableView->setSelectionModel(selectionModel);

    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->setAlternatingRowColors(true);
}

Dialog::~Dialog() { delete ui; }
