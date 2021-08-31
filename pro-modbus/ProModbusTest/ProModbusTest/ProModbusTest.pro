#-------------------------------------------------
#
# Project created by QtCreator 2021-07-07T10:19:30
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_promodbustesttest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += tst_promodbustesttest.cpp \
    ../../ProModbusTCP/ProModbusTcpMain/clamessagebuild.cpp \
    ../../ProModbusRTU/ProModbusRtuVice/clacrccheck.cpp \
    ../../ProModbusRTU/ProModbusRtuVice/clamessageanalyse.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

HEADERS += \
    ../../ProModbusTCP/ProModbusTcpMain/clamessagebuild.h \
    ../../ProModbusRTU/ProModbusRtuVice/clacrccheck.h \
    ../../ProModbusRTU/ProModbusRtuVice/clamessageanalyse.h
