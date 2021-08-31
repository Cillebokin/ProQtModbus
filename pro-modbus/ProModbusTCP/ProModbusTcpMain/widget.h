#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
//Basic
#include <QDebug>
#include <QGroupBox>
#include <QTableWidget>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QComboBox>
#include <QByteArray>

//Self
#include "clamessagebuild.h"

//DataBase

//NetWork
#include <QTcpSocket>
#include <QHostAddress>

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = 0);
    ~Widget();
    quint16 MessNo = 0;
    QByteArray LastCmd;
    bool ConnetStatus = false;

private:
    //Basic
    QLabel *labIpTitle;
    QLabel *labPortTitle;
    QLineEdit *lnedIp;
    QLineEdit *lnedPort;
    QPushButton *btnConnect;

    QGroupBox *gbNetMess;
    QGroupBox *gbDataMess;
    QGroupBox *gbOtherMess;

    QLabel *labCoilTitle;
    QLabel *labRegistTitle;
    QTableWidget *tbCoilData;
    QTableWidget *tbRegistData;

    QTextEdit *txtShowMess;

    QLabel *labViceNoTitle;
    QLineEdit *lnedViceNo;
    QLabel *labFuncCodeTitle;
    QComboBox *cbbxFunCode;

    QLabel *labBeginAddrTitle;
    QLineEdit *lnedBeginAddr;
    QLabel *labDataCountTitle;
    QLineEdit *lnedDataCount;

    QLabel *labDataIn;
    QLineEdit *lnedDataIn;
    QPushButton *btnSendMess;

    //NetWork
    QTcpSocket *socket;

    //Self
    ClaMessageBuild *myClaMessageBuild;


    //Fun
    void FunInitUI(void);

    void FunBtnConnect(void);
    void FunConnectSuccess(void);
    void FunDisconnected(void);
    void FunReadNetMess(void);

    void FunSendMess(void);

    QString FunGetCoilData( QTableWidget *tbCoilData,
                            quint16 beginAddr,
                            quint16 dataCount);

    QString FunGetRefistData(   QTableWidget *tbRegistData,
                                quint16 beginAddr,
                                quint16 dataCount);

public slots:
    void FunCheckInputCoilData(int row, int col);
    void FunCheckInputRegistData(int row, int col);

    void FunUpdateStatusForControl(int cbbxIndex);
};

#endif // WIDGET_H
