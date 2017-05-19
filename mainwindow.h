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
#include <QMessageBox>
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

    void on_showCommandTableAction_triggered();
    void on_sendCommand_triggered();
private:
    const int X_POSITION_FRAME=900;
    const int Y_POSITION_FRAME=0;
    const int LISTERNING_PORT=5824;
    int unicumMessageId = 1;
    const QString myPort = "5824";
    const QString targetPort = "5825";
    Ui::MainWindow *ui;
    QUdpSocket udpSocket;
    DbWorker dbConnect;
    QHostAddress myIp;
    QHostAddress targetIp;
    Converter *converter;

    QString getCurrentDateAndTime();
    QString assistParser( QString, int & );
    QString makeDatagramCommand(QString q);
};

#endif // MAINWINDOW_H
