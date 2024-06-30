#pragma once
#include <QMainWindow>
#include <QDataWidgetMapper>
#include <QTableView>
#include <QtSql>
#include "tcomboboxdelegate.h"

QT_BEGIN_NAMESPACE
namespace Ui { class AdminWindow; }
QT_END_NAMESPACE

class AdminWindow: public QMainWindow
{
    Q_OBJECT
public:
    AdminWindow(const char* dbName, QWidget* parent = nullptr);
    ~AdminWindow();
private:
    Ui::AdminWindow* ui;
    const char* dbName;
    enum Table{ Event, Ath, School, Judge, User, Entry } curTable;
    QSqlDatabase db;
    QSqlTableModel* model[6];
    QItemSelectionModel* selModel[6];
    QTableView* view[6];
    QDataWidgetMapper* dataMapper[6];
    TComboBoxDelegate delegateSex, delegateType,
        delegateSchool, delegateEvent, delegateAth; // 外键代理
    void openDB();
    void initModels();
    void initView(QTableView* view, QSqlTableModel* model, QItemSelectionModel* selModel);
    void initViews();
    void initDelegate();
    void initMapper();
    void initSignalSlots();
    void showRecordCount();
    QStringList getEventID();
    QStringList getAthID();
    QStringList getSchoolName();
    void updateForeignKeyDelegate();
private slots:
    void on_tabWidget_currentChanged(int index);
    void on_actAppend_triggered();
    void on_actInsert_triggered();
    void on_actRemove_triggered();
    void on_actSave_triggered();
    void on_actRevert_triggered();
    void on_actRefresh_triggered();
    void do_currentChanged(const QModelIndex& current, const QModelIndex& previous);
    void do_currentRowChanged(const QModelIndex& current, const QModelIndex& previous);
};
