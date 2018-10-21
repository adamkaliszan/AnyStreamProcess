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

enum class ParameterType
{
    None,
    OfferedTrafficPerAS,
    TrafficClass,
    SystemState,
    ServerState,
    BufferState,
    CombinationNumber,
    NumberOfGroups
};

struct ParametersSet
{
    decimal a;                  /// Offered traffic per Servers AU
    double aDebug;              //TODO remove it
    int    classIndex;          /// Some QoS parameters are concerned with traffic class
    int    systemState;         /// System state (server + buffer)
    int    serverState;         /// Server only state, buffer state is not considered
    int    bufferState;          /// Queue only state, server state is not considered
    int    combinationNumber;   /// In LAG systems, we can consider specified combination of groups e.g. groups {1, 3} or {1, 2}
    int    numberOfGroups;      /// In LAG systems, we can consider specified number of any groups e.g. 1 or 3 groups
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

    virtual ~Settings() {}
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

class SettingsAvarageNumbersOfCallsInBuffer: public Settings
{
public:
    SettingsAvarageNumbersOfCallsInBuffer();

    virtual bool getSinglePlot(QLineSeries *outPlot, RSystem &rSystem, Investigator *algorithm, const struct ParametersSet &parametersSet, bool linearScale=true) const;
};


class SettingsOccupancyDistribution: public Settings
{
public:
    SettingsOccupancyDistribution();

    bool getSinglePlot(QLineSeries *outPlot, RSystem &rSystem, Investigator *algorithm, const struct ParametersSet &parametersSet, bool linearScale=true) const;
};

class SettingsOccupancyDistributionServer: public Settings
{
public:
    SettingsOccupancyDistributionServer();

    bool getSinglePlot(QLineSeries *outPlot, RSystem &rSystem, Investigator *algorithm, const struct ParametersSet &parametersSet, bool linearScale=true) const;
};

class SettingsOccupancyDistributionBuffer: public Settings
{
public:
    SettingsOccupancyDistributionBuffer();

    bool getSinglePlot(QLineSeries *outPlot, RSystem &rSystem, Investigator *algorithm, const struct ParametersSet &parametersSet, bool linearScale=true) const;
};

class SettingsNumberOfCallsInStateN: public Settings
{
public:
    SettingsNumberOfCallsInStateN();

    bool getSinglePlot(QLineSeries *outPlot, RSystem &rSystem, Investigator *algorithm, const struct ParametersSet &parametersSet, bool linearScale=true) const;
};

class SettingsNumberOfCallsInStateN_inServer: public Settings
{
public:
    SettingsNumberOfCallsInStateN_inServer();

    bool getSinglePlot(QLineSeries *outPlot, RSystem &rSystem, Investigator *algorithm, const struct ParametersSet &parametersSet, bool linearScale=true) const;
};

class SettingsNumberOfCallsInStateN_inBuffer: public Settings
{
public:
    SettingsNumberOfCallsInStateN_inBuffer();

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

