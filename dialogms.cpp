#include "dialogms.h"
#include "ui_dialogms.h"

DialogMS::DialogMS(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogMS)
{
    ui->setupUi(this);

    labels[0] = ui->lbl_0; labels[1] = ui->lbl_1; labels[2] = ui->lbl_2; labels[3] = ui->lbl_3; labels[4] = ui->lbl_4;
    labels[5] = ui->lbl_5; labels[6] = ui->lbl_6; labels[7] = ui->lbl_7; labels[8] = ui->lbl_8; labels[9] = ui->lbl_9;

    boxes[0] = ui->cbx_0; boxes[1] = ui->cbx_1; boxes[2] = ui->cbx_2; boxes[3] = ui->cbx_3; boxes[4] = ui->cbx_4;
    boxes[5] = ui->cbx_5; boxes[6] = ui->cbx_6; boxes[7] = ui->cbx_7; boxes[8] = ui->cbx_8; boxes[9] = ui->cbx_9;

    for(int i=0; i<10; i++){
        boxes[i]->addItem(" ");
        for(int j=0; j<10; j++){
            boxes[i]->addItem(QString::number(j));
        }
        connect(boxes[i], SIGNAL(currentIndexChanged(int)), this, SIGNAL(boxes_changed(int)));
    }
    ui->gridLayout1->setAlignment(ui->lblFrom, Qt::AlignHCenter | Qt::AlignVCenter);
    ui->gridLayout2->setAlignment(ui->lblTo, Qt::AlignHCenter | Qt::AlignVCenter);
}

DialogMS::~DialogMS()
{
    delete ui;
}

void DialogMS::on_btnClear_pressed()
{
    clearAll();
}

void DialogMS::on_btnSubmit_pressed()
{
    emit submit();
}
