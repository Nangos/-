#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QFileDialog>
#include <QStringList>
#include <QMessageBox>
#include "dialogmp.h"
#include "dialogip.h"
#include "dialogap.h"
#include "dialoghp.h"
#include "dialogms.h"
#include "dialogsol.h"
#include "dialogpuz.h"
#include "ctime"

#include "secretary.h"
#include "core.h"
#include <cmath>
#include <QFile>
#include <QTextStream>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private slots:
    void on_btnMP_pressed();
    void on_btnIn_pressed();

    void on_btnAP_pressed();

    void on_btnHAP_pressed();

    void on_btnMS_pressed();

    void on_btnOut_pressed();

    void on_btnAIS_pressed();

    void do_puzzle_check(int id);
    void on_open_demo(int id);
    void on_goToSolution(int index);
    void on_boxes_changed(int dump);
    void on_submit();
    void on_input_changed();
    void on_retry();

    void on_btnAIA_pressed();

private:
    Ui::Widget *ui;
    DialogIP *dip;
    DialogMP *dmp;
    DialogAP *dap;
    Dialoghp *dhp;
    DialogMS *dms;
    DialogSol *dsol;
    DialogPuz *dpuz;

    Secretary *secretary;
    Secretary *secretary2;
    Secretary *secretary3;
    Core *core;
    Core *core2;

    void logFile(int type, int size1, int size2, int id, QTextStream &in);
    void logExec(QString fileName);

};

#endif // WIDGET_H
