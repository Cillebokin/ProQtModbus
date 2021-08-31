#ifndef CLAMESSAGEANALYSE_H
#define CLAMESSAGEANALYSE_H

#include <QObject>
#include <QDebug>
#include <QSettings>
#include <QVector>

//Self
#include "clacrccheck.h"

class ClaMessageAnalyse : public QObject
{
    Q_OBJECT
public:
    explicit ClaMessageAnalyse(QObject *parent = 0);
    ~ClaMessageAnalyse();

    //Self
    ClaCRCCheck *myClaCRCCheck;

    //Fun
    quint16 FunCombineTwoQuint8(quint8 preNum, quint8 afterNum);
    quint8 FunReverseOneQuint8(quint8 num);
    quint8 FunReverseOneQuint8AnotherTiga(QString numBin);

    int FunMessageAnalyse(QByteArray array, quint8 ViceNo);



signals:

public slots:
};

#endif // CLAMESSAGEANALYSE_H
