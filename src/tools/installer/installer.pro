QT += core
QT -= gui

CONFIG -= embed_manifest_exe

include( ../../../common.pri )

TARGET = installer
DESTDIR = $${BIN_PATH}/
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += \
    main.cpp

RESOURCES_DIR = resources

RC_FILE += \
    $${RESOURCES_DIR}/AdminManifest.rc

build_all:!build_pass {
    CONFIG -= build_all
    CONFIG += release
}

LIBS += -lgdi32 -luser32
