#ifndef DIALOGAP_H
#define DIALOGAP_H

#include <QDialog>
#include <QTextCodec>

namespace Ui {
class DialogAP;
}

class DialogAP : public QDialog
{
    Q_OBJECT

public:
    explicit DialogAP(QWidget *parent = 0);
    ~DialogAP();

//private:
public:
    Ui::DialogAP *ui;
};

#endif // DIALOGAP_H
