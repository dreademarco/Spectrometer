#-------------------------------------------------
#
# Project created by QtCreator 2014-10-13T09:39:26
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ProducerConsumer3
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    array2dworker.cpp \
    circulararray2dworker.cpp \
    spectrogramdata.cpp \
    spectrogramplot.cpp \
    customcolormap.cpp \
    circulararray2dworkersafe.cpp \
    producerunsafe.cpp \
    consumerunsafe.cpp \
    circulararray2dworkerthreaded.cpp \
    array2dspectrum.cpp \
    circulararray2dspectrum.cpp \
    circulararray2dspectrumthreaded.cpp \
    pipeline.cpp \
    plotter.cpp \
    framedata.cpp \
    framedatacomplex.cpp \
    ppf.cpp

HEADERS  += mainwindow.h \
    array2dworker.h \
    circulararray2dworker.h \
    colormap.h \
    common.h \
    myConstants.h \
    spectrogramdata.h \
    spectrogramplot.h \
    customcolormap.h \
    circulararray2dworkersafe.h \
    producerunsafe.h \
    consumerunsafe.h \
    circulararray2dworkerthreaded.h \
    array2dspectrum.h \
    circulararray2dspectrum.h \
    circulararray2dspectrumthreaded.h \
    pipeline.h \
    plotter.h \
    framedata.h \
    framedatacomplex.h \
    ppf.h

FORMS    += mainwindow.ui

QMAKE_CXXFLAGS+= -fopenmp
QMAKE_LFLAGS +=  -fopenmp

# remove possible other optimization flags
QMAKE_CXXFLAGS_RELEASE -= -O
QMAKE_CXXFLAGS_RELEASE -= -O1
QMAKE_CXXFLAGS_RELEASE -= -O3
# add the desired -O3 if not present
QMAKE_CXXFLAGS_RELEASE *= -O2
QMAKE_FLAGS_RELEASE += -msse4.1 -mssse3 -msse3 -msse2 -msse
QMAKE_CXXFLAGS_RELEASE += -msse4.1 -mssse3 -msse3 -msse2 -msse

INCLUDEPATH += /usr/local/qwt-6.1.1/include #Added this
LIBS += -L/usr/local/qwt-6.1.1/lib -lqwt #Added this

LIBS += -lm -lfftw3f -lfftw3f_omp
#LIBS += -lm -lfftw3 -lfftw3_omp
