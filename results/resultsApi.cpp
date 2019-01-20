#include "resultsApi.h"

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
    _myMap.insert(Type::NewCallOfAllClassesIntensityOut_inSystemVsSystemState  , new SettingsTypeForSystemState(TypeForSystemState::IntensityNewCallOut                                , "All Classes Calls arrival intensity vs current system state"       , "LOut(n)" ));
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
                    *outPlot<<QPointF(static_cast<double>(parametersSet.a), y);
                    result = true;
                }
            }
        }
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
            if ((*singlePoint)->read(y, TypeResourcess_VsNumberOfServerGroups::AvailabilityOnlyInAllTheGroups, t, parametersSet.numberOfGroups))
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

            if ((*singlePoint)->read(y, TypeResourcess_VsNumberOfServerGroups::AvailabilityOnlyInAllTheGroups, t, k))
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

            if ((*singlePoint)->read(y, TypeResourcess_VsNumberOfServerGroups::AvailabilityOnlyInAllTheGroups, t, parametersSet.numberOfGroups))
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

} //namespace Results
