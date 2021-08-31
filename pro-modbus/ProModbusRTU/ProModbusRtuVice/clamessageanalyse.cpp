#include "clamessageanalyse.h"

ClaMessageAnalyse::ClaMessageAnalyse(QObject *parent) : QObject(parent)
{
    myClaCRCCheck = new ClaCRCCheck(this);
}

ClaMessageAnalyse::~ClaMessageAnalyse()
{

}

//将两个quint8合并成一个quint16 ==> complete
quint16 ClaMessageAnalyse::FunCombineTwoQuint8(quint8 preNum, quint8 afterNum)
{
    quint16 tmp = (preNum << 8) | afterNum;
    return tmp;
}

//逆转一个二进制
quint8 ClaMessageAnalyse::FunReverseOneQuint8(quint8 num)
{
    quint8 reNum = 0;
    QChar tmpChar;
    QString tmpStr = QString("%1").arg(num,8,2,QChar('0'));
    for(int i = 7,j = 0; i >= 4; i--,j++)
    {
        tmpChar = tmpStr.at(i);
        tmpStr[i] = tmpStr.at(j);
        tmpStr[j] = tmpChar;
    }
    reNum = tmpStr.toInt(NULL,2);
    return reNum;
}
quint8 ClaMessageAnalyse::FunReverseOneQuint8AnotherTiga(QString numBin)
{
    quint8 reNum = 0;
    QChar tmpChar;

    if(numBin.length() == 8)
    {
        for(int i = 7,j = 0; i >= 4; i--,j++)
        {
            tmpChar = numBin.at(i);
            numBin[i] = numBin.at(j);
            numBin[j] = tmpChar;
        }
        reNum = (quint8)numBin.toInt(NULL,2);
    }
    else
    {
        for(int i = numBin.length() - 1,j = 0; i >= (numBin.length() / 2); i--,j++)
        {
            tmpChar = numBin.at(i);
            numBin[i] = numBin.at(j);
            numBin[j] = tmpChar;
        }
        reNum = (quint8)numBin.toInt(NULL,2);
    }

    return reNum;
}

int ClaMessageAnalyse::FunMessageAnalyse(QByteArray array, quint8 ViceNo)
{
    //先确定报文长度有没有达到最小限度
    if(array.size() < 4)
    {
        return -2;
    }

    //先计算此报文的crc校验码是否正确
    QString hexText,crcCheck;
    for(int i = 0; i < array.size() - 2; i++)
    {
        hexText += QString("%1").arg((quint8)array.at(i),2,16,QLatin1Char('0'));
    }
    crcCheck += QString("%1").arg((quint8)array.at(array.size() - 2),2,16,QLatin1Char('0'));
    crcCheck += QString("%1").arg((quint8)array.at(array.size() - 1),2,16,QLatin1Char('0'));
    QString checkResult = myClaCRCCheck->crc16ForModbus(hexText);
    if(checkResult != crcCheck)
        return -1;

    //判断请求的从站是不是我自己
    if(ViceNo != (quint8)array.at(0))
        return -3;//错付了

    //判断功能码是否符合正常的范围
    if((quint8)array.at(1) != 0x01 && (quint8)array.at(1) != 0x03 && (quint8)array.at(1) != 0x0f && (quint8)array.at(1) != 0x10)
        return 1;//不正经的功能码

    //确定功能码正常后，区分不同功能码
    switch((quint8)array.at(1))
    {
    case 0x01:
        //确定功能后，判断报文长度正不正常
        if(array.size() != 8)
            return -4;
        //判断请求报文的数量项是否正常
        if(FunCombineTwoQuint8((quint8)array.at(4),(quint8)array.at(5)) < 1 || FunCombineTwoQuint8((quint8)array.at(4),(quint8)array.at(5)) > 2000)
            return 3; //不正常
        //正常
        //判断起始地址是否正确，以及起始地址 + 数量是否正确
        if(FunCombineTwoQuint8((quint8)array.at(2),(quint8)array.at(3)) < 0 || FunCombineTwoQuint8((quint8)array.at(2),(quint8)array.at(3)) > 9999)
            return 2;//起始地址不正常
        else
        {
            if(FunCombineTwoQuint8((quint8)array.at(2),(quint8)array.at(3)) + FunCombineTwoQuint8((quint8)array.at(4),(quint8)array.at(5)) > 10000)
            {
                qDebug() << FunCombineTwoQuint8((quint8)array.at(2),(quint8)array.at(3));
                qDebug() << FunCombineTwoQuint8((quint8)array.at(4),(quint8)array.at(5));
                return 2;//起始地址 + 数量 不正确
            }
        }
        return 0;
        break;

    case 0x03:
        //确定功能后，判断报文长度正不正常
        if(array.size() != 8)
            return -4;
        //判断请求报文的数量项是否正常
        if(FunCombineTwoQuint8((quint8)array.at(4),(quint8)array.at(5)) < 1 || FunCombineTwoQuint8((quint8)array.at(4),(quint8)array.at(5)) > 125)
            return 3; //不正常
        //正常
        //判断起始地址是否正确，以及起始地址 + 数量是否正确
        if(FunCombineTwoQuint8((quint8)array.at(2),(quint8)array.at(3)) < 0 || FunCombineTwoQuint8((quint8)array.at(2),(quint8)array.at(3)) > 9999)
            return 2;//起始地址不正常
        else
        {
            if(FunCombineTwoQuint8((quint8)array.at(2),(quint8)array.at(3)) + FunCombineTwoQuint8((quint8)array.at(4),(quint8)array.at(5)) > 10000)
                return 2;//起始地址 + 数量 不正确
        }
        return 0;
        break;

    case 0x0f:
        //确定功能后，判断报文长度正不正常
        if(array.size() < 10)
            return -4;
        //判断请求报文的数量项是否正常
        if(FunCombineTwoQuint8((quint8)array.at(4),(quint8)array.at(5)) < 1 || FunCombineTwoQuint8((quint8)array.at(4),(quint8)array.at(5)) > 1968)
            return 3; //不正常
        //正常
        //判断起始地址是否正确，以及起始地址 + 数量是否正确
        if(FunCombineTwoQuint8((quint8)array.at(2),(quint8)array.at(3)) < 0 || FunCombineTwoQuint8((quint8)array.at(2),(quint8)array.at(3)) > 9999)
            return 2;//起始地址不正常
        else
        {
            if(FunCombineTwoQuint8((quint8)array.at(2),(quint8)array.at(3)) + FunCombineTwoQuint8((quint8)array.at(4),(quint8)array.at(5)) > 10000)
                return 2;//起始地址 + 数量 不正确
        }
        return 0;
        break;

    case 0x10:
        //确定功能后，判断报文长度正不正常
        if(array.size() < 10)
            return -4;
        //判断请求报文的数量项是否正常
        if(FunCombineTwoQuint8((quint8)array.at(4),(quint8)array.at(5)) < 1 || FunCombineTwoQuint8((quint8)array.at(4),(quint8)array.at(5)) > 123)
            return 3; //不正常
        //正常
        //判断起始地址是否正确，以及起始地址 + 数量是否正确
        if(FunCombineTwoQuint8((quint8)array.at(2),(quint8)array.at(3)) < 0 || FunCombineTwoQuint8((quint8)array.at(2),(quint8)array.at(3)) > 9999)
            return 2;//起始地址不正常
        else
        {
            if(FunCombineTwoQuint8((quint8)array.at(2),(quint8)array.at(3)) + FunCombineTwoQuint8((quint8)array.at(4),(quint8)array.at(5)) > 10000)
                return 2;//起始地址 + 数量 不正确
        }
        return 0;
        break;
    }

    return 0;
}
