#ifndef DIALOGSOL_H
#define DIALOGSOL_H

#include <QDialog>

namespace Ui {
class DialogSol;
}

class DialogSol : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSol(QWidget *parent = 0);
    ~DialogSol();

//private:
    Ui::DialogSol *ui;
signals:
    void goToSolution(int);
private slots:
    void on_btnPrev_pressed();
    void on_btnNext_pressed();
    void on_spinBox_valueChanged(int n);
};

#endif // DIALOGSOL_H
