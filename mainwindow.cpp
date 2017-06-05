#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "deldialog.h"
#include "ipdialog.h"

MainWindow::MainWindow(DbWorker dbConnect, QWidget *parent) :
    dbConnect(dbConnect),
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    QPoint pos(X_POSITION_FRAME, Y_POSITION_FRAME);
    this->move(pos);
    ui->setupUi(this);
    setWindowTitle("Система управления");
    makeLogNote("Начало работы");

    udpSocket.bind(LISTERNING_PORT);
    on_combObjTableBut_clicked();
    converter = new Converter();
    setIp();
    targetIp.setAddress("192.168.1.42");
    connect(&udpSocket, SIGNAL(readyRead()), this, SLOT(readDatagram()));
    ui->dateTimeEdit->setDateTime(QDateTime::currentDateTime());
    for (int i=0; i < ui->commandTable->columnCount(); i++)
    {
        ui->commandTable->resizeColumnToContents(i);
    }
    for (int i=0; i < ui->modeTable->columnCount(); i++)
    {
        ui->modeTable->resizeColumnToContents(i);
    }
    for (int i=0; i < ui->coordinatTable->columnCount(); i++)
    {
        ui->coordinatTable->resizeColumnToContents(i);
    }
    for (int i=0; i < ui->routeTable->columnCount(); i++)
    {
        ui->routeTable->resizeColumnToContents(i);
    }
    for (int i=0; i < ui->documentTable->columnCount(); i++)
    {
        ui->documentTable->resizeColumnToContents(i);
    }
    for (int i=0; i < ui->objectTable->columnCount(); i++)
    {
        ui->objectTable->resizeColumnToContents(i);
    }
    fillTables();
}

void MainWindow::fillTables() {
    QSqlQuery query = QSqlQuery(dbConnect.getDb());
    //подумать что делать с пунктом время исполнения
    QString selectPattern = "SELECT t1.termname, inf.date_add, inf.order_id "
                "FROM orders_alerts.orders_alerts_info  inf "
                "JOIN reference_data.terms t1 ON inf.order_tid = t1.termhierarchy;";
    if (!query.exec(selectPattern)) {
        qDebug() << "Unable to make select operation!" << query.lastError();
    }
    ui->commandTable->setRowCount(query.size());
    int i= 0;
    while (query.next()) {
        ui->commandTable->setItem(i, 0, new QTableWidgetItem(query.value(0).toString()));
        ui->commandTable->setItem(i, 1, new QTableWidgetItem(query.value(1).toString()));
        ui->commandTable->setItem(i, 2, new QTableWidgetItem(query.value(2).toString()));
        i++;
    }
    /*root->setExpanded(true);
    out = new QTreeWidgetItem(documents);
    out->setText(0, "Исходящие");
    in = new QTreeWidgetItem(documents);
    in->setText(0, "Входящие");
    root = out;
    selectPattern = "SELECT cinf.outgoing_reg_number, cinf.outgoing_reg_datetime, t1.termname, t2.termname, cinf.cmbdid "
            "FROM combatdocs.combatdocs_info cinf "
              "JOIN combatdocs.combatdocs_type ctyp ON cinf.cmbdid = ctyp.cmbdid "
              "JOIN combatdocs.combatdocs_theme cthm ON cinf.cmbdid = cthm.cmbdid "
              "JOIN reference_data.terms t1 ON ctyp.doctype_tid = t1.termhierarchy "
              "JOIN reference_data.terms t2 ON cthm.doctheme_tid = t2.termhierarchy;";
    if (!query.exec(selectPattern)) {
        qDebug() << "Unable to make select operation!" << query.lastError();
    }
    while (query.next()) {
        addDocument(root, query.value(0).toString(), query.value(1).toDateTime(),
                    query.value(2).toString(), query.value(3).toString(), query.value(4).toString());
    }
    root->setExpanded(true);*/
}

MainWindow::~MainWindow()
{
    delete ui;
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
    QStringList messageMembersList = converter->decodeDatagram( datagram );
    parsingMessage( messageMembersList.at( 12 ) );
    if ( dbConnect.makeNote( 1, getCurrentDateAndTime(), 1, messageMembersList.at( 12 ), 3 ) ) {
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
    if ( dbConnect.makeNote( 1, getCurrentDateAndTime(), 1, datagram2, 2 ) ) {
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
    ui->tableView->update();
    ui->logField->append( tr( "%1 таблица обновлена" ).arg( QTime::currentTime().toString( "hh:mm:ss" ) ) );
}

void MainWindow::on_clearBut_clicked()
{
    ui->logField->clear();
}

void MainWindow::on_combObjTableBut_clicked()
{
    QSqlTableModel *model = dbConnect.getTable(ui->tableView, "own_forces.combatobject_location", "combatobject_location");
    ui->tableView->setModel( model );
    for ( int i = 0; i < model->columnCount(); i++ ) {
        ui->tableView->horizontalHeader()->setSectionResizeMode( i , QHeaderView::ResizeToContents);
    }
    makeLogNote( "Загружены данные combat objects" );
}

void MainWindow::on_logTableBut_clicked()
{
    QSqlTableModel *model = dbConnect.getTable(ui->tableView, "log.log_table_message", "log_table_message");
    ui->tableView->setModel( model );
    for ( int i = 0; i < model->columnCount(); i++ ) {
        ui->tableView->horizontalHeader()->setSectionResizeMode( i , QHeaderView::ResizeToContents);
    }
    makeLogNote( "Загружены данные log table" );
}

void MainWindow::on_showCommandTable_triggered()
{
    QSqlTableModel *model = dbConnect.getTable(ui->tableView, "orders_alerts.orders_alerts_info", "orders_alerts_info");
    ui->tableView->setModel( model );
    for ( int i = 0; i < model->columnCount(); i++ ) {
        ui->tableView->horizontalHeader()->setSectionResizeMode( i , QHeaderView::ResizeToContents);
    }
    makeLogNote( "Загружены данные orders_alerts_info" );
}

void MainWindow::on_sendCommand_triggered()
{
    QString trash;
    DelDialog dia;
    if ( dia.exec() ) {
        trash = dia.value();
    }
    QString data = makeDatagramCommand( "1" );
    if ( data == "error" ) {
        makeLogNote( "ошибка создания датаграммы" );
        QMessageBox::information(this, "ОШИБКА", "такой записи не существует!");
        return;
    }
    QStringList list;
    list << myIp.toString()
         << targetIp.toString()
         << "17"
         << QString::number( data.length() + 224 )
         << myPort
         << targetPort
         << QString::number( data.length() )
         << ""
         << "0001"
         << QString::number( unicumMessageId )
         << "1"
         << "1"
         << data;
    unicumMessageId++;
    QByteArray datagram = converter->encodeDatagram(list);
    udpSocket.writeDatagram( datagram, targetIp, targetPort.toLong( Q_NULLPTR, 10) );
    makeLogNote( "отправлен пакет" );
    QMessageBox::information(this, "УСПЕХ", "Пакет отправлен успешно");
    bool x = dbConnect.makeNote( 1, getCurrentDateAndTime(), 1, data, 2);
    if ( x ) {
        makeLogNote( "запись действия добавлена в БД" );
    }
        else {
        makeLogNote( "ошиба записи действия в БД" );
    }
}

void MainWindow::on_showRouteTable_triggered()
{
    makeLogNote( "Загружены данные о маршрутах" );
}

void MainWindow::on_sendRoute_triggered()
{

}

void MainWindow::on_showDocumentTable_triggered()
{
    makeLogNote( "Загружены данные о документах" );
}

void MainWindow::on_sendDocument_triggered()
{

}

void MainWindow::on_showModeTable_triggered()
{
    makeLogNote( "Загружены данные о режимах" );
}

void MainWindow::on_sendMode_triggered()
{

}

void MainWindow::on_showPositionTable_triggered()
{
    makeLogNote( "Загружены данные о местоположениях" );
}

void MainWindow::on_sendPosition_triggered()
{

}

void MainWindow::setIp() {
    IpDialog dia;
    if ( dia.exec() ) {
        myIp = dia.value();
    }
}

QString MainWindow::makeDatagramCommand( QString q )
{
    QString answer = "";
    answer.append( "0" );                        //метод сжатия
    answer.append( converter->dobei( q, 6 ) );      //отправитель добить до 6
    answer.append( converter->dobei( "mbu" , 6) );  //получатель
    answer.append( "0" );                        //категория данных
    answer.append( "P" );                        //данные о сообщении
    answer.append( "K1" );                       //Идентификатор приложения, которое  должно обрабатывать переданные данные.
    answer.append( "=" );                        //Признак начала передаваемых данных
    QString request = dbConnect.getCommandInformation(q);
    if (request.compare("error") == 0) {
        return "error";
    }
    else {
        answer.append(request);
    }
    return answer;
}

void MainWindow::parsingMessage( QString s )
{
    if ( s.at( 0 ) == '1' ) {
        makeLogNote( "oшибка, данные сжаты" );
        //не работаем пока со сжатием данных
        return;
    }
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
    int i = 0;
    QString x = assistParser( data, i );
    QString y = assistParser( data, i );
    QString z = assistParser( data, i );
    QString dir = assistParser( data, i );
    if ( dbConnect.writeCoordinats(x, y, z, dir, getCurrentDateAndTime(), object) ) {
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
            x="";
            if ( !dbConnect.writeRocket(x, getCurrentDateAndTime(), object) ) {
                makeLogNote( "ошибка запроса" );
                return;
            }
            x="";
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
