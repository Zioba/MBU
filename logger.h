#ifndef LOGGER_H
#define LOGGER_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QSqlError>
#include <QSqlRecord>


class Logger
{
public:
    Logger( QSqlDatabase );

    //accept 1 - тип_сообщения, 2 - дата, 3 - отправитель/получатель, 4 - пакет, 5 - состояние обработки
    //return true - success; false - error
    bool makeNote( int, QString, int, QString, int );

private:
    QSqlDatabase db;
};

#endif // LOGGER_H
