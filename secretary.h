#ifndef SECRETARY_H
#define SECRETARY_H

#include <QString>
#include <QStringList>
#include <ctime>

class Secretary
{
//private:
public:
    char originalPuzzle[400];
    char normalPuzzle[400];
    QString charTable;
    QString puzzleCharTable;

    QString parseUnicode(const char *str);

    QString fullNumber;
    QString fullUpper;
    QString fullLower;
    QChar fullQuote;
    QChar fullSpace;
    QChar fullEqual;
    QString info1, info2;

    QString demo[2];
    int demot[2];
    static const char s_quote = '?', s_enter = '\n', s_uBase = 'A', s_lBase = 'a', s_dBase = '0';

    char autoPuzzle[400];
    char ntovTable[10];
    bool changeToVar[300];
    int nDigits;

    long long bigRand(){
        long long result = rand()%100;
        for(int i=0; i<4; i++){
            result *= 100;
            result += rand()%100;
        }
        return result;
    }

public:
    Secretary();
    int puzzleType;
    QString rawString;

    QString checkError(int type, QString str, bool halfMode);
    QString checkPuzzleError(int type, QString table, QString str);
    QString show(int type, QString str);
    QString show(int type, char* str, QString table);
    QString showDemo(int i);
    void initPuzzle(int type, QString str, bool skip);

    void autoPuzzleInit(int type, int size1, int size2);
    void initFromAuto(int type, char* src, char *table, bool* changes);
    void getRandomSizes(int type, int* p1, int* p2);

    void initFromHalf(int type, QString oldPuzzleCharTable, char* oldOriginal, char* solution, QString oldCharTable, bool fillNumber);

};

#endif // SECRETARY_H
