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
QMAKE_CXXFLAGS += -bigobj

INCLUDEPATH +=	"C:/Program Files (x86)/C++ libraries" \
		"C:/Program Files (x86)/C++ libraries/GSL 2.4 Bruot/include" \
#		"C:/Program Files (x86)/C++ libraries/eigen 3.3.1" \
		"C:/Program Files (x86)/C++ libraries/SwarmOps/include" \
		"C:/Program Files (x86)/C++ libraries/RandomOps/include"

LIBS += "C:/Program Files (x86)/C++ libraries/GSL 2.4 Bruot/lib/gsl/cblas.lib" \
		"C:/Program Files (x86)/C++ libraries/GSL 2.4 Bruot/lib/gsl/gsl.lib" \
		"C:/Program Files (x86)/C++ libraries/SwarmOps/SwarmOps.lib" \
		"C:/Program Files (x86)/C++ libraries/RandomOps/RandomOps.lib"
}
unix {
LIBS += -lgsl -lgslcblas -lSwarmRandOps

QMAKE_CXXFLAGS += -Wno-reorder
}

SOURCES += \
	launcher.cpp \
	main.cpp \
	calculation/fitting/fitting.cpp \
	calculation/fitting/fitting_swarmops.cpp \
	calculation/fitting/fitting_gsl.cpp \
    calculation/unwrapped/unwrapped_reflection.cpp \
    calculation/unwrapped/unwrapped_structure.cpp \
    calculation/calculation_tree.cpp \
    calculation/main_calculation_module.cpp \
    calculation/node.cpp \
    calculation/optical_constants.cpp \
	global/global_definitions.cpp \
    global/global_variables.cpp \
    global/layer_data_types.cpp \
	global/settings.cpp \
	multilayer_approach/fitting_settings/fitting_settings_editor.cpp \
	multilayer_approach/fitting_settings/fitting_settings.cpp \
	multilayer_approach/fits_selector/fits_selector.cpp \
	multilayer_approach/calculation_settings_editor/calculation_settings_editor.cpp \
	multilayer_approach/multilayer/independent_variables/independent_calc_function_selection.cpp \
    multilayer_approach/multilayer/independent_variables/independent_variables.cpp \
    multilayer_approach/multilayer/independent_variables/independent_variables_editor.cpp \
    multilayer_approach/multilayer/independent_variables/variable_selection.cpp \
    multilayer_approach/multilayer/structure_tree/grading_editor.cpp \
    multilayer_approach/multilayer/structure_tree/item_editor.cpp \
    multilayer_approach/multilayer/structure_tree/structure_toolbar.cpp \
    multilayer_approach/multilayer/structure_tree/structure_tree.cpp \
    multilayer_approach/multilayer/target_curve/target_curve.cpp \
    multilayer_approach/multilayer/target_curve/target_curve_editor.cpp \
    multilayer_approach/multilayer/multilayer.cpp \
    multilayer_approach/table_of_structures/coupling_editor.cpp \
    multilayer_approach/table_of_structures/my_table_widget.cpp \
    multilayer_approach/table_of_structures/table_of_structures.cpp \
    multilayer_approach/multilayer_approach.cpp \
	standard/menu.cpp \
	standard/spoiler.cpp


HEADERS += \
	launcher.h \
    calculation/unwrapped/unwrapped_reflection.h \
    calculation/unwrapped/unwrapped_structure.h \
    calculation/calculation_tree.h \
    calculation/main_calculation_module.h \
    calculation/node.h \
    calculation/optical_constants.h \
    global/global_definitions.h \
    global/global_variables.h \
    global/layer_data_types.h \
    global/settings.h \
	calculation/fitting/fitting.h \
	calculation/fitting/fitting_swarmops.h \
	calculation/fitting/fitting_gsl.h \
	multilayer_approach/fits_selector/fits_selector.h \
	multilayer_approach/fitting_settings/fitting_settings_editor.h \
	multilayer_approach/fitting_settings/fitting_settings.h \
	multilayer_approach/calculation_settings_editor/calculation_settings_editor.h \
	multilayer_approach/multilayer/independent_variables/independent_calc_function_selection.h \
    multilayer_approach/multilayer/independent_variables/independent_variables.h \
    multilayer_approach/multilayer/independent_variables/independent_variables_editor.h \
    multilayer_approach/multilayer/independent_variables/variable_selection.h \
    multilayer_approach/multilayer/structure_tree/grading_editor.h \
    multilayer_approach/multilayer/structure_tree/item_editor.h \
    multilayer_approach/multilayer/structure_tree/structure_toolbar.h \
    multilayer_approach/multilayer/structure_tree/structure_tree.h \
    multilayer_approach/multilayer/target_curve/target_curve.h \
    multilayer_approach/multilayer/target_curve/target_curve_editor.h \
    multilayer_approach/multilayer/multilayer.h \
    multilayer_approach/table_of_structures/coupling_editor.h \
    multilayer_approach/table_of_structures/my_table_widget.h \
    multilayer_approach/table_of_structures/table_of_structures.h \
    multilayer_approach/multilayer_approach.h \
    standard/menu.h \
	standard/spoiler.h
