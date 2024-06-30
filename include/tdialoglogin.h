#ifndef TDIALOGLOGIN_H
#define TDIALOGLOGIN_H

#include <QDialog>
#include <QtSql>

QT_BEGIN_NAMESPACE
namespace Ui {class TDialogLogin;}
QT_END_NAMESPACE


class TDialogLogin : public QDialog {
    Q_OBJECT

private:
    bool    m_moving=false;     //表示窗口是否在鼠标操作下移动
    QPoint  m_lastPos;          //上一次的鼠标位置

    const QString adminID="admin", adminPWD="admin"; // 管理员ID和密码

    const char* dbName; //数据库名
    QString& type; //用户类型
    int& id; //用户ID
    QString pwd; //用户密码
protected:
    //鼠标事件处理函数，用于拖动窗口
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    // 获取密码
    bool getPWD(int id);
public:
    TDialogLogin(const char* dbName, QString& type, int& id, QWidget *parent = nullptr);
    ~TDialogLogin();
    // 获取用户类型
    QString getType() const;
    // 获取用户ID
    int getID() const;
private slots:
    void on_btnOK_clicked();    //"确定"按钮
    void on_btnSelect_clicked();    //"查询"按钮
private:
    Ui::TDialogLogin *ui;
};

QT_BEGIN_NAMESPACE
namespace Ui {class Dialog;}
QT_END_NAMESPACE

class Dialog: public QDialog {
    Q_OBJECT
public:
    Dialog(const char* dbName, QWidget* parent = nullptr);
    ~Dialog();
private:
    Ui::Dialog* ui;
    const char* dbName;
    QSqlDatabase db;
    QSqlQueryModel* qryModel;
    QItemSelectionModel* selectionModel;
};

#endif // TDIALOGLOGIN_H
