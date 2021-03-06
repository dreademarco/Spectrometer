#-------------------------------------------------
#
# Project created by QtCreator 2014-10-13T09:39:26
#
#-------------------------------------------------

QT       += core network gui

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
    fftwsequence.cpp \
    DoubleBuffer.cpp \
    PacketChunker.cpp \
    fftwsequencebuffer.cpp \
    ppf.cpp

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
    fftwsequence.h \
    DoubleBuffer.h \
    PacketChunker.h \
    fftwsequencebuffer.h \
    ppf.h

FORMS    += mainwindow.ui

QMAKE_CXXFLAGS+= -fopenmp -march=native
QMAKE_LFLAGS +=  -fopenmp -march=native

#QMAKE_CXXFLAGS+= -fopenmp -mavx2 -march=native
#QMAKE_LFLAGS +=  -fopenmp -mavx2 -march=native

INCLUDEPATH += /usr/local/qwt-6.1.1/include #Added this
LIBS += -L/usr/local/qwt-6.1.1/lib -lqwt #Added this

#LIBS += -lm -lfftw3f -lfftw3f_omp -lsndfile
LIBS += -lm -lfftw3f -lfftw3f_omp
