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

TRANSLATIONS += \
    $${RESOURCES_DIR}/i18n/en.ts \
    $${RESOURCES_DIR}/i18n/ru.ts

RESOURCES += \
    $${RESOURCES_DIR}/pics.qrc \
    $${RESOURCES_DIR}/known_fonts.qrc \
    $${RESOURCES_DIR}/i18n/lang.qrc

RC_FILE += \
    $${RESOURCES_DIR}/app_icon.rc

build_all:!build_pass {
    CONFIG -= build_all
    CONFIG += release
}

LIBS += -lfontadb$${LIB_SUFFIX}

#QMAKE_POST_LINK = windeployqt.exe $${BIN_PATH}/$${TARGET}.exe
#QMAKE_POST_LINK = xcopy $${RESOURCES_DIR}/html $${BIN_PATH} -E
