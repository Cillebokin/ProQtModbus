#include "claserialportconnect.h"

ClaSerialportConnect::ClaSerialportConnect(QObject *parent) : QObject(parent)
{
    serial = new QSerialPort;
}

ClaSerialportConnect::~ClaSerialportConnect()
{

}
bool ClaSerialportConnect::FunSerialportConnect(QString comNum,
                                                qint32 baudRate,
                                                qint32 dataBit,
                                                qint32 parityCheck,
                                                qint32 stopBit)
{
    //设置串口号
    serial->setPortName(comNum);

    //设置波特率
    switch(baudRate)
    {
    case 0:
        serial->setBaudRate(QSerialPort::Baud9600);
        break;
    case 1:
        serial->setBaudRate(QSerialPort::Baud19200);
        break;
    case 2:
        serial->setBaudRate(QSerialPort::Baud57600);
        break;
    case 3:
        serial->setBaudRate(QSerialPort::Baud115200);
        break;
    }

    //设置数据位
    switch(dataBit)
    {
    case 0:
        serial->setDataBits(QSerialPort::Data8);
        break;
    case 1:
        serial->setDataBits(QSerialPort::Data7);
        break;
    }

    //设置校验位
    switch(parityCheck)
    {
    case 0:
        serial->setParity(QSerialPort::NoParity);
        break;
    case 1:
        serial->setParity(QSerialPort::OddParity);
        break;
    case 2:
        serial->setParity(QSerialPort::EvenParity);
        break;
    }

    //设置停止位
    switch(stopBit)
    {
    case 0:
        serial->setStopBits(QSerialPort::OneStop);
        break;
    case 1:
        serial->setStopBits(QSerialPort::TwoStop);
        break;
    }

    serial->setFlowControl(QSerialPort::NoFlowControl);

    bool portOpenCheck = serial->open(QIODevice::ReadWrite);
    return portOpenCheck;
}

//获取所有可用的串口
void ClaSerialportConnect::FunGetAllPort(QComboBox *cbbSerial)
{
    //实例化一个类型参数为QSerialPortInfo模板类链表
    QList<QSerialPortInfo> serialList;
    //给链表赋值为获取到的当前设备的所有串口信息的链表
    serialList = QSerialPortInfo::availablePorts();

    for(QList<QSerialPortInfo>::iterator iter = serialList.begin();
        iter != serialList.end();
        iter++)
    {
//        if(iter->description().left(19) == "Virtual Serial Port")
//        {
//            //qDebug()<< "Description : "<< iter->description();
//            cbbSerial->addItem(iter->portName());
//        }
        cbbSerial->addItem(iter->portName());
    }
}

