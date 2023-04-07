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
    QMAKE_LFLAGS += '-Wl,--disable-new-dtags,-rpath,\'\$$ORIGIN/lib\''
}

win32 {
    RC_ICONS = icon.ico
}

# recursive search for source files
SOURCES +=  $$files(*.cpp, true) \
            $$files(*.cc, true)

HEADERS +=  $$files(*.h, true)  \
            $$files(*.hh, true) \
            $$files(*.hpp, true)
