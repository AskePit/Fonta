QT += core
QT -= gui

CONFIG += c++11

TARGET = fonts_cleaner
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp

RESOURCES_DIR = resources

RC_FILE += \
    $${RESOURCES_DIR}/AdminManifest.rc

build_all:!build_pass {
    CONFIG -= build_all
    CONFIG += release
}

QMAKE_CXXFLAGS += -O2
