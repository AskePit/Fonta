QT += core gui widgets

CONFIG += c++11

TARGET = Fonta
TEMPLATE = app


SOURCES += \
    main.cpp\
    fontawindow.cpp \
    fontadb.cpp \
    fontawidgets.cpp \
    types.cpp \
    sampler.cpp \
    filterwizard.cpp \
    stylesheet.cpp

HEADERS += \
    fontawindow.h \
    fontadb.h \
    panose.h \
    fontawidgets.h \
    types.h \
    sampler.h \
    filterwizard.h \
    stylesheet.h

FORMS += \
    fontawindow.ui

RESOURCES += \
    pics.qrc

build_all:!build_pass {
    CONFIG -= build_all
    CONFIG += release
}

QMAKE_CXXFLAGS += -O3

DEFINES *= \
    QT_USE_QSTRINGBUILDER
