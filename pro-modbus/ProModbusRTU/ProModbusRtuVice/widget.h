#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
//Basic
#include <QDebug>
#include <QGroupBox>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QTableWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QByteArray>

//Self
#include "claserialportconnect.h"
#include "clamessageanalyse.h"
#include "clacrccheck.h"

//DataBase

//NetWork

//Files
#include <QSettings>

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = 0);
    ~Widget();
    quint8 ViceNo = 1;
    bool isOpen = false;
//    QString iniPath = "..\ProModbusRtuVice\Data.ini";
    QByteArray LastCmd;

private:
    //Self
    ClaSerialportConnect *myClaSerialportConnect;
    ClaMessageAnalyse *myClaMessageAnalyse;

    //Basic
    QGroupBox *gbNetMess;
    QGroupBox *gbDataMess;
    QGroupBox *gbOtherMess;

    QLabel *labSerialTitile;
    QComboBox *cbbSerial;
    QLabel *labBaudTitle;
    QComboBox *cbbBaud;
    QLabel *labDataBitTitle;
    QComboBox *cbbDataBit;
    QLabel *labParityTitle;
    QComboBox *cbbParity;
    QLabel *labStopTitle;
    QComboBox *cbbStop;

    QPushButton *btnOpenSerial;

    QLabel *labCoilTitle;
    QLabel *labRegistTitle;
    QTableWidget *tbCoilData;
    QTableWidget *tbRegistData;

    QTextEdit *txtShowMess;

    QLabel *labViceNoTitle;
    QLineEdit *lnedViceNo;
    QPushButton *btnSetViceNo;

    ClaCRCCheck *myClaCRCCheck;

    //Fun
    void FunInitUI(void);
    void FunGetIniAllData(void);

    void FunSetViceNo(void);

    void FunGetSerialValueAndConnect(void);
    void FunGetMessageFromSerial(void);

    QByteArray FunMakeUnreasonalCode(QByteArray array, quint8);

    int FunMakeReasonalCode(QByteArray array,
                            QByteArray *reDatatoMain,
                            QTableWidget *tbCoilData,
                            QTableWidget *tbRegistData,
                            QTextEdit *txtShowMess);

public slots:
    void FunRenewDataForCoil(int row, int col);
    void FunRenewDataForRegist(int row, int col);
};

#endif // WIDGET_H
