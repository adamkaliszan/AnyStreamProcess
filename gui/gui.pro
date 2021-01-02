#-------------------------------------------------
#
# Project created by QtCreator 2014-05-09T16:23:27
#
#-------------------------------------------------
TEMPLATE = app

QT += core gui sql
QT += widgets
QT += charts
QT += datavisualization

DEFINES +="INC_AGENDA_PERFORMANCE=1"

QTPLUGIN += qsqlmysql


CONFIG += create_prl
CONFIG += link_prl

LIBS += -lboost_iostreams -lboost_system -lboost_filesystem
LIBS += -L../core -lcore

#LIBS += -lpython3.8
LIBS += -lpython3.$$system(python3-config --includes | grep -Eo '[0-9]+$')

QMAKE_CXXFLAGS += -std=c++17
QMAKE_CXXFLAGS += -std=gnu++17
QMAKE_CXXFLAGS += -fext-numeric-literals
QMAKE_CXXFLAGS += $$system(python3-config --cflags)
QMAKE_LFLAGS += $$system(python3-config --cflags)


TRANSLATIONS = languages/kolejki_en.ts  languages/kolejki_pl.ts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

INCLUDEPATH += ../core
DEPENDPATH += ../core
SUBDIRS += ../core

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

