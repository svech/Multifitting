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
    multilayer_approach.cpp \
    multilayer/multilayer.cpp \
    global_definitions.cpp \
    multilayer/layer_content.cpp \
    multilayer/item_editing.cpp

HEADERS  += launcher.h \
    multilayer_approach.h \
    multilayer/multilayer.h \
    global_definitions.h \
    multilayer/layer_content.h \
    multilayer/item_editing.h
