#ifndef CLAMESSAGEBUILD_H
#define CLAMESSAGEBUILD_H

#include <QObject>
#include <QByteArray>
#include <QVector>
#include <QDebug>

class ClaMessageBuild : public QObject
{
    Q_OBJECT
public:
    explicit ClaMessageBuild(QObject *parent = 0);
    ~ClaMessageBuild();

    //Fun

    quint16 FunCombineTwoQuint8(quint8 preNum, quint8 afterNum);

    quint8 FunReverseOneQuint8(quint8 num);

    QByteArray Fun0x010x03MessageCreate(quint16 messNo,
                                        quint16 protocol,
                                        quint16 byteCount,
                                        quint8 viceNo,
                                        quint8 funcCode,
                                        quint16 beginAddr,
                                        quint16 dataCount);

    QByteArray Fun0x0FMessageCreate(    quint16 messNo,
                                        quint16 protocol,
                                        quint16 byteCount,
                                        quint8 viceNo,
                                        quint8 funcCode,
                                        quint16 beginAddr,
                                        quint16 dataCount,
                                        quint8 dataByteCount,
                                        QString dataStr);

    int FunAnalyseMessData(QByteArray LastCmd,
                           QByteArray array,
                           QString *tmpDataStrBin,
                           QVector<quint16> *dataVec);

signals:

public slots:
};

#endif // CLAMESSAGEBUILD_H
