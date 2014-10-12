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
        mainwindow.cpp \
    configwindow.cpp

LIBS += -lid3 -lz

HEADERS  += mainwindow.h \
    Song.h \
    configwindow.h \
    util/Singleton.h \
    util/Configuration.hh \

FORMS    += mainwindow.ui \
    configwindow.ui

RES_FILE += grabify.res

OTHER_FILES +=

RESOURCES += \
    grabify.qrc
