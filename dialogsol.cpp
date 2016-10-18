#include "dialogsol.h"
#include "ui_dialogsol.h"

DialogSol::DialogSol(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSol)
{
    ui->setupUi(this);
    ui->gridLayout->setAlignment(ui->lblShow, Qt::AlignHCenter | Qt::AlignVCenter);
}

DialogSol::~DialogSol()
{
    delete ui;
}

void DialogSol::on_btnPrev_pressed()
{
    ui->spinBox->setValue(ui->spinBox->value()-1);
}

void DialogSol::on_btnNext_pressed()
{
    ui->spinBox->setValue(ui->spinBox->value()+1);
}

void DialogSol::on_spinBox_valueChanged(int n)
{
    emit goToSolution(n);
}
