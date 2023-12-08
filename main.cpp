#include "widget.h"

#include <QApplication>

#include "controller/apicontroller.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    Widget w;
//    w.show();
    APIController apiController;
    apiController.startServer();
    return a.exec();
}
