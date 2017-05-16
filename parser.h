#ifndef PARSER_H
#define PARSER_H

#include <QString>


class Parser
{
public:
    Parser();
    void parsingMessage( QString );
private:
    const int POSITION_OBJECT_BEGIN = 1;
    const int POSITION_OBJECT_END = 7;
    const int POSITION_CODE_BEGIN = 15;
    const int POSITION_CODE_END = 17;

    void parsingCoord( QString, QString );
    void parsingRocket( QString, QString );
    QString assistParser( QString , int & );
};

#endif // PARSER_H
