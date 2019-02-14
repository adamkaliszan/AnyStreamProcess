#include "resultsApi.h"

#include "utils/lag.h"

namespace Results
{

bool TypesAndSettings::_isInitialized = false;

QMap<Type, Settings *> TypesAndSettings::_myMap = QMap<Type, Settings *>();

void TypesAndSettings::_initialize()
{
    if (_isInitialized)
        return;

    _myMap.insert(Type::BlockingProbability                                    , new SettingsTypeForClass(TypeForClass::BlockingProbability                                            , "Blocking probability"                                              , "E"        ));
    _myMap.insert(Type::LossProbability                                        , new SettingsTypeForClass(TypeForClass::LossProbability                                                , "Loss probability"                                                  , "B"        ));

    _myMap.insert(Type::OccupancyDistribution                                  , new SettingsTypeForSystemState(TypeForSystemState::StateProbability                                   , "Occupancy Distribution in system"                                  , "P(n)"     ));
    _myMap.insert(Type::NewCallOfAllClassesIntensityOut_inSystemVsSystemState  , new SettingsTypeForSystemState(TypeForSystemState::IntensityNewCallOutOffered                                , "All Classes Calls arrival intensity vs current system state"       , "LOut(n)" ));
    _myMap.insert(Type::NewCallOfAllClassesIntensityIn_inSystemVsSystemState   , new SettingsTypeForSystemState(TypeForSystemState::IntensityNewCallIn                                 , "All Classes Calls acceptance intensity vs new system state"        , "LIn(n)"  ));
    _myMap.insert(Type::NewCallOfAllClassesIntensityOut_inSystemVsSystemState  , new SettingsTypeForSystemState(TypeForSystemState::IntensityEndCallOut                                , "All Classes Calls service ending intensity vs current system state", "UOut(n)" ));
    _myMap.insert(Type::NewCallOfAllClassesIntensityIn_inSystemVsSystemState   , new SettingsTypeForSystemState(TypeForSystemState::IntensityEndCallIn                                 , "All Classes Calls service ending intensity vs new system state"    , "UIn(n)"  ));

    _myMap.insert(Type::OccupancyDistributionServerOnly                        , new SettingsTypeForServerState(TypeForServerState::StateProbability                                   , "Occupancy distribution in server"                     , "Ps(n)"    ));
    _myMap.insert(Type::OccupancyDistributionBufferOnly                        , new SettingsTypeForBufferState(TypeForBufferState::StateProbability                                   , "Occupancy distribution in buffer"                     , "Pb(n)"    ));

    _myMap.insert(Type::NumberOfCallsInSystemVsSystemState                     , new SettingsTypeForClassAndSystemState(TypeForClassAndSystemState::UsageForSystem                     , "Number of calls in system"                            , "yi(n)"    ));
    _myMap.insert(Type::NumberOfCallsInServerVsServerState                     , new SettingsTypeForClassAndSystemState(TypeForClassAndSystemState::UsageForServer                     , "Number of calls in server"                            , "yis(n)"   ));
    _myMap.insert(Type::NumberOfCallsInBufferVsBufferState                     , new SettingsTypeForClassAndSystemState(TypeForClassAndSystemState::UsageForBuffer                     , "Number of calls in buffer"                            , "yib(n)"   ));
    _myMap.insert(Type::NewCallOfSingleClassIntensityOut_inSystemVsSystemState , new SettingsTypeForClassAndSystemState(TypeForClassAndSystemState::OfferedNewCallIntensityOutForSystem, "Single Class Call arrival intensity vs current system state"       , "LiOut(n)" ));
    _myMap.insert(Type::NewCallOfSingleClassIntensityIn_inSystemVsSystemState  , new SettingsTypeForClassAndSystemState(TypeForClassAndSystemState::NewCallIntensityInForSystem        , "Single Class Call acceptance intensity vs new system state"        , "LiIn(n)"  ));
    _myMap.insert(Type::EndCallOfSingleClassIntensityOut_inSystemVsSystemState , new SettingsTypeForClassAndSystemState(TypeForClassAndSystemState::EndCallIntensityOutForSystem       , "Single Class Call service ending intensity vs current system state", "UiOut(n)" ));
    _myMap.insert(Type::EndCallOfSingleClassIntensityIn_inSystemVsSystemState  , new SettingsTypeForClassAndSystemState(TypeForClassAndSystemState::EndCallIntensityInForSystem        , "Single Class Call service ending intensity vs new system state"    , "UiIn(n)"  ));

    _myMap.insert(Type::NewCallOutIntensityServerVsSystemState                 , new SettingsTypeForClassAndServerState(TypeForClassAndServerState::OfferedNewCallIntensityOut         , "Call arrival intensity vs current server state"       , "LiSOut(n)"));
    _myMap.insert(Type::NewCallInIntensityServerVsSystemState                  , new SettingsTypeForClassAndServerState(TypeForClassAndServerState::NewCallIntensityIn                 , "Call acceptance intensity vs new server state"        , "LiSIn(n)" ));
    _myMap.insert(Type::EndCallOutIntensityServerVsSystemState                 , new SettingsTypeForClassAndServerState(TypeForClassAndServerState::EndCallIntensityOut                , "Call service ending intensity vs current server state", "UiSOut(n)"));
    _myMap.insert(Type::EndCallInIntensityServerVsSystemState                  , new SettingsTypeForClassAndServerState(TypeForClassAndServerState::EndCallIntensityIn                 , "Call service ending intensity vs new server state"    , "UiSIn(n)" ));

    _myMap.insert(Type::NewCallOutIntensityBufferVsSystemState                 , new SettingsTypeForClassAndBufferState(TypeForClassAndBufferState::OfferedNewCallIntensityOut         , "Call arrival intensity vs current buffer state"       , "LiBOut(n)"));
    _myMap.insert(Type::NewCallInIntensityBufferVsSystemState                  , new SettingsTypeForClassAndBufferState(TypeForClassAndBufferState::NewCallIntensityIn                 , "Call acceptance intensity vs new buffer state"        , "LiBIn(n)" ));
    _myMap.insert(Type::EndCallOutIntensityBufferVsSystemState                 , new SettingsTypeForClassAndBufferState(TypeForClassAndBufferState::EndCallIntensityOut                , "Call service ending intensity vs current buffer state", "UiBOut(n)"));
    _myMap.insert(Type::EndCallInIntensityBufferVsSystemState                  , new SettingsTypeForClassAndBufferState(TypeForClassAndBufferState::EndCallIntensityIn                 , "Call service ending intensity vs new buffer state"    , "UiBIn(n)" ));

    _myMap.insert(Type::AllSugbrupsInGivenCombinationNotAvailableForCallsOfGivenClass
      , new SettingsInavailabilityForClassInAllGroupsInCombination("All groups in combination are not available for class i"    , "combNA_i"));
    _myMap.insert(Type::AvailableSubroupDistribution
      , new SettingsAvailableSubroupDistribution(                  "Distribution of available groups"                           , "RDP"));

    _isInitialized = true;
}

const Settings* TypesAndSettings::getSettingConst(Type type)
{
    if (!_isInitialized)
    {
        _initialize();
    }

    return _myMap[type];
}

Settings* TypesAndSettings::getSetting(Type type)
{
    if (!_isInitialized)
    {
        _initialize();
    }

    return _myMap[type];
}

QString TypesAndSettings::typeToString(Type type)
{
    return _myMap[type]->name;
}

QString TypesAndSettings::typeToX_AxisString(Type type)
{
    QString result;
    const Settings *tmp = getSettingConst(type);
    switch (tmp->functionalParameter)
    {
    case ParameterType::OfferedTrafficPerAS:
        result = "a";
        break;

    case ParameterType::TrafficClass:
        result = "Traffic Class";
        break;

    case ParameterType::SystemState:
        result = "System state";
        break;

    case ParameterType::ServerState:
        result = "Server state";
        break;

    default:
        qFatal("Not supported type");

    }
    return result;
}

QMap<ParametersSet, QVector<double> > TypesAndSettings::getPlots(RSystem &rSystem, Type qos, ParameterType functionalParameter, Investigator *algorithm)
{
    Settings *qosSettings = _myMap[qos];

    QList<ParametersSet> sysParams = qosSettings->getParametersList(&rSystem.getModel());

    QMap<ParametersSet, QVector<double>> result;

    foreach (ParametersSet sysParam, sysParams)
    {
        QVector<QPair<decimal, double>> pairVector;
        qosSettings->getSinglePlot(pairVector, rSystem, algorithm, sysParam);

        QVector<double> vector;
        result.insert(sysParam, vector);
    }
    return result;
}

QString TypesAndSettings::parameterToString(ParameterType parameter)
{
    QString result;

    switch(parameter)
    {
    case ParameterType::None:
        result = "N/A";
        break;

    case ParameterType::OfferedTrafficPerAS:
        result = "Offered traffic per AS";
        break;

    case ParameterType::BufferState:
        result = "Queue state";
        break;

    case ParameterType::ServerState:
        result = "Server state";
        break;

    case ParameterType::SystemState:
        result = "System state";
        break;

    case ParameterType::TrafficClass:
        result = "Traffic class";
        break;

    case ParameterType::NumberOfGroups:
        result = "Number of groups";
        break;

    case ParameterType::CombinationNumber:
        result = "Combination number";
        break;


//    default:
//        qFatal("Not implemented parameter");
    }

    return result;
}


SettingsTypeForClass::SettingsTypeForClass(TypeForClass qos, QString name, QString shortName): Settings (name, shortName), qos(qos)
{
    dependencyParameters.append(ParameterType::OfferedTrafficPerAS);
    dependencyParameters.append(ParameterType::TrafficClass);

    functionalParameter  = ParameterType::OfferedTrafficPerAS;
    additionalParameter1 = ParameterType::TrafficClass;
    additionalParameter2 = ParameterType::None;
}

bool SettingsTypeForClass::getSinglePlot(QLineSeries *outPlot, QPair<double, double> &yMinAndMax, RSystem &rSystem, Investigator *algorithm, const ParametersSet &parametersSet, bool linearScale) const
{
    bool result = false;

    outPlot->clear();
    if (functionalParameter == ParameterType::OfferedTrafficPerAS)
    {
        foreach(decimal a, rSystem.getAvailableAperAU())
        {
            const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, a);

            double y=0;
            double x = static_cast<double>(a);

            if ((*singlePoint)->read(y, qos, parametersSet.classIndex))
            {
                if ((y>0) || linearScale)
                {
                    if (yMinAndMax.first > y)
                        yMinAndMax.first = y;
                    if (yMinAndMax.second < y)
                        yMinAndMax.second = y;

                    *outPlot<<QPointF(x, y);
                    result = true;
                }
            }
        }
    }

    if (functionalParameter == ParameterType::TrafficClass)
    {
        const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, parametersSet.a);

        for (int i=0; i <rSystem.getModel().m(); i++)
        {
            double y=0;
            if ((*singlePoint)->read(y, qos, i))
            {
                if ((y>0) || linearScale)
                {
                    *outPlot<<QPointF(static_cast<double>(i), y);
                    result = true;
                }
            }
        }
    }
    return result;
}

bool SettingsTypeForClass::getSinglePlot(QVector<QPair<decimal, double> > &outPlot, RSystem &rSystem, Investigator *algorithm, const ParametersSet &parametersSet) const
{
    bool result = false;

    outPlot.clear();
    if (functionalParameter == ParameterType::OfferedTrafficPerAS)
    {
        foreach(decimal a, rSystem.getAvailableAperAU())
        {
            const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, a);

            double y=0;

            if ((*singlePoint)->read(y, qos, parametersSet.classIndex))
            {
                if ((y>0))
                {
                    outPlot.append(QPair<decimal, double>(a, y));
                    result = true;
                }
            }
        }
    }

    if (functionalParameter == ParameterType::TrafficClass)
    {
        const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, parametersSet.a);

        for (int i=0; i <rSystem.getModel().m(); i++)
        {
            double y=0;
            if ((*singlePoint)->read(y, qos, i))
            {
                if (y>0)
                {
                    outPlot.append(QPair<decimal, double>(i, y));
                    result = true;
                }
            }
        }
    }
    return result;
}

QList<ParametersSet> SettingsTypeForClass::getParametersList(const ModelSyst *system) const
{
    QList<ParametersSet> result;
    for (int i=0; i<system->m(); i++)
    {
        ParametersSet item;
        item.classIndex = i;
        result.append(item);
    }
    return result;
}


SettingsTypeForSystemState::SettingsTypeForSystemState(TypeForSystemState qos, QString name, QString shortName): Settings (name, shortName), qos(qos)
{
    dependencyParameters.append(ParameterType::OfferedTrafficPerAS);
    dependencyParameters.append(ParameterType::SystemState);

    functionalParameter  = ParameterType::SystemState;
    additionalParameter1 = ParameterType::OfferedTrafficPerAS;
    additionalParameter2 = ParameterType::None;
}

bool SettingsTypeForSystemState::getSinglePlot(QLineSeries *outPlot, QPair<double, double> &yMinAndMax, RSystem &rSystem, Investigator *algorithm, const ParametersSet &parametersSet, bool linearScale) const
{
    bool result = false;

    outPlot->clear();
    if (functionalParameter == ParameterType::OfferedTrafficPerAS)
    {
        foreach(decimal a, rSystem.getAvailableAperAU())
        {
            const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, a);

            double y=0;
            if ((*singlePoint)->read(y, qos, parametersSet.systemState))
            {
                if ((y > 0) || linearScale)
                {
                    *outPlot<<QPointF(static_cast<double>(a), y);
                    result = true;
                }
            }
        }
    }

    if (functionalParameter == ParameterType::SystemState)
    {
        const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, parametersSet.a);
        for (int n=0; n<=rSystem.getModel().V(); n++)
        {
            double y=0;

            if ((*singlePoint)->read(y, qos, n))
            {
                if (yMinAndMax.first > y)
                    yMinAndMax.first = y;
                if (yMinAndMax.second < y)
                    yMinAndMax.second = y;

                if ((y > 0) || linearScale)
                {
                    *outPlot<<QPointF(n, y);
                    result = true;
                }
            }
        }
    }
    return result;
}

bool SettingsTypeForSystemState::getSinglePlot(QVector<QPair<decimal, double> > &outPlot, RSystem &rSystem, Investigator *algorithm, const ParametersSet &parametersSet) const
{
    bool result = false;

    outPlot.clear();
    if (functionalParameter == ParameterType::OfferedTrafficPerAS)
    {
        foreach(decimal a, rSystem.getAvailableAperAU())
        {
            const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, a);

            double y=0;
            if ((*singlePoint)->read(y, qos, parametersSet.systemState))
            {
                if (y > 0)
                {
                    outPlot.append(QPair<decimal, double>(a, y));
                    result = true;
                }
            }
        }
    }

    if (functionalParameter == ParameterType::SystemState)
    {
        const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, parametersSet.a);
        for (int n=0; n<=rSystem.getModel().V(); n++)
        {
            double y=0;

            if ((*singlePoint)->read(y, qos, n))
            {
                if (y > 0)
                {
                    outPlot.append(QPair<decimal, double>(n, y));
                    result = true;
                }
            }
        }
    }
    return result;
}

QList<ParametersSet> SettingsTypeForSystemState::getParametersList(const ModelSyst *system) const
{
    QList<ParametersSet> result;
    for (int n=0; n <= system->V(); n++)
    {
        ParametersSet item;
        item.systemState = n;
        result.append(item);
    }
    return result;
}

SettingsTypeForServerState::SettingsTypeForServerState(TypeForServerState qos, QString name, QString shortName): Settings (name, shortName), qos(qos)
{
    dependencyParameters.append(ParameterType::OfferedTrafficPerAS);
    dependencyParameters.append(ParameterType::ServerState);

    functionalParameter  = ParameterType::ServerState;
    additionalParameter1 = ParameterType::OfferedTrafficPerAS;
    additionalParameter2 = ParameterType::None;
}

bool SettingsTypeForServerState::getSinglePlot(QLineSeries *outPlot, QPair<double, double> &yMinAndMax, RSystem &rSystem, Investigator *algorithm, const ParametersSet &parametersSet, bool linearScale) const
{
    bool result = false;

    outPlot->clear();
    if (functionalParameter == ParameterType::OfferedTrafficPerAS)
    {
        foreach(decimal a, rSystem.getAvailableAperAU())
        {
            const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, a);

            double y=0;
            if ((*singlePoint)->read(y, qos, parametersSet.serverState))
            {
                if ((y > 0) || linearScale)
                {
                    if (yMinAndMax.first > y)
                        yMinAndMax.first = y;
                    if (yMinAndMax.second < y)
                        yMinAndMax.second = y;

                    *outPlot<<QPointF(static_cast<double>(a), y);
                    result = true;
                }
            }
        }
    }

    if (functionalParameter == ParameterType::ServerState)
    {
        const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, parametersSet.a);
        for (int n=0; n<=rSystem.getModel().vk_s(); n++)
        {
            double y=0;

            if ((*singlePoint)->read(y, qos, n))
            {
                if ((y > 0) || linearScale)
                {
                    *outPlot<<QPointF(n, y);
                    result = true;
                }
            }
        }
    }
    return result;
}

bool SettingsTypeForServerState::getSinglePlot(QVector<QPair<decimal, double> > &outPlot, RSystem &rSystem, Investigator *algorithm, const ParametersSet &parametersSet) const
{
    bool result = false;

    outPlot.clear();
    if (functionalParameter == ParameterType::OfferedTrafficPerAS)
    {
        foreach(decimal a, rSystem.getAvailableAperAU())
        {
            const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, a);

            double y=0;
            if ((*singlePoint)->read(y, qos, parametersSet.serverState))
            {
                if (y > 0)
                    outPlot.append(QPair<decimal, double>(a, y));
                    result = true;
            }
        }
    }

    if (functionalParameter == ParameterType::ServerState)
    {
        const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, parametersSet.a);
        for (int n=0; n<=rSystem.getModel().vk_s(); n++)
        {
            double y=0;

            if ((*singlePoint)->read(y, qos, n))
            {
                if (y > 0)
                {
                    outPlot.append(QPair<decimal, double>(n, y));
                    result = true;
                }
            }
        }
    }
    return result;
}

QList<ParametersSet> SettingsTypeForServerState::getParametersList(const ModelSyst *system) const
{
    QList<ParametersSet> result;
    for (int n=0; n <= system->vk_s(); n++)
    {
        ParametersSet item;
        item.serverState = n;
        result.append(item);
    }
    return result;
}

SettingsTypeForBufferState::SettingsTypeForBufferState(TypeForBufferState qos, QString name, QString shortName): Settings (name, shortName), qos(qos)
{
    dependencyParameters.append(ParameterType::OfferedTrafficPerAS);
    dependencyParameters.append(ParameterType::BufferState);

    functionalParameter  = ParameterType::BufferState;
    additionalParameter1 = ParameterType::OfferedTrafficPerAS;
    additionalParameter2 = ParameterType::None;
}

bool SettingsTypeForBufferState::getSinglePlot(QLineSeries *outPlot, QPair<double, double> &yMinAndMax, RSystem &rSystem, Investigator *algorithm, const ParametersSet &parametersSet, bool linearScale) const
{
    bool result = false;

    outPlot->clear();
    if (functionalParameter == ParameterType::OfferedTrafficPerAS)
    {
        foreach(decimal a, rSystem.getAvailableAperAU())
        {
            const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, a);

            double y=0;
            if ((*singlePoint)->read(y, qos, parametersSet.bufferState))
            {
                if ((y > 0) || linearScale)
                {
                    if (yMinAndMax.first > y)
                        yMinAndMax.first = y;
                    if (yMinAndMax.second < y)
                        yMinAndMax.second = y;

                    *outPlot<<QPointF(static_cast<double>(a), y);
                    result = true;
                }
            }
        }
    }

    if (functionalParameter == ParameterType::BufferState)
    {
        const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, parametersSet.a);
        for (int n=0; n<=rSystem.getModel().vk_b(); n++)
        {
            double y=0;

            if ((*singlePoint)->read(y, qos, n))
            {
                if ((y > 0) || linearScale)
                {
                    *outPlot<<QPointF(n, y);
                    result = true;
                }
            }
        }
    }
    return result;
}

bool SettingsTypeForBufferState::getSinglePlot(QVector<QPair<decimal, double> > &outPlot, RSystem &rSystem, Investigator *algorithm, const ParametersSet &parametersSet) const
{
    bool result = false;

    outPlot.clear();
    if (functionalParameter == ParameterType::OfferedTrafficPerAS)
    {
        foreach(decimal a, rSystem.getAvailableAperAU())
        {
            const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, a);

            double y=0;
            if ((*singlePoint)->read(y, qos, parametersSet.bufferState))
            {
                outPlot.append(QPair<decimal, double>(a, y));
                result = true;
            }
        }
    }

    if (functionalParameter == ParameterType::BufferState)
    {
        const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, parametersSet.a);
        for (int n=0; n<=rSystem.getModel().vk_b(); n++)
        {
            double y=0;

            if ((*singlePoint)->read(y, qos, n))
            {
                outPlot.append(QPair<decimal, double>(n, y));
                result = true;
            }
        }
    }
    return result;
}

QList<ParametersSet> SettingsTypeForBufferState::getParametersList(const ModelSyst *system) const
{
    QList<ParametersSet> result;
    for (int n=0; n <= system->vk_b(); n++)
    {
        ParametersSet item;
        item.bufferState = n;
        result.append(item);
    }
    return result;
}

SettingsTypeForClassAndSystemState::SettingsTypeForClassAndSystemState(TypeForClassAndSystemState qos, QString name, QString shortName): Settings (name, shortName), qos(qos)
{
    dependencyParameters.append(ParameterType::SystemState);
    dependencyParameters.append(ParameterType::TrafficClass);
    dependencyParameters.append(ParameterType::OfferedTrafficPerAS);

    functionalParameter  = ParameterType::SystemState;
    additionalParameter2 = ParameterType::TrafficClass;
    additionalParameter1 = ParameterType::OfferedTrafficPerAS;
}

bool SettingsTypeForClassAndSystemState::getSinglePlot(QLineSeries *outPlot, QPair<double, double> &yMinAndMax, RSystem &rSystem, Investigator *algorithm, const ParametersSet &parametersSet, bool linearScale) const
{
    bool result = false;

    outPlot->clear();
    if (functionalParameter == ParameterType::OfferedTrafficPerAS)
    {
        foreach(decimal a, rSystem.getAvailableAperAU())
        {
            const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, a);
            double y=0;
            if ((*singlePoint)->read(y, qos, parametersSet.classIndex, parametersSet.systemState))
            {
                if (y > 0)
                {
                    *outPlot<<QPointF(static_cast<double>(a), y);
                    result = true;
                }
            }
        }
    }

    if (functionalParameter == ParameterType::SystemState)
    {
        const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, parametersSet.a);
        for (int n=0; n<=rSystem.getModel().V(); n++)
        {
            double y=0;
            if ((*singlePoint)->read(y, qos, parametersSet.classIndex, n))
            {
                if ((y > 0) || linearScale)
                {
                    *outPlot<<QPointF(n, y);
                    result = true;
                }
            }
        }
    }

    if (functionalParameter == ParameterType::TrafficClass)
    {
        const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, parametersSet.a);
        for (int i=0; i<rSystem.getModel().m(); i++)
        {
            double y=0;
            if ((*singlePoint)->read(y, qos, i, parametersSet.systemState))
            {
                if (y > 0)
                {
                    *outPlot<<QPointF(i, y);
                    result = true;
                }
            }
        }
    }
    return result;
}

bool SettingsTypeForClassAndSystemState::getSinglePlot(QVector<QPair<decimal, double> > &outPlot, RSystem &rSystem, Investigator *algorithm, const ParametersSet &parametersSet) const
{
    bool result = false;

    outPlot.clear();
    if (functionalParameter == ParameterType::OfferedTrafficPerAS)
    {
        foreach(decimal a, rSystem.getAvailableAperAU())
        {
            const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, a);
            double y=0;
            if ((*singlePoint)->read(y, qos, parametersSet.classIndex, parametersSet.systemState))
            {
                outPlot.append(QPair<decimal, double>(a, y));
                result = true;
            }
        }
    }

    if (functionalParameter == ParameterType::SystemState)
    {
        const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, parametersSet.a);
        for (int n=0; n<=rSystem.getModel().V(); n++)
        {
            double y=0;
            if ((*singlePoint)->read(y, qos, parametersSet.classIndex, n))
            {
                if ((y > 0))
                {
                    outPlot.append(QPair<decimal, double>(n, y));
                    result = true;
                }
            }
        }
    }

    if (functionalParameter == ParameterType::TrafficClass)
    {
        const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, parametersSet.a);
        for (int i=0; i<rSystem.getModel().m(); i++)
        {
            double y=0;
            if ((*singlePoint)->read(y, qos, i, parametersSet.systemState))
            {
                if ((y > 0))
                {
                    outPlot.append(QPair<decimal, double>(i, y));
                    result = true;
                }
            }
        }
    }
    return result;
}

QList<ParametersSet> SettingsTypeForClassAndSystemState::getParametersList(const ModelSyst *system) const
{
    QList<ParametersSet> result;
    for (int i=0; i < system->m(); i++)
    {
        for (int n=0; n <= system->V(); n++)
        {
            ParametersSet item;
            item.systemState = n;
            item.classIndex = i;
            result.append(item);
        }
    }
    return result;
}

SettingsTypeForClassAndServerState::SettingsTypeForClassAndServerState(TypeForClassAndServerState qos, QString name, QString shortName): Settings (name, shortName), qos(qos)
{
    dependencyParameters.append(ParameterType::ServerState);
    dependencyParameters.append(ParameterType::TrafficClass);
    dependencyParameters.append(ParameterType::OfferedTrafficPerAS);

    functionalParameter  = ParameterType::ServerState;
    additionalParameter2 = ParameterType::TrafficClass;
    additionalParameter1 = ParameterType::OfferedTrafficPerAS;
}

bool SettingsTypeForClassAndServerState::getSinglePlot(QLineSeries *outPlot, QPair<double, double> &yMinAndMax, RSystem &rSystem, Investigator *algorithm, const ParametersSet &parametersSet, bool linearScale) const
{
    bool result = false;

    outPlot->clear();
    if (functionalParameter == ParameterType::OfferedTrafficPerAS)
    {
        foreach(decimal a, rSystem.getAvailableAperAU())
        {
            const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, a);

            double y=0;
            if ((*singlePoint)->read(y, qos, parametersSet.classIndex, parametersSet.serverState))
            {
                if (yMinAndMax.first > y)
                    yMinAndMax.first = y;
                if (yMinAndMax.second < y)
                    yMinAndMax.second = y;

                if ((y > 0) || linearScale)
                {
                    *outPlot<<QPointF(static_cast<double>(a), y);
                    result = true;
                }
            }
        }
    }

    if (functionalParameter == ParameterType::SystemState)
    {
        const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, parametersSet.a);
        for (int n=0; n<=rSystem.getModel().vk_s(); n++)
        {
            double y=0;

            if ((*singlePoint)->read(y, qos, parametersSet.classIndex, n))
            {
                if ((y > 0) || linearScale)
                {
                    *outPlot<<QPointF(n, y);
                    result = true;
                }
            }
        }
    }

    if (functionalParameter == ParameterType::TrafficClass)
    {
        const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, parametersSet.a);
        for (int i=0; i<rSystem.getModel().m(); i++)
        {
            double y=0;

            if ((*singlePoint)->read(y, qos, i, parametersSet.serverState))
            {
                if ((y > 0) || linearScale)
                {
                    *outPlot<<QPointF(i, y);
                    result = true;
                }
            }
        }
    }
    return result;
}

bool SettingsTypeForClassAndServerState::getSinglePlot(QVector<QPair<decimal, double> > &outPlot, RSystem &rSystem, Investigator *algorithm, const ParametersSet &parametersSet) const
{
    bool result = false;

    outPlot.clear();
    if (functionalParameter == ParameterType::OfferedTrafficPerAS)
    {
        foreach(decimal a, rSystem.getAvailableAperAU())
        {
            const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, a);

            double y=0;
            if ((*singlePoint)->read(y, qos, parametersSet.classIndex, parametersSet.serverState))
            {
                outPlot.append(QPair<decimal, double>(a, y));
                result = true;
            }
        }
    }

    if (functionalParameter == ParameterType::SystemState)
    {
        const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, parametersSet.a);
        for (int n=0; n<=rSystem.getModel().vk_s(); n++)
        {
            double y=0;

            if ((*singlePoint)->read(y, qos, parametersSet.classIndex, n))
            {
                outPlot.append(QPair<decimal, double>(n, y));
                result = true;
            }
        }
    }

    if (functionalParameter == ParameterType::TrafficClass)
    {
        const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, parametersSet.a);
        for (int i=0; i<rSystem.getModel().m(); i++)
        {
            double y=0;

            if ((*singlePoint)->read(y, qos, i, parametersSet.serverState))
            {
                if (y > 0)
                {
                    outPlot.append(QPair<decimal, double>(i, y));
                    result = true;
                }
            }
        }
    }
    return result;
}

QList<ParametersSet> SettingsTypeForClassAndServerState::getParametersList(const ModelSyst *system) const
{
    QList<ParametersSet> result;
    for (int i=0; i < system->m(); i++)
    {
        for (int n=0; n <= system->vk_s(); n++)
        {
            ParametersSet item;
            item.serverState = n;
            item.classIndex = i;
            result.append(item);
        }
    }
    return result;
}

SettingsTypeForClassAndBufferState::SettingsTypeForClassAndBufferState(TypeForClassAndBufferState qos, QString name, QString shortName): Settings (name, shortName), qos(qos)
{
    dependencyParameters.append(ParameterType::BufferState);
    dependencyParameters.append(ParameterType::TrafficClass);
    dependencyParameters.append(ParameterType::OfferedTrafficPerAS);

    functionalParameter  = ParameterType::BufferState;
    additionalParameter2 = ParameterType::TrafficClass;
    additionalParameter1 = ParameterType::OfferedTrafficPerAS;
}

bool SettingsTypeForClassAndBufferState::getSinglePlot(QLineSeries *outPlot, QPair<double, double> &yMinAndMax, RSystem &rSystem, Investigator *algorithm, const ParametersSet &parametersSet, bool linearScale) const
{
    bool result = false;

    outPlot->clear();
    if (functionalParameter == ParameterType::OfferedTrafficPerAS)
    {
        foreach(decimal a, rSystem.getAvailableAperAU())
        {
            const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, a);

            double y=0;
            if ((*singlePoint)->read(y, qos, parametersSet.classIndex, parametersSet.bufferState))
            {
                if (yMinAndMax.first > y)
                    yMinAndMax.first = y;
                if (yMinAndMax.second < y)
                    yMinAndMax.second = y;

                if ((y > 0) || linearScale)
                {
                    *outPlot<<QPointF(static_cast<double>(a), y);
                    result = true;
                }
            }
        }
    }

    if (functionalParameter == ParameterType::SystemState)
    {
        const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, parametersSet.a);
        for (int n=0; n<=rSystem.getModel().vk_b(); n++)
        {
            double y=0;

            if ((*singlePoint)->read(y, qos, parametersSet.classIndex, n))
            {
                if ((y > 0) || linearScale)
                {
                    *outPlot<<QPointF(n, y);
                    result = true;
                }
            }
        }
    }

    if (functionalParameter == ParameterType::TrafficClass)
    {
        const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, parametersSet.a);
        for (int i=0; i<rSystem.getModel().m(); i++)
        {
            double y=0;

            if ((*singlePoint)->read(y, qos, i, parametersSet.bufferState))
            {
                if ((y > 0) || linearScale)
                {
                    *outPlot<<QPointF(i, y);
                    result = true;
                }
            }
        }
    }
    return result;
}

bool SettingsTypeForClassAndBufferState::getSinglePlot(QVector<QPair<decimal, double> > &outPlot, RSystem &rSystem, Investigator *algorithm, const ParametersSet &parametersSet) const
{
    bool result = false;

    outPlot.clear();
    if (functionalParameter == ParameterType::OfferedTrafficPerAS)
    {
        foreach(decimal a, rSystem.getAvailableAperAU())
        {
            const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, a);

            double y=0;
            if ((*singlePoint)->read(y, qos, parametersSet.classIndex, parametersSet.bufferState))
            {
                if ((y > 0))
                {
                    outPlot.append(QPair<decimal, double>(a, y));
                    result = true;
                }
            }
        }
    }

    if (functionalParameter == ParameterType::SystemState)
    {
        const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, parametersSet.a);
        for (int n=0; n<=rSystem.getModel().vk_b(); n++)
        {
            double y=0;

            if ((*singlePoint)->read(y, qos, parametersSet.classIndex, n))
            {
                if ((y > 0))
                {
                    outPlot.append(QPair<decimal, double>(n, y));
                    result = true;
                }
            }
        }
    }

    if (functionalParameter == ParameterType::TrafficClass)
    {
        const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, parametersSet.a);
        for (int i=0; i<rSystem.getModel().m(); i++)
        {
            double y=0;

            if ((*singlePoint)->read(y, qos, i, parametersSet.bufferState))
            {
                if ((y > 0))
                {
                    outPlot.append(QPair<decimal, double>(i, y));
                    result = true;
                }
            }
        }
    }
    return result;
}

QList<ParametersSet> SettingsTypeForClassAndBufferState::getParametersList(const ModelSyst *system) const
{
    QList<ParametersSet> result;
    for (int i=0; i < system->m(); i++)
    {
        for (int n=0; n <= system->vk_b(); n++)
        {
            ParametersSet item;
            item.bufferState = n;
            item.classIndex = i;
            result.append(item);
        }
    }
    return result;
}

SettingsInavailabilityForClassInAllGroupsInCombination::SettingsInavailabilityForClassInAllGroupsInCombination(QString name, QString shortName): Settings (name, shortName)
{
    dependencyParameters.append(ParameterType::OfferedTrafficPerAS);
    dependencyParameters.append(ParameterType::TrafficClass);
    dependencyParameters.append(ParameterType::CombinationNumber);

    functionalParameter  = ParameterType::OfferedTrafficPerAS;
    additionalParameter1 = ParameterType::TrafficClass;
    additionalParameter2 = ParameterType::CombinationNumber;
}

bool SettingsInavailabilityForClassInAllGroupsInCombination::getSinglePlot(QLineSeries *outPlot, QPair<double, double> &yMinAndMax, RSystem &rSystem, Investigator *algorithm, const ParametersSet &parametersSet, bool linearScale) const
{
    bool result = false;

    outPlot->clear();
    if (functionalParameter == ParameterType::OfferedTrafficPerAS)
    {
        foreach(decimal a, rSystem.getAvailableAperAU())
        {
            const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, a);
            double x = static_cast<double>(a);
            double y=0;
            int t = rSystem.getModel().getClass(parametersSet.classIndex)->t();

            if ((*singlePoint)->read(y, TypeResourcess_VsServerGroupsCombination::InavailabilityInAllTheGroups, t, parametersSet.combinationNumber))
            {
                if ((y > 0) || linearScale)
                {
                    if (yMinAndMax.first > y)
                        yMinAndMax.first = y;
                    if (yMinAndMax.second < y)
                        yMinAndMax.second = y;

                    *outPlot<<QPointF(x, y);
                    result = true;
                }
            }
        }
    }

    if (functionalParameter == ParameterType::NumberOfGroups)
    {
        int noOfCombinations = rSystem.getNoOfGroupsCombinations();

        const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, parametersSet.a);
        for (int n=0; n <= noOfCombinations; n++)
        {
            double y=0;
            int t = rSystem.getModel().getClass(parametersSet.classIndex)->t();

            if ((*singlePoint)->read(y, TypeResourcess_VsServerGroupsCombination::AvailabilityInAllTheGroups, t, n))
            {
                if ((y > 0) || linearScale)
                {
                    *outPlot<<QPointF(n, y);
                    result = true;
                }
            }
        }
    }

    if (functionalParameter == ParameterType::TrafficClass)
    {
        const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, parametersSet.a);
        for (int i=0; i < rSystem.getModel().m(); i++)
        {
            double y=0;
            int t = rSystem.getModel().getClass(i)->t();

            if ((*singlePoint)->read(y, TypeResourcess_VsServerGroupsCombination::AvailabilityInAllTheGroups, t, parametersSet.combinationNumber))
            {
                if ((y > 0) || linearScale)
                {
                    *outPlot<<QPointF(i, y);
                    result = true;
                }
            }
        }
    }

    return result;
}

bool SettingsInavailabilityForClassInAllGroupsInCombination::getSinglePlot(QVector<QPair<decimal, double> > &outPlot, RSystem &rSystem, Investigator *algorithm, const ParametersSet &parametersSet) const
{
    bool result = false;

    outPlot.clear();
    if (functionalParameter == ParameterType::OfferedTrafficPerAS)
    {
        foreach(decimal a, rSystem.getAvailableAperAU())
        {
            const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, a);
            double x = static_cast<double>(a);
            double y=0;
            int t = rSystem.getModel().getClass(parametersSet.classIndex)->t();

            if ((*singlePoint)->read(y, TypeResourcess_VsServerGroupsCombination::InavailabilityInAllTheGroups, t, parametersSet.combinationNumber))
            {
                if (y > 0)
                {
                    outPlot.append(QPair<decimal, double>(x, y));
                    result = true;
                }
            }
        }
    }

    if (functionalParameter == ParameterType::NumberOfGroups)
    {
        int noOfCombinations = rSystem.getNoOfGroupsCombinations();

        const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, parametersSet.a);
        for (int n=0; n <= noOfCombinations; n++)
        {
            double y=0;
            int t = rSystem.getModel().getClass(parametersSet.classIndex)->t();

            if ((*singlePoint)->read(y, TypeResourcess_VsServerGroupsCombination::AvailabilityInAllTheGroups, t, n))
            {
                if (y > 0)
                {
                    outPlot.append(QPair<decimal, double>(n, y));
                    result = true;
                }
            }
        }
    }

    if (functionalParameter == ParameterType::TrafficClass)
    {
        const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, parametersSet.a);
        for (int i=0; i < rSystem.getModel().m(); i++)
        {
            double y=0;
            int t = rSystem.getModel().getClass(i)->t();

            if ((*singlePoint)->read(y, TypeResourcess_VsServerGroupsCombination::AvailabilityInAllTheGroups, t, parametersSet.combinationNumber))
            {
                if (y > 0)
                {
                    outPlot.append(QPair<decimal, double>(i, y));
                    result = true;
                }
            }
        }
    }

    return result;
}

QList<ParametersSet> SettingsInavailabilityForClassInAllGroupsInCombination::getParametersList(const ModelSyst *system) const
{
    QList<ParametersSet> result;
    int noOfCOmbinations = Utils::UtilsLAG::getPossibleCombinations(system->k_s()).length();

    for (int i=0; i < system->m(); i++)
    {
        for (int combNo=0; combNo < noOfCOmbinations; combNo++)
        {
            ParametersSet item;
            item.combinationNumber = combNo;
            item.classIndex = i;
            result.append(item);
        }
    }
    return result;
}

SettingsAvailableSubroupDistribution::SettingsAvailableSubroupDistribution(QString name, QString shortName): Settings (name, shortName)
{
    dependencyParameters.append(ParameterType::OfferedTrafficPerAS);
    dependencyParameters.append(ParameterType::TrafficClass);
    dependencyParameters.append(ParameterType::NumberOfGroups);

    functionalParameter  = ParameterType::OfferedTrafficPerAS;
    additionalParameter1 = ParameterType::TrafficClass;
    additionalParameter2 = ParameterType::NumberOfGroups;
}

bool SettingsAvailableSubroupDistribution::getSinglePlot(QLineSeries *outPlot, QPair<double, double> &yMinAndMax, RSystem &rSystem, Investigator *algorithm, const ParametersSet &parametersSet, bool linearScale) const
{
    bool result = false;

    outPlot->clear();
    if (functionalParameter == ParameterType::OfferedTrafficPerAS)
    {
        int t = rSystem.getModel().getClass(parametersSet.classIndex)->t();
        foreach(decimal a, rSystem.getAvailableAperAU())
        {
            const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, a);
            double x = static_cast<double>(a);


            double y=0;
            if ((*singlePoint)->read(y, TypeForResourcessAndNumberOfServerGroups::AvailabilityOnlyInAllTheGroups, t, parametersSet.numberOfGroups))
            {
                if ((y > 0) || linearScale)
                {
                    if (yMinAndMax.first > y)
                        yMinAndMax.first = y;
                    if (yMinAndMax.second < y)
                        yMinAndMax.second = y;

                    *outPlot<<QPointF(x, y);
                    result = true;
                }
            }
        }
    }

    if (functionalParameter == ParameterType::NumberOfGroups)
    {
        const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, parametersSet.a);
        int t = rSystem.getModel().getClass(parametersSet.classIndex)->t();
        for (int k=0; k <= rSystem.getModel().k_s(); k++)
        {
            double y=0;

            if ((*singlePoint)->read(y, TypeForResourcessAndNumberOfServerGroups::AvailabilityOnlyInAllTheGroups, t, k))
            {
                if ((y > 0) || linearScale)
                {
                    *outPlot<<QPointF(k, y);
                    result = true;
                }
            }
        }
    }

    if (functionalParameter == ParameterType::TrafficClass)
    {
        const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, parametersSet.a);
        for (int i=0; i < rSystem.getModel().m(); i++)
        {
            double y=0;
            int t = rSystem.getModel().getClass(i)->t();

            if ((*singlePoint)->read(y, TypeForResourcessAndNumberOfServerGroups::AvailabilityOnlyInAllTheGroups, t, parametersSet.numberOfGroups))
            {
                if ((y > 0) || linearScale)
                {
                    *outPlot<<QPointF(i, y);
                    result = true;
                }
            }
        }
    }

    return result;
}

bool SettingsAvailableSubroupDistribution::getSinglePlot(QVector<QPair<decimal, double> > &outPlot, RSystem &rSystem, Investigator *algorithm, const ParametersSet &parametersSet) const
{
    bool result = false;

    outPlot.clear();
    if (functionalParameter == ParameterType::OfferedTrafficPerAS)
    {
        int t = rSystem.getModel().getClass(parametersSet.classIndex)->t();
        foreach(decimal a, rSystem.getAvailableAperAU())
        {
            const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, a);
            double x = static_cast<double>(a);


            double y=0;
            if ((*singlePoint)->read(y, TypeForResourcessAndNumberOfServerGroups::AvailabilityOnlyInAllTheGroups, t, parametersSet.numberOfGroups))
            {
                if (y > 0)
                    result = true;

                outPlot.append(QPair<decimal, double>(x, y));
            }
        }
    }

    if (functionalParameter == ParameterType::NumberOfGroups)
    {
        const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, parametersSet.a);
        int t = rSystem.getModel().getClass(parametersSet.classIndex)->t();
        for (int k=0; k <= rSystem.getModel().k_s(); k++)
        {
            double y=0;

            if ((*singlePoint)->read(y, TypeForResourcessAndNumberOfServerGroups::AvailabilityOnlyInAllTheGroups, t, k))
            {
                if (y > 0)
                    result = true;
                outPlot.append(QPair<decimal, double>(k, y));
            }
        }
    }

    if (functionalParameter == ParameterType::TrafficClass)
    {
        const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, parametersSet.a);
        for (int i=0; i < rSystem.getModel().m(); i++)
        {
            double y=0;
            int t = rSystem.getModel().getClass(i)->t();

            if ((*singlePoint)->read(y, TypeForResourcessAndNumberOfServerGroups::AvailabilityOnlyInAllTheGroups, t, parametersSet.numberOfGroups))
            {
                if (y > 0)
                    result = true;
                outPlot.append(QPair<decimal, double>(i, y));
            }
        }
    }

    return result;
}

QList<ParametersSet> SettingsAvailableSubroupDistribution::getParametersList(const ModelSyst *system) const
{
    QList<ParametersSet> result;
    for (int k=0; k < system->k_s(); k++)
    {
        ParametersSet item;
        item.numberOfGroups = k;
        result.append(item);
    }
    return result;
}

bool Settings::setFunctionalParameter(ParameterType param)
{
    bool result = false;
    if (dependencyParameters.contains(param))
    {
        result = true;
        functionalParameter = param;
        additionalParameter1 = ParameterType::None;
        additionalParameter2 = ParameterType::None;


        foreach (ParameterType tmpParam, this->dependencyParameters)
        {
            if (functionalParameter == tmpParam)
                continue;
            if (additionalParameter1 == ParameterType::None)
                additionalParameter1 = tmpParam;
            else
                additionalParameter2 = tmpParam;
        }
    }
    return result;
}

double Settings::getXmin(RSystem &rSystem) const
{
    double result = -1;

    switch (functionalParameter)
    {
    case ParameterType::OfferedTrafficPerAS:
        result = rSystem.getMinAperAU();
        break;

    case ParameterType::TrafficClass:
    case ParameterType::SystemState:
    case ParameterType::ServerState:
    case ParameterType::BufferState:
    case ParameterType::NumberOfGroups:
    case ParameterType::CombinationNumber:
        result = 0;
        break;

    case ParameterType::None:
        break;
    }

    return result;
}

double Settings::getXmax(RSystem &rSystem) const
{
    double result = -1;

    switch (functionalParameter)
    {
    case ParameterType::OfferedTrafficPerAS:
        result = rSystem.getMaxAperAU();
        break;

    case ParameterType::TrafficClass:
        result =  rSystem.getModel().m()-1;
        break;

    case ParameterType::NumberOfGroups:
        result = rSystem.getModel().k_s();
        break;

    case ParameterType::CombinationNumber:
        result = rSystem.getNoOfGroupsCombinations();
        break;

    case ParameterType::SystemState:
        result = rSystem.getModel().V();
        break;

    case ParameterType::ServerState:
        result = rSystem.getModel().vk_s();
        break;

    case ParameterType::BufferState:
        result = rSystem.getModel().vk_b();
        break;

    case ParameterType::None:
        break;

    }

    return result;
}

QString Settings::updateParameters(ParametersSet &outParameters, const QVariant &variant, ParameterType paramType, const ModelSyst *system, RSystem *resultsForSystem)
{
    QString result;

    switch (paramType)
    {
    case Results::ParameterType::OfferedTrafficPerAS:
        outParameters.a = variant.value<double>();
        result = QString("%1").arg(static_cast<double>(outParameters.a));
        break;

    case Results::ParameterType::TrafficClass:
        outParameters.classIndex = variant.value<int>();
        result = QString("%1").arg(system->getClass(outParameters.classIndex)->shortName());
        break;

    case Results::ParameterType::SystemState:
        outParameters.systemState = variant.value<int>();
        result = QString("%1").arg(outParameters.systemState);
        break;

    case Results::ParameterType::ServerState:
        outParameters.serverState = variant.value<int>();
        result = QString("%1").arg(outParameters.serverState);
        break;

    case Results::ParameterType::BufferState:
        outParameters.bufferState = variant.value<int>();
        result = QString("%1").arg(outParameters.bufferState);
        break;

    case Results::ParameterType::CombinationNumber:
        outParameters.combinationNumber = variant.value<int>();
        result = QString("%1").arg(resultsForSystem->getGroupCombinationStr(outParameters.combinationNumber));
        break;

    case Results::ParameterType::NumberOfGroups:
        outParameters.numberOfGroups = variant.value<int>();
        break;

    case Results::ParameterType::None:
        break;
    }
    return result;
}

void Settings::fillListWithParameters(QList<QVariant> &list, ParameterType paramType, const ModelSyst *system, QList<decimal> offeredTraffic)
{
    list.clear();
    QVariant tmpVariant;
    int i;
    switch (paramType)
    {
    case ParameterType::None:
        break;
    case ParameterType::TrafficClass:
        for (i=0; i<system->m(); i++)
        {
            tmpVariant.setValue<int>(i);
            list.append(tmpVariant);
        }
        break;
    case ParameterType::OfferedTrafficPerAS:
        foreach(decimal a, offeredTraffic)
        {
            tmpVariant.setValue<decimal>(a);
            list.append(tmpVariant);
        }
        break;
    case ParameterType::SystemState:
        for (i=0; i<=system->V(); i++)
        {
            tmpVariant.setValue<int>(i);
            list.append(tmpVariant);
        }
        break;
    case ParameterType::ServerState:
        for (i=0; i<=system->vk_s(); i++)
        {
            tmpVariant.setValue<int>(i);
            list.append(tmpVariant);
        }
        break;

    case ParameterType::BufferState:
        for (i=0; i<=system->vk_b(); i++)
        {
            tmpVariant.setValue<int>(i);
            list.append(tmpVariant);
        }
        break;

    case ParameterType::NumberOfGroups:
        for (i=0; i<=system->k_s(); i++)
        {
            tmpVariant.setValue<int>(i);
            list.append(tmpVariant);
        }
        break;

    case ParameterType::CombinationNumber:
        int noOfComb = Utils::UtilsLAG::getPossibleCombinations(system->k_s()).length();
        for (i=0; i < noOfComb; i++)
        {
            tmpVariant.setValue<int>(i);
            list.append(tmpVariant);
        }
        break;
    }

}

bool ParametersSet::operator<(const ParametersSet &rho) const
{
    bool result = true;

    if (a > 0)
        return a < rho.a;

    if (classIndex > 0)
        return classIndex < rho.classIndex;

    if (systemState > 0)
        return systemState < rho.systemState;

    if (serverState > 0)
        return serverState < rho.serverState;

    if (bufferState > 0)
        return bufferState < rho.bufferState;

    if (combinationNumber > 0)
        return combinationNumber < rho.combinationNumber;

    if (numberOfGroups > 0)
        return numberOfGroups < rho.numberOfGroups;

    return result;
}

} //namespace Results
