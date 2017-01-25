QT += core gui widgets network xml

CONFIG += c++11 #console

TARGET = Fonta
TEMPLATE = app


SOURCES += \
    main.cpp\
    fontadb.cpp \
    fontawidgets.cpp \
    types.cpp \
    sampler.cpp \
    filterwizard.cpp \
    stylesheet.cpp \
    mainwindow.cpp

HEADERS += \
    fontadb.h \
    panose.h \
    fontawidgets.h \
    types.h \
    sampler.h \
    filterwizard.h \
    stylesheet.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

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

QMAKE_CXXFLAGS += -O2

DEFINES *= \
    QT_USE_QSTRINGBUILDER \
    #FONTA_DETAILED_DEBUG \
    FONTA_MEASURES

