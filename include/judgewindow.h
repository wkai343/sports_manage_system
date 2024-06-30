#pragma once
#include <QMainWindow>
#include <QDialog>
#include <QtSql>
#include <QDataWidgetMapper>
#include <qwidget.h>

QT_BEGIN_NAMESPACE
namespace Ui { class JudgeWindow; }
QT_END_NAMESPACE

class JudgeWindow: public QMainWindow {
    Q_OBJECT
public:
    JudgeWindow(const char* dbName, QWidget* parent = nullptr);
    ~JudgeWindow();
private:
    Ui::JudgeWindow* ui;
    const char* dbName;
    QSqlDatabase db;
    QSqlQueryModel* qryModel;
    QItemSelectionModel* selectionModel;
    QDataWidgetMapper* mapper;
    void openDB();
    void move();
private slots:
    void on_actRefresh_triggered();
    void do_currentRowChanged(const QModelIndex& current, const QModelIndex& previous);
    void on_actFirst_triggered();
    void on_actPrevious_triggered();
    void on_actNext_triggered();
    void on_actLast_triggered();
    void on_eventView_doubleClicked(const QModelIndex& index);
};

QT_BEGIN_NAMESPACE
namespace Ui { class DialogRecord; }
QT_END_NAMESPACE

class DialogRecord : public QDialog {
    Q_OBJECT
public:
    DialogRecord(QWidget* parent = nullptr);
    ~DialogRecord();
    void setRecord(const QSqlRecord& record);
    QSqlRecord getRecord();
private:
    Ui::DialogRecord* ui;
    QSqlRecord record;
};