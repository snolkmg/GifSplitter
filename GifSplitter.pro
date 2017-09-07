#-------------------------------------------------
#
# Project created by QtCreator 2017-09-07T14:34:24
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GifSplitter
TEMPLATE = app
CONFIG += c++11


SOURCES += main.cpp\
        GifSplitter.cpp \
    splitWorker.cpp

HEADERS  += GifSplitter.h \
    splitWorker.h

RC_ICONS += gif.ico
