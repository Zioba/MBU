#ifndef IPDIALOG_H
#define IPDIALOG_H

#include <QDialog>

namespace Ui {
class IpDialog;
}

class IpDialog : public QDialog
{
    Q_OBJECT

public:
    explicit IpDialog( QWidget *parent = 0 );

    QString value();

    ~IpDialog();

private:
    Ui::IpDialog *ui;
};

#endif // IPDIALOG_H
