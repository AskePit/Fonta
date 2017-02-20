QT += core gui widgets network xml

CONFIG += c++11

TARGET = Fonta
TEMPLATE = app


SOURCES += \
    main.cpp\
    fontadb.cpp \
    types.cpp \
    sampler.cpp \
    filterwizard.cpp \
    stylesheet.cpp \
    mainwindow.cpp \
    widgets/about.cpp \
    widgets/renametabedit.cpp \
    widgets/togglepanel.cpp \
    widgets/field.cpp \
    widgets/workarea.cpp \
    widgets/filteredit.cpp \
    widgets/combobox.cpp \
    loremgenerator.cpp

HEADERS += \
    fontadb.h \
    panose.h \
    types.h \
    sampler.h \
    filterwizard.h \
    stylesheet.h \
    mainwindow.h \
    widgets/about.h \
    widgets/renametabedit.h \
    widgets/togglepanel.h \
    widgets/field.h \
    widgets/workarea.h \
    widgets/filteredit.h \
    widgets/combobox.h \
    loremgenerator.h

FORMS += \
    mainwindow.ui

RESOURCES_DIR = resources

RESOURCES += \
    $${RESOURCES_DIR}/pics.qrc

build_all:!build_pass {
    CONFIG -= build_all
    CONFIG += release
}

QMAKE_CXXFLAGS += -O2

DEFINES *= \
    QT_USE_QSTRINGBUILDER \
    #FONTA_DETAILED_DEBUG \
    FONTA_MEASURES
