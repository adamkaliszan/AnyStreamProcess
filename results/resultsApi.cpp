#include "resultsApi.h"

namespace Results
{

bool TypesAndSettings::_isInitialized = false;

QMap<Type, Settings *> TypesAndSettings::_myMap = QMap<Type, Settings *>();

void TypesAndSettings::_initialize()
{
    if (_isInitialized)
        return;

    _myMap.insert(Type::BlockingProbability, new SettingsBlockingProbability());
    _myMap.insert(Type::LossProbability, new SettingsLossProbability());

    _myMap.insert(Type::OccupancyDistribution, new SettingsOccupancyDistribution());
    _myMap.insert(Type::OccupancyDistributionServerOnly, new SettingsOccupancyDistributionServer());
    _myMap.insert(Type::OccupancyDistributionServerBufferOnly, new SettingsOccupancyDistributionBuffer());

    _myMap.insert(Type::NumberOfCallsInStateN, new SettingsNumberOfCallsInStateN());
    _myMap.insert(Type::NumberOfCallsInStateN_inServer, new SettingsNumberOfCallsInStateN_inServer());
    _myMap.insert(Type::NumberOfCallsInStateN_inBuffer, new SettingsNumberOfCallsInStateN_inBuffer());

    _myMap.insert(Type::AllSugbrupsInGivenCombinationAndClassAvailable, new SettingsInavailabilityForClassInAllGroupsInCombination());
    _myMap.insert(Type::AvailableSubroupDistribution, new SettingsAvailableSubroupDistribution());

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
    QString result;
    switch (type)
    {
    case Type::BlockingProbability:
        result = "Blocking probability";
        break;

    case Type::LossProbability:
        result = "Loss probability";
        break;

    case Type::OccupancyDistribution:
        result = "Traffic distribution in a system";
        break;

    case Type::OccupancyDistributionServerOnly:
        result = "Traffic distribution in a server";
        break;

    case Type::OccupancyDistributionServerBufferOnly:
        result = "Traffic distribution in a buffer";
        break;

    case Type::NumberOfCallsInStateN:
        result = "Avarage number of calls in a state";
        break;

    case Type::NumberOfCallsInStateN_inServer:
        result = "Avarage number of calls in a server";
        break;

    case Type::NumberOfCallsInStateN_inBuffer:
        result = "Avarage number of calls in a buffer";
        break;

    case Type::AllSugbrupsInGivenCombinationAndClassAvailable:
        result = "Availability of all subgroup in a combination";
        break;

    case Type::AvailableSubroupDistribution:
        result = "Distribution of subgroup availability";
        break;
    }
    return result;
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

SettingsBlockingProbability::SettingsBlockingProbability()
{
    dependencyParameters.append(ParameterType::OfferedTrafficPerAS);
    dependencyParameters.append(ParameterType::TrafficClass);

    functionalParameter  = ParameterType::OfferedTrafficPerAS;
    additionalParameter1 = ParameterType::TrafficClass;
    additionalParameter2 = ParameterType::None;
}

bool SettingsBlockingProbability::getSinglePlot(QLineSeries *outPlot, RSystem &rSystem, Investigator *algorithm, const ParametersSet &parametersSet, bool linearScale) const
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

            if ((*singlePoint)->read(y, TypeForClass::BlockingProbability, parametersSet.classIndex))
            {
                if ((y>0) || linearScale)
                {
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
            if ((*singlePoint)->read(y, TypeForClass::BlockingProbability, i))
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

SettingsLossProbability::SettingsLossProbability()
{
    dependencyParameters.append(ParameterType::OfferedTrafficPerAS);
    dependencyParameters.append(ParameterType::TrafficClass);

    functionalParameter  = ParameterType::OfferedTrafficPerAS;
    additionalParameter1 = ParameterType::TrafficClass;
    additionalParameter2 = ParameterType::None;
}

bool SettingsLossProbability::getSinglePlot(QLineSeries *outPlot, RSystem &rSystem, Investigator *algorithm, const ParametersSet &parametersSet, bool linearScale) const
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

            if ((*singlePoint)->read(y, TypeForClass::LossProbability, parametersSet.classIndex))
            {
                if ((y>0) || linearScale)
                {
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
            if ((*singlePoint)->read(y, TypeForClass::LossProbability, i))
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

SettingsOccupancyDistribution::SettingsOccupancyDistribution()
{
    dependencyParameters.append(ParameterType::OfferedTrafficPerAS);
    dependencyParameters.append(ParameterType::SystemState);

    functionalParameter  = ParameterType::SystemState;
    additionalParameter1 = ParameterType::OfferedTrafficPerAS;
    additionalParameter2 = ParameterType::None;
}

bool SettingsOccupancyDistribution::getSinglePlot(QLineSeries *outPlot, RSystem &rSystem, Investigator *algorithm, const ParametersSet &parametersSet, bool linearScale) const
{
    bool result = false;

    outPlot->clear();
    if (functionalParameter == ParameterType::OfferedTrafficPerAS)
    {
        foreach(decimal a, rSystem.getAvailableAperAU())
        {
            const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, a);

            double y=0;
            if ((*singlePoint)->read(y, TypeForSystemState::StateProbability, parametersSet.systemState))
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

            if ((*singlePoint)->read(y, TypeForSystemState::StateProbability, n))
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

SettingsOccupancyDistributionServer::SettingsOccupancyDistributionServer()
{
    dependencyParameters.append(ParameterType::OfferedTrafficPerAS);
    dependencyParameters.append(ParameterType::ServerState);

    functionalParameter  = ParameterType::ServerState;
    additionalParameter1 = ParameterType::OfferedTrafficPerAS;
    additionalParameter2 = ParameterType::None;
}

bool SettingsOccupancyDistributionServer::getSinglePlot(QLineSeries *outPlot, RSystem &rSystem, Investigator *algorithm, const ParametersSet &parametersSet, bool linearScale) const
{
    bool result = false;

    outPlot->clear();
    if (functionalParameter == ParameterType::OfferedTrafficPerAS)
    {
        foreach(decimal a, rSystem.getAvailableAperAU())
        {
            const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, a);

            double y=0;
            if ((*singlePoint)->read(y, TypeForServerState::StateProbability, parametersSet.serverState))
            {
                if ((y > 0) || linearScale)
                {
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

            if ((*singlePoint)->read(y, TypeForServerState::StateProbability, n))
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

SettingsOccupancyDistributionBuffer::SettingsOccupancyDistributionBuffer()
{
    dependencyParameters.append(ParameterType::OfferedTrafficPerAS);
    dependencyParameters.append(ParameterType::BufferState);

    functionalParameter  = ParameterType::BufferState;
    additionalParameter1 = ParameterType::OfferedTrafficPerAS;
    additionalParameter2 = ParameterType::None;
}

bool SettingsOccupancyDistributionBuffer::getSinglePlot(QLineSeries *outPlot, RSystem &rSystem, Investigator *algorithm, const ParametersSet &parametersSet, bool linearScale) const
{
    bool result = false;

    outPlot->clear();
    if (functionalParameter == ParameterType::OfferedTrafficPerAS)
    {
        foreach(decimal a, rSystem.getAvailableAperAU())
        {
            const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, a);

            double y=0;
            if ((*singlePoint)->read(y, TypeForBufferState::StateProbability, parametersSet.bufferState))
            {
                if ((y > 0) || linearScale)
                {
                    *outPlot<<QPointF(static_cast<double>(a), y);
                    result = true;
                }
            }
        }
    }

    if (functionalParameter == ParameterType::BufferState)
    {
        const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, parametersSet.a);
        for (int n=0; n<=rSystem.getModel().V_b(); n++)
        {
            double y=0;

            if ((*singlePoint)->read(y, TypeForSystemState::StateProbability, n))
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

SettingsNumberOfCallsInStateN::SettingsNumberOfCallsInStateN()
{
    dependencyParameters.append(ParameterType::SystemState);
    dependencyParameters.append(ParameterType::TrafficClass);
    dependencyParameters.append(ParameterType::OfferedTrafficPerAS);

    functionalParameter  = ParameterType::SystemState;
    additionalParameter2 = ParameterType::TrafficClass;
    additionalParameter1 = ParameterType::OfferedTrafficPerAS;
}

bool SettingsNumberOfCallsInStateN::getSinglePlot(QLineSeries *outPlot, RSystem &rSystem, Investigator *algorithm, const ParametersSet &parametersSet, bool linearScale) const
{

}

SettingsNumberOfCallsInStateN_inServer::SettingsNumberOfCallsInStateN_inServer()
{
    dependencyParameters.append(ParameterType::ServerState);
    dependencyParameters.append(ParameterType::TrafficClass);
    dependencyParameters.append(ParameterType::OfferedTrafficPerAS);

    functionalParameter  = ParameterType::ServerState;
    additionalParameter2 = ParameterType::TrafficClass;
    additionalParameter1 = ParameterType::OfferedTrafficPerAS;
}

bool SettingsNumberOfCallsInStateN_inServer::getSinglePlot(QLineSeries *outPlot, RSystem &rSystem, Investigator *algorithm, const ParametersSet &parametersSet, bool linearScale) const
{

}

SettingsNumberOfCallsInStateN_inBuffer::SettingsNumberOfCallsInStateN_inBuffer()
{
    dependencyParameters.append(ParameterType::BufferState);
    dependencyParameters.append(ParameterType::TrafficClass);
    dependencyParameters.append(ParameterType::OfferedTrafficPerAS);

    functionalParameter  = ParameterType::BufferState;
    additionalParameter2 = ParameterType::TrafficClass;
    additionalParameter1 = ParameterType::OfferedTrafficPerAS;
}

bool SettingsNumberOfCallsInStateN_inBuffer::getSinglePlot(QLineSeries *outPlot, RSystem &rSystem, Investigator *algorithm, const ParametersSet &parametersSet, bool linearScale) const
{

}

SettingsInavailabilityForClassInAllGroupsInCombination::SettingsInavailabilityForClassInAllGroupsInCombination()
{
    dependencyParameters.append(ParameterType::OfferedTrafficPerAS);
    dependencyParameters.append(ParameterType::TrafficClass);
    dependencyParameters.append(ParameterType::CombinationNumber);

    functionalParameter  = ParameterType::OfferedTrafficPerAS;
    additionalParameter1 = ParameterType::TrafficClass;
    additionalParameter2 = ParameterType::CombinationNumber;
}

bool SettingsInavailabilityForClassInAllGroupsInCombination::getSinglePlot(QLineSeries *outPlot, RSystem &rSystem, Investigator *algorithm, const ParametersSet &parametersSet, bool linearScale) const
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
            if ((*singlePoint)->read(y, TypeClassForServerGroupsCombination::SerImpossibilityInAllTheSubgroups, parametersSet.classIndex, parametersSet.combinationNumber))
            {
                if ((y > 0) || linearScale)
                {
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

            if ((*singlePoint)
                    ->read(y, TypeClassForServerGroupsCombination::SerPossibilityInAllTheSubgroups, parametersSet.classIndex, n))
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

            if ((*singlePoint)->read(y, TypeClassForServerGroupsCombination::SerPossibilityInAllTheSubgroups, i, parametersSet.numberOfGroups))
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


SettingsAvailableSubroupDistribution::SettingsAvailableSubroupDistribution()
{
    dependencyParameters.append(ParameterType::OfferedTrafficPerAS);
    dependencyParameters.append(ParameterType::TrafficClass);
    dependencyParameters.append(ParameterType::NumberOfGroups);

    functionalParameter  = ParameterType::OfferedTrafficPerAS;
    additionalParameter1 = ParameterType::TrafficClass;
    additionalParameter2 = ParameterType::NumberOfGroups;
}

bool SettingsAvailableSubroupDistribution::getSinglePlot(QLineSeries *outPlot, RSystem &rSystem, Investigator *algorithm, const ParametersSet &parametersSet, bool linearScale) const
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
            if ((*singlePoint)->read(y, TypeClassForServerExactGroupsSet::ServPossibilityOnlyInAllTheSubgroups, parametersSet.classIndex, parametersSet.numberOfGroups))
            {
                if ((y > 0) || linearScale)
                {
                    *outPlot<<QPointF(x, y);
                    result = true;
                }
            }
        }
    }

    if (functionalParameter == ParameterType::NumberOfGroups)
    {
        const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, parametersSet.a);
        for (int k=0; k <= rSystem.getModel().Ks(); k++)
        {
            double y=0;

            if ((*singlePoint)->read(y, TypeStateForServerGroupsSet::AvailabilityOnlyInAllTheGroups, parametersSet.classIndex, k))
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

            if ((*singlePoint)->read(y, TypeStateForServerGroupsSet::AvailabilityOnlyInAllTheGroups, i, parametersSet.numberOfGroups))
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
        result = rSystem.getModel().Ks();
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
        result = rSystem.getModel().V_b();
        break;

    case ParameterType::None:
        break;

    }

    return result;
}

} //namespace Results
