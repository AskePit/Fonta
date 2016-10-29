#-------------------------------------------------
#
# Project created by QtCreator 2016-03-16T22:27:34
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Fonta
TEMPLATE = app


SOURCES += main.cpp\
        fontawindow.cpp \
    fontadb.cpp \
    fontawidgets.cpp \
    types.cpp \
    sampler.cpp \
    filterwizard.cpp \
    stylesheet.cpp

HEADERS  += fontawindow.h \
    fontadb.h \
    panose.h \
    fontawidgets.h \
    types.h \
    sampler.h \
    filterwizard.h \
    stylesheet.h

FORMS    += fontawindow.ui

RESOURCES += \
    pics.qrc
build_all:!build_pass {
    CONFIG -= build_all
    CONFIG += release
}

QMAKE_CXXFLAGS += -O2 -Wall -Wpedantic
QMAKE_LFLAGS_RELEASE += -static -static-libgcc

LIBS += -lgdi32 -luser32
