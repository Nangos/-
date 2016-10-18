#include "dialoghp.h"
#include "ui_dialoghp.h"

Dialoghp::Dialoghp(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialoghp)
{
    ui->setupUi(this);
    QTextCodec *codec = QTextCodec::codecForName("GB18030");
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);

    ui->cbxType->addItem(codec->toUnicode("加法"));
    ui->cbxType->addItem(codec->toUnicode("减法"));
    ui->cbxType->addItem(codec->toUnicode("乘法"));
    ui->cbxType->addItem(codec->toUnicode("除法"));

    ui->cbxQuote->addItem(codec->toUnicode("全部以数字填充"));
    ui->cbxQuote->addItem(codec->toUnicode("填尽量多的变量"));

    ui->gridLayout->setAlignment(ui->lblShow, Qt::AlignHCenter | Qt::AlignVCenter);
}

Dialoghp::~Dialoghp()
{
    delete ui;
}

void Dialoghp::on_cbxType_currentIndexChanged(int index)
{
    index;
    emit input_changed();
}

void Dialoghp::on_lineEdit_textChanged(const QString &arg1)
{
    arg1;
    emit input_changed();
}

void Dialoghp::on_textEdit_textChanged()
{
    emit input_changed();
}
