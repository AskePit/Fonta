QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

include( ../../../common.pri )

TARGET = cogwheel_gen
DESTDIR = $${BIN_PATH}/
TEMPLATE = app


SOURCES += \
    main.cpp\
    mainwindow.cpp \
    canvas.cpp

HEADERS += \
    mainwindow.h \
    canvas.h

FORMS   += \
    mainwindow.ui
