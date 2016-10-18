#ifndef DIALOGMP_H
#define DIALOGMP_H

#include <QDialog>
#include <QPushButton>
#include <QTextCodec>
#include <QRadioButton>
#include <QButtonGroup>

namespace Ui {
class DialogMP;
}

class DialogMP : public QDialog
{
    Q_OBJECT

public:
    explicit DialogMP(QWidget *parent = 0);
    ~DialogMP();
    Ui::DialogMP *ui;
    QButtonGroup *group;
    int id;

signals:
    void type_changed(int);

public slots:
    void on_group_clicked();

private slots:
    void on_textEdit_textChanged();
};

#endif // DIALOGMP_H
