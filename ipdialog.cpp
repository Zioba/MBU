#include "ipdialog.h"
#include "ui_ipdialog.h"

IpDialog::IpDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::IpDialog)
{
    ui->setupUi(this);
}

IpDialog::~IpDialog()
{
    delete ui;
}

QString IpDialog::value()
{
    return ui->codeLine->text();
}
