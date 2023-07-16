#-------------------------------------------------
#
# Project created by QtCreator 2016-10-18T21:29:16
#
#-------------------------------------------------

QT       += core gui
CONFIG   += console c++17

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = multifitting
TEMPLATE = app

#message( $$QMAKE_TARGET.arch )

INCLUDEPATH +=  3rdparty \
                3rdparty/Faddeeva \
                3rdparty/QCustomPlot

# requires cloned repositories
INCLUDEPATH +=  ../../RandomOps \
                ../../SwarmOps

win32 {
    INCLUDEPATH +=  "C:\Users\Mikhail\lib\boost_1_82_0"

    QMAKE_CXXFLAGS += -bigobj
    equals(QMAKE_TARGET.arch, x86) {
                message( "It's 32-bit build" )
                    LIBS += ../../RandomOps/build_x86/release/RandomOps.lib \
                            ../../SwarmOps/build_x86/release/SwarmOps.lib
                    INCLUDEPATH += "C:\Users\Mikhail\vcpkg\installed\x86-windows\include" #gsl
                    LIBS += "C:\Users\Mikhail\vcpkg\installed\x86-windows\lib\gslcblas.lib" \
                            "C:\Users\Mikhail\vcpkg\installed\x86-windows\lib\gsl.lib"

#                    INCLUDEPATH += "C:/Program Files (x86)/C++ libraries/GSL 2.2 Bruot/include"
#                        LIBS += "C:/Program Files (x86)/C++ libraries/GSL 2.2 Bruot/msvc2015_32/cblas.lib" \
#                               "C:/Program Files (x86)/C++ libraries/GSL 2.2 Bruot/msvc2015_32/gsl.lib"
    } else {
    equals(QMAKE_TARGET.arch, x86_64) {
                message( "It's 64-bit build" )
                    LIBS += ../../RandomOps/build_x64/release/RandomOps.lib \
                            ../../SwarmOps/build_x64/release/SwarmOps.lib
                    INCLUDEPATH +=  "C:\Users\Mikhail\vcpkg\installed\x64-windows\include" # gsl
                    LIBS += "C:\Users\Mikhail\vcpkg\installed\x64-windows\lib\gslcblas.lib" \
                            "C:\Users\Mikhail\vcpkg\installed\x64-windows\lib\gsl.lib"

#                    INCLUDEPATH += "C:/Program Files (x86)/C++ libraries/GSL 2.4 Bruot/include"
#                        LIBS += "C:/Program Files (x86)/C++ libraries/GSL 2.4 Bruot/msvc2015_64/cblas.lib" \
#                                    "C:/Program Files (x86)/C++ libraries/GSL 2.4 Bruot/msvc2015_64/gsl.lib"
        }
    }
}

unix {
    INCLUDEPATH +=  ../../boost_1_81_0
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
