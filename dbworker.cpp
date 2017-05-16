#include "dbworker.h"

DbWorker::DbWorker( QString hostName, int port, QString baseName,
                    QString userName, QString password )
{
    db = QSqlDatabase::addDatabase( "QPSQL" );
    db.setHostName( hostName );
    db.setPort( port );
    db.setDatabaseName( baseName );
    db.setUserName( userName );
    db.setPassword( password );
    if ( !db.open() ) {
        connectionStatus = false;
    }
    else {
        connectionStatus = true;
    }
}

bool DbWorker::getConnectionStatus()
{
    return connectionStatus;
}

bool DbWorker::makeNote( int type, QString date, int x, QString package, int status )
{
    QSqlQuery query = QSqlQuery( db );
    QString s;
    s = "SELECT id_note FROM log.log_table_message ORDER BY id_note DESC LIMIT 1;";
    if ( !query.exec( s ) ) {
        return false;
    }
    else {
        int id;
        while ( query.next() ) {
            id = query.value( 0 ).toInt();
        }
        id++;
        db.transaction();
        query.prepare( "INSERT INTO log.log_table_message(id_note, type_message, date, reciver_sender, package, status)"
                                      "VALUES (?, ?, ?, ?, ?, ?)" );
        query.addBindValue( id );
        query.addBindValue( type );
        query.addBindValue( date );
        query.addBindValue( x );
        query.addBindValue( package );
        query.addBindValue( status );
        query.exec();
        return db.commit();
    }
    return false;
}
