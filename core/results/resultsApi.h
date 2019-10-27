#ifndef RESULTS_API_H
#define RESULTS_API_H

#include <QString>
#include <QMap>
#include <QMetaType>

#include <QtDataVisualization/QValue3DAxis>

#include <QtDataVisualization/Q3DScatter>
#include <QtDataVisualization/QScatterDataProxy>
#include <QtDataVisualization/QScatter3DSeries>


#include <QtDataVisualization/Q3DTheme>

#include <QtCharts>
#include <QLineSeries>

#include "results/resultsSystem.h"
#include "algorithms/investigator.h"
#include "resultsApiTypes.h"


using namespace QtDataVisualization;

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

    ParametersSet(): a(-1), classIndex(-1), systemState(-1), serverState(-1), bufferState(-1), combinationNumber(-1), numberOfGroups(-1) {}

    bool operator<(const ParametersSet &rho) const;
};


class ResultMap
{
    QVector<decimal> xValue;
    QVector<QVector<double>> yValues;
    QVector<QPair<Investigator*, ParametersSet>> columnDescriptor;
};

class Settings
{
protected:
    ParameterType functionalParameterX;
    ParameterType functionalParameterZ;
    ParameterType additionalParameter[3];

public:
    ParameterType getFunctionalParameterX() const {return  functionalParameterX;}
    ParameterType getFunctionalParameterZ() const {return  functionalParameterZ;}
    ParameterType getAdditionalParameter1() const {return additionalParameter[0];}
    ParameterType getAdditionalParameter2() const {return additionalParameter[1];}
    ParameterType getAdditionalParameter3() const {return additionalParameter[2];}

    enum class GnuplotKeyPosition
    {
        GKP_topRight
      , GKP_topLeft
      , GKP_bottomRight
      , GKP_bottomLeft
    };

    Settings() {;}
    Settings(QString name, QString shortName): name(name), shortName(shortName) {;}

    virtual ~Settings() {}

    QList<ParameterType> dependencyParameters;



    bool setFunctionalParameterX(ParameterType param);
    bool setFunctionalParameterY(ParameterType param);

    virtual bool getSinglePlot(QLineSeries *outPlot, QPair<double, double> &yMinAndMax, RSystem &rSystem, Investigator *algorithm, const struct ParametersSet &parametersSet, bool linearScale=true) const =0;
    virtual bool getSinglePlot(QVector<double> &outPlot, RSystem &rSystem, Investigator *algorithm, const struct ParametersSet &parametersSet) const = 0;

    virtual bool getSinglePlot3d(QScatter3DSeries &outPlot, RSystem &rSystem, Investigator *algorithm, const struct ParametersSet &parametersSet) const = 0;

    virtual QList<ParametersSet> getParametersList(const ModelSystem &system, const QList<decimal> &aOfPerAU) const = 0;

    virtual double getXmin(RSystem &rSystem) const;
    virtual double getXmax(RSystem &rSystem) const;

    virtual double getZmin(RSystem &rSystem) const;
    virtual double getZmax(RSystem &rSystem) const;


    QString name;               ///< Name without shortcuts
    QString shortName;          ///< Name with shortcuts

    static QString updateParameters(struct Results::ParametersSet &outParameters, const QVariant &variant, Results::ParameterType paramType, const ModelSystem &system, Results::RSystem *resultsForSystem);
    static void fillListWithParameters(QList<QVariant> &list, ParameterType paramType, const ModelSystem &system, QList<decimal> offeredTraffic);

    QString getTypeValue(const ParametersSet &params, Results::ParameterType type, const ModelSystem &system);

    QString getParameterDescription(const ParametersSet &params, const ModelSystem &system);
    QString getParameterDescription(const ParametersSet &params, const ModelSystem &system, const QList<ParameterType> dontDescribeMe);
};

class SettingsTypeForClass: public Settings
{
private:
    TypeForClass qos;//::AvarageNumbersOfCallsInBuffer

public:
    SettingsTypeForClass(TypeForClass qos, QString name, QString shortName);

    bool getSinglePlot(QLineSeries *outPlot, QPair<double, double> &yMinAndMax, RSystem &rSystem, Investigator *algorithm, const struct ParametersSet &parametersSet, bool linearScale=true) const;
    bool getSinglePlot(QVector<double> &outPlot, RSystem &rSystem, Investigator *algorithm, const struct ParametersSet &parametersSet) const; //TODO ADAM
    bool getSinglePlot3d(QScatter3DSeries &outPlot, RSystem &rSystem, Investigator *algorithm, const struct ParametersSet &parametersSet) const;

    virtual QList<ParametersSet> getParametersList(const ModelSystem &system, const QList<decimal> &aOfPerAU) const;
};


class SettingsTypeForSystemState: public Settings
{
private:
    const TypeForSystemState qos;
public:
    SettingsTypeForSystemState(TypeForSystemState qos, QString name, QString shortName);

    bool getSinglePlot(QLineSeries *outPlot, QPair<double, double> &yMinAndMax, RSystem &rSystem, Investigator *algorithm, const struct ParametersSet &parametersSet, bool linearScale=true) const;
    bool getSinglePlot(QVector<double> &outPlot, RSystem &rSystem, Investigator *algorithm, const struct ParametersSet &parametersSet) const;
    bool getSinglePlot3d(QScatter3DSeries &outPlot, RSystem &rSystem, Investigator *algorithm, const struct ParametersSet &parametersSet) const;

    virtual QList<ParametersSet> getParametersList(const ModelSystem &system, const QList<decimal> &aOfPerAU) const;
};

class SettingsTypeForServerState: public Settings
{
private:
    const TypeForServerState qos;
public:
    SettingsTypeForServerState(TypeForServerState qos, QString name, QString shortName);

    bool getSinglePlot(QLineSeries *outPlot, QPair<double, double> &yMinAndMax, RSystem &rSystem, Investigator *algorithm, const struct ParametersSet &parametersSet, bool linearScale=true) const;
    bool getSinglePlot(QVector<double> &outPlot, RSystem &rSystem, Investigator *algorithm, const struct ParametersSet &parametersSet) const; //TODO ADAM

    bool getSinglePlot3d(QScatter3DSeries &outPlot, RSystem &rSystem, Investigator *algorithm, const struct ParametersSet &parametersSet) const;

    virtual QList<ParametersSet> getParametersList(const ModelSystem &system, const QList<decimal> &aOfPerAU) const;
};

class SettingsTypeForBufferState: public Settings
{
private:
    const TypeForBufferState qos;
public:
    SettingsTypeForBufferState(TypeForBufferState qos, QString name, QString shortName);

    bool getSinglePlot(QLineSeries *outPlot, QPair<double, double> &yMinAndMax, RSystem &rSystem, Investigator *algorithm, const struct ParametersSet &parametersSet, bool linearScale=true) const;
    bool getSinglePlot(QVector<double> &outPlot, RSystem &rSystem, Investigator *algorithm, const struct ParametersSet &parametersSet) const; //TODO ADAM
    bool getSinglePlot3d(QScatter3DSeries &outPlot, RSystem &rSystem, Investigator *algorithm, const struct ParametersSet &parametersSet) const;

    virtual QList<ParametersSet> getParametersList(const ModelSystem &system, const QList<decimal> &aOfPerAU) const;
};

class SettingsTypeForServerAndBufferState: public Settings
{
private:
    const TypeForServerAngBufferState qos;
public:
    SettingsTypeForServerAndBufferState(TypeForServerAngBufferState qos, QString name, QString shortName);

    bool getSinglePlot(QLineSeries *outPlot, QPair<double, double> &yMinAndMax, RSystem &rSystem, Investigator *algorithm, const struct ParametersSet &parametersSet, bool linearScale=true) const;
    bool getSinglePlot(QVector<double> &outPlot, RSystem &rSystem, Investigator *algorithm, const struct ParametersSet &parametersSet) const; //TODO ADAM
    bool getSinglePlot3d(QScatter3DSeries &outPlot, RSystem &rSystem, Investigator *algorithm, const struct ParametersSet &parametersSet) const;

    virtual QList<ParametersSet> getParametersList(const ModelSystem &system, const QList<decimal> &aOfPerAU) const;
};

class SettingsTypeForClassAndSystemState: public Settings
{
private:
    const TypeForClassAndSystemState qos;
public:
    SettingsTypeForClassAndSystemState(TypeForClassAndSystemState qos, QString name, QString shortName);

    bool getSinglePlot(QLineSeries *outPlot, QPair<double, double> &yMinAndMax, RSystem &rSystem, Investigator *algorithm, const struct ParametersSet &parametersSet, bool linearScale=true) const;
    bool getSinglePlot(QVector<double> &outPlot, RSystem &rSystem, Investigator *algorithm, const struct ParametersSet &parametersSet) const; //TODO ADAM
    bool getSinglePlot3d(QScatter3DSeries &outPlot, RSystem &rSystem, Investigator *algorithm, const struct ParametersSet &parametersSet) const;

    virtual QList<ParametersSet> getParametersList(const ModelSystem &system, const QList<decimal> &aOfPerAU) const;
};

class SettingsTypeForClassAndServerState: public Settings
{
private:
    const TypeForClassAndServerState qos;
public:
    SettingsTypeForClassAndServerState(TypeForClassAndServerState qos, QString name, QString shortName);

    bool getSinglePlot(QLineSeries *outPlot, QPair<double, double> &yMinAndMax, RSystem &rSystem, Investigator *algorithm, const struct ParametersSet &parametersSet, bool linearScale=true) const;
    bool getSinglePlot(QVector<double> &outPlot, RSystem &rSystem, Investigator *algorithm, const struct ParametersSet &parametersSet) const; //TODO ADAM
    bool getSinglePlot3d(QScatter3DSeries &outPlot, RSystem &rSystem, Investigator *algorithm, const struct ParametersSet &parametersSet) const;

    virtual QList<ParametersSet> getParametersList(const ModelSystem &system, const QList<decimal> &aOfPerAU) const;
};

class SettingsTypeForClassAndBufferState: public Settings
{
private:
    const TypeForClassAndBufferState qos;
public:
    SettingsTypeForClassAndBufferState(TypeForClassAndBufferState qos, QString name, QString shortName);

    bool getSinglePlot(QLineSeries *outPlot, QPair<double, double> &yMinAndMax, RSystem &rSystem, Investigator *algorithm, const struct ParametersSet &parametersSet, bool linearScale=true) const;
    bool getSinglePlot(QVector<double> &outPlot, RSystem &rSystem, Investigator *algorithm, const struct ParametersSet &parametersSet) const; //TODO ADAM
    bool getSinglePlot3d(QScatter3DSeries &outPlot, RSystem &rSystem, Investigator *algorithm, const struct ParametersSet &parametersSet) const;

    virtual QList<ParametersSet> getParametersList(const ModelSystem &system, const QList<decimal> &aOfPerAU) const;
};

class SettingsForClassAndServerGroupsCombination: public Settings
{
private:
    TypeClasses_VsServerGroupsCombination qos;

public:
    SettingsForClassAndServerGroupsCombination(TypeClasses_VsServerGroupsCombination qos, QString name, QString shortName);

    bool getSinglePlot(QLineSeries *outPlot, QPair<double, double> &yMinAndMax, RSystem &rSystem, Investigator *algorithm, const struct ParametersSet &parametersSet, bool linearScale=true) const;
    bool getSinglePlot(QVector<double> &outPlot, RSystem &rSystem, Investigator *algorithm, const struct ParametersSet &parametersSet) const; //TODO ADAM
    bool getSinglePlot3d(QScatter3DSeries &outPlot, RSystem &rSystem, Investigator *algorithm, const struct ParametersSet &parametersSet) const;

    virtual QList<ParametersSet> getParametersList(const ModelSystem &system, const QList<decimal> &aOfPerAU) const;
};

class SettingsAvailableSubroupDistribution: public Settings
{
public:
    SettingsAvailableSubroupDistribution(QString name, QString shortName);

    bool getSinglePlot(QLineSeries *outPlot, QPair<double, double> &yMinAndMax, RSystem &rSystem, Investigator *algorithm, const struct ParametersSet &parametersSet, bool linearScale=true) const;
    bool getSinglePlot(QVector<double> &outPlot, RSystem &rSystem, Investigator *algorithm, const struct ParametersSet &parametersSet) const;
    bool getSinglePlot3d(QScatter3DSeries &outPlot, RSystem &rSystem, Investigator *algorithm, const struct ParametersSet &parametersSet) const;

    virtual QList<ParametersSet> getParametersList(const ModelSystem &system, const QList<decimal> &aOfPerAU) const;
};


class TypesAndSettings
{
    //Q_DECLARE_TR_FUNCTIONS(TypesAndSettings)
private:
    static QMap<Type, Settings *> _myMap;
    static bool _isInitialized;

    static void _initialize();

public:
    static void release();

    static const Settings *getSettingConst(Type type);
    static Settings *getSetting(Type type);
    static QString typeToString(Type type);

    static QString parameterToString(ParameterType parameter);

    static QString typeToX_AxisString(Type type);



    static QMap<ParametersSet, QVector<double>> getPlotsValues(RSystem &rSystem, Type qos, ParameterType functionalParameter, Investigator *algorithm);
    static QMap<ParametersSet, QVector<double>> getPlotsValues3d(RSystem &rSystem, Type qos, ParameterType functionalParameter1, ParameterType functionalParameter2, Investigator *algorithm);

    static const QVector<decimal> getPlotsXorZ(RSystem &rSystem, ParameterType functionalParameter);
};


} //namespace Results

Q_DECLARE_METATYPE(Results::Type)
Q_DECLARE_METATYPE(Results::ParameterType)

#endif // RESULTS_API_H
