#-------------------------------------------------
#
# Project created by QtCreator 2019-10-26T15:03:05
#
#-------------------------------------------------

TEMPLATE = lib

DEFINES +="INC_AGENDA_PERFORMANCE=1"

QT += charts
QT += datavisualization

CONFIG += staticlib

SOURCES += \
    model.cpp \
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
    algorithms/simulatorAllSystems.cpp \
    algorithms/simulatorStatistics.cpp \
    algorithms/algRekLagGS.cpp \
    algorithms/algRekGeneral.cpp \
    algorithms/algRekLag.cpp \
    results/resultsGnuplot.cpp \
    results/resultsSingle.cpp \
    results/resultsInvestigator.cpp \
    results/resultsSystem.cpp \
    results/resultsSingleGroup.cpp \
    results/resultsApi.cpp \
    utils/probDistributions.cpp \
    utils/lag.cpp \
    utils/vectorUtils.cpp \
    utils/utilsMisc.cpp \


HEADERS += \
    model.h \
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
    algorithms/algRekGeneral.h \
    algorithms/algRekLag.h \
    algorithms/algRekLagGS.h \
    algorithms/simulatorAllSystems.h \
    algorithms/simulatorStatistics.h \
    results/resultsGnuplot.h \
    results/resultsSingle.h \
    results/resultsUtils.h \
    results/baseTypes.h \
    results/resultsInvestigator.h \
    results/resultsSystem.h \
    results/resultsSingleGroup.h \
    results/resultsSingleGroup.h \
    results/resultsApi.h \
    results/resultsApiTypes.h \
    utils/lag.h \
    utils/probDistributions.h \
    utils/decimal.h \
    utils/vectorUtils.h \
    utils/utilsMisc.h \

