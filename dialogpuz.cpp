#include "dialogpuz.h"
#include "ui_dialogpuz.h"

DialogPuz::DialogPuz(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogPuz)
{
    ui->setupUi(this);
    QTextCodec *codec = QTextCodec::codecForName("GB18030");

    QPushButton *btnOk = ui->buttonBox->button(QDialogButtonBox::Ok);
    QPushButton *btnCancel = ui->buttonBox->button(QDialogButtonBox::Cancel);
    btnOk->setText(codec->toUnicode("接受"));
    btnCancel->setText(codec->toUnicode("取消"));

    ui->gridLayout->setAlignment(ui->lblPuzzle, Qt::AlignHCenter | Qt::AlignVCenter);
    ui->lblStep->setText("0");
    ui->lblPuzzle->setText("text");
    ui->lblPuzzle->adjustSize();
}

DialogPuz::~DialogPuz()
{
    delete ui;
}

void DialogPuz::on_btnRetry_pressed()
{
    emit retry();
}
