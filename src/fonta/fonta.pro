QT += core gui widgets network xml

include( ../../common.pri )

TARGET = fonta
DESTDIR = $${BIN_PATH}/
TEMPLATE = app

SOURCES += \
    main.cpp\
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
    loremgenerator.cpp \
    launcher.cpp \
    utils.cpp

HEADERS += \
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
    loremgenerator.h \
    launcher.h \
    types_fonta.h \
    utils.h

FORMS += \
    mainwindow.ui

RESOURCES_DIR = resources

RESOURCES += \
    $${RESOURCES_DIR}/pics.qrc

build_all:!build_pass {
    CONFIG -= build_all
    CONFIG += release
}

LIBS += -lfontadb$${LIB_SUFFIX}
