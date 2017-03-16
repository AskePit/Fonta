QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

include( $${_PRO_FILE_PWD_}/common.pri )

TARGET = cogwheel_gen
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    canvas.cpp

HEADERS  += mainwindow.h \
    canvas.h

FORMS    += mainwindow.ui
