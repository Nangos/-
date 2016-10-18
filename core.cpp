#include "core.h"

Core::Core()
{
    solutions = new QVector<char>;
    srand((unsigned)time(0));
}

Core::~Core()
{
    delete solutions;
}

void Core::run(){

}

void Core::init(int type, char *str)
{
    //init puzzle in:
    stringCopy(str, puzzle);
    stringCopy(str, activePuzzle);
    puzzleType = type;

    //count variables:
    nVar = 0;
    nQuote = 0;
    char tempp = 0, tempq = 0;
    for(char *p = str; *p; p++){
        if( (*p)>='A' && (*p)<='J' ){
            if(*p > tempp){
                tempp = *p;
            }
        }else if( (*p)>='a' && (*p)<='z'){
            if(*p > tempq){
                tempq = *p;
            }
        }
    }
    nVar = tempp? tempp - 'A' + 1: 0;
    nQuote = tempq? tempq - 'a' + 1: 0;

    //register cursors for each line of the puzzle:
    cursor[0] = activePuzzle;
    int temp = 1;
    for(char *p = activePuzzle; *p; p++){
        if(*p != '\n'){
            continue;
        }
        p++;
        cursor[temp] = p;
        temp++;
    }
    cursor[temp] = NULL;
    nCursor = temp;

    //init the orderTable: (too long so that I write it in another function)
    char tempPuzzle[400];
    stringCopy(activePuzzle, tempPuzzle); //"stack" it to recover
    getOrder(activePuzzle);
    orderCursor = orderTable;
    stringCopy(tempPuzzle, activePuzzle);

    //init the randomTable:
    for(int i=0; i<nVar+nQuote; i++){
        //get a random string in which 0~9 each appears just once:
        char temp[] = "0123456789";
        for(int j=0; j<10; j++){
            int r = rand()%(10-j);
            randomTable[i][j] = temp[r];
            temp[r] = temp[9-j];
        }
        randomTable[i][10] = '\0';
        randomCursor[i] = randomTable[i];
    }

    //init stack:
    currLayer = -1;

    //init usedNumber:
    for(int i=0; i<10; i++){
        usedNumber[i] = false;
    }

    //init steps:
    nSteps = 0;

    //clear solutions:
    solutions->clear();

    return;
}


void Core::getScoreForAdd(int *scores, char *op1, char *op2, char *sum, int length, bool increasing){
    int carry[36];
    int count[36];
    int numCount;
    int carryNumCount;
    for(int i=0; i<nVar; i++){
        carry[i] = 2;
    }
    for(int i=0; i<nQuote; i++){
        carry[i+nVar] = 2;
    }
    char c[3];

    for(int index=length-1; index>=0; index--){
        if(increasing){
            c[0] = (index>=1 && index<=length-1)? op1[index-1]: '0';
            c[1] = (index>=0 && index<=length-2)? op2[index]: '0';
            c[2] = sum[index];
        }else{
            c[0] = op1[index];
            c[1] = op2[index];
            c[2] = sum[index];
        }
        numCount = 0;
        for(int i=0; i<nVar+nQuote; i++){
            count[i] = 0;
        }
        numCount += (c[0] >= '0' && c[0] <= '9');
        numCount += (c[1] >= '0' && c[1] <= '9');
        carryNumCount = numCount;
        numCount += (c[2] >= '0' && c[2] <= '9');
        for(int i=0; i<3; i++){
            if((c[i] >= 'A' && c[i] <= 'J') || (c[i] >= 'a' && c[i] <= 'z')){
                int offset = c[i] >= 'a'? c[i] - 'a' + nVar: c[i] - 'A';
                if(count[offset] + numCount >= 2){
                    scores[offset] += (carry[offset]==2)? 10:9;
                }else{
                    scores[offset] += 1;
                }
                if(i<2){
                    count[offset]++;
                }
            }
        }
        for(int i=0; i<nVar+nQuote; i++){
            carry[i] = count[i] + carryNumCount;
        }
    }
}

void Core::getScoreForAdd(int *scores, char **first_op, int op_num, char *sum, int length, bool increasing)
{
    int carry[36];
    int count[36];
    int numCount;
    int carryNumCount;
    for(int i=0; i<nVar; i++){
        carry[i] = op_num;
    }
    for(int i=0; i<nQuote; i++){
        carry[i+nVar] = op_num;
    }
    char c[10];

    for(int index=length-1; index>=0; index--){
        if(increasing){
            for(int i=0; i<op_num; i++){
                c[i] = (index>=op_num-i-1 && index<=length-i-1)? first_op[i][index-(op_num-i-1)]: '0';
            }
            c[op_num] = sum[index];
        }else{
            for(int i=0; i<op_num; i++){
                c[i] = first_op[i][index];
            }
            c[op_num] = sum[index];
        }
        numCount = 0;
        for(int i=0; i<nVar+nQuote; i++){
            count[i] = 0;
        }
        for(int i=0; i<op_num; i++){
            numCount += (c[i] >= '0' && c[i] <= '9');
        }
        carryNumCount = numCount;
        numCount += (c[op_num] >= '0' && c[op_num] <= '9');

        for(int i=0; i<=op_num; i++){
            if((c[i] >= 'A' && c[i] <= 'J') || (c[i] >= 'a' && c[i] <= 'z')){
                int offset = c[i] >= 'a'? c[i] - 'a' + nVar: c[i] - 'A';
                if(count[offset] + numCount >= op_num){
                    scores[offset] += (carry[offset]==0)? 10-(op_num - carry[offset]): 11-op_num;
                }else{
                    scores[offset] += 1;
                }
                if(i<op_num){
                    count[offset]++;
                }
            }
        }
        for(int i=0; i<nVar+nQuote; i++){
            carry[i] = count[i] + carryNumCount;
        }
    }
}

void Core::getScoreForMul(int *scores, char *op1, char *op2, char *prod, int length)
{
    int carry[36];
    int count[36];
    int carryNumCount;
    bool isProdNum;
    for(int i=0; i<nVar; i++){
        carry[i] = 2;
    }
    for(int i=0; i<nQuote; i++){
        carry[i+nVar] = 2;
    }
    char c[3];
    c[0] = *op2;

    for(int index=length-1; index>=0; index--){
        c[1] = op1[index];
        c[2] = prod[index];
        carryNumCount = 0;
        for(int i=0; i<nVar+nQuote; i++){
            count[i] = 0;
        }
        carryNumCount += (c[0] >= '0' && c[0] <= '9');
        carryNumCount += (c[1] >= '0' && c[1] <= '9');
        isProdNum = (c[2] >= '0' && c[2] <= '9')? true: false;

        for(int i=0; i<3; i++){
            if((c[i] >= 'A' && c[i] <= 'J') || (c[i] >= 'a' && c[i] <= 'z')){
                int offset = c[i] >= 'a'? c[i] - 'a' + nVar: c[i] - 'A';

                if(carry[offset] >= 2){
                    if((i==0&&index>0) || i==1){
                        scores[offset] += isProdNum? ((carryNumCount+count[offset]>0)? 10: 5): 1;
                    }else if(i==2){
                        scores[offset] += (carryNumCount+count[offset]>1)? 10: (carryNumCount+count[offset]>0)? 5: 1;
                    }
                }else{
                    if((i==0&&index>0) || i==1){
                        scores[offset] += (isProdNum && carryNumCount+count[offset]>0)? 5: 1;
                    }else if(i==2){
                        scores[offset] += (carryNumCount+count[offset]>1)? 5: 1;
                    }
                }
                if(i<2){
                    count[offset]++;
                }
            }
        }
        for(int i=0; i<nVar+nQuote; i++){
            carry[i] = count[i] + carryNumCount;
        }
    }
}

void Core::getOrder(char *str)
{
    int scores[36];
    int size;

    for(int ii=0; ii<nVar+nQuote; ii++){
        for(int j=0; j<36; j++){
            scores[j] = 0;
        }
        switch(puzzleType){
        case 0: //Add
            size = 0;
            for(char *p = cursor[0]; *p != '\n'; p++){
                size++;
            }
            getScoreForAdd(scores, cursor, nCursor-1, cursor[nCursor-1], size, false);
            break;
        case 1: //Sub
            size = 0;
            for(char *p = cursor[0]; *p != '\n'; p++){
                size++;
            }
            getScoreForAdd(scores, cursor+1, nCursor-1, cursor[0], size, false);
            break;
        case 2: //Mul
            size = 0;
            for(char *p = cursor[0]; *p != '\n'; p++){
                size++;
            }
            if(nCursor == 3){
                getScoreForMul(scores, cursor[0], cursor[1], cursor[2], size);
            }else{
                for(int i=0; i<nCursor-3; i++){
                    getScoreForMul(scores, cursor[0], cursor[1]+(nCursor-4-i), cursor[2+i], size);
                }
                getScoreForAdd(scores, cursor+2, nCursor-3, cursor[nCursor-1], size+nCursor-4, true);
            }
            break;
        case 3: //Div
            size = 0;
            for(char *p = cursor[1]; *p != '\n'; p++){
                size++;
            }
            for(int i=0; i<(nCursor-3)/2; i++){
                getScoreForMul(scores, cursor[1], cursor[2]+i, cursor[2*i+3], size);
                if(i<(nCursor-3)/2-1){
                    char temp[13];
                    int target = (i==0)? 0: 2*i+2;
                    stringCopy(cursor[target], temp, size);
                    temp[size] = cursor[0][size+i];
                    temp[size+1] = '\n';
                    getScoreForAdd(scores, cursor[2*i+4], cursor[2*i+3], temp, size+1, true);
                }else{
                    getScoreForAdd(scores, cursor[2*i+4], cursor[2*i+3], cursor[i==0? 0: 2*i+2], size, false);
                }
            }
            break;
        default:
            break;
        }

        // now sort:
        int hi=0, hiScore=0;
        for(int i=0; i<nVar+nQuote; i++){
            if(scores[i] > hiScore){
                hi = i;
                hiScore = scores[i];
            }
        }
        if(hi<nVar){
            orderTable[ii] = hi+'A';
            stringReplace(str, hi+'A', '0');
        }else{
            orderTable[ii] = hi-nVar+'a';
            stringReplace(str, hi-nVar+'a', '0');
        }
    }
    orderTable[nVar+nQuote] = '\0';
    //by the way
    puzzleSize = size;
}

bool Core::solve(bool every, long long stepLimit=0, bool testOnly=false)
{
    bool found = tryOneStep();
    if(stepLimit){
        while(nSteps < stepLimit && currLayer >= 0){
            if(testOnly && getNSolutions()>1){
                return false;
            }
            if(found && (!every)){
                break;
            }
            found = tryOneStep();
        }
        if(nSteps == stepLimit){
            return false;
        }
    }else{
        while(currLayer >= 0){
            if(found && (!every)){
                break;
            }
            found = tryOneStep();
        }
    }
    return true;
}

bool Core::tryOneStep()
{
    bool result = true;
    bool toReturn;

    //step 1: check current
    switch (puzzleType) {
    case 0: //add
        result = isAddPossible(cursor, nCursor-1, cursor[nCursor-1], puzzleSize, false);
        break;
    case 1: //sub
        result = isAddPossible(cursor+1, nCursor-1, cursor[0], puzzleSize, false);
        break;
    case 2: //mul
        if(nCursor == 3){
            result = isMulPossible(cursor[0], cursor[1], cursor[2], puzzleSize);
        }else{
            for(int i=0; i<nCursor-3; i++){
                if(result){
                    result &= isMulPossible(cursor[0], cursor[1]+(nCursor-4-i), cursor[2+i], puzzleSize);
                }
            }
            if(result){
                result &= isAddPossible(cursor+2, nCursor-3, cursor[nCursor-1], puzzleSize+nCursor-4, true);
            }
        }
        break;
    case 3: //Div
        result &= isRemainPossible(cursor[1], cursor[nCursor-1], puzzleSize);
        for(int i=0; i<(nCursor-3)/2; i++){
            if(!result){
                break;
            }
            result &= isMulPossible(cursor[1], cursor[2]+i, cursor[2*i+3], puzzleSize);
            if(!result){
                break;
            }
            if(i<(nCursor-3)/2-1){
                char temp[13];
                int target = (i==0)? 0: 2*i+2;
                stringCopy(cursor[target], temp, puzzleSize);
                temp[puzzleSize] = cursor[0][puzzleSize+i];
                temp[puzzleSize+1] = '\n';
                result &= isAddPossible(cursor[2*i+4], cursor[2*i+3], temp, puzzleSize+1, true);
            }else{
                result &= isAddPossible(cursor[2*i+4], cursor[2*i+3], cursor[i==0? 0: 2*i+2], puzzleSize, false);
            }
        }
        break;
    default:
        break;
    }
    if(result && (currLayer == nVar+nQuote-1)){ //solution found
        for(int i=0; i<nVar+nQuote; i++){
            solutions->append(*(randomCursor[i]));
        }
        toReturn = true;
    }else{
        toReturn = false;
    }

    //step 2: go to next
    bool f = false;
    if(result && (currLayer < nVar+nQuote-1)){
        if(currLayer>=0){
            if(!isCurrentQuote()){
                usedNumber[*(randomCursor[currLayer])-'0'] = true;
            }
            stringCopy(activePuzzle, stack[currLayer]);
        }
        currLayer++;
        randomCursor[currLayer] = randomTable[currLayer];
        if(!isCurrentQuote()){
            while(usedNumber[*(randomCursor[currLayer])-'0']){
                (randomCursor[currLayer])++;
            }
        }
    }else{
        while(currLayer>=0){
            (randomCursor[currLayer])++;
            if(!isCurrentQuote()){
                while(*(randomCursor[currLayer])){
                    if(!usedNumber[*(randomCursor[currLayer])-'0']){
                        f = true;
                        break;
                    }
                    (randomCursor[currLayer])++;
                }
            }else{
                if(*(randomCursor[currLayer])){
                    f = true;
                }
            }
            if(f){
                break;
            }
            if(!(*(randomCursor[currLayer]))){
                currLayer--;
                if(currLayer>=0){
                    if(!isCurrentQuote()){
                        usedNumber[*(randomCursor[currLayer])-'0'] = false;
                    }
                }
            }
        }
    }
    if(currLayer>=0){
        if(currLayer>0){
            stringReplaceCopy(stack[currLayer-1], activePuzzle, orderTable[currLayer], *(randomCursor[currLayer]));
        }else{
            stringReplaceCopy(puzzle, activePuzzle, orderTable[currLayer], *(randomCursor[currLayer]));
        }
    }

    nSteps++;
    return toReturn;
}

bool Core::isAddPossible(char *op1, char *op2, char *sum, int length, bool increasing)
{
    long long add1=0, add2=0;
    long long summ=0;
    long long range=1;
    long long top, bottom;
    int index = 0;
    char a1, a2, s;
    for(index=0; index<length+1; index++){
        if(increasing){
            a1 = (index>=1 && index<=length-1)? op1[index-1]: '0';
            a2 = (index>=0 && index<=length-2)? op2[index]: '0';
        }else{
            a1 = op1[index];
            a2 = op2[index];
        }
        s = sum[index];

        if(a1>='0' && a1<='9' && a2>='0' && a2<='9' && s>='0' && s<='9'){
            add1 = add1*10+(a1-'0');
            add2 = add2*10+(a2-'0');
            summ = summ*10+(s-'0');
            range *= 10;
        }else{
            if(index==length){
                if((add1 + add2 - summ)%range == 0){
                    return true;
                }else{
                    return false;
                }
            }else{
                if(range==1){
                    continue;
                }
                top = (add1+add2+1)%range;
                bottom = (add1+add2)%range;
                if((bottom <= summ) + (summ <= top) + (bottom > top) == 2){
                    add1 = 0;
                    add2 = 0;
                    summ = 0;
                    range = 1;
                }else{
                    return false;
                }
            }
        }
    }
    return true;
}

bool Core::isAddPossible(char **first_op, int op_num, char *sum, int length, bool increasing)
{
    long long sumt=0;
    long long summ=0;
    long long range=1;
    long long top, bottom;
    int index = 0;
    bool flag;
    char a[9], s;
    for(index=0; index<length+1; index++){
        flag = true;
        if(increasing){
            for(int i=0; i<op_num; i++){
                a[i] = (index>=op_num-i-1 && index<=length-i-1)? first_op[i][index-(op_num-i-1)]: '0';
                if( a[i]<'0' || a[i]>'9' ){
                    flag = false;
                    break;
                }
            }
        }else{
            for(int i=0; i<op_num; i++){
                a[i] = first_op[i][index];
                if( a[i]<'0' || a[i]>'9' ){
                    flag = false;
                    break;
                }
            }
        }
        if(flag){
            s = sum[index];
            if( s<'0' || s>'9' ){
                flag = false;
            }
        }

        if(flag){
            sumt *= 10;
            for(int i=0; i<op_num; i++){
                sumt += (a[i]-'0');
            }
            summ = summ*10+(s-'0');
            range *= 10;
        }else{
            if(index==length){
                if((sumt - summ)%range == 0){
                    return true;
                }else{
                    return false;
                }
            }else{
                if(range==1){
                    continue;
                }
                top = (sumt+op_num-1)%range;
                bottom = sumt%range;
                if((bottom <= summ) + (summ <= top) + (bottom > top) == 2){//quick version of Y=A'BC+AB'C+ABC'
                    sumt = 0;
                    summ = 0;
                    range = 1;
                }else{
                    return false;
                }
            }
        }
    }
    return true;
}

bool Core::isMulPossible(char *op1, char *op2, char *prod, int length)
{
    long long a = 0;
    int b = 0;
    long long p = 0;
    long long range = 1;
    long long top, bottom;

    char c1, c2;
    if(*op2 < '0' || *op2 > '9'){
        return true;
    }
    b = *op2 - '0';

    for(int index=0; index<length+1; index++){
        c1 = op1[index];
        c2 = prod[index];
        if(c1>='0' && c1<='9' && c2>='0' && c2<='9'){
            a = a*10 + b*(c1-'0');
            p = p*10 + (c2-'0');
            range *= 10;
        }else{
            if(index==length){
                if((a-p)%range == 0){
                    return true;
                }else{
                    return false;
                }
            }else{
                if(range==1){
                    continue;
                }
                top = (a+b-(b!=0))%range;
                bottom = a%range;
                if((bottom <= p) + (p <= top) + (bottom > top) == 2){//quick version of Y=A'BC+AB'C+ABC'
                    a = 0;
                    p = 0;
                    range = 1;
                }else{
                    return false;
                }
            }
        }
    }
    return true;
}

bool Core::isRemainPossible(char *dividor, char *remain, int length)
{
    char c1, c2;
    for(int i=0; i<length; i++){
        c1 = dividor[i];
        if(c1<'0' || c1>'9'){
            return true;
        }
        c2 = remain[i];
        if(c2<'0' || c2>'9'){
            return true;
        }
        if(c2>c1){
            return false;
        }else if(c2<c1){
            return true;
        }
    }
    return false;
}

void Core:: getSolutionAt(char* c, int index){
    for(int i=0; i<10; i++){
        c[i] = '?'; //De-'\0'-ize
    }
    for(int i=0; i<nVar+nQuote; i++){
        if( orderTable[i]>='A' && orderTable[i]<='J' ){
            c[orderTable[i]-'A'] = solutions->at((index-1)*(nVar+nQuote)+i);
        }else if( orderTable[i]>='a' && orderTable[i]<='z' ){
            c[orderTable[i]-'a'+10] = solutions->at((index-1)*(nVar+nQuote)+i);
        }
    }
    c[10+nQuote] = '\0';
}
