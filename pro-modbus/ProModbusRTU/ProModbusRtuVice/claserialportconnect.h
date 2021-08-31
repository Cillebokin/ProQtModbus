#ifndef CLASERIALPORTCONNECT_H
#define CLASERIALPORTCONNECT_H

#include <QObject>
//Basic
#include <QComboBox>

//Network
#include <QSerialPort>        //提供访问串口的功能
#include <QSerialPortInfo>    //提供系统中存在的串口的信息

class ClaSerialportConnect : public QObject
{
    Q_OBJECT
public:
    explicit ClaSerialportConnect(QObject *parent = 0);
    ~ClaSerialportConnect();

    QSerialPort *serial;

    //Fun
    bool FunSerialportConnect(QString comNum,
                              qint32 baudRate,
                              qint32 dataBit,
                              qint32 parityCheck,
                              qint32 stopBit);

    void FunGetAllPort(QComboBox *cbbSerial);

signals:

public slots:
};

#endif // CLASERIALPORTCONNECT_H
