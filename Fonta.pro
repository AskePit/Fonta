QT += core gui widgets

CONFIG += c++11 #console

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

RESOURCES_DIR = resources

RESOURCES += \
    $${RESOURCES_DIR}/pics.qrc

RC_FILE += \
    #$${RESOURCES_DIR}/AdminManifest.rc
    $${RESOURCES_DIR}/version.rc

build_all:!build_pass {
    CONFIG -= build_all
    CONFIG += release
}

QMAKE_CXXFLAGS += -O3

DEFINES *= \
    QT_USE_QSTRINGBUILDER \
    #FONTA_DETAILED_DEBUG \
    FONTA_MEASURES

