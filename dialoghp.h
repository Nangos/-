#ifndef DIALOGHP_H
#define DIALOGHP_H

#include <QDialog>
#include <QPushButton>
#include <QTextCodec>

namespace Ui {
class Dialoghp;
}

class Dialoghp : public QDialog
{
    Q_OBJECT

public:
    explicit Dialoghp(QWidget *parent = 0);
    ~Dialoghp();

signals:
    void input_changed();

private slots:
    void on_cbxType_currentIndexChanged(int index);

    void on_lineEdit_textChanged(const QString &arg1);

    void on_textEdit_textChanged();
public:
//private:
    Ui::Dialoghp *ui;
};

#endif // DIALOGHP_H
