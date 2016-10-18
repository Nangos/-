#include "widget.h"
#include "ui_widget.h"
#include "dialogip.cpp"
#include "dialogmp.cpp"
#include "dialogap.cpp"
#include "dialoghp.cpp"
#include "dialogms.cpp"
#include "dialogsol.cpp"
#include "dialogpuz.cpp"
#include "secretary.cpp"
#include "core.cpp"
#include <QLayout>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    srand((unsigned)time(0));
    dmp = new DialogMP();
    dip = new DialogIP();
    dap = new DialogAP();
    dhp = new Dialoghp();
    dms = new DialogMS();
    dsol = new DialogSol();
    dpuz = new DialogPuz();

    secretary = new Secretary();
    secretary2 = new Secretary();
    secretary3 = new Secretary();
    core = new Core();
    core2 = new Core();
    ui->gridLayout->setAlignment(ui->lblP, Qt::AlignHCenter | Qt::AlignVCenter);

    connect(dmp, SIGNAL(type_changed(int)), this, SLOT(do_puzzle_check(int)));
    connect(dip, SIGNAL(open_demo(int)), this, SLOT(on_open_demo(int)));
    connect(dsol, SIGNAL(goToSolution(int)), this, SLOT(on_goToSolution(int)));
    connect(dms, SIGNAL(boxes_changed(int)), this, SLOT(on_boxes_changed(int)));
    connect(dms, SIGNAL(submit()), this, SLOT(on_submit()));
    connect(dhp, SIGNAL(input_changed()), this, SLOT(on_input_changed()));
    connect(dpuz, SIGNAL(retry()), this, SLOT(on_retry()));
}

void Widget::logFile(int type, int size1, int size2, int id, QTextStream &in)
{
    //Print head:
    in<<"_________________________________________________________________________________________________________________________________";
    in<<"\n\n";

    //Generating puzzles:
    secretary->autoPuzzleInit(type, size1, size2);
    bool ch[300];
    int shuffle[300];
    for(int i=0; i<secretary->nDigits; i++){
        shuffle[i] = i;
        ch[i] = false;
    }
    for(int i=secretary->nDigits-1; i>=0; i--){
        int r = rand()%(i+1);
        int temp = shuffle[r];
        shuffle[r] = shuffle[i];
        shuffle[i] = temp;
    }
    in<<QString("Type: %1\t\tSize: %2*%3\tnDigits: %4\tNo.%5")
        .arg(QString::number(type))
        .arg(QString::number(size1))
        .arg(QString::number(size2))
        .arg(QString::number(secretary->nDigits))
        .arg(QString::number(id))
     <<"\n\n";
    in<<secretary->show(type, secretary->autoPuzzle)<<"\n\n";
    in<<"order: ";
    for(int i=0; i<secretary->nDigits; i++){
        in<<shuffle[i]<<", ";
        if((i+1)%30==0){
            in<<'\n';
        }
    }
    in<<"\n\n";
    in<<"nVar\tnStep\tnSol\tTry1\tTry2\tTry3\tTry4\tTry5\tTry6\tTry7\tTry8\tTry9\tMid9\tMid5\tMid1\tSPS\n";

    for(int i=0; i<secretary->nDigits; i++){
        int nVar, nStep, nSol, trys[9], mid9, mid5, mid1, sps;
        nVar = i+1;
        ch[shuffle[i]] = true;
        secretary->initFromAuto(type, secretary->autoPuzzle, secretary->ntovTable, ch);
        core->init(type, secretary->normalPuzzle);
        core->solve(true, 100000);
        nStep = core->getNSteps();
        nSol = core->getNSolutions();
        for(int i=0; i<9; i++){
            core->init(type, secretary->normalPuzzle);
            core->solve(false, 10000);
            trys[i] = core->getNSteps();
        }
        in<<nVar<<'\t';
        in<<(nStep == 100000? QString("99999+"): QString::number(nStep))<<'\t'<<nSol;
        in<<(nStep == 100000? "+": "")<<'\t';
        for(int i=0; i<9; i++){
            in<<(trys[i]==10000? QString("9999+"): QString::number(trys[i]))<<'\t';
        }
        for(int i=0; i<9; i++){
            for(int j=0; j<i; j++){
                if(trys[i] < trys[j]){
                    int temp = trys[i];
                    trys[i] = trys[j];
                    trys[j] = temp;
                }
            }
        }
        double temp[9], av9, av5, av1;
        for(int i=0; i<9; i++){
            temp[i] = std::log(trys[i]);
        }
        av9 = (temp[0] + temp[1] + temp[2] + temp[3] + temp[4] + temp[5] + temp[6] + temp[7] + temp[8]) / 9;
        av5 = (temp[2] + temp[3] + temp[4] + temp[5] + temp[6]) / 5;
        av1 = temp[4];
        mid9 = std::exp(av9);
        mid5 = std::exp(av5);
        mid1 = std::exp(av1);
        sps = nSol==0? -1: nStep/nSol;
        in<<mid9<<'\t'<<mid5<<'\t'<<mid1<<'\t'<<(sps<0? QString("+inf"): QString::number(sps));
        in<<"\n";
    }
}

void Widget::logExec(QString fileName)
{
    QFile file(fileName);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream in(&file);
    int count = 0;
    ui->lblP->setText(QString("%1 of 276").arg(QString::number(count)));
    this->repaint();
    //add
    for(int s1=2; s1<=9; s1++){
        for(int s2=2; s2<=5; s2++){
            for(int id=0; id<3; id++){
               count++;
               logFile(0, s1, s2, id, in);
               ui->lblP->setText(QString("%1 of 276").arg(QString::number(count)));
               this->repaint();
            }
        }
    }
    //mul
    for(int s1=2; s1<=9; s1++){
        for(int s2=1; s2<=5; s2++){
            for(int id=0; id<3; id++){
               count++;
               logFile(2, s1, s2, id, in);
               ui->lblP->setText(QString("%1 of 276").arg(QString::number(count)));
               this->repaint();
            }
        }
    }
    //div
    for(int s1=1; s1<=5; s1++){
        for(int s2=1; s2<=4; s2++){
            for(int id=0; id<3; id++){
               count++;
               logFile(3, s1, s2, id, in);
               ui->lblP->setText(QString("%1 of 276").arg(QString::number(count)));
               this->repaint();
            }
        }
    }
    file.close();
    QMessageBox::information(this, "OK.", "OK.");
}

Widget::~Widget()
{
    delete ui;
    delete dmp;
    delete dip;
    delete dap;
    delete dhp;
    delete dms;
    delete dsol;

    delete secretary;
}

void Widget::on_btnMP_pressed()
{
    if(secretary2->puzzleType != -1){
        emit do_puzzle_check(secretary2->puzzleType);
    }
    int res = dmp->exec();
    if(res == QDialog::Accepted){
        secretary->checkError(secretary2->puzzleType, secretary2->rawString, false);
        secretary->initPuzzle(secretary2->puzzleType, secretary2->rawString);
        ui->lblP->setText(secretary->show(secretary->puzzleType, secretary->rawString));
        ui->lblP->adjustSize();
        ui->btnAIA->setEnabled(true);
        ui->btnAIS->setEnabled(true);
        ui->btnMS->setEnabled(true);

        dms->clearAll();
        dms->ui->lblFrom->setText(secretary->show(secretary->puzzleType, secretary->rawString));
        dms->ui->lblTo->setText(secretary->show(secretary->puzzleType, secretary->rawString));
        QString table = secretary->charTable;
        QString str;
        for(int i=0; i<table.size(); i++){
            str.append(table.at(i));
            str.append(secretary->fullEqual);
            if(i!=table.size()-1){
                str.append('.');
            }
        }
        QStringList list = str.split('.');
        dms->init(list);
        ui->btnMS->setEnabled(true);
    }
}

void Widget::on_btnIn_pressed()
{
    dip->exec();
}

void Widget::on_btnAP_pressed()
{
    int res = dap->exec();
    if(res == QDialog::Accepted){
        int type = dap->ui->cbxType->currentIndex();
        if(type == 4){ //random
            type = rand()%4;
        }
        int diff = dap->ui->cbxDiffi->currentIndex();
        int size1, size2;
        bool foo[300];
        long long levelSplit[5] = {20, 40, 200, 1000, 100000};
        long long countSteps = 0;
        bool toBreak = false;
        while(true){
            secretary2->getRandomSizes(type, &size1, &size2);
            secretary2->autoPuzzleInit(type, size1, size2);
            int count = 0;
            char order[10];
            for(int i=0; i<10; i++){
                if(secretary2->ntovTable[i]!=0){
                    order[count] = i;
                    count++;
                }
            }
            int maxHint; //replace some var to num to make it easier
            switch(diff){
            case 0:
                maxHint = count/2;
                break;
            case 1:
                maxHint = count>4? 2: count/2;
                break;
            case 2:
                maxHint = count>2? 1: count/2;
                break;
            case 3:
                maxHint = 0;
                break;
            default:
                maxHint = count/2;
                break;
            }
            int sh[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
            int shuffle[10];
            for(int i=0; i<maxHint; i++){
                int r = rand()%(count-i);
                shuffle[i] = sh[r];
                sh[r] = sh[count-1-i];
            }
            for(int i=0; i<300; i++){
                foo[i] = true;
            }
            for(int hint=0; hint<=maxHint; hint++){
                int curs;
                for(int i=0; i<hint; i++){
                    curs = 0;
                    for(char *p=secretary2->autoPuzzle; *p; p++){
                        if(*p == order[shuffle[i]]+'0'){
                            foo[curs] = false;
                        }
                        if(*p>='0' && *p<='9'){
                            curs++;
                        }
                    }
                }
                secretary2->initFromAuto(type, secretary2->autoPuzzle, secretary2->ntovTable, foo);
                core->init(type, secretary2->normalPuzzle);
                int result = core->solve(true, 100000, true);
                countSteps += core->getNSteps();
                if(!result){
                    continue;
                }
                long long s = core->getNSteps();
                if(diff!=4){
                    if(s <= levelSplit[diff] || s > levelSplit[diff+1]){
                        continue;
                    }
                }
                toBreak = true;
                break;
            }
            if(toBreak){
                break;
            }
        }
        dpuz->ui->lblContent->setText(secretary->info1);
        dpuz->ui->lblStep->setText(QString::number(countSteps));
        dpuz->ui->lblPuzzle->setText(secretary2->show(type, secretary2->originalPuzzle, secretary2->charTable));

        dpuz->autoMode = true;
        int subRes = dpuz->exec();
        if(subRes == QDialog::Accepted){
            for(char *p = secretary2->originalPuzzle; *p; p++){
                if(*p>='A' && *p<='J'){
                    *p += 'a'-'A';
                }
            }
            type = secretary2->puzzleType;
            secretary->initPuzzle(type, secretary2->originalPuzzle, false);
            ui->lblP->setText(secretary->show(type, secretary->originalPuzzle, secretary->charTable));
            ui->btnAIA->setEnabled(true);
            ui->btnAIS->setEnabled(true);

            dms->clearAll();
            dms->ui->lblFrom->setText(secretary->show(type, secretary->originalPuzzle, secretary->charTable));
            dms->ui->lblTo->setText(secretary->show(type, secretary->originalPuzzle, secretary->charTable));
            QString table = secretary->charTable;
            QString str;
            for(int i=0; i<table.size(); i++){
                str.append(table.at(i));
                str.append(secretary->fullEqual);
                if(i!=table.size()-1){
                    str.append('.');
                }
            }
            QStringList list = str.split('.');
            dms->init(list);
            ui->btnMS->setEnabled(true);
        }
    }
}

void Widget::on_btnHAP_pressed()
{
    emit on_input_changed();
    int res = dhp->exec();
    if(res == QDialog::Accepted){
        int type = dhp->ui->cbxType->currentIndex();
        int fill = dhp->ui->cbxQuote->currentIndex();
        secretary3->initPuzzle(type, secretary3->rawString, false);
        core2->init(type, secretary3->normalPuzzle);
        if(core2->nVar == 0 && fill == 0){
            QMessageBox::warning(this, "Error!!", "There are only numbers since every '?' is also a number.");
            return;
        }
        core2->solve(false, 0, false);
        if(core2->getNSolutions()==0){
            QString s = QString("There is no solution whatever the '?'s are.\nLet alone finding a puzzle!!\nTotal steps: %1")
                    .arg(QString::number(core2->getNSteps()));
            QMessageBox::warning(this, "No Solution!", s);
            return;
        }
        //from now on: there is solution
        char c[40];
        core2->getSolutionAt(c, 1);
        secretary2->initFromHalf(type, secretary3->puzzleCharTable, secretary3->originalPuzzle,
                                 c, secretary3->charTable, fill==0);
        core->init(type, secretary2->normalPuzzle);
        bool multi = core->solve(true, 123456789, true);
        dpuz->ui->lblContent->setText(multi? secretary->info1: secretary->info2);
        dpuz->ui->lblStep->setText(QString::number(core2->getNSteps()+core->getNSteps()));
        dpuz->ui->lblPuzzle->setText(secretary2->show(type, secretary2->originalPuzzle, secretary2->charTable));
        dpuz->autoMode = false;
        int subRes = dpuz->exec();
        if(subRes == QDialog::Accepted){
            secretary->initPuzzle(type, secretary2->originalPuzzle, false);
            secretary->charTable = QString(secretary2->charTable);
            ui->lblP->setText(secretary->show(type, secretary->originalPuzzle, secretary->charTable));
            ui->btnAIA->setEnabled(true);
            ui->btnAIS->setEnabled(true);

            dms->clearAll();
            dms->ui->lblFrom->setText(secretary->show(type, secretary->originalPuzzle, secretary->charTable));
            dms->ui->lblTo->setText(secretary->show(type, secretary->originalPuzzle, secretary->charTable));
            QString table = secretary->charTable;
            QString str;
            for(int i=0; i<table.size(); i++){
                str.append(table.at(i));
                str.append(secretary->fullEqual);
                if(i!=table.size()-1){
                    str.append('.');
                }
            }
            QStringList list = str.split('.');
            dms->init(list);
            ui->btnMS->setEnabled(true);
        }
    }
}

void Widget::on_btnMS_pressed()
{
    core->init(secretary->puzzleType, secretary->normalPuzzle);
    core->solve(true);
    dms->exec();
}

void Widget::on_btnOut_pressed()
{
    //QFileDialog::getSaveFileName(this, tr("Save File"), "", tr("Text Files(*.txt)"));
}

void Widget::on_btnAIS_pressed()
{
    core->init(secretary->puzzleType, secretary->normalPuzzle);
    core->solve(false);
    dsol->ui->lblStep->setText(QString::number(core->getNSteps()));
    dsol->ui->lblSol->setText(QString::number(core->getNSolutions()));
    if(core->getNSolutions()==0){
        dsol->ui->spinBox->setEnabled(false);
        dsol->ui->btnNext->setEnabled(false);
        dsol->ui->btnPrev->setEnabled(false);
        dsol->ui->lblList->setText("");
        dsol->ui->lblShow->setText("NO SOLUTION WAS FOUND.");
    }else{
        dsol->ui->spinBox->setEnabled(true);
        dsol->ui->btnPrev->setEnabled(false);
        dsol->ui->spinBox->setMaximum(core->getNSolutions());
        dsol->ui->btnNext->setEnabled(core->getNSolutions()>1? true: false);
        char s1[40];
        core->getSolutionAt(s1, 1);
        QString s2 = QString(s1);
        dsol->ui->lblShow->setText(secretary->show(secretary->puzzleType, secretary->originalPuzzle, s2));
        QString t1 = "";
        for(int i=0; i<(core->nVar); i++){
            t1.append(secretary->charTable.at(i));
            t1.append(secretary->fullEqual);
            t1.append(secretary->fullNumber.at(s1[i]-'0'));
            if(i==4){
                t1.append("\n");
            }else if(i!=(core->nVar-1)){
                t1.append(secretary->fullSpace);
                t1.append(secretary->fullSpace);
                t1.append(secretary->fullSpace);
            }
        }
        dsol->ui->lblList->setText(t1);
    }
    dsol->exec();
    dsol->ui->lblShow->adjustSize();
}

void Widget::do_puzzle_check(int id)
{
    QString str = secretary2->checkError(id, dmp->ui->textEdit->toPlainText(), false);
    if(str.size() == 0){
        str = dmp->ui->textEdit->toPlainText();
        dmp->ui->lblPreview->setText(secretary2->show(id, str));
        dmp->ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
    }else{
        dmp->ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
        dmp->ui->lblPreview->setText(str);
    }
    dmp->ui->lblPreview->adjustSize();
}

void Widget::on_open_demo(int id)
{
    if(id == 1 || id == 0){
        ui->lblP->setText(secretary->showDemo(id));
        secretary->initPuzzle(secretary->puzzleType, secretary->rawString);
        ui->btnAIA->setEnabled(true);
        ui->btnAIS->setEnabled(true);

        dms->clearAll();
        dms->ui->lblFrom->setText(secretary->show(secretary->puzzleType, secretary->rawString));
        dms->ui->lblTo->setText(secretary->show(secretary->puzzleType, secretary->rawString));
        QString table = secretary->charTable;
        QString str;
        for(int i=0; i<table.size(); i++){
            str.append(table.at(i));
            str.append(secretary->fullEqual);
            if(i!=table.size()-1){
                str.append('.');
            }
        }
        QStringList list = str.split('.');
        dms->init(list);
        ui->btnMS->setEnabled(true);
    }
}

void Widget::on_goToSolution(int index)
{
    char s1[40];
    core->getSolutionAt(s1, index);
    dsol->ui->btnNext->setEnabled(index==core->getNSolutions()? false: true);
    dsol->ui->btnPrev->setEnabled(index==1? false: true);

    QString s2 = QString(s1);
    dsol->ui->lblShow->setText(secretary->show(secretary->puzzleType, secretary->originalPuzzle, s2));
    QString t1 = "";
    for(int i=0; i<core->nVar; i++){
        t1.append(secretary->charTable.at(i));
        t1.append(secretary->fullEqual);
        t1.append(secretary->fullNumber.at(s1[i]-'0'));
        if(i==4){
            t1.append("\n");
        }else if(i!=core->nVar-1){
            t1.append(secretary->fullSpace);
            t1.append(secretary->fullSpace);
            t1.append(secretary->fullSpace);
        }
    }
    dsol->ui->lblList->setText(t1);
}

void Widget::on_btnAIA_pressed()
{
    core->init(secretary->puzzleType, secretary->normalPuzzle);
    core->solve(true);
    dsol->ui->lblStep->setText(QString::number(core->getNSteps()));
    dsol->ui->lblSol->setText(QString::number(core->getNSolutions()));
    if(core->getNSolutions()==0){
        dsol->ui->spinBox->setEnabled(false);
        dsol->ui->btnNext->setEnabled(false);
        dsol->ui->btnPrev->setEnabled(false);
        dsol->ui->lblList->setText("");
        dsol->ui->lblShow->setText("NO SOLUTION WAS FOUND.");
    }else{
        dsol->ui->spinBox->setEnabled(true);
        dsol->ui->btnPrev->setEnabled(false);
        dsol->ui->spinBox->setMaximum(core->getNSolutions());
        dsol->ui->btnNext->setEnabled(core->getNSolutions()>1? true: false);

        char s1[40];
        core->getSolutionAt(s1, 1);
        QString s2 = QString(s1);
        dsol->ui->lblShow->setText(secretary->show(secretary->puzzleType, secretary->originalPuzzle, s2));

        QString t1 = "";
        for(int i=0; i<core->nVar; i++){
            t1.append(secretary->charTable.at(i));
            t1.append(secretary->fullEqual);
            t1.append(secretary->fullNumber.at(s1[i]-'0'));
            if(i==4){
                t1.append("\n");
            }else if(i!=core->nVar-1){
                t1.append(secretary->fullSpace);
                t1.append(secretary->fullSpace);
                t1.append(secretary->fullSpace);
            }
        }
        dsol->ui->lblList->setText(t1);
    }
    dsol->exec();
    dsol->ui->lblShow->adjustSize();
}

void Widget::on_boxes_changed(int dump)
{
    dump = 0; //No warning
    QString str = secretary->charTable;
    for(int i=0; i<str.size(); i++){
        int opt = dms->boxes[i]->currentIndex();
        if(opt>0){
            str.replace(i, 1, secretary->fullNumber[opt-1]);
        }
    }
    dms->ui->lblTo->setText(secretary->show(secretary->puzzleType, secretary->originalPuzzle, str));
    dms->ui->lblTo->adjustSize();
}

void Widget::on_submit()
{
    //do virtual solving:
    char temp[400];
    for(int i=0; secretary->normalPuzzle[i]; i++){
        temp[i] = secretary->normalPuzzle[i];
    }
    int v[10];
    QString str = secretary->charTable;
    for(int i=0; i<str.size(); i++){
        v[i] = dms->boxes[i]->currentIndex();
    }
    bool flag = true;
    for(int i=0; i<str.size(); i++){
        for(int j=0; j<i; j++){
            if(v[i]!=0 && v[i]==v[j]){
                flag = false;
            }
        }
    }
    if(!flag){
        QMessageBox::warning(dms, tr("Error!!"), tr("Some number is used MORE THAN ONCE."));
        return;
    }

    bool complete = true;
    for(int i=0; i<str.size(); i++){
        if(v[i]==0){
            complete = false;
        }
    }

    bool find = false;
    char sol[40];
    for(int i=0; i<core->getNSolutions(); i++){
        core->getSolutionAt(sol, i+1);
        bool temp = true;
        for(int j=0; sol[j]; j++){
            if(v[j]!=0 && v[j]-1!=sol[j]-'0' && sol[j]!='?'){
                temp = false;
                break;
            }
        }
        if(temp){
            find = true;
            break;
        }
    }

    if(find){
        if(complete){
            QMessageBox::information(dms, tr("Yes!!!"), tr("This is a solution."));
        }else{
            QMessageBox::information(dms, tr("Carry on!"), tr("Continue to find a solution."));
        }
    }else{
        if(complete){
            QMessageBox::warning(dms, tr("No!!!"), tr("This is NOT a solution."));
        }else{
            QMessageBox::warning(dms, tr("Modify!"), tr("I already know this CANNOT be a solution."));
        }
    }
}

void Widget::on_input_changed()
{
    int type = dhp->ui->cbxType->currentIndex();
    QString table = dhp->ui->lineEdit->text();
    QString str = dhp->ui->textEdit->toPlainText();
    //check table:
    if(table.size() == 0){
        dhp->ui->lblShow->setText("Error.\n\nThe variable list is EMPTY.");
        dhp->ui->lblShow->adjustSize();
        dhp->ui->textEdit->setEnabled(false);
        dhp->ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
        return;
    }else if(table.size() > 10){
        dhp->ui->lblShow->setText("Error.\n\nThe variable list is TOO LONG.\nThe maximum length is 10.");
        dhp->ui->lblShow->adjustSize();
        dhp->ui->textEdit->setEnabled(false);
        dhp->ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
        return;
    }
    for(int i=0; i<table.size(); i++){
        QChar c = table.at(i);
        if(!(c >= 0x4E00 && c <= 0x9FA5) && !(c.isLetter())){
            dhp->ui->lblShow->setText("Error.\n\nThe variable list contains ILLEGAL CHAR(S).");
            dhp->ui->lblShow->adjustSize();
            dhp->ui->textEdit->setEnabled(false);
            dhp->ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
            return;
        }
        if(c.toLatin1()!=0 && c.isLetter()){
            if(c.isUpper()){
                table.replace(i, 1, secretary3->fullUpper.at(c.toLatin1()-'A'));
            }else{
                table.replace(i, 1, secretary3->fullLower.at(c.toLatin1()-'a'));
            }
        }
        for(int j=0; j<i; j++){
            if(table.at(j) == table.at(i)){
                dhp->ui->lblShow->setText("Error.\n\nThe variable list contains chars that occured\nMORE THAN ONCE.");
                dhp->ui->lblShow->adjustSize();
                dhp->ui->textEdit->setEnabled(false);
                dhp->ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
                return;
            }
        }
    }
    dhp->ui->textEdit->setEnabled(true);
    secretary3->puzzleCharTable = table;
    QString toShow = secretary3->checkError(type, str, true);
    if(toShow.size()==0){ //success
        QString s = secretary3->show(type, str);
        dhp->ui->lblShow->setText(s);
        dhp->ui->lblShow->adjustSize();
        dhp->ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
    }else{
        dhp->ui->lblShow->setText(toShow);
        dhp->ui->lblShow->adjustSize();
        dhp->ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    }
}

void Widget::on_retry()
{
    int type;
    if(dpuz->autoMode){
        type = dap->ui->cbxType->currentIndex();
        if(type == 4){ //random
            type = rand()%4;
        }
        int diff = dap->ui->cbxDiffi->currentIndex();
        int size1, size2;
        bool foo[300];
        long long levelSplit[5] = {20, 40, 200, 1000, 100000};
        long long countSteps = 0;
        bool toBreak = false;
        while(true){
            secretary2->getRandomSizes(type, &size1, &size2);
            secretary2->autoPuzzleInit(type, size1, size2);
            int count = 0;
            char order[10];
            for(int i=0; i<10; i++){
                if(secretary2->ntovTable[i]!=0){
                    order[count] = i;
                    count++;
                }
            }
            int maxHint; //replace some var to num to make it easier
            switch(diff){
            case 0:
                maxHint = count/2;
                break;
            case 1:
                maxHint = count>4? 2: count/2;
                break;
            case 2:
                maxHint = count>2? 1: count/2;
                break;
            case 3:
                maxHint = 0;
                break;
            default:
                maxHint = count/2;
                break;
            }
            int sh[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
            int shuffle[10];
            for(int i=0; i<maxHint; i++){
                int r = rand()%(count-i);
                shuffle[i] = sh[r];
                sh[r] = sh[count-1-i];
            }
            for(int i=0; i<300; i++){
                foo[i] = true;
            }
            for(int hint=0; hint<=maxHint; hint++){
                int curs;
                for(int i=0; i<hint; i++){
                    curs = 0;
                    for(char *p=secretary2->autoPuzzle; *p; p++){
                        if(*p == order[shuffle[i]]+'0'){
                            foo[curs] = false;
                        }
                        if(*p>='0' && *p<='9'){
                            curs++;
                        }
                    }
                }
                secretary2->initFromAuto(type, secretary2->autoPuzzle, secretary2->ntovTable, foo);
                core->init(type, secretary2->normalPuzzle);
                int result = core->solve(true, 100000, true);
                countSteps += core->getNSteps();
                if(!result){
                    continue;
                }
                long long s = core->getNSteps();
                if(diff!=4){
                    if(s <= levelSplit[diff] || s > levelSplit[diff+1]){
                        continue;
                    }
                }
                toBreak = true;
                break;
            }
            if(toBreak){
                break;
            }
        }
        dpuz->ui->lblContent->setText(secretary->info1);
        dpuz->ui->lblStep->setText(QString::number(countSteps));
    }else{
        type = dhp->ui->cbxType->currentIndex();
        int fill = dhp->ui->cbxQuote->currentIndex();
        secretary3->initPuzzle(type, secretary3->rawString, false);
        core2->init(type, secretary3->normalPuzzle);
        core2->solve(false, 0, false);
        //from now on: there is solution
        char c[40];
        core2->getSolutionAt(c, 1);
        secretary2->initFromHalf(type, secretary3->puzzleCharTable, secretary3->originalPuzzle,
                                 c, secretary3->charTable, fill==0);
        core->init(type, secretary2->normalPuzzle);
        bool multi = core->solve(true, 123456789, true);
        dpuz->ui->lblContent->setText(multi? secretary->info1: secretary->info2);
        dpuz->ui->lblStep->setText(QString::number(core2->getNSteps()+core->getNSteps()));
    }
    dpuz->ui->lblPuzzle->setText(secretary2->show(type, secretary2->originalPuzzle, secretary2->charTable));
}
