#include "parser.h"

Parser::Parser()
{

}

void Parser::parsingMessage( QString s )
{
    /*if ( s.at( 0 ) == "1" ) {
        //выдать ошибку
        //не работаем пока со сжатием данных
        return;
    }*/
    bool trigger = false;
    QString object = "";
    for ( int i = POSITION_OBJECT_BEGIN; i < POSITION_OBJECT_END; i++ )
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
    for ( int i = POSITION_CODE_BEGIN; i < POSITION_CODE_END; i++ )
    {
        messageCode+=s.at( i );
    }
    /*if (QString::compare( messageCode, "C1") == 0) {
        parsingCoord(s, object);
    }
    if (QString::compare( messageCode, "T1") == 0) {
        parsingRocket(s, object);
    }*/
}

void Parser::parsingCoord(QString s, QString object)
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
    /*QSqlQuery query = QSqlQuery( db );
    QString queryString;
    queryString = "UPDATE own_forces.combatobject_location SET obj_location=ST_MakePoint ("+x+","+y+","+z+"), direction='"+dir+
            "', date_edit='"+ getCurrentDateAndTime() + "' WHERE combat_hierarchy='"+object+"';";
    if ( query.exec( queryString ) ) {
        makeLogNote( "база обновлена" );
    }
    else {
        makeLogNote( "ошибка запроса" );
    }*/
}

void Parser::parsingRocket(QString s, QString object)
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
            /*QSqlQuery query = QSqlQuery( db );
            QString queryString;
            queryString = "UPDATE own_forces.rocket SET type_tid='"+ x +
                    "', date_edit='"+ getCurrentDateAndTime() + "' WHERE combatobjectid='"+object+"';";
            x="";
            if ( !query.exec( queryString ) ) {
                makeLogNote( "ошибка запроса" );
                return;
            }
            else {
            }*/
        }
        i++;
    }
   // makeLogNote( "база обновлена" );
}

QString Parser::assistParser( QString data, int &counter )
{
    QString answer = "";
    while ( data.at( counter ) != ';' ) {
        answer.append( data.at( counter ) );
        counter++;
    }
    counter++;
    return answer;
}
