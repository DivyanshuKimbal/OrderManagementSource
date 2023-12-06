#include "widget.h"

#include <QApplication>

#include "Server/BusinessLogic/requesthandler.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    Widget w;
//    w.show();
    QString dbPath = "database.db";
    RequestHandler *reqHandler = new RequestHandler(dbPath);
    reqHandler->listenToServer();
    return a.exec();
}
