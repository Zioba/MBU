#include <QApplication>
#include "mainwindow.h"
#include "dbworker.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    DbWorker DBconnection( "127.0.0.1", 5432, "A200", "postgres", "qwerty" );
    MainWindow win(DBconnection);
    win.show();
    return app.exec();
}
