#ifndef DELDIALOG_H
#define DELDIALOG_H

#include <QDialog>

namespace Ui {
class DelDialog;
}

class DelDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DelDialog( QWidget *parent = 0 );

    QString value();

    ~DelDialog();

private:
    Ui::DelDialog *ui;
};

#endif // DELDIALOG_H
