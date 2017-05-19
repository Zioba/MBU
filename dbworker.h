#ifndef DBWORKER_H
#define DBWORKER_H
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QSqlRecord>
#include <QTableView>

#include <QString>


class DbWorker
{
public:
    DbWorker( QString hostName, int port, QString baseName,
              QString userName, QString password );

    bool getConnectionStatus();
    bool makeNote( int type, QString date, int x, QString package, int status);
    QSqlTableModel *getTable (QTableView *table, QString tableName, QString shortName);
    bool writeCoordinats (QString x, QString y, QString z, QString direction, QString time, QString object);
    bool writeRocket(QString x, QString time, QString object);
    QSqlDatabase getDb() const;

private:
    bool connectionStatus;
    QSqlDatabase db;
};

#endif // DBWORKER_H
