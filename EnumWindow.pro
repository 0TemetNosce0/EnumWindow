#-------------------------------------------------
#
# Project created by QtCreator 2019-11-04T22:28:44
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = EnumWindow
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

CONFIG += c++11

SOURCES += \
        main.cpp \
        widget.cpp \
    GetWindowHelpers.cpp \
    WindowInfo.cpp

HEADERS += \
        widget.h \
    GetWindowHelpers.h \
    windowinfo.h

FORMS += \
        widget.ui


LIBS += -lshell32 -luser32  -lDwmapi
