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

INCLUDEPATH +=  "3rdparty" \
                "3rdparty/Faddeeva" \
                "3rdparty/QCustomPlot"
win32 {

    INCLUDEPATH +=  "C:/Program Files (x86)/C++ libraries/boost_1_78_0"
    QMAKE_CXXFLAGS += -bigobj
    equals(QMAKE_TARGET.arch, x86) {
                message( "It's 32-bit build" )
                INCLUDEPATH += "C:/Program Files (x86)/C++ libraries/GSL 2.2 Bruot/include"
#			       "C:/Program Files (x86)/C++ libraries/GSL 2.7.1/include"
                       LIBS += "C:/Program Files (x86)/C++ libraries/GSL 2.2 Bruot/msvc2015_32/cblas.lib" \
                               "C:/Program Files (x86)/C++ libraries/GSL 2.2 Bruot/msvc2015_32/gsl.lib"
#                              "C:/Program Files (x86)/C++ libraries/GSL 2.7.1/msvc2019_32/gslcblas.lib" \
#                              "C:/Program Files (x86)/C++ libraries/GSL 2.7.1/msvc2019_32/gsl.lib"
    } else {
    equals(QMAKE_TARGET.arch, x86_64) {
		    message( "It's 64-bit build" )
			INCLUDEPATH += "C:/Program Files (x86)/C++ libraries/GSL 2.4 Bruot/include"
#				       "C:/Program Files (x86)/C++ libraries/GSL 2.7.1/include"
                            LIBS += "C:/Program Files (x86)/C++ libraries/GSL 2.4 Bruot/msvc2015_64/cblas.lib" \
                                    "C:/Program Files (x86)/C++ libraries/GSL 2.4 Bruot/msvc2015_64/gsl.lib"
#                                   "C:/Program Files (x86)/C++ libraries/GSL 2.7.1/msvc2019_64/gslcblas.lib" \
#                                   "C:/Program Files (x86)/C++ libraries/GSL 2.7.1/msvc2019_64/gsl.lib" \
#                                   "C:/Program Files (x86)/C++ libraries/Faddeeva Package/msvc2019_64/Faddeeva.lib" \
#                                   "C:/Program Files (x86)/C++ libraries/QCustomPlot 2.0.1/qt_5.12.0/msvc2017_64/qcustomplot.lib"
        }
    }
}

unix {
    LIBS += -lgsl -lgslcblas
    QMAKE_CXXFLAGS += -Wno-reorder
}

win32 {
    RC_ICONS = icon.ico
}

# recursive search for source files
SOURCES +=  $$files(*.cpp, true) \
            $$files(*.c, true)

HEADERS +=  $$files(*.h, true)  \
            $$files(*.hh, true) \
            $$files(*.hpp, true)
