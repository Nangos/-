#ifndef DIALOGPUZ_H
#define DIALOGPUZ_H

#include <QDialog>
#include <QTextCodec>

namespace Ui {
class DialogPuz;
}

class DialogPuz : public QDialog
{
    Q_OBJECT

public:
    explicit DialogPuz(QWidget *parent = 0);
    ~DialogPuz();
    bool autoMode;

signals:
    void retry();
//private:
public:
    Ui::DialogPuz *ui;
private slots:
    void on_btnRetry_pressed();
};

#endif // DIALOGPUZ_H
