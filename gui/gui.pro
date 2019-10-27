#-------------------------------------------------
#
# Project created by QtCreator 2014-05-09T16:23:27
#
#-------------------------------------------------

QT += core gui sql
QT += widgets
QT += charts
QT += datavisualization

DEFINES +="INC_AGENDA_PERFORMANCE=1"

QTPLUGIN += qsqlmysql

TEMPLATE = subdirs
TEMPLATE = app

CONFIG += create_prl
CONFIG += link_prl

LIBS += -lboost_iostreams -lboost_system -lboost_filesystem
LIBS += -L../core -lcore

INCLUDEPATH += $$PWD/../core

QMAKE_CXXFLAGS += -std=c++11
QMAKE_CXXFLAGS += -std=gnu++11
QMAKE_CXXFLAGS += -fext-numeric-literals

TRANSLATIONS = languages/kolejki_en.ts  languages/kolejki_pl.ts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = anyStreamInvestigatorGui


SOURCES += main.cpp\
        mainwindow.cpp \
    dialogAbout.cpp \
    dialogconfig.cpp \
    config.cpp

HEADERS  += mainwindow.h \
    dialogAbout.h \
    gnuplot-iostream.h \
    dialogconfig.h \
    config.h

FORMS    += mainwindow.ui \
    dialogAbout.ui \

    dialogconfig.ui

RESOURCES += \
    resources.qrc

DISTFILES += \
    languages/kolejki_pl.ts \
    languages/kolejki_en.ts


