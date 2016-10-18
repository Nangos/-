#include "dialogip.h"
#include "ui_dialogip.h"

DialogIP::DialogIP(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogIP)
{
    ui->setupUi(this);

    group = new QButtonGroup;
    group->addButton(ui->rdbDemo1, 0);
    group->addButton(ui->rdbDemo2, 1);
}

DialogIP::~DialogIP()
{
    delete ui;
    delete group;
}

void DialogIP::on_buttonBox_accepted()
{
    int id = group->checkedId();
    emit open_demo(id);
}
