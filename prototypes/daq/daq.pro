#-------------------------------------------------
#
# Project created by QtCreator 2015-01-14T10:15:19
#
#-------------------------------------------------

QT       += core network

QT       -= gui

TARGET = daq
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += \
    DoubleBuffer.cpp \
    pipeline.cpp \
    PacketChunker.cpp

HEADERS += \
    DoubleBuffer.h \
    PacketChunker.h
