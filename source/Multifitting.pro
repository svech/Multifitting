#-------------------------------------------------
#
# Project created by QtCreator 2016-10-18T21:29:16
#
#-------------------------------------------------

QT       += core gui
CONFIG   += console c++17

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = Multifitting
TEMPLATE = app

#message( $$QMAKE_TARGET.arch )

INCLUDEPATH +=  3rdparty \
                3rdparty/Faddeeva \
                3rdparty/QCustomPlot

# requires cloned repositories
INCLUDEPATH +=  ../../RandomOps \
                ../../SwarmOps

win32 {
    INCLUDEPATH +=  "C:/Program Files (x86)/C++ libraries/boost_1_81_0"

    # requires prebuilt libs
    LIBS += ../../RandomOps/build/release/RandomOps.lib \
            ../../SwarmOps/build/release/SwarmOps.lib

    QMAKE_CXXFLAGS += -bigobj
    equals(QMAKE_TARGET.arch, x86) {
                message( "It's 32-bit build" )
                    INCLUDEPATH += "C:/Program Files (x86)/C++ libraries/GSL 2.2 Bruot/include"
                        LIBS += "C:/Program Files (x86)/C++ libraries/GSL 2.2 Bruot/msvc2015_32/cblas.lib" \
                               "C:/Program Files (x86)/C++ libraries/GSL 2.2 Bruot/msvc2015_32/gsl.lib"
    } else {
    equals(QMAKE_TARGET.arch, x86_64) {
                message( "It's 64-bit build" )
                    INCLUDEPATH += "C:/Program Files (x86)/C++ libraries/GSL 2.4 Bruot/include"
                        LIBS += "C:/Program Files (x86)/C++ libraries/GSL 2.4 Bruot/msvc2015_64/cblas.lib" \
                                    "C:/Program Files (x86)/C++ libraries/GSL 2.4 Bruot/msvc2015_64/gsl.lib"
        }
    }
}

unix {
    INCLUDEPATH +=  ../../GSL/gsl_2.7.1/build/include
#    LIBS += -lgsl -lgslcblas # dynamic linking
    LIBS += -L$$PWD/../../GSL/gsl_2.7.1/build/lib_static -lgsl -lgslcblas
    LIBS += -L$$PWD/../../SwarmOps/build -lSwarmOps
    LIBS += -L$$PWD/../../RandomOps/build -lRandomOps
    QMAKE_CXXFLAGS += -Wno-reorder
    QMAKE_LFLAGS += '-Wl,--disable-new-dtags,-rpath,\'\$$ORIGIN/libs\''
}

win32 {
    RC_ICONS = icon.ico
}

SOURCES += \
    3rdparty/QCustomPlot/qcustomplot.cpp \
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
    multilayer_approach/profile_plots/profile_plot.cpp \
    multilayer_approach/profile_plots/profile_plots_window.cpp \
    multilayer_approach/table_of_roughness/table_of_roughness.cpp \
    multilayer_approach/table_of_structures/coupling_editor.cpp \
    multilayer_approach/table_of_structures/confidence_interval_editor.cpp \
    multilayer_approach/table_of_structures/my_table_widget.cpp \
    multilayer_approach/table_of_structures/table_of_structures.cpp \
    multilayer_approach/multilayer_approach.cpp \
    standard/menu.cpp \
    standard/spoiler.cpp \
    multilayer_approach/multilayer/target_curve/target_curve_plot.cpp \
    multilayer_approach/graphs/optical_graphs.cpp \
    multilayer_approach/graphs/curve_plot.cpp \
    multilayer_approach/table_of_structures/layer_thickness_transfer.cpp \
    standard/mydoublespinbox.cpp \
    multilayer_approach/multilayer/structure_tree/aperiodic_load_setup.cpp \
    multilayer_approach/multilayer/structure_tree/regular_aperiodic_table.cpp \
    multilayer_approach/multilayer/structure_tree/regular_aperiodic_table_widget.cpp


HEADERS += \
    3rdparty/tree.hh \
    3rdparty/exprtk.hpp \
    3rdparty/QCustomPlot/qcustomplot.h \
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
    multilayer_approach/profile_plots/profile_plot.h \
    multilayer_approach/profile_plots/profile_plots_window.h \
    multilayer_approach/table_of_roughness/table_of_roughness.h \
    multilayer_approach/table_of_structures/coupling_editor.h \
    multilayer_approach/table_of_structures/confidence_interval_editor.h \
    multilayer_approach/table_of_structures/my_table_widget.h \
    multilayer_approach/table_of_structures/table_of_structures.h \
    multilayer_approach/multilayer_approach.h \
    standard/menu.h \
    standard/spoiler.h \
    multilayer_approach/multilayer/target_curve/target_curve_plot.h \
    multilayer_approach/graphs/optical_graphs.h \
    multilayer_approach/graphs/curve_plot.h \
    multilayer_approach/table_of_structures/layer_thickness_transfer.h \
    standard/mydoublespinbox.h \
    multilayer_approach/multilayer/structure_tree/aperiodic_load_setup.h \
    multilayer_approach/multilayer/structure_tree/regular_aperiodic_table.h \
    multilayer_approach/multilayer/structure_tree/regular_aperiodic_table_widget.h

DISTFILES += \
    ../cppcheck-suppressions-list.txt
