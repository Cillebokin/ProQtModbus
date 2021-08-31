#-------------------------------------------------
#
# Project created by QtCreator 2021-06-30T19:44:02
#
#-------------------------------------------------

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ProModbusRtuVice
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    claserialportconnect.cpp \
    clacrccheck.cpp \
    clamessageanalyse.cpp

HEADERS  += widget.h \
    claserialportconnect.h \
    clacrccheck.h \
    clamessageanalyse.h
