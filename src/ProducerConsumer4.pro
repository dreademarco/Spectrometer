#-------------------------------------------------
#
# Project created by QtCreator 2014-10-13T09:39:26
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ProducerConsumer4
TEMPLATE = app


SOURCES += main.cpp\
    mainwindow.cpp \
    spectrogramdata.cpp \
    spectrogramplot.cpp \
    customcolormap.cpp \
    pipeline.cpp \
    plotter.cpp \
    framedata.cpp \
    framedatacomplex.cpp \
    streamingppf.cpp \
    fftwsequence.cpp \
    fftwsequencecircular.cpp \
    fftwsequencecircularthreaded.cpp

HEADERS  += mainwindow.h \
    colormap.h \
    common.h \
    myConstants.h \
    spectrogramdata.h \
    spectrogramplot.h \
    customcolormap.h \
    pipeline.h \
    plotter.h \
    framedata.h \
    framedatacomplex.h \
    streamingppf.h \
    fftwsequence.h \
    fftwsequencecircular.h \
    fftwsequencecircularthreaded.h

FORMS    += mainwindow.ui

QMAKE_CXXFLAGS+= -fopenmp -march=native
QMAKE_LFLAGS +=  -fopenmp -march=native

#QMAKE_CXXFLAGS+= -fopenmp -mavx2 -march=native
#QMAKE_LFLAGS +=  -fopenmp -mavx2 -march=native

INCLUDEPATH += /usr/local/qwt-6.1.1/include #Added this
LIBS += -L/usr/local/qwt-6.1.1/lib -lqwt #Added this

LIBS += -lm -lfftw3f -lfftw3f_omp
#LIBS += -lm -lfftw3 -lfftw3_omp
