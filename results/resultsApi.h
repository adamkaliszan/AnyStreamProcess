#ifndef RESULTS_API_H
#define RESULTS_API_H

#include <QString>
#include <QMap>
#include <QMetaType>

#include "results/resultsSystem.h"
#include "algorithms/investigator.h"
#include "resultsApiTypes.h"

namespace Results
{

class RSystem;

enum class ParameterType
{
    None,
    OfferedTrafficPerAS,
    TrafficClass,
    SystemState,
    ServerState,
    QueueState,
    CombinationNumber,
    NumberOfGroups,
    NumberOfAUs
};

struct ParametersSet
{
    decimal a;
    double aDebug;
    int    classIndex;
    int    systemState;
    int    serverState;
    int    queueState;
    int    combinationNumber;
    int    numberOfGroups;
    int    numberOfAus;
};

class Settings
{
public:
    QList<ParameterType> dependencyParameters;

    ParameterType functionalParameter;
    ParameterType additionalParameter1;
    ParameterType additionalParameter2;

    bool setFunctionalParameter(ParameterType param);

    virtual bool getSinglePlot(QLineSeries *outPlot, RSystem &rSystem, Investigator *algorithm, const struct ParametersSet &parametersSet, bool linearScale=true) const = 0;
    virtual double getXmin(RSystem &rSystem) const;
    virtual double getXmax(RSystem &rSystem) const;
};

class SettingsBlockingProbability: public Settings
{
public:
    SettingsBlockingProbability();

    virtual bool getSinglePlot(QLineSeries *outPlot, RSystem &rSystem, Investigator *algorithm, const struct ParametersSet &parametersSet, bool linearScale=true) const;
};

class SettingsLossProbability: public Settings
{
public:
    SettingsLossProbability();

    virtual bool getSinglePlot(QLineSeries *outPlot, RSystem &rSystem, Investigator *algorithm, const struct ParametersSet &parametersSet, bool linearScale=true) const;
};

class SettingsOccupancyDistribution: public Settings
{
public:
    SettingsOccupancyDistribution();

    bool getSinglePlot(QLineSeries *outPlot, RSystem &rSystem, Investigator *algorithm, const struct ParametersSet &parametersSet, bool linearScale=true) const;
};

class SettingsInavailabilityForClassInAllGroupsInCombination: public Settings
{
public:
    SettingsInavailabilityForClassInAllGroupsInCombination();

    bool getSinglePlot(QLineSeries *outPlot, RSystem &rSystem, Investigator *algorithm, const struct ParametersSet &parametersSet, bool linearScale=true) const;
};

class SettingsAvailableSubroupDistribution: public Settings
{
public:
    SettingsAvailableSubroupDistribution();

    bool getSinglePlot(QLineSeries *outPlot, RSystem &rSystem, Investigator *algorithm, const struct ParametersSet &parametersSet, bool linearScale=true) const;
};


class TypesAndSettings
{
    //Q_DECLARE_TR_FUNCTIONS(TypesAndSettings)
private:
    static QMap<Type, Settings *> _myMap;
    static bool _isInitialized;

    static void _initialize();

public:
    static const Settings *getSettingConst(Type type);
    static  Settings *getSetting(Type type);
    static QString typeToString(Type type);
    static QString parameterToString(ParameterType parameter);

    static QString typeToX_AxisString(Type type);
};


} //namespace Results

Q_DECLARE_METATYPE(Results::Type)
Q_DECLARE_METATYPE(Results::ParameterType)

#endif // RESULTS_API_H

