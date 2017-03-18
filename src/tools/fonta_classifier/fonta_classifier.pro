QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

include( ../../../common.pri )

TARGET = fonta_classifier
DESTDIR = $${BIN_PATH}/
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \

HEADERS  += mainwindow.h \

FORMS    += mainwindow.ui

LIBS += -lfontadb$${LIB_SUFFIX}
