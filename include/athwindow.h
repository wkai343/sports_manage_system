#pragma once
#include <QMainWindow>
#include <QtSql>

QT_BEGIN_NAMESPACE
namespace Ui { class AthWindow; }
QT_END_NAMESPACE

class AthWindow: public QMainWindow {
    Q_OBJECT
public:
    AthWindow(const char* dbName, int id, QWidget* parent = nullptr);
    ~AthWindow();
private:
    Ui::AthWindow* ui;
    const char* dbName;
    QSqlDatabase db;
    QSqlQueryModel* qryModel;
    QItemSelectionModel* selectionModel;
    QSqlQuery qry;
    int id;
    void openDB();
};