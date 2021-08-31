#include "clacrccheck.h"

ClaCRCCheck::ClaCRCCheck(QObject *parent) : QObject(parent)
{
    //wura
}

ClaCRCCheck::~ClaCRCCheck()
{

}

QString ClaCRCCheck::crc16ForModbus(const QString &hexText)
{
    QByteArray data  = QByteArray::fromHex(hexText.toLocal8Bit());

    quint8 buf;
    quint16 crc16 = 0xFFFF;

    for (int i = 0; i < data.size(); ++i )
    {
        buf = data.at( i ) ^ crc16;
        crc16 >>= 8;
        crc16 ^= crc16Table[ buf ];
    }
    QString reCrcStr = QString("%1").arg(crc16 , 4, 16, QLatin1Char('0'));//拼凑成4个16进制字符，空位补0
    QChar tmpChar;

    tmpChar = reCrcStr.at(0);
    reCrcStr[0] = reCrcStr.at(2);
    reCrcStr[2] = tmpChar;

    tmpChar = reCrcStr.at(1);
    reCrcStr[1] = reCrcStr.at(3);
    reCrcStr[3] = tmpChar;

   return reCrcStr;
   //	return QString::number(crc16,16).toUpper();
   //   return crc16;
}
