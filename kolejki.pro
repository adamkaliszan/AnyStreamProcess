#-------------------------------------------------
#
# Project created by QtCreator 2014-05-09T16:23:27
#
#-------------------------------------------------

QT += core gui sql
QT += widgets
QT += charts
QT += datavisualization

QTPLUGIN += qsqlmysql

LIBS += -lboost_iostreams -lboost_system -lboost_filesystem

DEFINES +="INC_AGENDA_PERFORMANCE=1"

QMAKE_CXXFLAGS += -std=c++11
QMAKE_CXXFLAGS += -std=gnu++11
QMAKE_CXXFLAGS += -fext-numeric-literals

TRANSLATIONS = languages/kolejki_en.ts  languages/kolejki_pl.ts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = kolejki
TEMPLATE = app

INCLUDEPATH += 3rd/QtXlsxWriter/include/QtXlsx

SOURCES += main.cpp\
        mainwindow.cpp \
    model.cpp \
    dialogAbout.cpp \
    algorithms/investigator.cpp \
    algorithms/algorithmConvGamma3.cpp \
    algorithms/algorithmConvGamma2d.cpp \
    algorithms/algorithmHybridDiscr.cpp \
    algorithms/algorithmHybridDiscrDesc.cpp \
    algorithms/alg_cFIFO_hybrid.cpp \
    algorithms/alg_cFIFO_convGamma.cpp \
    algorithms/alg_noFIFO.cpp \
    algorithms/alg_FAG_AnyStream.cpp \
    algorithms/trclvector2.cpp \
    algorithms/algorithmConv.cpp \
    algorithms/simulatordatacollection.cpp \
    algorithms/simulator.cpp \
    algorithms/algorithmConv3d.cpp \
    algorithms/simulationParameters.cpp \
    results/resultsGnuplot.cpp \
    surfacegraph.cpp \
    utils/probDistributions.cpp \
    utils/lag.cpp \
    utils/vectorUtils.cpp \
    algorithms/algRekGeneral.cpp \
    algorithms/algRekLag.cpp \
    results/resultsSingle.cpp \
    results/resultsInvestigator.cpp \
    results/resultsSystem.cpp \
    algorithms/algRekLagGS.cpp \
    utils/utilsMisc.cpp \
    results/resultsSingleGroup.cpp \
    results/resultsApi.cpp \
    algorithms/simulatorAllSystems.cpp \
    algorithms/simulatorStatistics.cpp \
    dialogconfig.cpp \
    config.cpp

HEADERS  += mainwindow.h \
    model.h \
    dialogAbout.h \
    algorithms/algorithmConvGamma2d.h \
    algorithms/algorithmHybridDiscr.h \
    algorithms/algorithmHybridDiscrDesc.h \
    algorithms/alg_cFIFO_hybrid.h \
    algorithms/alg_cFIFO_convGamma.h \
    algorithms/alg_noFIFO.h \
    algorithms/alg_FAG_AnyStream.h \
    algorithms/investigator.h \
    algorithms/trclvector2.h \
    algorithms/algorithmConv.h \
    algorithms/simulatordatacollection.h \
    algorithms/simulator.h \
    algorithms/algorithmConv3d.h \
    algorithms/simulationParameters.h \
    algorithms/algorithmConvGamma3.h \
    gnuplot-iostream.h \
    results/resultsGnuplot.h \
    results/resultsSingle.h \
    surfacegraph.h \
    utils/lag.h \
    utils/probDistributions.h \
    utils/decimal.h \
    utils/vectorUtils.h \
    algorithms/algRekGeneral.h \
    algorithms/algRekLag.h \
    results/baseTypes.h \
    results/resultsInvestigator.h \
    results/resultsSystem.h \
    algorithms/algRekLagGS.h \
    results/resultsUtils.h \
    utils/utilsMisc.h \
    results/resultsSingleGroup.h \
    results/resultsSingleGroup.h \
    results/resultsApi.h \
    results/resultsApiTypes.h \
    algorithms/simulatorAllSystems.h \
    algorithms/simulatorStatistics.h \
    dialogconfig.h \
    config.h

FORMS    += mainwindow.ui \
    dialogAbout.ui \
    dialogconfig.ui \
    dialogconfig.ui

RESOURCES += \
    resources.qrc

DISTFILES += \
    languages/kolejki_pl.ts \
    languages/kolejki_en.ts

SUBDIRS += \
    Core/Core.pro
