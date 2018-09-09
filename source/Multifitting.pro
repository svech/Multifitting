#-------------------------------------------------
#
# Project created by QtCreator 2016-10-18T21:29:16
#
#-------------------------------------------------

QT       += core gui
CONFIG   += console

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = Multifitting
TEMPLATE = app

message( $$QMAKE_TARGET.arch )

win32 {

    INCLUDEPATH +=	"C:/Program Files (x86)/C++ libraries" \
                    "C:/Program Files (x86)/C++ libraries/SwarmOps/include" \
                    "C:/Program Files (x86)/C++ libraries/RandomOps/include" \
                    "C:/Program Files (x86)/C++ libraries/QCustomPlot 2.0.1"

#---MinGW---MinGW---MinGW---MinGW---MinGW---MinGW---MinGW---MinGW---
# DOESN'T WORK
    win32-g++ {
        message( "It's mingw" )
#        equals(QMAKE_TARGET.arch, x86) {
            message( "It's mingw x86" )
            INCLUDEPATH += "C:/Program Files (x86)/C++ libraries/GSL 2.2.1 mingw/include"
#            LIBS += -L "C:/Program Files (x86)/C++ libraries/RandomOps/mingw_32/" -lRandomOps \
#                    -L "C:/Program Files (x86)/C++ libraries/SwarmOps/mingw_32/" -lSwarmOps \
#                    -L "C:/Program Files (x86)/C++ libraries/GSL 2.2.1 mingw/" -lgslcblas \
#                    -L "C:/Program Files (x86)/C++ libraries/GSL 2.2.1 mingw/" -lgsl \
#                    -L "C:/Program Files (x86)/C++ libraries/QCustomPlot 2.0.0/qt_5.10.1/mingw_32/" -lqcustomplot
#            LIBS += -L"$$_PRO_FILE_PWD_/" -lRandomOps -lSwarmOps -lgslcblas -lgsl -lqcustomplot
#        }
    } else {
#---MSVC---MSVC---MSVC---MSVC---MSVC---MSVC---MSVC---MSVC---MSVC---
        QMAKE_CXXFLAGS += -bigobj

        win32-msvc2015 {
            equals(QMAKE_TARGET.arch, x86) {
                message( "It's msvc2015_32" )
                INCLUDEPATH += "C:/Program Files (x86)/C++ libraries/GSL 2.2 Bruot/include"
                LIBS += "C:/Program Files (x86)/C++ libraries/RandomOps/msvc2015_32/RandomOps.lib" \
                        "C:/Program Files (x86)/C++ libraries/SwarmOps/msvc2015_32/SwarmOps.lib" \
                        "C:/Program Files (x86)/C++ libraries/GSL 2.2 Bruot/msvc2015_32/cblas.lib" \
                        "C:/Program Files (x86)/C++ libraries/GSL 2.2 Bruot/msvc2015_32/gsl.lib" \
                        "C:/Program Files (x86)/C++ libraries/QCustomPlot 2.0.0/qt_5.10.1/msvc2015_32/qcustomplot.lib"
            }

            equals(QMAKE_TARGET.arch, x86_64) {
                message( "It's msvc2015_64" )
                INCLUDEPATH += "C:/Program Files (x86)/C++ libraries/GSL 2.4 Bruot/include"
                LIBS += "C:/Program Files (x86)/C++ libraries/RandomOps/msvc2015_64/RandomOps.lib" \
                        "C:/Program Files (x86)/C++ libraries/SwarmOps/msvc2015_64/SwarmOps.lib" \
                        "C:/Program Files (x86)/C++ libraries/GSL 2.4 Bruot/msvc2015_64/cblas.lib" \
                        "C:/Program Files (x86)/C++ libraries/GSL 2.4 Bruot/msvc2015_64/gsl.lib" \
                        "C:/Program Files (x86)/C++ libraries/QCustomPlot 2.0.0/qt_5.10.1/msvc2015_64/qcustomplot.lib"
            }
        } else {

            message( "It's msvc2017" )
            message( "It's x64" )
            INCLUDEPATH += "C:/Program Files (x86)/C++ libraries/GSL 2.4 Bruot/include"
            LIBS += "C:/Program Files (x86)/C++ libraries/RandomOps/msvc2017_64/RandomOps.lib" \
                    "C:/Program Files (x86)/C++ libraries/SwarmOps/msvc2017_64/SwarmOps.lib" \
                    "C:/Program Files (x86)/C++ libraries/GSL 2.4 Bruot/msvc2015_64/cblas.lib" \
                    "C:/Program Files (x86)/C++ libraries/GSL 2.4 Bruot/msvc2015_64/gsl.lib" \
                    "C:/Program Files (x86)/C++ libraries/QCustomPlot 2.0.1/qt_5.11.1/msvc2017_64/qcustomplot.lib"
        }
    }
}

unix {
    LIBS += -lgsl -lgslcblas -lSwarmRandOps -lqcustomplot
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
	multilayer_approach/table_of_structures/confidence_interval_editor.cpp \
	multilayer_approach/table_of_structures/my_table_widget.cpp \
    multilayer_approach/table_of_structures/table_of_structures.cpp \
    multilayer_approach/multilayer_approach.cpp \
	standard/menu.cpp \
	standard/spoiler.cpp \
    multilayer_approach/multilayer/target_curve/target_curve_plot.cpp \
    multilayer_approach/graphs/optical_graphs.cpp \
    multilayer_approach/graphs/curve_plot.cpp


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
	multilayer_approach/table_of_structures/confidence_interval_editor.h \
	multilayer_approach/table_of_structures/my_table_widget.h \
    multilayer_approach/table_of_structures/table_of_structures.h \
    multilayer_approach/multilayer_approach.h \
    standard/menu.h \
	standard/spoiler.h \
    multilayer_approach/multilayer/target_curve/target_curve_plot.h \
    multilayer_approach/graphs/optical_graphs.h \
    multilayer_approach/graphs/curve_plot.h

DISTFILES += \
    ../cppcheck-suppressions-list.txt

# add PVS-studio checker
pvs_studio.target = pvs
pvs_studio.output = true
pvs_studio.cxxflags = -std=c++11
pvs_studio.sources = $${SOURCES}
include(../PVS-Studio.pri)
