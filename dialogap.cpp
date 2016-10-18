#include "dialogap.h"
#include "ui_dialogap.h"

DialogAP::DialogAP(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogAP)
{
    ui->setupUi(this);
    QTextCodec *codec = QTextCodec::codecForName("GB18030");

    ui->cbxType->addItem(codec->toUnicode("加法"));
    ui->cbxType->addItem(codec->toUnicode("减法"));
    ui->cbxType->addItem(codec->toUnicode("乘法"));
    ui->cbxType->addItem(codec->toUnicode("除法"));
    ui->cbxType->addItem(codec->toUnicode("随机"));

    ui->cbxDiffi->addItem(codec->toUnicode("信手拈来"));
    ui->cbxDiffi->addItem(codec->toUnicode("迎刃而解"));
    ui->cbxDiffi->addItem(codec->toUnicode("深思熟虑"));
    ui->cbxDiffi->addItem(codec->toUnicode("大海捞针"));
    ui->cbxDiffi->addItem(codec->toUnicode("难度不限"));
}

DialogAP::~DialogAP()
{
    delete ui;
}
