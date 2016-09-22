#-------------------------------------------------
#
# Project created by QtCreator 2016-09-12T21:12:57
#
#-------------------------------------------------

QT       += core gui
CONFIG   += console

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Multifitting
TEMPLATE = app

# under windows
RC_FILE = icon.rc

SOURCES += main.cpp\
        launcher.cpp \
    settings.cpp \
    gui.cpp \
    multilayer_approach.cpp \
    optical_constants.cpp \
    multilayer/multilayer.cpp

HEADERS  += launcher.h \
    global_definitions.h \
    settings.h \
    gui.h \
    multilayer_approach.h \
    optical_constants.h \
    multilayer/multilayer.h
