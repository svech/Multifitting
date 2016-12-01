#-------------------------------------------------
#
# Project created by QtCreator 2016-10-18T21:29:16
#
#-------------------------------------------------

QT       += core gui
CONFIG   += console

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Multifitting
TEMPLATE = app

INCLUDEPATH += "C:/Program Files (x86)/C++ libraries" \
               "C:/Program Files (x86)/C++ libraries/GSL 2.1/include"

LIBS += "C:/Program Files (x86)/C++ libraries/GSL 2.1/lib_VS2015/x64/gslcblas.lib" \
        "C:/Program Files (x86)/C++ libraries/GSL 2.1/lib_VS2015/x64/gsl.lib"

SOURCES += main.cpp\
        launcher.cpp \
    global/global_definitions.cpp \
    global/settings.cpp \
    global/global_variables.cpp \
    standard/menu.cpp \
    multilayer_approach.cpp \
    multilayer_approach/multilayer.cpp \
    multilayer_approach/multilayer/structure_tree.cpp \
    global/layer_data_types.cpp \
    multilayer_approach/multilayer/structure_tree/item_editor.cpp \
    multilayer_approach/multilayer/structure_tree/structure_toolbar.cpp \
    multilayer_approach/multilayer/independent_variables.cpp \
    multilayer_approach/multilayer/independent_variables/independent_variables_editor.cpp \
    multilayer_approach/multilayer/variable_selection.cpp \
    calculation/optical_constants.cpp \
    main_calculation_module.cpp \
    calculation/calculation_tree.cpp \
    calculation/node.cpp

HEADERS  += launcher.h \
    global/global_definitions.h \
    global/settings.h \
    global/global_variables.h \
    standard/menu.h \
    multilayer_approach.h \
    multilayer_approach/multilayer.h \
    multilayer_approach/multilayer/structure_tree.h \
    global/layer_data_types.h \
    multilayer_approach/multilayer/structure_tree/item_editor.h \
    multilayer_approach/multilayer/structure_tree/structure_toolbar.h \
    multilayer_approach/multilayer/independent_variables.h \
    multilayer_approach/multilayer/independent_variables/independent_variables_editor.h \
    multilayer_approach/multilayer/variable_selection.h \
    calculation/optical_constants.h \
    main_calculation_module.h \
    calculation/calculation_tree.h \
    calculation/node.h

