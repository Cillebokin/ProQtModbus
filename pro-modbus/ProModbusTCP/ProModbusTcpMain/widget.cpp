#include "widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    FunInitUI();

    socket = NULL;
    socket = new QTcpSocket(this);

    myClaMessageBuild = new ClaMessageBuild(this);

    //选择不同的功能码时触发的槽函数
    connect(cbbxFunCode,SIGNAL(currentIndexChanged(int)),this,SLOT(FunUpdateStatusForControl(int)));
    //当用户修改了线圈数据表中的内容时触发的槽函数
    connect(tbCoilData,SIGNAL(cellChanged(int,int)),this,SLOT(FunCheckInputCoilData(int,int)));
    //当用户修改了寄存器数据表中的内容时触发的槽函数
    connect(tbRegistData,SIGNAL(cellChanged(int,int)),this,SLOT(FunCheckInputRegistData(int,int)));

    //按下连接按钮触发的槽函数
    connect(btnConnect,&QPushButton::clicked,this,&Widget::FunBtnConnect);
    //Tcp连接成功触发的槽函数
    connect(socket,&QTcpSocket::connected,this,&Widget::FunConnectSuccess);
    //Tcp断开连接时触发的槽函数
    connect(socket,&QTcpSocket::disconnected,this,&Widget::FunDisconnected);
    //Tcp网络读到消息时触发的槽函数
    connect(socket,&QTcpSocket::readyRead,this,&Widget::FunReadNetMess);

    //按下发送按钮触发的槽函数
    connect(btnSendMess,&QPushButton::clicked,this,&Widget::FunSendMess);
}

Widget::~Widget()
{
    socket->disconnectFromHost();
    socket->close();
}

//界面布局的初始化 ==> complete
void Widget::FunInitUI(void)
{
    this->setFixedSize(1200,800);
    this->setWindowTitle("Modbus TCP MainStation");

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
    labIpTitle = new QLabel("IP地址：",this);
    labIpTitle->resize(100,40);
    labIpTitle->move(40,30);
    labIpTitle->setFont(fontTitle);
    labPortTitle = new QLabel("端口号：",this);
    labPortTitle->resize(100,40);
    labPortTitle->move(40,80);
    labPortTitle->setFont(fontTitle);

    lnedIp = new QLineEdit(this);
    lnedIp->resize(190,30);
    lnedIp->move(160,35);
    lnedIp->setPlaceholderText("127.0.0.1");
    QRegExp rx("^((2[0-4]\\d|25[0-5]|[01]?\\d\\d?)\\.){3}(2[0-4]\\d|25[0-5]|[01]?\\d\\d?)$");
    QRegExpValidator *m_IPValidator = new QRegExpValidator(rx, this);
    lnedIp->setValidator(m_IPValidator);
    lnedPort = new QLineEdit(this);
    lnedPort->resize(190,30);
    lnedPort->move(160,85);
    lnedPort->setPlaceholderText("502");
    lnedPort->setValidator(new QIntValidator(0,65535,this));

    btnConnect = new QPushButton("开始连接",this);
    btnConnect->resize(310,35);
    btnConnect->move(40,130);

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

        QString str2 =  "0x" + QString("%1").arg(i,4,16,QLatin1Char('0'));
        tbRegistData->setItem(i,0, new QTableWidgetItem(QString(str2)));
        tbRegistData->item(i,0)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        tbRegistData->setItem(i,1, new QTableWidgetItem("0"));
        tbRegistData->item(i,1)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        tbRegistData->item(i,0)->setFlags(Qt::NoItemFlags);

    }

    txtShowMess = new QTextEdit("消息显示",this);
    txtShowMess->resize(1164,150);
    txtShowMess->move(18,640);
    txtShowMess->setReadOnly(true);

    labViceNoTitle = new QLabel("从机地址:",this);
    labViceNoTitle->resize(110,40);
    labViceNoTitle->move(40,180);
    labViceNoTitle->setFont(fontTitle);
    lnedViceNo = new QLineEdit(this);
    lnedViceNo->resize(190,30);
    lnedViceNo->move(160,185);
    lnedViceNo->setPlaceholderText("1 - 247");
    lnedViceNo->setValidator(new QIntValidator(1, 247, this));

    labFuncCodeTitle = new QLabel("功能码：",this);
    labFuncCodeTitle->resize(100,40);
    labFuncCodeTitle->move(40,230);
    labFuncCodeTitle->setFont(fontTitle);
    cbbxFunCode = new QComboBox(this);
    cbbxFunCode->resize(190,30);
    cbbxFunCode->move(160,235);
    cbbxFunCode->addItem("0x01");
    cbbxFunCode->addItem("0x03");
    cbbxFunCode->addItem("0x0F");
    cbbxFunCode->addItem("0x10");

    labBeginAddrTitle = new QLabel("起始地址:",this);
    labBeginAddrTitle->resize(110,40);
    labBeginAddrTitle->move(40,280);
    labBeginAddrTitle->setFont(fontTitle);
    lnedBeginAddr = new QLineEdit(this);
    lnedBeginAddr->resize(190,30);
    lnedBeginAddr->move(160,285);
    lnedBeginAddr->setPlaceholderText("0 - 9999(编号)");
    lnedBeginAddr->setValidator(new QIntValidator(0, 9999, this));

    labDataCountTitle = new QLabel("数量：",this);
    labDataCountTitle->resize(100,40);
    labDataCountTitle->move(40,330);
    labDataCountTitle->setFont(fontTitle);
    lnedDataCount = new QLineEdit(this);
    lnedDataCount->resize(190,30);
    lnedDataCount->move(160,335);
    lnedDataCount->setPlaceholderText("1 - 2000");
    lnedDataCount->setValidator(new QIntValidator(1, 2000, this));

    btnSendMess = new QPushButton("发送报文",this);
    btnSendMess->resize(310,35);
    btnSendMess->move(40,380);
}

//选择不同的功能码时改变控件的输入状态和提示信息 ==> complete
void Widget::FunUpdateStatusForControl(int cbbxIndex)
{
    lnedDataCount->setText("");
    lnedBeginAddr->setText("");
    if(cbbxIndex == 0)
    {
        lnedDataCount->setPlaceholderText("1 - 2000");
        lnedDataCount->setValidator(new QIntValidator(1, 2000, this));
    }
    else if(cbbxIndex == 1)
    {
        lnedDataCount->setPlaceholderText("1 - 125");
        lnedDataCount->setValidator(new QIntValidator(1, 125, this));
    }
    else if(cbbxIndex == 2)
    {
        lnedDataCount->setPlaceholderText("1 - 1968");
        lnedDataCount->setValidator(new QIntValidator(1, 1968, this));
    }
    else
    {
        lnedDataCount->setPlaceholderText("1 - 123");
        lnedDataCount->setValidator(new QIntValidator(1, 123, this));
    }
}

//线圈数据表中的内容被修改后进行数据检测 ==> complete
void Widget::FunCheckInputCoilData(int row,int col)
{
    tbCoilData->blockSignals(true);
    QString tmpStr = tbCoilData->item(row,col)->text();
    if(tmpStr.toInt() == 0)
    {
        tbCoilData->setItem(row,col,new QTableWidgetItem("0"));
        tbCoilData->item(row,col)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    }
    else
    {
        tbCoilData->setItem(row,col,new QTableWidgetItem("1"));
        tbCoilData->item(row,col)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    }
    tbCoilData->blockSignals(false);
}

//寄存器数据表中的内容被修改后进行数据检测 ==> complete
void Widget::FunCheckInputRegistData(int row, int col)
{
    tbRegistData->blockSignals(true);
    QString tmpStr = tbRegistData->item(row,col)->text();
    if(tmpStr.toInt() != 0)
    {
        if(tmpStr.toInt() <= 32767 && tmpStr.toInt() >= -32768)
        {
            tbRegistData->setItem(row,col,new QTableWidgetItem(QString::number(tmpStr.toInt())));
            tbRegistData->item(row,col)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        }
        else
        {
            tbRegistData->setItem(row,col,new QTableWidgetItem("0"));
            tbRegistData->item(row,col)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        }
    }
    else
    {
        tbRegistData->setItem(row,col,new QTableWidgetItem("0"));
        tbRegistData->item(row,col)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    }
    tbRegistData->blockSignals(false);
}

//连接按钮槽函数，点击后进行网络连接，再次点击断开连接 ==> complete
void Widget::FunBtnConnect(void)
{
    QString ipAddr = lnedIp->text();
    qint16 portNum = lnedPort->text().toInt();
    if(ipAddr == NULL)
        ipAddr = "127.0.0.1";//未输入时默认设置为127.0.0.1
    if(portNum == NULL)
        portNum = 502;//为输入时默认设置为502

    if(btnConnect->text() == "开始连接")
    {
        socket->abort();//取消原有连接
        socket->connectToHost(QHostAddress(ipAddr),portNum);
        if(socket->waitForConnected(1000))
        {
            btnConnect->setText("停止连接");
        }
        else
        {
            btnConnect->setText("开始连接");
            txtShowMess->append("连接超时，请检查网络!");
        }
    }
    else    //停止连接
    {
        socket->disconnectFromHost();
        socket->close();
        btnConnect->setText("开始连接");
    }
}

//成功连接TCP网络后，显示成功连接的信息 ==> complete
void Widget::FunConnectSuccess(void)
{
    ConnetStatus = true;
    btnConnect->setText("停止连接");
    txtShowMess->append("已成功连接网络！");
}

//TCP网络断开时，显示网络断开信息 ==> complete
void Widget::FunDisconnected(void)
{
    socket->disconnectFromHost();
    socket->close();
    ConnetStatus = false;
    btnConnect->setText("开始连接");
    txtShowMess->append("连接已停止！");
}

//TCP网络读到消息，做显示处理
void Widget::FunReadNetMess(void)
{
    //先显示一下收到的消息
    QByteArray array = socket->readAll();
    QString reDataStr;
    for(int i = 0; i < array.size(); i++)
    {
        reDataStr += QString("%1").arg((quint8)array.at(i),2,16,QLatin1Char('0'));
        reDataStr += " ";
    }
    txtShowMess->append("RX:" + reDataStr);

    //再进行解析
    QString tmpDataStrBin;
    QVector<quint16> dataVec;
    QVector<quint16>::iterator iter;

    int analyseCode = myClaMessageBuild->FunAnalyseMessData(LastCmd,array,&tmpDataStrBin,&dataVec);
    switch(analyseCode)
    {
    case 1:
        //截取需要的长度，显示在线圈数据表中
        for(quint16 i = myClaMessageBuild->FunCombineTwoQuint8((quint8)LastCmd.at(8),(quint8)LastCmd.at(9));
           i < myClaMessageBuild->FunCombineTwoQuint8((quint8)LastCmd.at(8),(quint8)LastCmd.at(9)) + myClaMessageBuild->FunCombineTwoQuint8((quint8)LastCmd.at(10),(quint8)LastCmd.at(11)) && i < 10000;
           i++)
        {
           tbCoilData->setItem(i,1,new QTableWidgetItem(QString(tmpDataStrBin.left(1) ) ) );
           tbCoilData->item(i,1)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
           tmpDataStrBin = tmpDataStrBin.right(tmpDataStrBin.length() - 1);
        }
        txtShowMess->append("线圈读取成功！");
        break;

    case 2:
        //取出需要的字节数据
        for(int i = 9; i < array.size();  i = i + 2)
        {
            dataVec.append(myClaMessageBuild->FunCombineTwoQuint8((quint8)array.at(i),(quint8)array.at(i+1)));
        }
        iter = dataVec.begin();
        for(quint16 i = myClaMessageBuild->FunCombineTwoQuint8(LastCmd.at(8),LastCmd.at(9));
          i < myClaMessageBuild->FunCombineTwoQuint8(LastCmd.at(8),LastCmd.at(9)) + myClaMessageBuild->FunCombineTwoQuint8(LastCmd.at(10),LastCmd.at(11)) && i < 10000;
          i++)
        {
            tbRegistData->setItem(i,1,new QTableWidgetItem(QString::number((qint16)*iter) ) );
            tbRegistData->item(i,1)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
            iter++;
        }
        txtShowMess->append("寄存器读取成功！");
        break;

    case 3:
        txtShowMess->append("线圈数据写入成功！");
        break;

    case 4:
        txtShowMess->append("寄存器数据写入成功！");
        break;

    case -1:
        txtShowMess->append("↑未发送请求就接收到了消息！此消息不予解析！");
        break;

    case -2:
        txtShowMess->append("↑ 接收到的报文长度不正常！请重新发送请求！");
        break;

    case -3:
        txtShowMess->append("↑ 接收到的报文请求序列号不一致！请重新发送请求!");
        break;

    case -4:
        txtShowMess->append("↑ 接收到的报文协议标识有误！请重新发送请求!");
        break;

    case -5:
        txtShowMess->append("↑ 接收到的报文报头其后字节数不正确！请重新发送请求！");
        break;

    case -6:
        txtShowMess->append("↑ 响应此请求的从站不正确！请重新发送请求！");
        break;

    case -7:
        txtShowMess->append("↑ 无法被识别的功能码！请重新发送请求！");
        break;

    case -8:
        txtShowMess->append("↑ 请求异常！异常码为：1");
        break;

    case -9:
        txtShowMess->append("↑ 请求异常！异常码为：" + QString::number(array.at(8)));
        break;

    case -10:
        txtShowMess->append("↑ 请求异常，且异常报文长度不正确！");
        break;

    case -11:
        txtShowMess->append("↑ 响应报文的功能码与请求不一致！请重新发送请求！");
        break;

    case -12:
        txtShowMess->append("↑ 响应报文的字节数不正常或不完整！请重新发送请求！");
        break;

    case -13:
        txtShowMess->append("↑ 响应报文的数据字节数与请求无法对应！请重新发送请求！");
        break;

    case -14:
        txtShowMess->append("↑ 响应报文的数据字节数不正确！请重新发送请求！");
        break;

    case -15:
        txtShowMess->append("↑ 响应报文的其后字节数不正确！请重新发送请求！");
        break;

    case -16:
        txtShowMess->append("↑ 响应报文解析与预期响应不一致（请求地址与响应地址不一致）！");
        break;

    case -17:
        txtShowMess->append("↑ 响应报文解析与预期响应不一致（请求数量与响应数量不一致）！");
        break;

    case -18:
        txtShowMess->append("↑ 响应报文的其后字节数不正确！请重新发送请求！");
        break;

    case -19:
        txtShowMess->append("↑ 响应报文长度不完整或不正确！请重新发送请求！");
        break;
    }

    //清空请求
    LastCmd.clear();
}

//获取数据表中的线圈数据
QString Widget::FunGetCoilData(QTableWidget *tbCoilData,
                                quint16 beginAddr,
                                quint16 dataCount)
{
    //先从表格从按顺序取出一定数量的二进制形式字符
    QString coilDataStr;
    QString preStrHex;
    for(quint16 i = beginAddr;
        (i < 10000) && (i < beginAddr + dataCount);
        i++)
    {
        coilDataStr += tbCoilData->item(i,1)->text();
    }
    //计算其能组合成几个字节
    int tmpNum;
    if(coilDataStr.length() % 8 == 0)
        tmpNum = coilDataStr.length() / 8;
    else
        tmpNum = coilDataStr.length() / 8 + 1;



    QString tmpStr1;
    for(int i = 0; i < coilDataStr.length(); i++)
    {
        tmpStr1 = coilDataStr.at(i) + tmpStr1;
        if((i + 1) % 8 == 0)
        {
            preStrHex += QString("%1").arg(tmpStr1.toInt(NULL,2),2,16,QChar('0'));
            tmpStr1 = "";
        }
    }
    if(tmpStr1 != NULL)
    {
        preStrHex += QString("%1").arg(tmpStr1.toInt(NULL,2),2,16,QChar('0'));
        tmpStr1 = "";
    }
    qDebug() << preStrHex;
    return preStrHex;
}

//获取数据表中的寄存器数据
QString Widget::FunGetRefistData(  QTableWidget *tbRegistData,
                                    quint16 beginAddr,
                                    quint16 dataCount)
{
    QString registDataStr;
    for(quint16 i = beginAddr;
        (i < 10000) && (i < beginAddr + dataCount);
        i++)
    {
        registDataStr += QString("%1").arg((quint16)(tbRegistData->item(i,1)->text().toInt()),
                                           4,
                                           16,
                                           QChar('0'));
    }

    qDebug() << registDataStr;
    return registDataStr;
}

//按下发送按钮，对要发送的数据做封装处理
void Widget::FunSendMess(void)
{
    //===报头部分===
    //MessNo++;                       //事务处理标识符（请求报文序号）-->0/1
    quint16 protocolCheck = 0;      //协议标识符（0x0000即Modbus协议）-->2/3
    quint16 byteCount = 6;          //报文此位置其后的字节数，默认设置为6-->4/5

    //===PDU部分===
    quint8 viceNo;                  //单元标识符（从站编号）默认设置为1-->6
    if(lnedViceNo->text() == NULL)
        viceNo = 0x01;
    else if(lnedViceNo->text().toInt() == 0)
        viceNo = (quint8)1;
    else
        viceNo = (quint8)(lnedViceNo->text().toInt());

    quint8 funcCode;                //功能码-->7
    switch(cbbxFunCode->currentIndex())
    {
    case 0:
        funcCode = (quint8)1;
        break;
    case 1:
        funcCode = (quint8)3;
        break;
    case 2:
        funcCode = (quint8)15;
        break;
    case 3:
        funcCode = (quint8)16;
        break;
    }

    quint16 beginAddr;              //起始地址,输入十进制的编号,默认设置为1-->8/9
    if(lnedBeginAddr->text().toInt() != 0)
        beginAddr = (quint16)lnedBeginAddr->text().toInt();
    else
        beginAddr = (quint16)0;

    quint16 dataCount;              //数量-->10/11
    if(lnedDataCount->text().toInt() != 0)
        dataCount = lnedDataCount->text().toInt();
    else
        dataCount = (quint16)1;

    QByteArray messPack;
    QString tmpDataStr;

    switch(cbbxFunCode->currentIndex())
    {
    case 0:
        messPack = myClaMessageBuild->Fun0x010x03MessageCreate(MessNo,
                                                               protocolCheck,
                                                               byteCount,
                                                               viceNo,
                                                               funcCode,
                                                               beginAddr,
                                                               dataCount);
        LastCmd = messPack;
        break;
    case 1:
        messPack = myClaMessageBuild->Fun0x010x03MessageCreate(MessNo,
                                                               protocolCheck,
                                                               byteCount,
                                                               viceNo,
                                                               funcCode,
                                                               beginAddr,
                                                               dataCount);
        LastCmd = messPack;
        break;
    case 2:
        tmpDataStr = FunGetCoilData( tbCoilData,beginAddr,dataCount);

        messPack = myClaMessageBuild->Fun0x0FMessageCreate(MessNo,
                                                           protocolCheck,
                                                           byteCount,
                                                           viceNo,
                                                           funcCode,
                                                           beginAddr,
                                                           dataCount,
                                                           tmpDataStr.length() / 2,
                                                           tmpDataStr);
        LastCmd = messPack;
        break;
    case 3:
        tmpDataStr = FunGetRefistData(tbRegistData,beginAddr,dataCount);

        messPack = myClaMessageBuild->Fun0x0FMessageCreate(MessNo,
                                                           protocolCheck,
                                                           byteCount,
                                                           viceNo,
                                                           funcCode,
                                                           beginAddr,
                                                           dataCount,
                                                           tmpDataStr.length() / 2,
                                                           tmpDataStr);
        LastCmd = messPack;
        break;
    }

    if(ConnetStatus == true)
    {
        socket->write(messPack);
        QString reDataStr;
        for(int i = 0; i < messPack.size(); i++)
        {
            reDataStr += QString("%1").arg((quint8)messPack.at(i),2,16,QLatin1Char('0'));
            reDataStr += " ";
        }
        txtShowMess->append("TX:" + reDataStr);
    }
    else
    {
        txtShowMess->append("连接已断开，无法发送！");
    }

    if(!socket->waitForReadyRead(2000))
    {
        txtShowMess->append("响应等待时间太长！请检查从站的成分！");
    }
}
