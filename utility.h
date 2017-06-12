#ifndef UTILITY_H
#define UTILITY_H

#include <QtWidgets>
#include <QtSql>

class Utility {

public:
    Utility();

    static void initializeTableSettings(QTableWidget*);
    static void closeNewEditTab(QTabWidget*);
    static void clearLayout(QLayout*);
    static QList<double> convertCoordToDecimalFormat(QString);
    static QString convertCoordToDegreeFormat(double);
    static QString convertCodeToReferenceName(QSqlDatabase  db, QString code);
    static QString convertReferenceNameTOCode(QSqlDatabase  db, QString referenceName);
};

#endif // UTILITY_H
