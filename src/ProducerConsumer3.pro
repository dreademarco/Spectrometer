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
    consumerunsafe.cpp

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
    consumerunsafe.h

FORMS    += mainwindow.ui

INCLUDEPATH += /usr/local/qwt-6.1.1/include #Added this
LIBS += -L/usr/local/qwt-6.1.1/lib -lqwt #Added this
