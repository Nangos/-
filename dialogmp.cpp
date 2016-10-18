#include "dialogmp.h"
#include "ui_dialogmp.h"

DialogMP::DialogMP(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogMP)
{
    ui->setupUi(this);
    QTextCodec *codec = QTextCodec::codecForName("GB18030");

    QPushButton *btnOk = ui->buttonBox->button(QDialogButtonBox::Ok);
    QPushButton *btnCancel = ui->buttonBox->button(QDialogButtonBox::Cancel);
    btnOk->setText(codec->toUnicode("确定"));
    btnCancel->setText(codec->toUnicode("取消"));
    btnOk->setEnabled(false);

    ui->gridLayout->setAlignment(ui->lblPreview, Qt::AlignHCenter | Qt::AlignVCenter);

    group = new QButtonGroup;
    group->addButton(ui->rdbAdd, 0);
    group->addButton(ui->rdbSub, 1);
    group->addButton(ui->rdbMul, 2);
    group->addButton(ui->rdbDiv, 3);
    connect(ui->rdbAdd, SIGNAL(clicked()), this, SLOT(on_group_clicked()));
    connect(ui->rdbSub, SIGNAL(clicked()), this, SLOT(on_group_clicked()));
    connect(ui->rdbMul, SIGNAL(clicked()), this, SLOT(on_group_clicked()));
    connect(ui->rdbDiv, SIGNAL(clicked()), this, SLOT(on_group_clicked()));

}

DialogMP::~DialogMP()
{
    delete ui;
    delete group;
}

void DialogMP::on_group_clicked()
{
    QString msg;
    QTextCodec *codec = QTextCodec::codecForName("GB18030");
    id = group->checkedId();
    switch(id)
    {
    case 0:
        msg = codec->toUnicode("请逐行在下方文本框中输入竖式\n每个加数各占一行\n和写在最后一行\n不用加空格");
        break;
    case 1:
        msg = codec->toUnicode("请逐行在下方文本框中输入竖式\n被减数在第一行\n之后每个减数各占一行\n差写在最后一行\n不用加空格");
        break;
    case 2:
        msg = codec->toUnicode("请逐行在下方文本框中输入竖式\n乘数写在前两行\n之后每个“部分积”占一行\n积写在最后一行\n不用加空格");
        break;
    case 3:
        msg = codec->toUnicode("请逐行在下方文本框中输入竖式\n被除数、除数、商分别占第一、二、三行\n剩下的部分按自然竖式逐行输入\n不用加空格");
        break;
    default:
        break;
    }
    ui->lblGuide->setText(msg);
    if(!(ui->textEdit->isEnabled())){
        ui->textEdit->setEnabled(true);
    }
    emit type_changed(id);
}

void DialogMP::on_textEdit_textChanged()
{
    emit type_changed(id);
}
