#include "widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    FunInitUI();

    FunGetIniAllData();

    myClaSerialportConnect = new ClaSerialportConnect(this);
    myClaSerialportConnect->FunGetAllPort(cbbSerial);

    myClaMessageAnalyse = new ClaMessageAnalyse(this);

    myClaCRCCheck = new ClaCRCCheck(this);

    //重新设置从站编号，按下按钮时触发的槽函数
    connect(btnSetViceNo,&QPushButton::clicked,this,&Widget::FunSetViceNo);

    //修改线圈数据表中的数据并储存在ini文件中
    connect(tbCoilData,SIGNAL(cellChanged(int,int)),this,SLOT(FunRenewDataForCoil(int,int)));
    //修改寄存器数据表中的数据并储存在ini文件中
    connect(tbRegistData,SIGNAL(cellChanged(int,int)),this,SLOT(FunRenewDataForRegist(int,int)));

    //打开或关闭串口，按下按钮时触发的槽函数
    connect(btnOpenSerial,&QPushButton::clicked,this,&Widget::FunGetSerialValueAndConnect);
    //当接收到来自串口的消息时所触发的槽函数
    connect(myClaSerialportConnect->serial,&QSerialPort::readyRead,this,&Widget::FunGetMessageFromSerial);
}

Widget::~Widget()
{

}
//界面布局的初始化
void Widget::FunInitUI(void)
{
    this->setFixedSize(1200,800);
    this->setWindowTitle("Modbus RTU ViceStation");

    gbNetMess = new QGroupBox("网络设置信息",this);
    gbNetMess->resize(390,610);
    gbNetMess->move(4,2);
    gbDataMess = new QGroupBox("数据显示信息",this);
    gbDataMess->resize(794,610);
    gbDataMess->move(400,2);
    gbOtherMess = new QGroupBox("消息显示界面",this);
    gbOtherMess->resize(1192,180);
    gbOtherMess->move(4,616);

    QFont fontTitle;
    fontTitle.setFamily("宋体");
    fontTitle.setBold(true);
    fontTitle.setPointSize(12);

    labSerialTitile = new QLabel("串口：",this);
    labSerialTitile->resize(100,40);
    labSerialTitile->move(40,30);
    labSerialTitile->setFont(fontTitle);
    cbbSerial = new QComboBox(this);
    cbbSerial->resize(190,30);
    cbbSerial->move(160,35);

    labBaudTitle = new QLabel("波特率：",this);
    labBaudTitle->resize(100,40);
    labBaudTitle->move(40,80);
    labBaudTitle->setFont(fontTitle);
    cbbBaud = new QComboBox(this);
    cbbBaud->resize(190,30);
    cbbBaud->move(160,85);
    cbbBaud->addItem("9600");
    cbbBaud->addItem("19200");
    cbbBaud->addItem("57600");
    cbbBaud->addItem("115200");

    labDataBitTitle = new QLabel("数据位：",this);
    labDataBitTitle->resize(100,40);
    labDataBitTitle->move(40,130);
    labDataBitTitle->setFont(fontTitle);
    cbbDataBit = new QComboBox(this);
    cbbDataBit->resize(190,30);
    cbbDataBit->move(160,135);
    cbbDataBit->addItem("8 bits");
    cbbDataBit->addItem("7 bits");

    labParityTitle = new QLabel("校验：",this);
    labParityTitle->resize(100,40);
    labParityTitle->move(40,180);
    labParityTitle->setFont(fontTitle);
    cbbParity = new QComboBox(this);
    cbbParity->resize(190,30);
    cbbParity->move(160,185);
    cbbParity->addItem("无校验");
    cbbParity->addItem("奇校验");
    cbbParity->addItem("偶校验");

    labStopTitle = new QLabel("停止位",this);
    labStopTitle->resize(100,40);
    labStopTitle->move(40,230);
    labStopTitle->setFont(fontTitle);
    cbbStop = new QComboBox(this);
    cbbStop->resize(190,30);
    cbbStop->move(160,235);
    cbbStop->addItem("1 stop bits");
    cbbStop->addItem("2 stop bits");

    btnOpenSerial = new QPushButton("打开串口",this);
    btnOpenSerial->resize(315,35);
    btnOpenSerial->move(40,285);

    labCoilTitle = new QLabel("线圈数据：",this);
    labCoilTitle->resize(140,30);
    labCoilTitle->move(408,32);
    labCoilTitle->setFont(fontTitle);
    tbCoilData = new QTableWidget(10000,2,this);
    tbCoilData->resize(380,533);
    tbCoilData->move(408,69);
    QStringList TableHeader1;
    TableHeader1<<"地址"<<"数据";
    tbCoilData->setHorizontalHeaderLabels(TableHeader1);
    tbCoilData->setSelectionMode(QAbstractItemView::SingleSelection);
    tbCoilData->setColumnWidth(1,170);

    labRegistTitle = new QLabel("寄存器数据：",this);
    labRegistTitle->resize(140,30);
    labRegistTitle->move(805,32);
    labRegistTitle->setFont(fontTitle);

    tbRegistData = new QTableWidget(10000,2,this);
    tbRegistData->resize(380,533);
    tbRegistData->move(805,69);
    QStringList TableHeader2;
    TableHeader2<<"地址"<<"数据";
    tbRegistData->setHorizontalHeaderLabels(TableHeader2);
    tbRegistData->setSelectionMode(QAbstractItemView::SingleSelection);
    tbRegistData->setColumnWidth(1,170);

    for(int i = 0; i < 10000; i++)
    {
        QString str1 =  "0x" + QString("%1").arg(i,4,16,QLatin1Char('0'));
        tbCoilData->setItem(i,0, new QTableWidgetItem(QString(str1)));
        tbCoilData->item(i,0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        tbCoilData->setItem(i,1, new QTableWidgetItem("0"));
        tbCoilData->item(i,1)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        tbCoilData->item(i,0)->setFlags(Qt::NoItemFlags);
        //tbCoilData->item(i,1)->setFlags(Qt::NoItemFlags);

        QString str2 =  "0x" + QString("%1").arg(i,4,16,QLatin1Char('0'));
        tbRegistData->setItem(i,0, new QTableWidgetItem(QString(str2)));
        tbRegistData->item(i,0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        tbRegistData->setItem(i,1, new QTableWidgetItem("0"));
        tbRegistData->item(i,1)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        tbRegistData->item(i,0)->setFlags(Qt::NoItemFlags);
        //tbRegistData->item(i,1)->setFlags(Qt::NoItemFlags);
    }

    txtShowMess = new QTextEdit("消息显示",this);
    txtShowMess->resize(1164,150);
    txtShowMess->move(18,640);
    txtShowMess->setReadOnly(true);

    labViceNoTitle = new QLabel("从站编号：",this);
    labViceNoTitle->resize(140,30);
    labViceNoTitle->move(40,330);
    labViceNoTitle->setFont(fontTitle);

    lnedViceNo = new QLineEdit(this);
    lnedViceNo->resize(190,30);
    lnedViceNo->move(160,335);
    lnedViceNo->setPlaceholderText("1 - 247");
    lnedViceNo->setValidator(new QIntValidator(1,247,this));

    btnSetViceNo = new QPushButton("重新设置从站编号",this);
    btnSetViceNo->resize(315,35);
    btnSetViceNo->move(40,385);
}

//读取ini文件所有线圈、寄存器数据并显示 ==> complete
void Widget::FunGetIniAllData(void)
{
    QString iniFilePath = "../ProModbusRtuVice/Data.ini";
    QSettings settings(iniFilePath,QSettings::IniFormat);

    for(int i = 0; i < 10000; i++)
    {
        QString coilData = settings.value("Section" + QString::number(i+1) + "/coil").toString();
        QString regiData = settings.value("Section" + QString::number(i+1) + "/regi").toString();

        tbCoilData->setItem(i,1,new QTableWidgetItem(QString(coilData)));
        tbCoilData->item(i,1)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);

        tbRegistData->setItem(i,1,new QTableWidgetItem(QString(regiData)));
        tbRegistData->item(i,1)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    }
}

//重新设置从站编号，并显示当前编号 ==> complete
void Widget::FunSetViceNo(void)
{
    quint8 newViceNo = lnedViceNo->text().toInt();
    if(newViceNo == 0)
    {
        ViceNo = 1;
    }
    else
    {
        ViceNo = newViceNo;
    }
    txtShowMess->append("当前从站编号为:" + QString::number(ViceNo));
}

//修改线圈数据表中的数据并储存在ini文件中 ==> complete
void Widget::FunRenewDataForCoil(int row, int col)
{
    //锁住修改信号
    tbCoilData->blockSignals(true);
    //更新表中数据和ini数据
    QString tmpStr = tbCoilData->item(row,col)->text();
    QString iniPath = "../ProModbusRtuVice/Data.ini";
    QSettings setting(iniPath,QSettings::IniFormat);
    if(tmpStr.toInt() == 0)
    {
        tbCoilData->setItem(row,col,new QTableWidgetItem("0"));
        tbCoilData->item(row,col)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);

        setting.setValue("Section" + QString::number(row + 1) + "/coil","0");
    }
    else
    {
        tbCoilData->setItem(row,col,new QTableWidgetItem("1"));
        tbCoilData->item(row,col)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);

        setting.setValue("Section" + QString::number(row + 1) + "/coil","1");
    }
    //解锁修改信号
    tbCoilData->blockSignals(false);
}

//修改寄存器数据表中的数据并储存在ini文件中 ==> complete
void Widget::FunRenewDataForRegist(int row, int col)
{
    //锁住修改信号
    tbRegistData->blockSignals(true);
    //更新表中数据和ini数据
    QString tmpStr = tbRegistData->item(row,col)->text();
    QString iniPath = "../ProModbusRtuVice/Data.ini";
    QSettings setting(iniPath,QSettings::IniFormat);
    if(tmpStr.toInt() != 0)
    {
        if(tmpStr.toInt() <= 32767 && tmpStr.toInt() >= -32768)
        {
            tbRegistData->setItem(row,col,new QTableWidgetItem(QString::number(tmpStr.toInt())));
            tbRegistData->item(row,col)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);

            setting.setValue("Section" + QString::number(row + 1) + "/regi",QString::number(tmpStr.toInt()));
        }
        else
        {
            tbRegistData->setItem(row,col,new QTableWidgetItem("0"));
            tbRegistData->item(row,col)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);

            setting.setValue("Section" + QString::number(row + 1) + "/regi","0");
        }
    }
    else
    {
        tbRegistData->setItem(row,col,new QTableWidgetItem("0"));
        tbRegistData->item(row,col)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);

        setting.setValue("Section" + QString::number(row + 1) + "/regi","0");
    }
    //解锁修改信号
    tbRegistData->blockSignals(false);
}

//打开或关闭串口，按下按钮时触发的槽函数 ==> complete
void Widget::FunGetSerialValueAndConnect(void)
{
    QString comNum = cbbSerial->currentText();
    qint32 baudRate = cbbBaud->currentIndex();
    qint32 dataBit = cbbDataBit->currentIndex();
    qint32 parityCheck = cbbParity->currentIndex();
    qint32 stopBit = cbbStop->currentIndex();

    if(btnOpenSerial->text() == "打开串口")
    {
        bool portOpenChek = myClaSerialportConnect->FunSerialportConnect(comNum,baudRate,dataBit,parityCheck,stopBit);
        if(portOpenChek)
        {
            txtShowMess->append("串口打开成功！");
            btnOpenSerial->setText("关闭串口");

            cbbSerial->setEnabled(false);
            cbbBaud->setEnabled(false);
            cbbDataBit->setEnabled(false);
            cbbParity->setEnabled(false);
            cbbStop->setEnabled(false);

            isOpen = true;
        }
        else
        {
            txtShowMess->append("串口打开失败！请检查串口！");
            btnOpenSerial->setText("打开串口");

            cbbSerial->setEnabled(true);
            cbbBaud->setEnabled(true);
            cbbDataBit->setEnabled(true);
            cbbParity->setEnabled(true);
            cbbStop->setEnabled(true);

            isOpen = true;
        }
    }
    else//关闭串口
    {
        if(myClaSerialportConnect->serial->isOpen())
        {
            myClaSerialportConnect->serial->close();
        }
        txtShowMess->append("串口已关闭！");
        btnOpenSerial->setText("打开串口");

        cbbSerial->setEnabled(true);
        cbbBaud->setEnabled(true);
        cbbDataBit->setEnabled(true);
        cbbParity->setEnabled(true);
        cbbStop->setEnabled(true);

        isOpen = false;
    }
}

//获取从串口接收到的数据，并做解析操作
void Widget::FunGetMessageFromSerial(void)
{
    //显示
    QByteArray array = myClaSerialportConnect->serial->readAll();
    QString reDataStr;
    for(int i = 0; i < array.size(); i++)
    {
        reDataStr += QString("%1").arg((quint8)array.at(i),2,16,QLatin1Char('0'));
        reDataStr += " ";
    }
    txtShowMess->append("RX:" + reDataStr);

    //解析
    QByteArray reDataToMain;
    QString tmpStr = "TX:";
    int analyseCode = myClaMessageAnalyse->FunMessageAnalyse(array, ViceNo);

    switch(analyseCode)
    {
    case -2://不做响应：报文长度不符合最低限制
        txtShowMess->append("↑此请求的报文长度不合符最低限制！");
        break;

    case -1://不做响应：CRC校验不正确
        txtShowMess->append("↑此请求的报文CRC校验计算不正确！");
        break;

    case -3://不做响应：请求的从机不是我自己
        txtShowMess->append("↑此请求的目标从机非本机！");
        break;

    case -4://不做响应：确定功能码后判定长度正不正常
        txtShowMess->append("↑从请求的报文长度不正常！");
        break;

    case 1://返回异常响应 1：非法功能码
        txtShowMess->append("↑1！");
        reDataToMain = FunMakeUnreasonalCode(array,1);
        for(int i = 0; i < 5; i++)
        {
            tmpStr += QString("%1").arg((quint8)reDataToMain.at(i),2,16,QLatin1Char('0'));
            tmpStr += " ";
        }
        txtShowMess->append(tmpStr);
        myClaSerialportConnect->serial->write(reDataToMain);
        reDataToMain.clear();
        break;

    case 2://返回异常响应 2：数量不正常
        txtShowMess->append("↑2！");
        reDataToMain = FunMakeUnreasonalCode(array,2);
        for(int i = 0; i < 5; i++)
        {
            tmpStr += QString("%1").arg((quint8)reDataToMain.at(i),2,16,QLatin1Char('0'));
            tmpStr += " ";
        }
        txtShowMess->append(tmpStr);
        myClaSerialportConnect->serial->write(reDataToMain);
        reDataToMain.clear();
        break;

    case 3://返回异常响应 3： 起始地址不正常 或 起始地址+数量不正常
        txtShowMess->append("↑3！");
        reDataToMain = FunMakeUnreasonalCode(array,3);
        for(int i = 0; i < 5; i++)
        {
            tmpStr += QString("%1").arg((quint8)reDataToMain.at(i),2,16,QLatin1Char('0'));
            tmpStr += " ";
        }
        txtShowMess->append(tmpStr);
        myClaSerialportConnect->serial->write(reDataToMain);
        reDataToMain.clear();
        break;

    case 0://返回正常响应 正确（可能也有错误，区分情况）
        if(FunMakeReasonalCode(array,&reDataToMain,tbCoilData,tbRegistData,txtShowMess) == 1)
        {
            myClaSerialportConnect->serial->write(reDataToMain);
            reDataToMain.clear();
        }
        break;
    }
}

//制作异常响应报文
QByteArray Widget::FunMakeUnreasonalCode(QByteArray array, quint8 errorCode)
{
    QByteArray unreasonalCode;
    unreasonalCode.resize(5);

    unreasonalCode[0] = (quint8)array.at(0);
    unreasonalCode[1] = (quint8)((quint8)array.at(1) + (quint8)0x80);
    unreasonalCode[2] = errorCode;

    //先计算此报文的crc校验码是否正确
    QString hexText;
    for(int i = 0; i < 3; i++)
    {
        hexText += QString("%1").arg((quint8)unreasonalCode.at(i),2,16,QLatin1Char('0'));
    }
    QString checkResult = myClaCRCCheck->crc16ForModbus(hexText);

    unreasonalCode[3] = (quint8)((checkResult.mid(0,2).toInt(NULL,16)));
    unreasonalCode[4] = (quint8)((checkResult.mid(2,2)).toInt(NULL,16));

    return unreasonalCode;
}

//制作正常响应报文
int Widget::FunMakeReasonalCode(QByteArray array, QByteArray *reDataToMain, QTableWidget *tbCoilData, QTableWidget *tbRegistData, QTextEdit *txtShowMess)
{
    QVector<quint8> reDataToMainVec;

    QSettings setting("../ProModbusRtuVice/Data.ini",QSettings::IniFormat);

    quint16 beginAddr = myClaMessageAnalyse->FunCombineTwoQuint8(array.at(2), array.at(3));
    quint16 dataCount = myClaMessageAnalyse->FunCombineTwoQuint8(array.at(4),array.at(5));

    quint16 dataByteCount;
    QString tmpReDataHexStr;
    QString crcCheck;
    QString coilDataBinStr;
    QString tmpStr = "TX:";

    QVector<qint16> tmpRegistDataVec;
    QVector<qint16>::iterator iter;

    quint16 tmpNum;
    quint8 tmpNum1,tmpNum2;

    switch((quint8)array.at(1))
    {
    case 1://读线圈
        //读取的请求报文一定得是八字节
        if(array.size() != 8)
        {
            txtShowMess->append("↑此请求报文的总字节数不符合规定！");
            return -1;
        }
        reDataToMainVec.append(array.at(0));
        reDataToMainVec.append(array.at(1));
        //从ini文件中获取线圈数据,二进制形式字符串
        for(int i = beginAddr + 1; i < beginAddr + dataCount + 1; i++)
        {
            coilDataBinStr += setting.value("Section" + QString::number(i) + "/coil").toString();
        }
        //响应数据的字节数
        if(coilDataBinStr.length() % 8 == 0)
        {
            reDataToMainVec.append((quint8)(coilDataBinStr.length() / 8));
        }
        else
        {
            reDataToMainVec.append((quint8)(coilDataBinStr.length() / 8 + 1));
        }
        //将二进制形式字符串转换成quint8
        for(int i = 0; i < coilDataBinStr.length(); i = i + 8)
        {
            reDataToMainVec.append((quint8)myClaMessageAnalyse->FunReverseOneQuint8AnotherTiga(coilDataBinStr.mid(i,8)));
        }
        //添加CRC校验
        for(QVector<quint8>::iterator iter = reDataToMainVec.begin();
            iter != reDataToMainVec.end();
            iter++)
        {
            tmpReDataHexStr += QString("%1").arg(*iter,2,16,QLatin1Char('0'));
        }
        crcCheck = myClaCRCCheck->crc16ForModbus(tmpReDataHexStr);
        qDebug() << tmpReDataHexStr;
        reDataToMainVec.append(crcCheck.mid(0,2).toInt(NULL,16));
        reDataToMainVec.append(crcCheck.mid(2,2).toInt(NULL,16));
        //组合最终报文
        for(QVector<quint8>::iterator iter = reDataToMainVec.begin();
            iter != reDataToMainVec.end();
            iter++)
        {
            reDataToMain->append(*iter);
        }
        for(int i = 0; i < reDataToMain->size(); i++)
        {
            tmpStr += QString("%1").arg((quint8)reDataToMain->at(i),2,16,QLatin1Char('0'));
            tmpStr += " ";
        }
        txtShowMess->append(tmpStr);
        return 1;
        break;

    case 3://读寄存器
        //读取的请求报文一定得是八字节
        if(array.size() != 8)
        {
            txtShowMess->append("↑此请求报文的总字节数不符合规定！");
            return -1;
        }
        reDataToMainVec.append(array.at(0));
        reDataToMainVec.append(array.at(1));
        //响应数据的字节数(一个寄存器两个字节)
        reDataToMainVec.append((quint8)(dataCount * 2));
        //从ini文件里读寄存器数据
        for(int i = beginAddr + 1; i < beginAddr + dataCount + 1; i++)
        {
            tmpNum = setting.value("Section" + QString::number(i) + "/regi").toInt();
            tmpNum1 = (quint8)(tmpNum >> 8);
            tmpNum2 = (quint8)tmpNum;
            qDebug("%x / %x",tmpNum1,tmpNum2);
            reDataToMainVec.append(tmpNum1);
            reDataToMainVec.append(tmpNum2);
        }
        //添加CRC校验
        for(QVector<quint8>::iterator iter = reDataToMainVec.begin();
            iter != reDataToMainVec.end();
            iter++)
        {
            tmpReDataHexStr += QString("%1").arg(*iter,2,16,QLatin1Char('0'));
        }
        crcCheck = myClaCRCCheck->crc16ForModbus(tmpReDataHexStr);
        reDataToMainVec.append(crcCheck.mid(0,2).toInt(NULL,16));
        reDataToMainVec.append(crcCheck.mid(2,2).toInt(NULL,16));
        //组合最终报文
        for(QVector<quint8>::iterator iter = reDataToMainVec.begin();
            iter != reDataToMainVec.end();
            iter++)
        {
            reDataToMain->append(*iter);
        }
        for(int i = 0; i < reDataToMain->size(); i++)
        {
            tmpStr += QString("%1").arg((quint8)reDataToMain->at(i),2,16,QLatin1Char('0'));
            tmpStr += " ";
        }
        txtShowMess->append(tmpStr);
        return 1;
        break;

    case 15://写线圈
        //先判断请求报文基本长度够不够操作
        if(array.size() < 10)
        {
            txtShowMess->append("↑此请求报文的总字节数不完整！");
            return -1;
        }
        //再判定数量和字节数对不对得上
        if(myClaMessageAnalyse->FunCombineTwoQuint8((quint8)array.at(4),(quint8)array.at(5)) % 8 == 0)
        {
            dataByteCount = myClaMessageAnalyse->FunCombineTwoQuint8((quint8)array.at(4),(quint8)array.at(5)) / 8;
        }
        else
        {
            dataByteCount = myClaMessageAnalyse->FunCombineTwoQuint8((quint8)array.at(4),(quint8)array.at(5)) / 8 + 1;
        }
        if((quint8)dataByteCount != (quint8)array.at(6))
        {
            txtShowMess->append("↑此请求报文的请求数量与字节数不匹配！");
            return -1;
        }
        //再判定数据项有没有那么多的字节
        if((quint8)(array.size() - 9) != (quint8)array.at(6))
        {
            txtShowMess->append("↑此请求报文的实际数据项与字节数不匹配！");
            return -1;
        }

        //没错了
        //先拼接响应报文吧
        for(int i = 0; i < 6; i++)
        {
            reDataToMainVec.append((quint8)array.at(i));
        }
        //添加CRC校验
        for(QVector<quint8>::iterator iter = reDataToMainVec.begin();
            iter != reDataToMainVec.end();
            iter++)
        {
            tmpReDataHexStr += QString("%1").arg(*iter,2,16,QLatin1Char('0'));
        }
        crcCheck = myClaCRCCheck->crc16ForModbus(tmpReDataHexStr);
        reDataToMainVec.append(crcCheck.mid(0,2).toInt(NULL,16));
        reDataToMainVec.append(crcCheck.mid(2,2).toInt(NULL,16));
        //组合最终报文
        for(QVector<quint8>::iterator iter = reDataToMainVec.begin();
            iter != reDataToMainVec.end();
            iter++)
        {
            reDataToMain->append(*iter);
        }

        //解析请求报文中的数据
        for(int i = 7; i < 7 + dataByteCount; i++)
        {
            coilDataBinStr += QString("%1").arg((quint8)myClaMessageAnalyse->FunReverseOneQuint8AnotherTiga(QString("%1").arg((quint8)array.at(i),8,2,QLatin1Char('0'))),8,2,QLatin1Char('0'));
        }
        //按照规定的数量写入ini中,并显示在数据表中
        for(int i = beginAddr + 1,j = 0; i < beginAddr + dataCount + 1; i++,j++)
        {
            setting.setValue("Section" + QString::number(i) + "/coil",coilDataBinStr.mid(j,1));

            tbCoilData->blockSignals(true);
            tbCoilData->item(i - 1, 1)->setText(coilDataBinStr.mid(j,1));
            tbCoilData->blockSignals(false);
        }

        for(int i = 0; i < reDataToMain->size(); i++)
        {
            tmpStr += QString("%1").arg((quint8)reDataToMain->at(i),2,16,QLatin1Char('0'));
            tmpStr += " ";
        }
        txtShowMess->append(tmpStr);
        return 1;
        break;

    case 16://写寄存器
        //先判断请求报文基本长度够不够操作
        if(array.size() < 10)
        {
            txtShowMess->append("↑此请求报文的总字节数不完整！");
            return -1;
        }
        //再判定数量和字节数对不对得上
        if(myClaMessageAnalyse->FunCombineTwoQuint8(array.at(4),array.at(5)) * 2 != (quint8)array.at(6))
        {
            txtShowMess->append("↑此请求报文的请求数量与字节数不匹配！");
            return -1;
        }
        //再判定数据项有没有那么多的字节
        if((quint8)(array.size() - 9) != (quint8)array.at(6))
        {
            txtShowMess->append("↑此请求报文的实际数据项与字节数不匹配！");
            return -1;
        }

        //没错了
        //先拼接响应报文吧
        for(int i = 0; i < 6; i++)
        {
            reDataToMainVec.append((quint8)array.at(i));
        }
        //添加CRC校验
        for(QVector<quint8>::iterator iter = reDataToMainVec.begin();
            iter != reDataToMainVec.end();
            iter++)
        {
            tmpReDataHexStr += QString("%1").arg(*iter,2,16,QLatin1Char('0'));
        }
        crcCheck = myClaCRCCheck->crc16ForModbus(tmpReDataHexStr);
        reDataToMainVec.append(crcCheck.mid(0,2).toInt(NULL,16));
        reDataToMainVec.append(crcCheck.mid(2,2).toInt(NULL,16));
        //组合最终报文
        for(QVector<quint8>::iterator iter = reDataToMainVec.begin();
            iter != reDataToMainVec.end();
            iter++)
        {
            reDataToMain->append(*iter);
        }

        //解析请求报文中的数据
        for(int i = 7; i < array.size() - 2; i = i + 2)
        {
            tmpRegistDataVec.append(myClaMessageAnalyse->FunCombineTwoQuint8(array.at(i),array.at(i+1)));
        }
        //按照规定的数量写入ini中
        iter = tmpRegistDataVec.begin();
        for(int i = beginAddr + 1;
            i < beginAddr + dataCount + 1;
            i++,iter++)
        {
            qDebug() << "i:" << i << " / iter:" << *iter;
            setting.setValue("Section" + QString::number(i) + "/regi",QString::number(*iter));
        }
        //在表中显示数据
        for(int i = beginAddr + 1; i < beginAddr + dataCount + 1; i++)
        {
            tbRegistData->blockSignals(true);
            tbRegistData->item(i - 1, 1)->setText(setting.value("Section" + QString::number(i) + "/regi").toString());
            tbRegistData->blockSignals(false);
        }

        for(int i = 0; i < reDataToMain->size(); i++)
        {
            tmpStr += QString("%1").arg((quint8)reDataToMain->at(i),2,16,QLatin1Char('0'));
            tmpStr += " ";
        }
        txtShowMess->append(tmpStr);
        return 1;
        break;
    }

}
