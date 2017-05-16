#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "deldialog.h"
#include "ipdialog.h"

MainWindow::MainWindow(DbWorker dbConnect, QWidget *parent) :
    dbConnect(dbConnect),
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    QPoint pos( X_POSITION_FRAME, Y_POSITION_FRAME );
    this->move( pos );
    ui->setupUi( this );
    setWindowTitle( "MBU" );

    db = QSqlDatabase::addDatabase( "QPSQL" );
    db.setHostName( "127.0.0.1" );
    db.setPort( DB_PORT );
    db.setDatabaseName( "Database_MBU" );
    db.setUserName( "postgres" );
    db.setPassword( "qwerty" );
    if ( !db.open() ) {
        makeLogNote( db.lastError().text() );
    }
    else {
        makeLogNote( "Start working" );
    }

    model = new QSqlTableModel( this, db );
    udpSocket.bind( LISTERNING_PORT );
    on_combObjTableBut_clicked();
    logger = new Logger( db );
    converter = new Converter();
    setIp();
    connect( &udpSocket, SIGNAL( readyRead() ), this, SLOT( readDatagram() ));
}

MainWindow::~MainWindow()
{
    delete ui;
    delete logger;
    delete converter;
}

void MainWindow::makeLogNote(QString s)
{
    ui->logField->append( tr( "%1 %2 %3" ).arg( QDate::currentDate().toString( "dd.MM.yyyy" ) )
                                          .arg( QTime::currentTime().toString( "hh:mm:ss" ) )
                                          .arg( s ) );
}

void MainWindow::readDatagram()
{
    QByteArray datagram;
    datagram.resize( udpSocket.pendingDatagramSize() );
    udpSocket.readDatagram( datagram.data(), datagram.size() );
    QStringList messageMembersList = converter->decode( datagram );
    parsingMessage( messageMembersList.at( 12 ) );
    if ( logger->makeNote( 1, getCurrentDateAndTime(), 1, messageMembersList.at( 12 ), 3 ) ) {
        makeLogNote( "получена датаграмма" );
    }
        else {
        makeLogNote( "ошибка занесения в лог" );
    }
    QStringList dataMembersList;
    dataMembersList << myIp.toString()
          << targetIp.toString()
          << "17"
          << QString::number(224)
          << myPort
          << targetPort
          << QString::number( messageMembersList.at( 12 ).length() )
          << ""  //доделать чексумму
          << "0010"
          << QString::number( unicumMessageId )
          << "1"
          << "1";
    unicumMessageId++;
    QByteArray datagram2 = converter->generateReceiptResponse( dataMembersList );
    targetIp.setAddress("127.0.0.1");
    udpSocket.writeDatagram( datagram2, targetIp, targetPort.toLong( Q_NULLPTR, 10 ) );
    if ( logger->makeNote( 1, getCurrentDateAndTime(), 1, datagram2, 2 ) ) {
        makeLogNote( "отправлено подтверждение получения" );
    }
        else {
        makeLogNote( "ошибка занесения в лог" );
    }
}

void MainWindow::on_exitButton_clicked()
{
   this->close();
}

void MainWindow::on_updBut_clicked()
{
    model->select();
    ui->logField->append( tr( "%1 таблица обновлена" ).arg( QTime::currentTime().toString( "hh:mm:ss" ) ) );
}

void MainWindow::on_clearBut_clicked()
{
    ui->logField->clear();
}

void MainWindow::on_combObjTableBut_clicked()
{
    model->setTable( "own_forces.combatobject_location" );
    ui->tableView->setModel( model );
    model->select();
    for ( int i = 0; i < model->columnCount(); i++ ) {
        ui->tableView->horizontalHeader()->setSectionResizeMode( i , QHeaderView::ResizeToContents);
    }
    makeLogNote( "Загружены данные combat objects" );
    setRussianColomnIDs("combatobject_location");
}

void MainWindow::on_logTableBut_3_clicked()
{
    model->setTable( "log.log_table_message" );
    ui->tableView->setModel( model );
    model->select();
    makeLogNote( "Загружены данные log table" );
    for ( int i = 0; i < model->columnCount(); i++ ) {
        ui->tableView->horizontalHeader()->setSectionResizeMode( i , QHeaderView::ResizeToContents);
    }
    setRussianColomnIDs("log_table_message");
}

void MainWindow::setIp() {
    IpDialog dia;
    if ( dia.exec() ) {
        myIp = dia.value();
    }
}

void MainWindow::parsingMessage( QString s )
{
    /*if ( s.at( 0 ) == '1' ) {
        makeLogNote( "oшибка, данные сжаты" );
        //не работаем пока со сжатием данных
        return;
    }*/
    bool trigger = false;
    QString object = "";
    for ( int i = 1; i < 7; i++ )
    {
        if ( s.at( i ) != '0' ) {
            trigger = true;
            object += s.at( i );
        }
        else if ( trigger ) {
            object += s.at( i );
        }
    }
    QString messageCode;
    for ( int i = 15; i < 17; i++ )
    {
        messageCode+=s.at( i );
    }
    if (QString::compare( messageCode, "C1") == 0) {
        parsingCoord(s, object);
    }
    if (QString::compare( messageCode, "T1") == 0) {
        parsingRocket(s, object);
    }
}

void MainWindow::parsingCoord( QString s, QString object)
{
    QString data = "";
    for ( int i = 18; i < s.size(); i++ )
    {
        data += s.at( i );
    }
    QString x;
    QString y;
    QString z;
    QString dir;
    int i = 0;
    x = assistParser( data, i );
    y = assistParser( data, i );
    z = assistParser( data, i );
    dir = assistParser( data, i );
    QSqlQuery query = QSqlQuery( db );
    QString queryString;
    queryString = "UPDATE own_forces.combatobject_location SET obj_location=ST_MakePoint ("+x+","+y+","+z+"), direction='"+dir+
            "', date_edit='"+ getCurrentDateAndTime() + "' WHERE combat_hierarchy='"+object+"';";
    if ( query.exec( queryString ) ) {
        makeLogNote( "база обновлена" );
    }
    else {
        makeLogNote( "ошибка запроса" );
    }
}

void MainWindow::parsingRocket(QString s, QString object)
{
    QString data = "";
    for ( int i = 18; i < s.size(); i++ )
    {
        data += s.at( i );
    }
    QString x;
    int i = 0;
    while ( i < data.size() ) {
        if (data.at(i) != ';') {
            x.append( data.at(i) );
        }
        else {
            QSqlQuery query = QSqlQuery( db );
            QString queryString;
            queryString = "UPDATE own_forces.rocket SET type_tid='"+ x +
                    "', date_edit='"+ getCurrentDateAndTime() + "' WHERE combatobjectid='"+object+"';";
            x="";
            if ( !query.exec( queryString ) ) {
                makeLogNote( "ошибка запроса" );
                return;
            }
            else {
            }
        }
        i++;
    }
    makeLogNote( "база обновлена" );
}

QString MainWindow::getCurrentDateAndTime()
{
    return QDate::currentDate().toString( "dd.MM.yyyy" ) + " " + QTime::currentTime().toString( "hh:mm:ss.zzz" );
}

QString MainWindow::assistParser( QString data, int &counter )
{
    QString answer = "";
    while ( data.at( counter ) != ';' ) {
        answer.append( data.at( counter ) );
        counter++;
    }
    counter++;
    return answer;
}

void MainWindow::setRussianColomnIDs(QString tableName) {
    QSqlQuery query = QSqlQuery( db );
    query.prepare( "SELECT COLUMN_NAME FROM INFORMATION_SCHEMA.COLUMNS WHERE TABLE_NAME = '"+tableName+"';" );
    query.exec();
    if ( query.next() ){
        for ( int i = 0; i < query.size(); i++) {
            QSqlRecord rec;
            rec = query.record();
            QSqlQuery query2 = QSqlQuery( db );
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
}
