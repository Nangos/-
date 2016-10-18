#ifndef DIALOGMS_H
#define DIALOGMS_H

#include <QDialog>
#include <QComboBox>
#include <QLabel>
#include <QStringList>

namespace Ui {
class DialogMS;
}

class DialogMS : public QDialog
{
    Q_OBJECT

public:
    explicit DialogMS(QWidget *parent = 0);
    ~DialogMS();

signals:
    void boxes_changed(int);
    void submit();
//private:
public:
    Ui::DialogMS *ui;
    QLabel *(labels[10]);
    QComboBox *(boxes[10]);

    void clearAll(){
        for(int i=0; i<10; i++){
            boxes[i]->setCurrentIndex(0);
        }
    }
    void init(QStringList list){
        for(int i=0; i<list.size(); i++){
            labels[i]->setText(list.at(i));
            boxes[i]->setEnabled(true);
        }
        for(int i=list.size(); i<10; i++){
            labels[i]->setText("");
            boxes[i]->setEnabled(false);
        }
    }
private slots:
    void on_btnClear_pressed();
    void on_btnSubmit_pressed();
};

#endif // DIALOGMS_H
