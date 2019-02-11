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
    decimal a;                  ///< Offered traffic per Servers AU
    int    classIndex;          ///< Some QoS parameters are concerned with traffic class
    int    systemState;         ///< System state (server + buffer)
    int    serverState;         ///< Server only state, buffer state is not considered
    int    bufferState;         ///< Queue only state, server state is not considered
    int    combinationNumber;   ///< In LAG systems, we can consider specified combination of groups e.g. groups {1, 3} or {1, 2}
    int    numberOfGroups;      ///< In LAG systems, we can consider specified number of any groups e.g. 1 or 3 groups
};

class Settings
{
public:
    enum class GnuplotKeyPosition
    {
        GKP_topRight
      , GKP_topLeft
      , GKP_bottomRight
      , GKP_bottomLeft
    };

    Settings(QString name, QString shortName): name(name), shortName(shortName) {}

    QList<ParameterType> dependencyParameters;

    ParameterType functionalParameter;
    ParameterType additionalParameter1;
    ParameterType additionalParameter2;

    bool setFunctionalParameter(ParameterType param);

    virtual bool getSinglePlot(QLineSeries *outPlot, QPair<double, double> &yMinAndMax, RSystem &rSystem, Investigator *algorithm, const struct ParametersSet &parametersSet, bool linearScale=true) const = 0;
    virtual double getXmin(RSystem &rSystem) const;
    virtual double getXmax(RSystem &rSystem) const;

    virtual ~Settings() {}

    QString name;               ///< Name without shortcuts
    QString shortName;          ///< Name with shortcuts

    static QString updateParameters(struct Results::ParametersSet &outParameters, const QVariant &variant, Results::ParameterType paramType, const ModelSyst *system, Results::RSystem *resultsForSystem);
    static void fillListWithParameters(QList<QVariant> &list, ParameterType paramType, const ModelSyst *system, QList<decimal> offeredTraffic);
};

class SettingsTypeForClass: public Settings
{
private:
    TypeForClass qos;//::AvarageNumbersOfCallsInBuffer

public:
    SettingsTypeForClass(TypeForClass qos, QString name, QString shortName);

    virtual bool getSinglePlot(QLineSeries *outPlot, QPair<double, double> &yMinAndMax, RSystem &rSystem, Investigator *algorithm, const struct ParametersSet &parametersSet, bool linearScale=true) const;
};


class SettingsTypeForSystemState: public Settings
{
private:
    const TypeForSystemState qos;
public:
    SettingsTypeForSystemState(TypeForSystemState qos, QString name, QString shortName);

    bool getSinglePlot(QLineSeries *outPlot, QPair<double, double> &yMinAndMax, RSystem &rSystem, Investigator *algorithm, const struct ParametersSet &parametersSet, bool linearScale=true) const;
};

class SettingsTypeForServerState: public Settings
{
private:
    const TypeForServerState qos;
public:
    SettingsTypeForServerState(TypeForServerState qos, QString name, QString shortName);

    bool getSinglePlot(QLineSeries *outPlot, QPair<double, double> &yMinAndMax, RSystem &rSystem, Investigator *algorithm, const struct ParametersSet &parametersSet, bool linearScale=true) const;
};

class SettingsTypeForBufferState: public Settings
{
private:
    const TypeForBufferState qos;
public:
    SettingsTypeForBufferState(TypeForBufferState qos, QString name, QString shortName);

    bool getSinglePlot(QLineSeries *outPlot, QPair<double, double> &yMinAndMax, RSystem &rSystem, Investigator *algorithm, const struct ParametersSet &parametersSet, bool linearScale=true) const;
};

class SettingsTypeForClassAndSystemState: public Settings
{
private:
    const TypeForClassAndSystemState qos;
public:
    SettingsTypeForClassAndSystemState(TypeForClassAndSystemState qos, QString name, QString shortName);

    bool getSinglePlot(QLineSeries *outPlot, QPair<double, double> &yMinAndMax, RSystem &rSystem, Investigator *algorithm, const struct ParametersSet &parametersSet, bool linearScale=true) const;
};

class SettingsTypeForClassAndServerState: public Settings
{
private:
    const TypeForClassAndServerState qos;
public:
    SettingsTypeForClassAndServerState(TypeForClassAndServerState qos, QString name, QString shortName);

    bool getSinglePlot(QLineSeries *outPlot, QPair<double, double> &yMinAndMax, RSystem &rSystem, Investigator *algorithm, const struct ParametersSet &parametersSet, bool linearScale=true) const;
};

class SettingsTypeForClassAndBufferState: public Settings
{
private:
    const TypeForClassAndBufferState qos;
public:
    SettingsTypeForClassAndBufferState(TypeForClassAndBufferState qos, QString name, QString shortName);

    bool getSinglePlot(QLineSeries *outPlot, QPair<double, double> &yMinAndMax, RSystem &rSystem, Investigator *algorithm, const struct ParametersSet &parametersSet, bool linearScale=true) const;
};

class SettingsInavailabilityForClassInAllGroupsInCombination: public Settings
{
public:
    SettingsInavailabilityForClassInAllGroupsInCombination(QString name, QString shortName);

    bool getSinglePlot(QLineSeries *outPlot, QPair<double, double> &yMinAndMax, RSystem &rSystem, Investigator *algorithm, const struct ParametersSet &parametersSet, bool linearScale=true) const;
};

class SettingsAvailableSubroupDistribution: public Settings
{
public:
    SettingsAvailableSubroupDistribution(QString name, QString shortName);

    bool getSinglePlot(QLineSeries *outPlot, QPair<double, double> &yMinAndMax, RSystem &rSystem, Investigator *algorithm, const struct ParametersSet &parametersSet, bool linearScale=true) const;
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
