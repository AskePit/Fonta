QT += core gui

CONFIG += c++11

TARGET = font_sampler
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    sampler.cpp

HEADERS += \
    sampler.h
