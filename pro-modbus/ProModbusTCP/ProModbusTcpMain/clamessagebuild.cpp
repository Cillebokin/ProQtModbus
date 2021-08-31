#include "clamessagebuild.h"

ClaMessageBuild::ClaMessageBuild(QObject *parent) : QObject(parent)
{

}

ClaMessageBuild::~ClaMessageBuild()
{

}

//将两个quint8合并成一个quint16
quint16 ClaMessageBuild::FunCombineTwoQuint8(quint8 preNum, quint8 afterNum)
{
    quint16 tmp = (preNum << 8) | afterNum;
    return tmp;
}

//逆转一个二进制
quint8 ClaMessageBuild::FunReverseOneQuint8(quint8 num)
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

//读取线圈和读取寄存器（0x01和0x03）时，生成报文
QByteArray ClaMessageBuild::Fun0x010x03MessageCreate(   quint16 messNo,
                                                        quint16 protocol,
                                                        quint16 byteCount,
                                                        quint8 viceNo,
                                                        quint8 funcCode,
                                                        quint16 beginAddr,
                                                        quint16 dataCount)
{
    QByteArray mess0x010x03;
    mess0x010x03.resize(12);

    mess0x010x03[0] = messNo >> 8;
    mess0x010x03[1] = messNo & 0xff;

    mess0x010x03[2] = protocol >> 8;
    mess0x010x03[3] = protocol & 0x0ff;

    mess0x010x03[4] = byteCount >> 8;
    mess0x010x03[5] = byteCount & 0x0ff;

    mess0x010x03[6] = viceNo;
    mess0x010x03[7] = funcCode;

    mess0x010x03[8] = beginAddr >> 8;
    mess0x010x03[9] = beginAddr & 0x0ff;

    mess0x010x03[10] = dataCount >> 8;
    mess0x010x03[11] = dataCount & 0x0ff;

    return mess0x010x03;
}



//写入线圈和写入寄存器（0x0f）时，生成报文
QByteArray ClaMessageBuild::Fun0x0FMessageCreate(   quint16 messNo,
                                                    quint16 protocol,
                                                    quint16 byteCount,
                                                    quint8 viceNo,
                                                    quint8 funcCode,
                                                    quint16 beginAddr,
                                                    quint16 dataCount,
                                                    quint8 dataByteCount,
                                                    QString dataStr)
{
    QVector<quint8> mess0x0FVec;

    mess0x0FVec.append(messNo >> 8);            //0
    mess0x0FVec.append(messNo & 0xff);          //1

    mess0x0FVec.append(protocol >> 8);          //2
    mess0x0FVec.append(protocol & 0x0ff);       //3

    byteCount = 7 + (quint16)dataByteCount;
    mess0x0FVec.append(byteCount >> 8);         //4
    mess0x0FVec.append(byteCount & 0x0ff);      //5

    mess0x0FVec.append(viceNo);                 //6
    mess0x0FVec.append(funcCode);               //7

    mess0x0FVec.append(beginAddr >> 8);         //8
    mess0x0FVec.append(beginAddr & 0x0ff);      //9

    mess0x0FVec.append(dataCount >> 8);         //10
    mess0x0FVec.append(dataCount & 0x0ff);      //11

    mess0x0FVec.append(dataByteCount);          //12

    QString tmpStr = dataStr;
    for(int i = 0; i < dataByteCount * 2;)
    {
        bool ok;
        mess0x0FVec.append(tmpStr.left(2).toInt(&ok,16));
        i = i + 2;
        tmpStr = dataStr.right(dataStr.length() - i);
    }

    QByteArray mess0x0F;
    for(QVector<quint8>::iterator iter = mess0x0FVec.begin();
        iter != mess0x0FVec.end();
        iter++)
    {
        mess0x0F.append(*iter);
    }
    return mess0x0F;
}

int ClaMessageBuild::FunAnalyseMessData(QByteArray LastCmd,
                                        QByteArray array,
                                        QString *tmpDataStrBin,
                                        QVector<quint16> *dataVec)
{
    //判断是否有发送请求报文
    if(LastCmd.isEmpty())
        return -1;//未发送请求就接收到了消息！此消息不予解析

    //判断接收到的报文长度是否正常(报头 6 + viceNo 1 + FuncCode 1)
    if(array.size() < 8)
        return -2;// 接收到的报文长度不正常

    //判断响应报文的请求序号是否一致
    if(array.at(0) != LastCmd.at(0) || array.at(1) != LastCmd.at(1))
        return -3;//接收到的报文请求序列号不一致

    //判断响应报文的协议标识是否正确（00 00）
    if(array.at(2) != 0 || array.at(3) != 0)
        return -4;//接收到的报文协议标识有误

    //判断响应报文的其后字节数是否正确
    if(array.size() - 6 != FunCombineTwoQuint8(array.at(4),array.at(5)))
        return -5;//接收到的报文报头其后字节数不正确

    //判断响应从站的从站编号是否与请求一致
    if(array.at(6) != LastCmd.at(6))
        return -6;//响应此请求的从站不正确

    //判断功能码是否合法
    if( array.at(7) != 1 &&
       array.at(7) != 3 &&
       array.at(7) != 15 &&
       array.at(7) != 16 &&
       (quint8)array.at(7) != 0x81 &&
       (quint8)array.at(7) != 0x83 &&
       (quint8)array.at(7) != 0x8f &&
       (quint8)array.at(7) != 0x90)
    {
       return -7;//无法被识别的功能码
    }
    else
    {
       if( (quint8)array.at(7) == 0x81 ||
           (quint8)array.at(7) == 0x83 ||
           (quint8)array.at(7) == 0x8f ||
           (quint8)array.at(7) == 0x90)
       {
           if(array.size() == 9)
           {
               if((quint8)array.at(7) - 0x80 !=LastCmd.at(7))
               {
                   return -8;// 请求异常！异常码为：1
               }
               return -9;//请求异常！异常码为2或3
           }
           else
           {
               return -10;// 请求异常，且异常报文长度不正确
           }
       }
    }

    //判断请求与响应的功能码是否一致
    if(array.at(7) != LastCmd.at(7))
        return -11;//响应报文的功能码与请求不一致

    //根据不同的功能码做出不同的操作
    quint8 realRequestByte;
    switch(LastCmd.at(7))
    {
    case 1:
        //判断响应的字节数是否符合最低要求
        if(array.size() < 10)
            return -12;//(0x01/0x03)响应报文的字节数不正常或不完整

        //先判断响应报文的字节数是否和请求的数量对应
        if(FunCombineTwoQuint8((quint8)LastCmd.at(10),(quint8)LastCmd.at(11)) % 8 == 0)
            realRequestByte = (quint8)(FunCombineTwoQuint8((quint8)LastCmd.at(10),(quint8)LastCmd.at(11)) / 8);
        else
            realRequestByte = (quint8)(FunCombineTwoQuint8((quint8)LastCmd.at(10),(quint8)LastCmd.at(11)) / 8 + 1);
        if((quint8)realRequestByte != (quint8)array.at(8))
            return -13;//(0x01/0x03)响应报文的数据字节数与请求无法对应

        if((quint16)FunCombineTwoQuint8(array.at(4),array.at(5)) == array.size() - 6)
        {
           if((quint8)array.at(8) == array.size() - 9)//解析正确了
           {
               //取出需要的字节数据，并转成二进制形式字符串
               for(int i = 9; i < array.size();  i++)
               {
                    *tmpDataStrBin += QString("%1").arg(FunReverseOneQuint8(array.at(i)),8,2,QChar('0'));
               }
               *tmpDataStrBin = tmpDataStrBin->left(array.at(8) * 8);
               return 1;//解析正确
           }
           else
           {
               return -14;//(0x01/0x03)响应报文的数据字节数不正确
           }
        }
        else
        {
           return -15;//(0x01/0x03)响应报文的其后字节数不正确
        }
        break;

    case 3:
        //判断响应的字节数是否符合最低要求
        if(array.size() < 10)
           return -12;//(0x01/0x03)响应报文的字节数不正常或不完整

        //先判断响应报文的字节数是否和请求的数量对应
        realRequestByte = (quint16)(FunCombineTwoQuint8(LastCmd.at(10),LastCmd.at(11)) * 2);
        if((quint16)realRequestByte != (quint16)(quint8)array.at(8))
            return -13;//(0x01/0x03)响应报文的数据字节数与请求无法对应

        if((quint16)FunCombineTwoQuint8(array.at(4),array.at(5)) == array.size() - 6)
        {
           if((quint8)array.at(8) == array.size() - 9)
           {
               //取出需要的字节数据
                for(int i = 9; i < array.size();  i = i + 2)
                {
                   dataVec->append(FunCombineTwoQuint8((quint8)array.at(i),(quint8)array.at(i+1)));
                }
                return 2;//解析正确
           }
           else
           {
               return -14;//(0x01/0x03)响应报文的数据字节数不正确
           }
        }
        else
        {
           return -15;//(0x01/0x03)响应报文的其后字节数不正确
        }
        break;

    case 15:
        if(array.size() == 12)
        {
            if((quint16)FunCombineTwoQuint8(array.at(4),array.at(5)) == 6)
            {
               if((quint16)FunCombineTwoQuint8(array.at(8),array.at(9)) !=
                   (quint16)FunCombineTwoQuint8(LastCmd.at(8),LastCmd.at(9)))
                   return -16;//(0x0f/0x10)响应报文解析与预期响应不一致（请求地址与响应地址不一致）

               if((quint16)FunCombineTwoQuint8(array.at(10),array.at(11)) !=
                       (quint16)FunCombineTwoQuint8(LastCmd.at(10),LastCmd.at(11)))
                       return -17;//(0x0f/0x10)响应报文解析与预期响应不一致（请求数量与响应数量不一致）

                   return 3;//线圈数据写入成功
                }
                else
                {
                   return -18;//(0x0f/0x10)响应报文的其后字节数不正确
                }
            }
            else
            {
                return -19;//(0x0f/0x10)响应报文长度不完整或不正确
            }
            break;

    case 16:
        if(array.size() == 12)
        {
           if((quint16)FunCombineTwoQuint8(array.at(4),array.at(5)) == 6)
           {
               if((quint16)FunCombineTwoQuint8(array.at(8),array.at(9)) !=
                   (quint16)FunCombineTwoQuint8(LastCmd.at(8),LastCmd.at(9)))
               {
                   return -16;//(0x0f/0x10)响应报文解析与预期响应不一致（请求地址与响应地址不一致）
               }

               if((quint16)FunCombineTwoQuint8(array.at(10),array.at(11)) !=
                   (quint16)FunCombineTwoQuint8(LastCmd.at(10),LastCmd.at(11)))
               {
                   return -17;//(0x0f/0x10)响应报文解析与预期响应不一致（请求数量与响应数量不一致）
               }

               return 4;//寄存器写入成功
           }
           else
           {
               return -18;//(0x0f/0x10)响应报文的其后字节数不正确
           }
        }
        else
        {
           return -19;//(0x0f/0x10)响应报文长度不完整或不正确
        }
        break;
    }
}
