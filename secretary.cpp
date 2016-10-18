#include "secretary.h"
#include <QTextCodec>

//QTextCodec的使用模板：
//QTextCodec *codec = QTextCodec::codecForName("GB18030");
//QString str = codec->toUnicode("中文");

QString Secretary::parseUnicode(const char *str){
    QTextCodec *codec = QTextCodec::codecForName("GB18030");
    return codec->toUnicode(str);
}

Secretary::Secretary(){
    puzzleType = -1;
    srand((unsigned)time(0));
    info1 = parseUnicode("该谜题有唯一解。");
    info2 = parseUnicode("该谜题有多个解。");
    //加载全角字符表
    fullNumber = parseUnicode("０１２３４５６７８９？　＝");
    fullUpper = parseUnicode("ＡＢＣＤＥＦＧＨＩＪＫＬＭＮＯＰＱＲＳＴＵＶＷＸＹＺ");
    fullLower = parseUnicode("ａｂｃｄｅｆｇｈｉｊｋｌｍｎｏｐｑｒｓｔｕｖｗｘｙｚ");
    fullQuote = fullNumber.at(10);
    fullSpace = fullNumber.at(11);
    fullEqual = fullNumber.at(12);
    //加载demo
    demo[0] = parseUnicode("兵炮马卒\n兵炮车卒\n车卒马兵卒");
    demot[0] = 0;
    demo[1] = parseUnicode("ＡＢＣ\nＤＣ\nＤＥＡＣ\n７ＥＤ\nＦＤＢＣ");
    demot[1] = 2;
}

QString Secretary::showDemo(int i)
{
    rawString = demo[i];
    puzzleType = demot[i];
    return show(demot[i], demo[i]);
}

QString Secretary::checkError(int type, QString str, bool halfMode = false)
{
    //首先进行general检查

    //字串是否空：
    if(str.size() == 0){
        return parseUnicode("还没有输入竖式！");
    }

    //变量数是否符合要求，是否有非法字符：
    QString tempTable = "";
    int quoteCount = 0;
    for(int i=0; i<str.size(); i++)
    {
        QChar c = str.at(i);
        if(c == fullSpace
                || (!(c >= 0x4E00 && c <= 0x9FA5)
                    && !(c.isDigit() || c.isLetter() || c.toLatin1() == s_enter))
                ){
            if(halfMode){
                if(c != s_quote && c!= fullQuote){
                    return parseUnicode("输入中含非法字符。\n\n合法字符包括字母、数字、正常汉字、回车，\n以及问号。");
                }
            }else{
                return parseUnicode("输入中含非法字符。\n\n合法字符包括字母、数字、正常汉字和回车。");
            }
        }
        if(c.toLatin1() != 0){
            if(c.isUpper()){
                c = fullUpper.at( c.toLatin1() - s_uBase);
            }else if(c.isLower()){
                c = fullLower.at( c.toLatin1() - s_lBase);
            }else if(c.isNumber()){
                c = fullNumber.at( c.toLatin1() - s_dBase);
            }else if(c == QChar(s_quote)){
                c = fullQuote;
            }
        }
        if(c == fullQuote){
            quoteCount++;
        }else if(!(tempTable.contains(c))){
            if(halfMode){
                if(!(puzzleCharTable.contains(c) || c.toLatin1() == s_enter || c.isDigit())){
                    return parseUnicode("输入了变量列表中不存在的变量。");
                }
            }
            if(!(fullNumber.contains(c) || c.toLatin1() == s_enter)){
                tempTable.append(c);
            }
        }
    }
    if(halfMode){
        if(quoteCount == 0){
            return parseUnicode("竖式中不含问号！");
        }
        if(tempTable.size() > 10){
            return parseUnicode("竖式中含未知变量超过10个。");
        }
        if(quoteCount > 26){
            return parseUnicode("问号数超过限制。\n\n最多支持26个问号。");
        }
    }else{
        if(tempTable.size() == 0){
            return parseUnicode("竖式中不含未知变量！");
        }
        if(tempTable.size() > 10){
            return parseUnicode("竖式中含未知变量超过10个。");
        }
    }

    //分行：
    QStringList list = str.split(s_enter);

    //进行分种类格式检查：
    int temp = 0;
    int temp1 = 0;
    int temp2 = 0;
    int temp3 = 0;
    switch(type){

    case 0: //加法
        if(list.size() < 3){
            return parseUnicode("竖式行数过少。\n\n加法竖式应至少含有3行。");
        }else if(list.size() > 10){
            return parseUnicode("竖式行数过多。\n\n加法竖式至多含有10行。");
        }
        for(int i=0; i<list.size(); i++){
            if(list.at(i).size() == 0){
                return parseUnicode("竖式中不能含有空行。");
            }
            if(i == list.size()-1){
                break;
            }
            if(list.at(i).size() > 9){
                return parseUnicode("加数过大。\n\n不支持10位及以上的加数。");
            }
            if(temp < list.at(i).size()){
                temp = list.at(i).size();
            }
        }
        temp1 = list.at(list.size()-1).size();
        if(temp1 < temp || temp1 > temp+1){
            return parseUnicode("和的位数不符合要求。\n\n和的位数应与最大加数的位数一致，\n或多一位。");
        }
        break;

    case 1: //减法
        if(list.size() < 3){
            return parseUnicode("竖式行数过少。\n\n减法竖式应至少含有3行。");
        }else if(list.size() > 10){
            return parseUnicode("竖式行数过多。\n\n减法竖式至多含有10行。");
        }
        temp1 = list.at(0).size();
        temp = 0;
        for(int i=0; i<list.size(); i++){
            if(list.at(i).size() == 0){
                return parseUnicode("竖式中不能含有空行。");
            }
            if(i == 0){
                continue;
            }
            if(list.at(i).size() > 9){
                return parseUnicode("减数或差过大。\n\n不支持10位及以上的数。");
            }
            if(temp < list.at(i).size()){
                temp = list.at(i).size();
            }
        }
        if(temp1 < temp || temp1 > temp+1){
            return parseUnicode("被减数位数不符合要求。\n\n被减数位数应与其他数的位数最大值一致，\n或多一位。");
        }
        break;

    case 2: //乘法
        if(list.size() < 3){
            return parseUnicode("竖式行数过少。\n\n乘法竖式应至少含有3行。");
        }else if(list.size() > 12){
            return parseUnicode("竖式行数过多。\n\n乘法竖式至多含有12行。");
        }
        for(int i=0; i<list.size(); i++){
            if(list.at(i).size() == 0){
                return parseUnicode("竖式中不能含有空行。");
            }
        }
        temp = list.at(0).size();
        temp1 = list.at(1).size();
        if(temp > 9 || temp1 > 9){
            return parseUnicode("乘数位数超过限制。\n\n不支持10位及以上的乘数。");
        }
        if(list.size() == 3){
            if(temp1 != 1){
                return parseUnicode("竖式总行数与乘数位数不符。");
            }
            temp2 = list.at(2).size();
            if(temp2 < temp || temp2 > temp+1){
                return parseUnicode("积的位数不合理。");
            }
        }else{
            if(temp1 != list.size()-3 || list.size() == 4){
                return parseUnicode("竖式总行数与乘数位数不符。");
            }
            for(int i=2; i<list.size()-1; i++){
                temp2 = list.at(i).size();
                if(temp2 < temp || temp2 > temp+1){
                    return parseUnicode("部分积的位数不合理。");
                }
            }
            temp2 = list.at(list.size()-1).size();
            if(temp2 < (temp+temp1-1) || temp2 > (temp+temp1)){
                return parseUnicode("积的位数不合理。");
            }
        }
        break;

    case 3: //除法：
        if(list.size() < 5){
            return parseUnicode("竖式行数过少。\n\n除法竖式应至少含有5行。");
        }else if(list.size() > 21){
            return parseUnicode("竖式行数过多。\n\n除法竖式至多含有21行。");
        }
        for(int i=0; i<list.size(); i++){
            if(list.at(i).size() == 0){
                return parseUnicode("竖式中不能含有空行。");
            }
        }
        temp = list.at(0).size();
        temp1 = list.at(1).size();
        temp2 = list.at(2).size();
        temp3 = list.at(3).size();
        if(list.size() != temp2*2+3){
            return parseUnicode("竖式总行数与商的位数不匹配。");
        }
        if(temp > 9){
            return parseUnicode("被除数位数超过限制。\n\n不支持10位及以上的被除数。");
        }
        if(temp1 > temp){
            return parseUnicode("除数位数不能大于被除数位数。");
        }
        if(temp2 < temp - temp1 || temp2 > temp - temp1 + 1){
            return parseUnicode("商的位数不合理。");
        }
        if(temp2 == temp - temp1){
            if(temp3 != temp1+1){
                return parseUnicode("第4行的位数不合理。\n\n若检查无误，\n尝试将某些特殊位置用0补齐。");
            }
        }else{
            if(temp3 != temp1){
                return parseUnicode("第4行的位数不合理。\n\n若检查无误，\n尝试将某些特殊位置用0补齐。");
            }
        }
        for(int i=4; i<list.size()-1; i++){
            temp3 = list.at(i).size();
            if(temp3 != temp1+1){
                return parseUnicode("某一行中间结果的位数不合理。\n\n若检查无误，\n尝试将某些特殊位置用0补齐。");
            }
        }
        temp3 = list.at(list.size()-1).size();
        if(temp3 != temp1){
            return parseUnicode("余数的位数不合理。\n\n余数位数规定必须与除数位数一致，\n\n请用0补齐");
        }
        break;
    default:
        break;
    }

    //没有发现错误
    rawString = str;
    puzzleType = type;
    return parseUnicode("");
}

QString Secretary::show(int type, QString str)
{
    QString special = parseUnicode("＋－×　─┌│╯");
    //全角替换：
    for(int i=0; i<str.size(); i++){
        QChar c = str.at(i);
        if(c.toLatin1() != 0){
            if(c.isUpper()){
                c = fullUpper.at( c.toLatin1() - s_uBase);
            }else if(c.isLower()){
                c = fullLower.at( c.toLatin1() - s_lBase);
            }else if(c.isNumber()){
                c = fullNumber.at( c.toLatin1() - s_dBase);
            }else if(c.toLatin1() == s_quote){
                c = fullQuote;
            }
        }
        str.replace(i,1,c);
    }
    //分行：
    QStringList list = str.split(s_enter);
    int t1, t2;
    QString result = "";

    int width;
    //分式子类型处理：
    switch(type){
    case 0: //加法
        t1 = 0;
        for(int i=0; i<list.size()-1; i++){
            t2 = list.at(i).size();
            if(t1<t2){
                t1 = t2;
            }
        }
        width = t1+1;
        for(int i=0; i<list.size(); i++){
            t1 = list.at(i).size();
            if(i==0){
                for(int j=0; j<width-t1; j++){
                    result.append(special.at(3)); //空格
                }
                result.append(list.at(i));
                result.append(s_enter);
            }else if(i==list.size()-1){
                for(int j=0; j<width; j++){
                    result.append(special.at(4)); //横线
                }
                result.append(s_enter);
                for(int j=0; j<width-t1; j++){
                    result.append(special.at(3)); //空格
                }
                result.append(list.at(i));
            }else{
                result.append(special.at(0)); //加号
                for(int j=0; j<width-t1-1; j++){
                    result.append(special.at(3)); //空格
                }
                result.append(list.at(i));
                result.append(s_enter);
            }
        }
        break;

    case 1: //减法
        t1 = 0;
        for(int i=1; i<list.size(); i++){
            t2 = list.at(i).size();
            if(t1<t2){
                t1 = t2;
            }
        }
        width = t1+1;
        for(int i=0; i<list.size(); i++){
            t1 = list.at(i).size();
            if(i==0){
                for(int j=0; j<width-t1; j++){
                    result.append(special.at(3)); //空格
                }
                result.append(list.at(i));
                result.append(s_enter);
            }else if(i==list.size()-1){
                for(int j=0; j<width; j++){
                    result.append(special.at(4)); //横线
                }
                result.append(s_enter);
                for(int j=0; j<width-t1; j++){
                    result.append(special.at(3)); //空格
                }
                result.append(list.at(i));
            }else{
                result.append(special.at(1)); //减号
                for(int j=0; j<width-t1-1; j++){
                    result.append(special.at(3)); //空格
                }
                result.append(list.at(i));
                result.append(s_enter);
            }
        }
        break;

    case 2: //乘法
        width = list.at(0).size() + list.at(1).size();
        //top:
        for(int j=0; j<width-list.at(0).size(); j++){
            result.append(special.at(3)); //空格
        }
        result.append(list.at(0));
        result.append(s_enter);
        result.append(special.at(2)); //乘号
        for(int j=0; j<width-list.at(1).size()-1; j++){
            result.append(special.at(3)); //空格
        }
        result.append(list.at(1));
        result.append(s_enter);
        for(int j=0; j<width; j++){
            result.append(special.at(4)); //横线
        }
        result.append(s_enter);
        //middle:
        if(list.size() > 3){
            for(int i=2; i<list.size()-1; i++){
                t1 = list.at(i).size();
                for(int j=0; j<width-t1-(i-2); j++){
                    result.append(special.at(3)); //空格
                }
                result.append(list.at(i));
                for(int j=0; j<i-2; j++){
                    result.append(special.at(3)); //空格
                }
                result.append(s_enter);
            }
            for(int j=0; j<width; j++){
                result.append(special.at(4)); //横线
            }
            result.append(s_enter);
        }
        //bottom:
        t1 = list.at(list.size()-1).size();
        for(int j=0; j<width-t1; j++){
            result.append(special.at(3)); //空格
        }
        result.append(list.at(list.size()-1));
        break;

    case 3://除法：
        t1 = list.at(0).size(); //被除数长度
        t2 = list.at(1).size(); //除数长度
        width = t1+t2+1;
        //line 0:
        for(int j=0; j<width-list.at(2).size(); j++){
            result.append(special.at(3)); //空格
        }
        result.append(list.at(2));
        result.append(s_enter);
        //line 1:
        for(int j=0; j<t2; j++){
            result.append(special.at(3)); //空格
        }
        result.append(special.at(5)); //制表符
        for(int j=0; j<t1; j++){
            result.append(special.at(4)); //横线
        }
        result.append(s_enter);
        //line 2:
        result.append(list.at(1));
        result.append(special.at(6)); //制表符
        result.append(list.at(0));
        result.append(s_enter);
        //line 3:
        for(int j=0; j<t2; j++){
            result.append(special.at(3)); //空格
        }
        result.append(special.at(7)); //制表符
        result.append(list.at(3));
        for(int j=0; j<t1-list.at(3).size(); j++){
            result.append(special.at(3)); //空格
        }
        result.append(s_enter);
        //following lines:
        for(int i=4; i<list.size(); i++){
            if(i%2==0){
                for(int j=0; j<t2+1; j++){
                    result.append(special.at(3)); //空格
                }
                for(int j=0; j<t1; j++){
                    result.append(special.at(4)); //横线
                }
                result.append(s_enter);
            }
            if(i==list.size()-1){
                break;
            }
            int val = (list.at(2).size() == t1 - t2);
            for(int j=0; j<t2+i/2-1+val; j++){
                result.append(special.at(3)); //空格
            }
            result.append(list.at(i));
            for(int j=0; j<t1-t2+1-i/2-val; j++){
                result.append(special.at(3)); //空格
            }
            result.append(s_enter);
        }
        //last line:
        for(int j=0; j<t1+1; j++){
            result.append(special.at(3)); //空格
        }
        result.append(list.at(list.size()-1));
        break;

    default:
        break;
    }
    return result;
}

void Secretary::initPuzzle(int type, QString str, bool skip=false)
{
    puzzleType = type;
    if(!skip){
        charTable = "";
        int len = str.size();

        //全角替换：
        for(int i=0; i<len; i++){
            QChar c = str.at(i);
            if(c.toLatin1() != 0){
                if(c.isUpper()){
                    c = fullUpper.at( c.toLatin1() - s_uBase);
                }else if(c.isLower()){
                    c = fullLower.at( c.toLatin1() - s_lBase);
                }else if(c.isNumber()){
                    c = fullNumber.at( c.toLatin1() - s_dBase);
                }else if(c.toLatin1() == s_quote){
                    c = fullQuote;
                }
            }
            str.replace(i,1,c);
        }

        //生成变量表：
        for(int i=0; i<len; i++){
            QChar c = str.at(i);
            if(!(charTable.contains(c))){
                if(!(fullNumber.contains(c) || c.toLatin1() == s_enter)){
                    charTable.append(c);
                }
            }
        }

        //用大写字母A~J借助变量表换掉所有的变量：
        for(int i=0; i<charTable.size(); i++){
            QChar c1 = charTable.at(i);
            QChar c2 = QChar(s_uBase + i);
            str.replace(c1, c2);
        }

        //用半角数字换掉全角：
        for(int i=0; i<10; i++){
            QChar c1 = fullNumber.at(i);
            QChar c2 = QChar(s_dBase + i);
            str.replace(c1, c2);
        }

        //依次用小写字母a~z换掉所有的问号：
        int tempIndex = 0;
        for(int i=0; i<str.size(); i++){
            if(str.at(i) != fullQuote){
                continue;
            }
            str.replace(i, 1, QChar(s_lBase + tempIndex));
            tempIndex++;
        }

        //最终得到只含字母A~J，a~z，数字0~9和回车的char字符串
        for(int i=0; i<str.size(); i++){
            QChar c = str.at(i);
            originalPuzzle[i] = c.toLatin1();
        }
        originalPuzzle[str.size()] = 0; //传统终结符
    }

    //将合适位置补0以方便统一处理
    QStringList list = str.split(s_enter);
    int tmp1, tmp2;
    QString newString = QString();
    switch(type){
    case 0: //加法。全部补至比最长加数长1位
        tmp1 = 0;
        for(int i=0; i<list.size()-1; i++){
            tmp2 = list.at(i).size();
            if(tmp2 > tmp1){
                tmp1 = tmp2;
            }
        }
        for(int i=0; i<list.size(); i++){
            QString t = QString();
            for(int j=0; j<tmp1+1-list.at(i).size(); j++){
                t.append(QChar(s_dBase));
            }
            t.append(list.at(i));
            newString.append(t);
            if(i < list.size()-1){
                newString.append(QChar(s_enter));
            }
        }
        break;

    case 1: //减法。全部补至比最长减数/差长1位
        tmp1 = 0;
        for(int i=1; i<list.size(); i++){
            tmp2 = list.at(i).size();
            if(tmp2 > tmp1){
                tmp1 = tmp2;
            }
        }
        for(int i=0; i<list.size(); i++){
            QString t = QString();
            for(int j=0; j<tmp1+1-list.at(i).size(); j++){
                t.append(QChar(s_dBase));
            }
            t.append(list.at(i));
            newString.append(t);
            if(i < list.size()-1){
                newString.append(QChar(s_enter));
            }
        }
        break;

    case 2: //乘法。被乘数补1个0，所有部分积也补到那么长，积补到两个乘数的总长
        tmp1 = list.at(0).size();
        tmp2 = list.at(1).size();
        for(int i=0; i<list.size(); i++){
            QString t = QString();

            if(i==0){ //第1行
                t.append(QChar(s_dBase));
                t.append(list.at(i));
            }else if(i==1){ //第2行
                t.append(list.at(i));
            }else if(i!=list.size()-1){ //部分积
                for(int j=0; j<tmp1+1-list.at(i).size(); j++){
                    t.append(QChar(s_dBase));
                }
                t.append(list.at(i));
            }else{ //积
                for(int j=0; j<tmp1+tmp2-list.at(i).size(); j++){
                    t.append(QChar(s_dBase));
                }
                t.append(list.at(i));
            }

            newString.append(t);
            if(i < list.size()-1){
                newString.append(QChar(s_enter));
            }
        }
        break;

    case 3: //除法。不补或被除数与第4行各补1个0即可。除数补一个0，余数补一个0
        if(list.at(0).size() < list.at(1).size() + list.at(2).size()){ //补4个0
            for(int i=0; i<list.size(); i++){
                QString t = QString();
                if(i==0 || i==3 || i==1 || i==list.size()-1){
                    t.append(QChar(s_dBase));
                }
                t.append(list.at(i));
                newString.append(t);
                if(i < list.size()-1){
                    newString.append(QChar(s_enter));
                }
            }
        }else{ //补2个0
            for(int i=0; i<list.size(); i++){
                QString t = QString();
                if(i==1 || i==list.size()-1){
                    t.append(QChar(s_dBase));
                }
                t.append(list.at(i));
                newString.append(t);
                if(i < list.size()-1){
                    newString.append(QChar(s_enter));
                }
            }
        }
        break;
    default:
        break;
    }

    for(int i=0; i<newString.size(); i++){
        QChar c = newString.at(i);
        normalPuzzle[i] = c.toLatin1();
    }
    normalPuzzle[newString.size()] = 0;

    return; //强迫症专用行
}

QString Secretary::show(int type, char *str, QString table){
    QString s = QString(str);
    for(int i=0; i<table.size(); i++){
        if(i<10){
            s.replace(QChar('A'+i), table.at(i));
        }else{
            s.replace(QChar('a'+i-10), table.at(i));
        }
    }
    return show(type, s);
}

void Secretary::autoPuzzleInit(int type, int size1, int size2)
{
    //先生成一串随机数字作为题目
    long long tempNum1, tempNum2, tempNum3, tempNum4, tempNum5, tempRange;
    int cursor = 0; //用于字符串指针
    switch(type){
    case 0: //加法
        tempRange = 1;
        for(int i=0; i<size1; i++){
            tempRange *= 10;
        }
        tempNum1 = 0;
        for(int i=0; i<size2; i++){
            tempNum2 = bigRand()%(tempRange-1)+1; //让每一个加数都不为0
            tempNum1 += tempNum2;
            cursor += size1;
            for(int j=0; j<size1; j++){
                cursor--;
                char c = (tempNum2%10) + s_dBase;
                tempNum2 /= 10;
                autoPuzzle[cursor] = c;
            }
            cursor += size1;
            autoPuzzle[cursor] = s_enter;
            cursor++;
        }
        cursor += size1+1;
        for(int j=0; j<size1+1; j++){
            cursor--;
            char c = (tempNum1%10) + s_dBase;
            tempNum1 /= 10;
            autoPuzzle[cursor] = c;
        }
        cursor += size1+1;
        autoPuzzle[cursor] = 0;
        break;
    case 1: //减法
        tempRange = 1;
        for(int i=0; i<size1; i++){
            tempRange *= 10;
        }
        tempNum1 = 0;
        cursor += size1+1;
        autoPuzzle[cursor] = s_enter;
        cursor++;
        for(int i=0; i<size2; i++){
            tempNum2 = bigRand()%(tempRange-1)+1; //同理
            tempNum1 += tempNum2;
            cursor += size1;
            for(int j=0; j<size1; j++){
                cursor--;
                char c = (tempNum2%10) + s_dBase;
                tempNum2 /= 10;
                autoPuzzle[cursor] = c;
            }
            cursor += size1;
            autoPuzzle[cursor] = s_enter;
            cursor++;
        }
        cursor--;
        autoPuzzle[cursor] = 0;
        cursor = size1+1;
        for(int j=0; j<size1+1; j++){
            cursor--;
            char c = (tempNum1%10) + s_dBase;
            tempNum1 /= 10;
            autoPuzzle[cursor] = c;
        }
        break;
    case 2: //乘法
        tempRange = 1;
        for(int i=0; i<size1; i++){
            tempRange *= 10;
        }
        tempNum1 = bigRand()%(tempRange/10*9)+tempRange/10; //控制第一个乘数的首位不为0（略微影响题目优雅性）
        tempRange = 1;
        tempNum2 = 0;
        for(int i=0; i<size2; i++){
            tempNum2 = tempNum2*10+(rand()%9+1); //控制第二个乘数每一位都不为0（严重影响题目优雅性）
        }
        //前两行：
        cursor += size1;
        tempNum3 = tempNum1;
        for(int j=0; j<size1; j++){
            cursor--;
            char c = (tempNum3%10) + s_dBase;
            tempNum3 /= 10;
            autoPuzzle[cursor] = c;
        }
        cursor += size1;
        autoPuzzle[cursor] = s_enter;
        cursor++;
        cursor += size2;
        tempNum3 = tempNum2;
        for(int j=0; j<size2; j++){
            cursor--;
            char c = (tempNum3%10) + s_dBase;
            tempNum3 /= 10;
            autoPuzzle[cursor] = c;
        }
        cursor += size2;
        autoPuzzle[cursor] = s_enter;
        cursor++;
        //中间的几行:
        tempNum4 = tempNum2;
        if(size2 > 1){
            for(int i=0; i<size2; i++){
                tempNum3 = tempNum1 * (tempNum4%10);
                tempNum4 /= 10;
                cursor += size1+1;
                for(int j=0; j<size1+1; j++){
                    cursor--;
                    char c = (tempNum3%10) + s_dBase;
                    tempNum3 /= 10;
                    autoPuzzle[cursor] = c;
                }
                cursor += size1+1;
                autoPuzzle[cursor] = s_enter;
                cursor++;
            }
        }
        //最后一行：
        tempNum3 = tempNum1 * tempNum2;
        cursor += size1+size2;
        for(int j=0; j<size1+size2; j++){
            cursor--;
            char c = (tempNum3%10) + s_dBase;
            tempNum3 /= 10;
            autoPuzzle[cursor] = c;
        }
        cursor += size1+size2;
        autoPuzzle[cursor] = 0;
        break;
    case 3: //除法：(size1除数，size2商，被除数固定为size1+size2)
        tempRange = 1;
        for(int i=0; i<size1; i++){
            tempRange *= 10;
        }
        tempNum1 = bigRand()%(tempRange/10*9)+tempRange/10; //除数，首位不为0
        tempRange = 1;
        tempNum2 = 0;
        for(int i=0; i<size2; i++){
            tempNum2 = tempNum2*10+(rand()%9+1); //商的每一位都不为0
        }
        tempNum3 = bigRand()%tempNum1; //余数，必须小于除数
        tempNum4 = tempNum1 * tempNum2 + tempNum3; //被除数;
        //前3行：
        cursor += size1+size2;
        tempNum5 = tempNum4;
        for(int j=0; j<size1+size2; j++){
            cursor--;
            char c = (tempNum5%10) + s_dBase;
            tempNum5 /= 10;
            autoPuzzle[cursor] = c;
        }
        cursor += size1+size2;
        autoPuzzle[cursor] = s_enter;
        cursor++;

        cursor += size1;
        tempNum5 = tempNum1;
        for(int j=0; j<size1; j++){
            cursor--;
            char c = (tempNum5%10) + s_dBase;
            tempNum5 /= 10;
            autoPuzzle[cursor] = c;
        }
        cursor += size1;
        autoPuzzle[cursor] = s_enter;
        cursor++;

        cursor += size2;
        tempNum5 = tempNum2;
        for(int j=0; j<size2; j++){
            cursor--;
            char c = (tempNum5%10) + s_dBase;
            tempNum5 /= 10;
            autoPuzzle[cursor] = c;
        }
        cursor += size2;
        autoPuzzle[cursor] = s_enter;
        cursor++;
        //第4行至倒数第2行：
        for(int i=0; i<size2*2-1; i++){
            if(i%2==0){
                tempNum5 = tempNum2;
                for(int j=0; j<(size2*2-i-1)/2; j++){
                    tempNum5 /= 10;
                }
                tempNum5 = (tempNum5%10) * tempNum1;
            }else{
                tempNum5 = tempNum2;
                for(int j=0; j<(size2*2-i-1)/2; j++){
                    tempNum5 /= 10;
                }
                for(int j=0; j<(size2*2-i-1)/2; j++){
                    tempNum5 *= 10;
                }
                tempNum5 = tempNum5 * tempNum1;
                tempNum5 = tempNum4 - tempNum5;
                for(int j=0; j<(size2*2-i-2)/2; j++){
                    tempNum5 /= 10;
                }
            }
            cursor += size1+1;
            for(int j=0; j<size1+1; j++){
                cursor--;
                char c = (tempNum5%10) + s_dBase;
                tempNum5 /= 10;
                autoPuzzle[cursor] = c;
            }
            cursor += size1+1;
            autoPuzzle[cursor] = s_enter;
            cursor++;
        }
        //最后一行：
        cursor += size1;
        for(int j=0; j<size1; j++){
            cursor--;
            char c = (tempNum3%10) + s_dBase;
            tempNum3 /= 10;
            autoPuzzle[cursor] = c;
        }
        cursor += size1;
        autoPuzzle[cursor] = 0;
        break;
    default:
        break;
    }

    //生成数字转字符表，顺便数出数字个数，放到成员变量里
    for(int i=0; i<10; i++){
        ntovTable[i] = 0;
    }
    char sym = s_uBase;
    nDigits = 0;
    for(char *p = autoPuzzle; *p; p++){
        if(*p >= s_dBase && *p < s_dBase+10){
            if(ntovTable[*p-s_dBase] == 0){
                ntovTable[*p-s_dBase] = sym;
                sym++;
            }
            nDigits++;
        }
    }
    return;
}

void Secretary::initFromAuto(int type, char *src, char *table, bool *changes)
{
    puzzleType = type;
    //charTable:
    charTable = QString();
    for(int i=0; i<10; i++){
        charTable.append(fullLower.at(i));
    }

    //original puzzle:
    int index = 0;
    int curs;
    for(curs=0; src[curs]; curs++){
        if(src[curs]>=s_dBase && src[curs]<s_dBase+10){
            if(changes[index]){
                originalPuzzle[curs] = table[src[curs]-s_dBase];
            }else{
                originalPuzzle[curs] = src[curs];
            }
            index++;
        }else{
            originalPuzzle[curs] = src[curs];
        }
    }
    originalPuzzle[curs] = 0;
    char cur = s_uBase;
    char tab[10];
    for(int i=0; i<10; i++){
        tab[i] = 0;
    }
    for(int i=0; originalPuzzle[i]; i++){
        char c = originalPuzzle[i];
        if(c>=s_uBase && c<s_uBase+10){
            if(tab[c-s_uBase]){
                originalPuzzle[i] = tab[c-s_uBase];
            }else{
                tab[c-s_uBase] = cur;
                cur++;
                originalPuzzle[i] = tab[c-s_uBase];
            }
        }
    }

    //normalPuzzle:
    QString str = QString(originalPuzzle);
    initPuzzle(type, str, true);
}

void Secretary::getRandomSizes(int type, int *p1, int *p2) //获得随机的竖式尺寸，你们可以随意调整
{
    int ran1 = rand()%10;
    int ran2 = rand()%10;
    int add1[10] = {3, 3, 4, 4, 4, 5, 5, 6, 7, 8};
    int add2[10] = {2, 2, 2, 2, 2, 2, 2, 3, 3, 3};
    int mul1[10] = {3, 3, 3, 4, 4, 5, 5, 6, 7, 8};
    int mul2[10] = {1, 1, 1, 1, 1, 2, 2, 2, 3, 3};
    int div1[10] = {1, 1, 1, 1, 1, 2, 2, 2, 3, 3};
    int div2[10] = {1, 1, 1, 1, 1, 2, 2, 2, 2, 3};

    switch(type){
    case 0: //加法：
    case 1: //减法：
        *p1 = add1[ran1];
        *p2 = (*p1>6)? 2: add2[ran2];
        break;
    case 2: //乘法：
        *p1 = mul1[ran1];
        *p2 = (*p1>6)? 1: ((*p1>4)? mul2[ran2*2/3]: mul2[ran2]);
        break;
    case 3: //除法：
        *p1 = div1[ran1];
        *p2 = div2[ran2];
        break;
    default:
        break;
    }
}

void Secretary::initFromHalf(int type, QString oldPuzzleCharTable, char *oldOriginal, char *solution, QString oldCharTable, bool fillNumber)
{
    char ntov[10];
    char str[400];
    QString newCharTable = QString(oldCharTable);
    for(int i=0; oldOriginal[i]; i++){
        str[i] = oldOriginal[i];
        str[i+1] = 0;
    }
    //建立ntov表及维护newCharTable：
    for(int i=0; i<10; i++){
        ntov[i] = 0;
    }
    char cur = 'A';
    for(int i=0; i<10; i++){
        if(solution[i]>='0' && solution[i]<='9'){
            ntov[solution[i]-'0'] = cur;
            cur++;
        }
    }
    for(int i=10; solution[i]; i++){
        if(cur-'A' >= oldPuzzleCharTable.size()){
            break;
        }
        if(ntov[solution[i]-'0'] == 0){
            ntov[solution[i]-'0'] = cur;
            cur++;
            for(int j=0; j<oldPuzzleCharTable.size(); j++){
                if(!newCharTable.contains(oldPuzzleCharTable.at(j))){
                    newCharTable.append(oldPuzzleCharTable.at(j));
                    break;
                }
            }
        }
    }
    //对str进行小写字母到数字或大写字母的转换：
    for(int i=0; str[i]; i++){
        if(str[i]>='a' && str[i]<='z'){
            char c = solution[str[i]-'a'+10];
            if(fillNumber || ntov[c-'0']==0){
                str[i] = c;
            }else{
                str[i] = ntov[c-'0'];
            }
        }
    }
    //利用newCharTable换掉A~J：
    QString q = "";
    for(int i=0; str[i]; i++){
        if(str[i]>='A' && str[i]<='J'){
            q.append(newCharTable[str[i]-'A']);
        }else{
            q.append(str[i]);
        }
    }
    //调用普通初始化：
    initPuzzle(type, q, false);
    return;
}
