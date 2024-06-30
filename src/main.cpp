#include <QApplication>
#include "adminwindow.h"
#include "athwindow.h"
#include "judgewindow.h"
#include "tdialogLogin.h"

int main(int argc, char *argv[]) {
    const char* dbName = "./sports.db"; // 可以创建db对象
    QApplication a(argc, argv);
    QString type;
    int id{};
    if (auto dlgLogin = new TDialogLogin(dbName, type, id); dlgLogin->exec() == QDialog::Accepted) {
        if (type == "admin") {
            AdminWindow w(dbName);
            w.show();
            return a.exec();
        }
        else if (type == "judge") {
            JudgeWindow w(dbName);
            w.show();
            return a.exec();
        } else if (type == "athlete") {
            AthWindow w(dbName, id);
            w.show();
            return a.exec();
        } else
            return 0;
    }
    else
        return 0;
}
