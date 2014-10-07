#-------------------------------------------------
#
# Project created by QtCreator 2014-10-02T18:09:59
#
#-------------------------------------------------

QT       += core gui webkitwidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = grabify5
TEMPLATE = app

CONFIG += c++11
SOURCES += main.cpp\
        mainwindow.cpp

LIBS += libid3.a -lz

HEADERS  += mainwindow.h \
    Song.h \
    util/Singleton.h \
    util/Configuration.hh \


FORMS    += mainwindow.ui
