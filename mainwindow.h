#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>
#include <QTimer>
#include <QDateEdit>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QSqlError>
#include <QSqlRecord>
#include "logger.h"
#include "converter.h"
#include "dbworker.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(DbWorker dbConnect, QWidget *parent = 0);
    ~MainWindow();

private slots:
    void makeLogNote( QString );
    void readDatagram();
    void setIp();
    void on_exitButton_clicked();
    void on_updBut_clicked();
    void on_clearBut_clicked();
    void on_combObjTableBut_clicked();
    void on_logTableBut_3_clicked();
    void parsingMessage( QString );
    void parsingCoord ( QString, QString );
    void parsingRocket( QString, QString );

    QString getCurrentDateAndTime();
    QString assistParser( QString, int & );
    void setRussianColomnIDs( QString );

private:
    const int X_POSITION_FRAME=900;
    const int Y_POSITION_FRAME=0;
    const int LISTERNING_PORT=5824;
    const int DB_PORT=5432;
    int unicumMessageId = 1;

    const QString myPort = "5824";
    const QString targetPort = "5825";

    Ui::MainWindow *ui;

    QUdpSocket     udpSocket;
    QSqlDatabase   db;
    DbWorker dbConnect;
    QSqlTableModel *model;

    QHostAddress myIp;
    QHostAddress targetIp;

    Logger    *logger;
    Converter *converter;
};

#endif // MAINWINDOW_H
