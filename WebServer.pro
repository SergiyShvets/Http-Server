#-------------------------------------------------
#
# Project created by QtCreator 2016-04-19T11:09:17
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = WebServer
TEMPLATE = app

include("Tcp/tcp.pri")
include("Http/http.pri")

SOURCES += main.cpp\
        dialog.cpp \
    test.cpp

HEADERS  += dialog.h \
    test.h

FORMS    += dialog.ui
