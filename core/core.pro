#-------------------------------------------------
#
# Project created by QtCreator 2019-10-26T15:03:05
#
#-------------------------------------------------

TEMPLATE = lib

DEFINES +="INC_AGENDA_PERFORMANCE=1"

CONFIG += staticlib

QMAKE_CXXFLAGS += -std=c++11
QMAKE_CXXFLAGS += -std=gnu++11
QMAKE_CXXFLAGS += -fext-numeric-literals
QMAKE_CXXFLAGS += -I/usr/include/python3.8 -I/usr/include/python3.8  -Wno-unused-result -Wsign-compare -g -fdebug-prefix-map=/build/python3.8-xhc1jt/python3.8-3.8.5=. -specs=/usr/share/dpkg/no-pie-compile.specs -fstack-protector -Wformat -Werror=format-security  -DNDEBUG -g -fwrapv -O3 -Wall

LIBS += -lpython3.8
LIBS += -L/usr/lib/python3.8/config-3.8-x86_64-linux-gnu -L/usr/lib -lcrypt -lpthread -ldl  -lutil -lm -lm

INCLUDEPATH += /usr/include/python3.8/

SOURCES += \
    algorithms/FAG_AnyStr_ML.cpp \
    algorithms/FAG_AnyStr_hybrid.cpp \
    algorithms/FAG_hybrid.cpp \
    algorithms/LAG_reccurence.cpp \
    algorithms/LAG_reccurenceGS.cpp \
    algorithms/cFIFO_convGamma.cpp \
    algorithms/cFIFO_hybrid.cpp \
    algorithms/conv.cpp \
    algorithms/conv3d.cpp \
    algorithms/convGamma2d.cpp \
    algorithms/convGamma3.cpp \
    algorithms/dFIFO_hybridDesc.cpp \
    algorithms/hybridDiscr.cpp \
    algorithms/reccurenceGeneral.cpp \
    model.cpp \
    algorithms/investigator.cpp \
    algorithms/trclvector2.cpp \
    algorithms/simulatordatacollection.cpp \
    algorithms/simulator.cpp \
    algorithms/simulationParameters.cpp \
    algorithms/simulatorAllSystems.cpp \
    algorithms/simulatorStatistics.cpp \
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
    algorithms/FAG_AnyStr_ML.h \
    algorithms/FAG_AnyStr_hybrid.h \
    algorithms/FAG_hybrid.h \
    algorithms/LAG_reccurence.h \
    algorithms/LAG_recurenceGS.h \
    algorithms/cFIFO_convGamma.h \
    algorithms/cFIFO_hybrid.h \
    algorithms/conv.h \
    algorithms/conv3d.h \
    algorithms/convGamma2d.h \
    algorithms/convGamma3.h \
    algorithms/dFIFO_hybridDesc.h \
    algorithms/hybridDiscr.h \
    algorithms/reccurenceGeneral.h \
    model.h \
    algorithms/investigator.h \
    algorithms/trclvector2.h \
    algorithms/simulatordatacollection.h \
    algorithms/simulator.h \
    algorithms/simulationParameters.h \
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

