#include <QString>
#include <QTest>
#include <QObject>
#include <QCoreApplication>
#include <QSettings>
#include <QByteArray>
#include <QDebug>

//Self
#include "../../ProModbusTCP/ProModbusTcpMain/clamessagebuild.h"
#include "../../ProModbusRTU/ProModbusRtuVice/clamessageanalyse.h"

class ProModbusTestTest : public QObject
{
    Q_OBJECT

public:
    ClaMessageBuild myClaMessageBuild;
    ClaMessageAnalyse myClaMessageAnalyse;

    ProModbusTestTest();

private Q_SLOTS:
    void testCase1_data();
    void testCase1();
};

ProModbusTestTest::ProModbusTestTest()
{
}

void ProModbusTestTest::testCase1_data()
{

}

void ProModbusTestTest::testCase1()
{
    //Test for FunAnalyseMessData
    QSettings settingForFunAnalyseMessData("../ProModbusTest/config/FunAnalyseMessData.ini",QSettings::IniFormat);
    int testCount = settingForFunAnalyseMessData.value("Section0/count").toInt();
    for(int i = 1; i <= testCount; i++)
    {
        QString tx = (settingForFunAnalyseMessData.value("Section" + QString::number(i) + "/tx").toString());
        QString rx = (settingForFunAnalyseMessData.value("Section" + QString::number(i) + "/rx").toString());
        int result = (settingForFunAnalyseMessData.value("Section" + QString::number(i) + "/result").toInt());

        QByteArray txAry;
        QByteArray rxAry;
        int strSize = tx.size();
        for(int j = 0; j < strSize; j = j + 2)
        {
            txAry.append((qint8)(tx.mid(0,2).toInt(NULL,16)));
            tx = tx.mid(2,tx.size() - 2);
        }
        strSize = rx.size();
        for(int j = 0; j < strSize; j = j + 2)
        {
            rxAry.append((quint8)rx.mid(0,2).toInt(NULL,16));
            rx = rx.mid(2,rx.size() - 2);
        }
        QString tmpDataStrBin;
        QVector<quint16> 、;
        int analyseResult = myClaMessageBuild.FunAnalyseMessData(txAry,rxAry,&tmpDataStrBin,&dataVec);

        QCOMPARE(analyseResult,result);
    }

    //Test for FunMessageAnalyse
    QSettings settingForFunMessageAnalyse("../ProModbusTest/config/FunMessageAnalyse.ini",QSettings::IniFormat);
    testCount = settingForFunMessageAnalyse.value("Section0/count").toInt();
    for(int i = 1; i <= testCount; i++)
    {
        QString rx = (settingForFunMessageAnalyse.value("Section" + QString::number(i) + "/rx").toString());
        int viceNo = (settingForFunMessageAnalyse.value("Section" + QString::number(i) + "/viceNo").toInt());
        int result = (settingForFunMessageAnalyse.value("Section" + QString::number(i) + "/result").toInt());

        QByteArray rxAry;
        int strSize = rx.size();
        for(int j = 0; j < strSize; j = j + 2)
        {
            rxAry.append((qint8)(rx.mid(0,2).toInt(NULL,16)));
            rx = rx.mid(2,rx.size() - 2);
        }
        int analyseResult = myClaMessageAnalyse.FunMessageAnalyse(rxAry,viceNo);
        QCOMPARE(analyseResult,result);
    }
}

QTEST_MAIN(ProModbusTestTest)

#include "tst_promodbustesttest.moc"
