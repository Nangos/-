#ifndef CORE_H
#define CORE_H
#include <QThread>
#include <ctime>
#include <QVector>

class Core : public QThread
{
public://
//private:
    char puzzle[400];
    char activePuzzle[400];
    int puzzleType;
    int nVar;
    int nQuote;
    char *(cursor[25]);
    int nCursor;

    char orderTable[37];
    char randomTable[36][11];
    char *(randomCursor[36]);
    char *orderCursor;
    QVector<char> *solutions;
    int puzzleSize;

    char stack[37][400];
    int currLayer;
    bool usedNumber[10];

    bool isAddPossible(char *op1, char *op2, char *sum, int length, bool increasing);
    bool isAddPossible(char **first_op, int op_num, char *sum, int length, bool increasing);
    bool isMulPossible(char *op1, char *op2, char *prod, int length);
    bool isRemainPossible(char *dividor, char *remain, int length);

    bool tryOneStep();
    long long nSteps;

    void getScoreForAdd(int* scores, char *op1, char *op2, char *sum, int length, bool increasing);
    void getScoreForAdd(int* scores, char **first_op, int op_num, char *sum, int length, bool increasing);
    void getScoreForMul(int* scores, char *op1, char *op2, char *prod, int length);
    void getOrder(char *str);

    void inline stringCopy(char *from, char *to){
        char *p = from, *q = to;
        while(*p){
            *q = *p;
            p++;
            q++;
        }
        *q = 0;
    }
    void inline stringCopy(char *from, char *to, int n){
        char *p = from, *q = to;
        for(int i=0; i<n; i++){
            *q = *p;
            p++;
            q++;
        }
        *q = 0;
    }

    void inline stringReplace(char *str, char before, char after){
        char *p = str;
        while(*p){
            if(*p == before){
                *p = after;
            }
            p++;
        }
    }

    void inline stringReplaceCopy(char *from, char *to, char before, char after){
        char *p = from, *q = to;
        while(*p){
            if(*p == before){
                *q = after;
            }else{
                *q = *p;
            }
            p++;
            q++;
        }
        *q = 0;
    }
    bool inline isCurrentQuote(){
        return orderTable[currLayer] >= 'a';
    }

public:
    Core();
    ~Core();
    void init(int type, char *str);
    void run();
    bool solve(bool every, long long stepLimit, bool testOnly);
    long long getNSteps(){
        return nSteps;
    }

    //void design();
    void getSolutionAt(char* c, int index);
    int getNSolutions(){
        return solutions->size() / (nVar+nQuote);
    }
};

#endif // CORE_H
