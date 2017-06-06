#include <QApplication>
#include "mainwindow.h"
#include "dbworker.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    DbWorker DBconnection( "192.168.1.42", 5432, "A200", "postgres", "qwerty" );
    MainWindow win(DBconnection);
    win.show();
    return app.exec();
}
