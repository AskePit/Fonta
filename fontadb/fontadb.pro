#-------------------------------------------------
#
# Project created by QtCreator 2017-03-15T22:02:27
#
#-------------------------------------------------

QT       -= gui

TARGET = fontadb
TEMPLATE = lib
CONFIG += staticlib

SOURCES += fontadb.cpp

HEADERS += fontadb.h \
    panose.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}
