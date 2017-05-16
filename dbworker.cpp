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

QSqlTableModel *DbWorker::getTable(QTableView *table, QString tableName) {
    QSqlTableModel *model = new QSqlTableModel(table, db);
    model->setTable(tableName);
    model->select();
    QSqlQuery query = QSqlQuery(db);
    query.prepare( "SELECT COLUMN_NAME FROM INFORMATION_SCHEMA.COLUMNS WHERE TABLE_NAME = '"+tableName+"';" );
    query.exec();
    if ( query.next() ){
        for ( int i = 0; i < query.size(); i++) {
            QSqlRecord rec;
            rec = query.record();
            QSqlQuery query2 = QSqlQuery(db);
            QString s = "SELECT description FROM pg_description INNER JOIN ";
            query2.prepare( s +
                            "(SELECT oid FROM pg_class WHERE relname ='" +
                            tableName +
                            "') as table_oid " +
                            "ON pg_description.objoid = table_oid.oid " +
                            "AND pg_description.objsubid IN " +
                            "(SELECT attnum FROM pg_attribute WHERE attname = '" +
                            query.value(0).toString() +
                            "' AND pg_attribute.attrelid = table_oid.oid );" );
            query2.exec();
            if (query2.next()) {
                model->setHeaderData(i, Qt::Horizontal, query2.value(0).toString());
            }
            query.next();
        }
    }
    return model;
}

QSqlDatabase DbWorker::getDb() const
{
    return db;
}
