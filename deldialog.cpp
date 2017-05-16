#include "deldialog.h"
#include "ui_deldialog.h"

DelDialog::DelDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DelDialog)
{
    ui->setupUi(this);
}

DelDialog::~DelDialog()
{
    delete ui;
}

QString DelDialog::value()
{
    return ui->lineEdit->text();
}
