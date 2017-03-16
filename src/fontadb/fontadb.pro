include( $${_PRO_FILE_PWD_}/common.pri )

DESTDIR = $${LIBS_PATH}/

QT       += gui

TARGET = fontadb$${LIB_SUFFIX}
TEMPLATE = lib
CONFIG += staticlib

SOURCES += fontadb.cpp \
    classifier.cpp

HEADERS += fontadb.h \
    panose.h \
    types.h \
    classifier.h

VERSION = 0.0.1
QMAKE_TARGET_COPYRIGHT = (c) PitM
QMAKE_TARGET_PRODUCT = Fonta DB
QMAKE_TARGET_DESCRIPTION = Fonta core library for fonts classifying
