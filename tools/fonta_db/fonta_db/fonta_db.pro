#-------------------------------------------------
#
# Project created by QtCreator 2016-11-21T15:46:11
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = fonta_db
TEMPLATE = app

INCLUDEPATH += "../../.."


SOURCES += main.cpp\
        mainwindow.cpp \
    searchengine.cpp

HEADERS  += mainwindow.h \
    searchengine.h \
    ../../../types.h

FORMS    += mainwindow.ui
