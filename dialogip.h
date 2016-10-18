#ifndef DIALOGIP_H
#define DIALOGIP_H

#include <QDialog>
#include <QButtonGroup>
#include <QPushButton>
#include <QRadioButton>
#include <QFileDialog>

namespace Ui {
class DialogIP;
}

class DialogIP : public QDialog
{
    Q_OBJECT

public:
    explicit DialogIP(QWidget *parent = 0);
    ~DialogIP();

private slots:
    void on_buttonBox_accepted();

signals:
    void open_demo(int);

private:
    Ui::DialogIP *ui;
    QButtonGroup *group;
    QString fileName;

};

#endif // DIALOGIP_H
