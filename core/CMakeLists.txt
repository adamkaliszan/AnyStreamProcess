cmake_minimum_required(VERSION 3.5)

project(Gui VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt5 COMPONENTS Core Widgets LinguistTools REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Widgets LinguistTools REQUIRED)
find_package(TensorflowCC REQUIRED)

find_package(Qt5Core REQUIRED)
find_package(Qt5Charts REQUIRED)
find_package(Qt5DataVisualization REQUIRED)
find_package(Qt5PrintSupport REQUIRED)

add_library(core model.cpp model.h
    algorithms/FAG_AnyStr_ML.cpp              algorithms/FAG_AnyStr_ML.h
    algorithms/FAG_AnyStr_hybrid.cpp          algorithms/FAG_AnyStr_hybrid.h
    algorithms/FAG_hybrid.cpp                 algorithms/FAG_hybrid.h
    algorithms/LAG_reccurence.cpp             algorithms/LAG_reccurence.h
    algorithms/LAG_reccurenceGS.cpp           algorithms/LAG_recurenceGS.h
    algorithms/cFIFO_convGamma.cpp            algorithms/cFIFO_convGamma.h
    algorithms/cFIFO_hybrid.cpp               algorithms/cFIFO_hybrid.h
    algorithms/conv.cpp                       algorithms/conv.h
    algorithms/conv3d.cpp                     algorithms/conv3d.h
    algorithms/convGamma2d.cpp                algorithms/convGamma2d.h
    algorithms/convGamma3.cpp                 algorithms/convGamma3.h
    algorithms/dFIFO_hybridDesc.cpp           algorithms/dFIFO_hybridDesc.h
    algorithms/hybridDiscr.cpp                algorithms/hybridDiscr.h
    algorithms/reccurenceGeneral.cpp          algorithms/reccurenceGeneral.h
    algorithms/investigator.cpp               algorithms/investigator.h
    algorithms/trclvector2.cpp                algorithms/trclvector2.h
    algorithms/simulatordatacollection.cpp    algorithms/simulatordatacollection.h
    algorithms/simulator.cpp                  algorithms/simulator.h
    algorithms/simulationParameters.cpp       algorithms/simulationParameters.h
    algorithms/simulatorAllSystems.cpp        algorithms/simulatorAllSystems.h
    algorithms/simulatorStatistics.cpp        algorithms/simulatorStatistics.h
    results/resultsGnuplot.cpp                results/resultsGnuplot.h
    results/resultsSingle.cpp                 results/resultsSingle.h
    results/resultsInvestigator.cpp           results/resultsInvestigator.h
    results/resultsSystem.cpp                 results/resultsSystem.h
    results/resultsSingleGroup.cpp            results/resultsSingleGroup.h
    results/resultsApi.cpp                    results/resultsApi.h results/resultsApiTypes.h
    utils/probDistributions.cpp               utils/probDistributions.h
    utils/lag.cpp                             utils/lag.h
    utils/vectorUtils.cpp                     utils/vectorUtils.h
    utils/utilsMisc.cpp                       utils/utilsMisc.h
                                              utils/decimal.h
    )
target_link_libraries(core PRIVATE TensorflowCC::TensorflowCC Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Charts Qt${QT_VERSION_MAJOR}::DataVisualization protobuf)
