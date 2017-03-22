QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

include( ../../../common.pri )

TARGET = fonta_classifier
DESTDIR = $${BIN_PATH}/
TEMPLATE = app

SOURCES += \
    main.cpp\
    mainwindow.cpp \
    importdialog.cpp

HEADERS += \
    mainwindow.h \
    importdialog.h

FORMS   += \
    mainwindow.ui \
    importdialog.ui

RESOURCES += \
    pics.qrc

LIBS += -lfontadb$${LIB_SUFFIX}
