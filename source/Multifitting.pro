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

win32 {
   	INCLUDEPATH +=  "C:/Program Files (x86)/C++ libraries" \
              		"C:/Program Files (x86)/C++ libraries/GSL 2.1/include" \
                	"C:/Program Files (x86)/C++ libraries/eigen 3.3.1"

    	LIBS += "C:/Program Files (x86)/C++ libraries/GSL 2.1/lib_VS2015/x64/gslcblas.lib" \
            	"C:/Program Files (x86)/C++ libraries/GSL 2.1/lib_VS2015/x64/gsl.lib"
}
unix {
	LIBS += -lgsl -lgslcblas
	QMAKE_CXXFLAGS += -Wno-reorder
}

SOURCES += main.cpp\	
launcher.cpp \
global/global_definitions.cpp \
global/global_variables.cpp \
global/layer_data_types.cpp \
global/settings.cpp \
standard/menu.cpp \
multilayer_approach.cpp \
multilayer_approach/multilayer.cpp \
multilayer_approach/multilayer/structure_tree.cpp \
multilayer_approach/multilayer/structure_tree/item_editor.cpp \
multilayer_approach/multilayer/structure_tree/grading_editor.cpp \
multilayer_approach/multilayer/structure_tree/structure_toolbar.cpp \
multilayer_approach/multilayer/independent_variables.cpp \
multilayer_approach/multilayer/independent_variables/independent_variables_editor.cpp \
multilayer_approach/multilayer/variable_selection.cpp \
#    	main_calculation_module.cpp \
#	calculation/calculation_tree.cpp \
#    	calculation/node.cpp \
calculation/optical_constants.cpp \
#    	calculation/unwrapped/unwrapped_reflection.cpp \
#    	calculation/unwrapped/unwrapped_structure.cpp \
multilayer_approach/table_of_structures.cpp \
multilayer_approach/multilayer/target_curve.cpp \
multilayer_approach/multilayer/target_curve/target_curve_editor.cpp \
#multilayer_approach/table_of_structures/my_table_widget.cpp \
#   	multilayer_approach/table_of_structures/coupling_editor.cpp

HEADERS  += launcher.h \
global/global_definitions.h \
global/layer_data_types.h \
global/global_variables.h \
global/settings.h \
standard/menu.h \
multilayer_approach.h \
multilayer_approach/multilayer.h \
multilayer_approach/multilayer/structure_tree.h \
multilayer_approach/multilayer/structure_tree/item_editor.h \
multilayer_approach/multilayer/structure_tree/structure_toolbar.h \
multilayer_approach/multilayer/structure_tree/grading_editor.h \
multilayer_approach/multilayer/independent_variables.h \
multilayer_approach/multilayer/independent_variables/independent_variables_editor.h \
multilayer_approach/multilayer/variable_selection.h \
#   	main_calculation_module.h \
#   	calculation/calculation_tree.h \
#   	calculation/node.h \
calculation/optical_constants.h \
#   	calculation/unwrapped/unwrapped_reflection.h \
#   	calculation/unwrapped/unwrapped_structure.h \
multilayer_approach/table_of_structures.h \
multilayer_approach/multilayer/target_curve.h \
multilayer_approach/multilayer/target_curve/target_curve_editor.h \
#multilayer_approach/table_of_structures/my_table_widget.h \
#    	multilayer_approach/table_of_structures/coupling_editor.h
