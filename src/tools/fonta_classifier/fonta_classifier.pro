QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

include( $${_PRO_FILE_PWD_}/common.pri )

TARGET = fonta_db
TEMPLATE = app

include( $${_PRO_FILE_PWD_}/common.pri )

SOURCES += main.cpp\
        mainwindow.cpp \
    searchengine.cpp

HEADERS  += mainwindow.h \
    searchengine.h \

FORMS    += mainwindow.ui
