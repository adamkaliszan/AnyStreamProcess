#include <cmath>
#include "resultsApi.h"

#include "utils/lag.h"

using namespace QtDataVisualization;

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

    _myMap.insert(Type::OccupancyDistributionServerAndBuffer                   , new SettingsTypeForServerAndBufferState(TypeForServerAngBufferState::StateProbability                 , "Occupancy distribution 2d       "                     , "P(n_s, n_b)"));

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

    _myMap.insert(Type::AllSugbrupsInGivenCombinationNotAvailableForCallsOfGivenClass, new SettingsForClassAndServerGroupsCombination(TypeClasses_VsServerGroupsCombination::InavailabilityInAllTheGroups, "All groups in combination are not available for class i" , "combNA_i"));
    _myMap.insert(Type::AllSugbrupsInGivenCombinationAvailableForCallsOfGivenClass   , new SettingsForClassAndServerGroupsCombination(TypeClasses_VsServerGroupsCombination::AvailabilityInAllTheGroups  , "All groups in combination are available for class i"     , "combAA_i"));

    _myMap.insert(Type::AvailableSubroupDistribution
      , new SettingsAvailableSubroupDistribution(                  "Distribution of available groups"                           , "RDP"));

    _isInitialized = true;
}

void TypesAndSettings::release()
{
    if (_isInitialized)
    {
        foreach (Type tmp, _myMap.keys())
        {
            delete _myMap[tmp];
        }
        _myMap.clear();
        _isInitialized = false;
    }
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
    switch (tmp->getFunctionalParameterX())
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

QMap<ParametersSet, QVector<double>> TypesAndSettings::getPlotsValues(RSystem &rSystem, Type qos, ParameterType functionalParameter, Investigator *algorithm)
{
    Settings *qosSettings = _myMap[qos];
    qosSettings->setFunctionalParameterX(functionalParameter);
    QList<ParametersSet> sysParams = qosSettings->getParametersList(rSystem.getModel(), rSystem.getAvailableAperAU());

    QMap<ParametersSet, QVector<double>> result;

    foreach (ParametersSet sysParam, sysParams)
    {
        QVector<double> vector;
        qosSettings->getSinglePlot(vector, rSystem, algorithm, sysParam);
        result.insert(sysParam, vector);
    }
    return result;
}

QMap<ParametersSet, QVector<double> > TypesAndSettings::getPlotsValues3d(RSystem &rSystem, Type qos, ParameterType functionalParameter1, ParameterType functionalParameter2, Investigator *algorithm)
{
    Settings *qosSettings = _myMap[qos];
    qosSettings->setFunctionalParameterX(functionalParameter1);
    qosSettings->setFunctionalParameterY(functionalParameter2);
    QList<ParametersSet> sysParams = qosSettings->getParametersList(rSystem.getModel(), rSystem.getAvailableAperAU());

    QMap<ParametersSet, QVector<double>> result;

    foreach (ParametersSet sysParam, sysParams)
    {
        QVector<double> vector;
        qosSettings->getSinglePlot(vector, rSystem, algorithm, sysParam);
        result.insert(sysParam, vector);
    }
    return result;
}

const QVector<decimal> TypesAndSettings::getPlotsXorZ(RSystem &rSystem, ParameterType functionalParameter)
{
    QVector<decimal> result;

    int noOfComb = Utils::UtilsLAG::getPossibleCombinations(rSystem.getModel().getServer().k()).length();
    switch (functionalParameter)
    {
    case ParameterType::OfferedTrafficPerAS:
        foreach(decimal a, rSystem.getAvailableAperAU())
            result.append(a);
        break;

    case ParameterType::TrafficClass:
        for (int n=0; n < rSystem.getModel().m(); n++)
            result.append(n);
        break;

    case ParameterType::SystemState:
        for (int n=0; n <= rSystem.getModel().V(); n++)
            result.append(n);
        break;

    case ParameterType::ServerState:
        for (int n=0; n <= rSystem.getModel().getServer().V(); n++)
            result.append(n);
        break;

    case ParameterType::BufferState:
        for (int n=0; n <= rSystem.getModel().getBuffer().V(); n++)
            result.append(n);
        break;

    case ParameterType::NumberOfGroups:
        for (int n=0; n <= rSystem.getModel().getServer().k(); n++)
            result.append(n);
        break;

    case ParameterType::CombinationNumber:
        for (int n=0; n < noOfComb; n++)
            result.append(n);
        break;

    case ParameterType::None:
        break;
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

    functionalParameterX  = ParameterType::OfferedTrafficPerAS;
    functionalParameterZ  = ParameterType::None;
    additionalParameter[0] = ParameterType::TrafficClass;
    additionalParameter[1] = ParameterType::None;
    additionalParameter[2] = ParameterType::None;
}

bool SettingsTypeForClass::getSinglePlot(QLineSeries *outPlot, QPair<double, double> &yMinAndMax, RSystem &rSystem, Investigator *algorithm, const ParametersSet &parametersSet, bool linearScale) const
{
    bool result = false;

    outPlot->clear();
    if (functionalParameterX == ParameterType::OfferedTrafficPerAS)
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
                    *outPlot<<QPointF(x, y);
                    result = true;
                }
                yMinAndMax.first = qMin<double>(yMinAndMax.first, y);
                yMinAndMax.second = qMax<double>(yMinAndMax.second, y);
            }
        }
    }

    if (functionalParameterX == ParameterType::TrafficClass)
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
                yMinAndMax.first = qMin<double>(yMinAndMax.first, y);
                yMinAndMax.second = qMax<double>(yMinAndMax.second, y);
            }
        }
    }
    return result;
}

QList<ParametersSet> SettingsTypeForClass::getParametersList(const ModelSystem &system, const QList<decimal> &aOfPerAU) const
{
    QList<ParametersSet> result;
    int i;
    decimal a;
    switch (functionalParameterX)
    {
    case ParameterType::OfferedTrafficPerAS:
        switch(functionalParameterZ)
        {
            case ParameterType::None:
                for (i=0; i<system.m(); i++)
                {
                    ParametersSet item;
                    item.classIndex = i;
                    item.a = -1;
                    result.append(item);
                }
                break;

            case ParameterType::TrafficClass:
            {
                ParametersSet item;
                item.classIndex = -1;
                item.a = -1;
                result.append(item);
                break;
            }
            case ParameterType::OfferedTrafficPerAS:
                qFatal("Funstional parameters for X and Z axis are the same");

            default:
                qFatal("Wrong functional parameter 2");
        }
        break;

    case ParameterType::TrafficClass:
        switch(functionalParameterZ)
        {
        case ParameterType::None:
            foreach (a, aOfPerAU)
            {
                ParametersSet item;
                item.classIndex = -1;
                item.a = a;
                result.append(item);
            }
            break;

        case ParameterType::OfferedTrafficPerAS:
        {
            ParametersSet item;
            item.classIndex = -1;
            item.a = -1;
            result.append(item);
            break;
        }
        case ParameterType::TrafficClass:
            qFatal("Funstional parameters for X and Z axis are the same");

        default:
            qFatal("Wrong functional parameter 2");
        }
        break;

    default:
        qFatal("Wrong functional parameter");
    }
    return result;
}

bool SettingsTypeForClass::getSinglePlot(QVector<double> &outPlot, RSystem &rSystem, Investigator *algorithm, const ParametersSet &parametersSet) const
{
    bool result = false;

    outPlot.clear();
    if (functionalParameterX == ParameterType::OfferedTrafficPerAS)
    {
        foreach(decimal a, rSystem.getAvailableAperAU())
        {
            const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, a);

            double y=0;

            if ((*singlePoint)->read(y, qos, parametersSet.classIndex))
            {
                if (y>0)
                    result = true;
            }
            outPlot.append(y);
        }
    }

    if (functionalParameterX == ParameterType::TrafficClass)
    {
        const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, parametersSet.a);

        for (int i=0; i <rSystem.getModel().m(); i++)
        {
            double y=0;
            if ((*singlePoint)->read(y, qos, i))
            {
                if (y>0)
                    result = true;
            }
            outPlot.append(y);
        }
    }
    return result;
}

bool SettingsTypeForClass::getSinglePlot3d(QScatter3DSeries &outPlot, RSystem &rSystem, Investigator *algorithm, const ParametersSet &parametersSet) const
{
    bool result = false;

    float x, y, z;
    if (functionalParameterZ == ParameterType::OfferedTrafficPerAS)
    {
        foreach(decimal a, rSystem.getAvailableAperAU())
        {
            const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, a);
            z = static_cast<float>(a);

            for (int i=0; i < rSystem.getModel().m(); i++)
            {
                x = parametersSet.classIndex;

                double yTmp;
                if ((*singlePoint)->read(yTmp, qos, parametersSet.classIndex))
                {
                    if (yTmp>0)
                        result = true;

                    y = static_cast<float>(yTmp);
                    QScatterDataItem tmp(QVector3D(x, y, z));
                    outPlot.dataProxy()->addItem(tmp);
                }
            }
        }
    }

    if (functionalParameterZ == ParameterType::TrafficClass)
    {

        for (int i=0; i <rSystem.getModel().m(); i++)
        {
            z = i;

            foreach(decimal a, rSystem.getAvailableAperAU())
            {
                const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, a);
                x = static_cast<float>(a);

                double yTmp=0;
                if ((*singlePoint)->read(yTmp, qos, i))
                {
                    if (yTmp>0)
                        result = true;
                    y = static_cast<float>(yTmp);

                    QScatterDataItem tmp(QVector3D(x, y, z));
                    outPlot.dataProxy()->addItem(tmp);
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

    functionalParameterX  = ParameterType::SystemState;
    functionalParameterZ  = ParameterType::None;
    additionalParameter[0] = ParameterType::OfferedTrafficPerAS;
    additionalParameter[1] = ParameterType::None;
    additionalParameter[2] = ParameterType::None;
}

bool SettingsTypeForSystemState::getSinglePlot(QLineSeries *outPlot, QPair<double, double> &yMinAndMax, RSystem &rSystem, Investigator *algorithm, const ParametersSet &parametersSet, bool linearScale) const
{
    bool result = false;

    outPlot->clear();
    if (functionalParameterX == ParameterType::OfferedTrafficPerAS)
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
                yMinAndMax.first = qMin<double>(yMinAndMax.first, y);
                yMinAndMax.second = qMax<double>(yMinAndMax.second, y);
            }
        }
    }

    if (functionalParameterX == ParameterType::SystemState)
    {
        const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, parametersSet.a);
        for (int n=0; n<=rSystem.getModel().V(); n++)
        {
            double y=0;

            if ((*singlePoint)->read(y, qos, n))
            {
                if ((y > 0) || linearScale)
                {
                    *outPlot<<QPointF(n, y);
                    result = true;
                }
                yMinAndMax.first = qMin<double>(yMinAndMax.first, y);
                yMinAndMax.second = qMax<double>(yMinAndMax.second, y);
            }
        }
    }
    return result;
}

bool SettingsTypeForSystemState::getSinglePlot(QVector<double> &outPlot, RSystem &rSystem, Investigator *algorithm, const ParametersSet &parametersSet) const
{
    bool result = false;

    outPlot.clear();
    if (functionalParameterX == ParameterType::OfferedTrafficPerAS)
    {
        foreach(decimal a, rSystem.getAvailableAperAU())
        {
            const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, a);



            double y=0;

            if (functionalParameterZ == ParameterType::None)
            {
                if ((*singlePoint)->read(y, qos, parametersSet.systemState))
                {
                    if (y > 0)
                        result = true;
                }
                outPlot.append(y);
            }
            else if (functionalParameterZ == ParameterType::SystemState)
            {
                for (int n=0; n <= rSystem.getModel().V(); n++)
                {

                    if ((*singlePoint)->read(y, qos, parametersSet.systemState))
                    {
                        if (y > 0)
                            result = true;
                    }
                    else
                    {
                        y = 0;
                    }
                    outPlot.append(y);
                }
            }
        }
    }

    if (functionalParameterX == ParameterType::SystemState)
    {
        for (int n=0; n<=rSystem.getModel().V(); n++)
        {
            double y=0;

            if (functionalParameterZ == ParameterType::None)
            {
                const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, parametersSet.a);
                if ((*singlePoint)->read(y, qos, n))
                {
                    if (y > 0)
                        result = true;
                }
                outPlot.append(y);
            }
            else if (functionalParameterZ == ParameterType::OfferedTrafficPerAS)
            {
                foreach(decimal a, rSystem.getAvailableAperAU())
                {
                    const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, a);

                    if ((*singlePoint)->read(y, qos, parametersSet.systemState))
                    {
                        if (y > 0)
                            result = true;
                    }
                    else
                    {
                        y = 0;
                    }
                    outPlot.append(y);
                }
            }
        }
    }
    return result;
}

bool SettingsTypeForSystemState::getSinglePlot3d(QScatter3DSeries &outPlot, RSystem &rSystem, Investigator *algorithm, const ParametersSet &parametersSet) const
{
    bool result = false;
    float x, y, z;

    if (functionalParameterZ == ParameterType::OfferedTrafficPerAS)
    {
        foreach(decimal a, rSystem.getAvailableAperAU())
        {
            const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, a);
            z = static_cast<float>(a);

            for (int n=0; n <= rSystem.getModel().V(); n++)
            {
                x = n;

                double yTmp=0;
                if ((*singlePoint)->read(yTmp, qos, n))
                {
                    if (yTmp > 0)
                        result = true;

                    y = static_cast<float>(yTmp);
                    QScatterDataItem tmp(QVector3D(x, y, z));
                    outPlot.dataProxy()->addItem(tmp);
                }
            }
        }
    }

    if (functionalParameterZ == ParameterType::SystemState)
    {
        for (int n=0; n<=rSystem.getModel().V(); n++)
        {
            z = parametersSet.systemState;

            foreach(decimal a, rSystem.getAvailableAperAU())
            {
                const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, a);
                x = static_cast<float>(a);

                double yTmp=0;

                if ((*singlePoint)->read(yTmp, qos, n))
                {
                    if (yTmp > 0)
                        result = true;

                    y = static_cast<float>(yTmp);
                    QScatterDataItem tmp(QVector3D(x, y, z));
                    outPlot.dataProxy()->addItem(tmp);
                }
            }
        }
    }
    return result;
}

QList<ParametersSet> SettingsTypeForSystemState::getParametersList(const ModelSystem &system, const QList<decimal> &aOfPerAU) const
{
    QList<ParametersSet> result;
    int n;

    switch (functionalParameterX)
    {
    case ParameterType::OfferedTrafficPerAS:
        switch(functionalParameterZ)
        {
        case ParameterType::None:
            for (n=0; n <= system.V(); n++)
            {
                ParametersSet item;
                item.systemState = n;
                result.append(item);
            }
            break;
        case ParameterType::SystemState:
        {
            ParametersSet item;
            item.systemState = -1;
            item.a = -1;
            result.append(item);
            break;
        }

        case ParameterType::OfferedTrafficPerAS:
            qFatal("Funstional parameters for X and Z axis are the same");

        default:
            qFatal("Wrong functional parameter 2");
        }
        break;
    case ParameterType::SystemState:
        switch(functionalParameterZ)
        {
        case ParameterType::None:
            foreach (decimal a, aOfPerAU)
            {
                ParametersSet item;
                item.systemState = -1;
                item.a = a;
                result.append(item);
            }
            break;
        case ParameterType::OfferedTrafficPerAS:
        {
            ParametersSet item;
            item.systemState = -1;
            item.a = -1;
            result.append(item);
            break;
        }

        case ParameterType::SystemState:
            qFatal("Funstional parameters for X and Z axis are the same");

        default:
            qFatal("Wrong functional parameter 2");
        }
        break;
    default:
        qFatal("Wrong functional parameter");
    }

    return result;
}

SettingsTypeForServerState::SettingsTypeForServerState(TypeForServerState qos, QString name, QString shortName): Settings (name, shortName), qos(qos)
{
    dependencyParameters.append(ParameterType::OfferedTrafficPerAS);
    dependencyParameters.append(ParameterType::ServerState);

    functionalParameterX  = ParameterType::ServerState;
    functionalParameterZ  = ParameterType::None;
    additionalParameter[0] = ParameterType::OfferedTrafficPerAS;
    additionalParameter[1] = ParameterType::None;
    additionalParameter[2] = ParameterType::None;
}

bool SettingsTypeForServerState::getSinglePlot(QLineSeries *outPlot, QPair<double, double> &yMinAndMax, RSystem &rSystem, Investigator *algorithm, const ParametersSet &parametersSet, bool linearScale) const
{
    bool result = false;

    outPlot->clear();
    if (functionalParameterX == ParameterType::OfferedTrafficPerAS)
    {
        foreach(decimal a, rSystem.getAvailableAperAU())
        {
            const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, a);

            double y=0;
            if ((*singlePoint)->read(y, qos, parametersSet.serverState))
            {
                if ((y > 0) || linearScale)
                {
                    *outPlot<<QPointF(static_cast<double>(a), y);
                    result = true;
                }
                yMinAndMax.first = qMin<double>(yMinAndMax.first, y);
                yMinAndMax.second = qMax<double>(yMinAndMax.second, y);
            }
        }
    }

    if (functionalParameterX == ParameterType::ServerState)
    {
        const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, parametersSet.a);
        for (int n=0; n<=rSystem.getModel().getServer().V(); n++)
        {
            double y=0;

            if ((*singlePoint)->read(y, qos, n))
            {
                if ((y > 0) || linearScale)
                {
                    *outPlot<<QPointF(n, y);
                    result = true;
                }
                yMinAndMax.first = qMin<double>(yMinAndMax.first, y);
                yMinAndMax.second = qMax<double>(yMinAndMax.second, y);
            }
        }
    }
    return result;
}

bool SettingsTypeForServerState::getSinglePlot(QVector<double> &outPlot, RSystem &rSystem, Investigator *algorithm, const ParametersSet &parametersSet) const
{
    bool result = false;

    outPlot.clear();
    if (functionalParameterX == ParameterType::OfferedTrafficPerAS)
    {
        foreach(decimal a, rSystem.getAvailableAperAU())
        {
            const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, a);

            double y=0;
            if ((*singlePoint)->read(y, qos, parametersSet.serverState))
            {
                if (y > 0)
                    result = true;
            }
            outPlot.append(y);
        }
    }

    if (functionalParameterX == ParameterType::ServerState)
    {
        const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, parametersSet.a);
        for (int n=0; n<=rSystem.getModel().getServer().V(); n++)
        {
            double y=0;

            if ((*singlePoint)->read(y, qos, n))
            {
                if (y > 0)
                    result = true;
            }
            outPlot.append(y);
        }
    }
    return result;
}

bool SettingsTypeForServerState::getSinglePlot3d(QScatter3DSeries &outPlot, RSystem &rSystem, Investigator *algorithm, const ParametersSet &parametersSet) const
{
    (void) parametersSet;
    bool result = false;
    float x, y, z;

    if (functionalParameterZ == ParameterType::OfferedTrafficPerAS)
    {
        foreach(decimal a, rSystem.getAvailableAperAU())
        {
            const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, a);
            z = static_cast<float>(a);

            for (int n=0; n <= rSystem.getModel().getServer().V(); n++)
            {
                x = n;

                double yTmp=0;
                if ((*singlePoint)->read(yTmp, qos, n))
                {
                    if (yTmp > 0)
                        result = true;
                    y = static_cast<float>(yTmp);
                    QScatterDataItem tmp(QVector3D(x, y, z));
                    outPlot.dataProxy()->addItem(tmp);
                }
            }
        }
    }

    if (functionalParameterZ == ParameterType::ServerState)
    {
        for (int n=0; n<=rSystem.getModel().getServer().V(); n++)
        {
            z = n;

            foreach(decimal a, rSystem.getAvailableAperAU())
            {
                const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, a);
                x = static_cast<float>(a);

                double yTmp=0;
                QVector<double> newRow;

                if ((*singlePoint)->read(yTmp, qos, n))
                {
                    if (yTmp > 0)
                        result = true;
                    y = static_cast<float>(yTmp);
                    QScatterDataItem tmp(QVector3D(x, y, z));
                    outPlot.dataProxy()->addItem(tmp);
                }
            }
        }
    }
    return result;
}

QList<ParametersSet> SettingsTypeForServerState::getParametersList(const ModelSystem &system, const QList<decimal> &aOfPerAU) const
{
    QList<ParametersSet> result;
    int n;
    decimal a;

    switch (functionalParameterX)
    {
    case ParameterType::OfferedTrafficPerAS:
        switch(functionalParameterZ)
        {
        case ParameterType::None:
            for (n=0; n <= system.getServer().V(); n++)
            {
                ParametersSet item;
                item.serverState = n;
                item.a = -1;
                result.append(item);
            }
            break;

        case ParameterType::ServerState:
        {
            ParametersSet item;
            item.serverState = -1;
            item.a = -1;
            result.append(item);
            break;
        }

        case ParameterType::OfferedTrafficPerAS:
            qFatal("Funstional parameters for X and Z axis are the same");

        default:
            qFatal("Wrong functional parameter 2");
        }
        break;

    case ParameterType::ServerState:
        switch(functionalParameterZ)
        {
        case ParameterType::None:
            foreach (a, aOfPerAU)
            {
                ParametersSet item;
                item.serverState = -1;
                item.a = a;
                result.append(item);
            }
            break;
        case ParameterType::OfferedTrafficPerAS:
        {
            ParametersSet item;
            item.serverState = -1;
            item.a = -1;
            result.append(item);
            break;
        }

        case ParameterType::ServerState:
            qFatal("Funstional parameters for X and Z axis are the same");

        default:
            qFatal("Wrong functional parameter 2");
        }
        break;

    default:
        qFatal("Wrong functional parameter");
    }
    return result;
}

SettingsTypeForBufferState::SettingsTypeForBufferState(TypeForBufferState qos, QString name, QString shortName): Settings (name, shortName), qos(qos)
{
    dependencyParameters.append(ParameterType::OfferedTrafficPerAS);
    dependencyParameters.append(ParameterType::BufferState);

    functionalParameterX  = ParameterType::BufferState;
    functionalParameterZ  = ParameterType::None;
    additionalParameter[0] = ParameterType::OfferedTrafficPerAS;
    additionalParameter[1] = ParameterType::None;
    additionalParameter[2] = ParameterType::None;
}

bool SettingsTypeForBufferState::getSinglePlot(QLineSeries *outPlot, QPair<double, double> &yMinAndMax, RSystem &rSystem, Investigator *algorithm, const ParametersSet &parametersSet, bool linearScale) const
{
    bool result = false;

    outPlot->clear();
    if (functionalParameterX == ParameterType::OfferedTrafficPerAS)
    {
        foreach(decimal a, rSystem.getAvailableAperAU())
        {
            const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, a);

            double y=0;
            if ((*singlePoint)->read(y, qos, parametersSet.bufferState))
            {
                if ((y > 0) || linearScale)
                {
                    *outPlot<<QPointF(static_cast<double>(a), y);
                    result = true;
                }
                yMinAndMax.first = qMin<double>(yMinAndMax.first, y);
                yMinAndMax.second = qMax<double>(yMinAndMax.second, y);
            }
        }
    }

    if (functionalParameterX == ParameterType::BufferState)
    {
        const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, parametersSet.a);
        for (int n=0; n<=rSystem.getModel().getBuffer().V(); n++)
        {
            double y=0;

            if ((*singlePoint)->read(y, qos, n))
            {
                if ((y > 0) || linearScale)
                {
                    *outPlot<<QPointF(n, y);
                    result = true;
                }
                yMinAndMax.first = qMin<double>(yMinAndMax.first, y);
                yMinAndMax.second = qMax<double>(yMinAndMax.second, y);
            }
        }
    }
    return result;
}

bool SettingsTypeForBufferState::getSinglePlot(QVector<double> &outPlot, RSystem &rSystem, Investigator *algorithm, const ParametersSet &parametersSet) const
{
    bool result = false;

    outPlot.clear();
    if (functionalParameterX == ParameterType::OfferedTrafficPerAS)
    {
        foreach(decimal a, rSystem.getAvailableAperAU())
        {
            const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, a);

            double y=0;
            if ((*singlePoint)->read(y, qos, parametersSet.bufferState))
            {
                if (y>0)
                    result = true;
            }
            outPlot.append(y);
        }
    }

    if (functionalParameterX == ParameterType::BufferState)
    {
        const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, parametersSet.a);
        for (int n=0; n<=rSystem.getModel().getBuffer().V(); n++)
        {
            double y=0;

            if ((*singlePoint)->read(y, qos, n))
            {
                if (y>0)
                    result = true;
            }
            outPlot.append(y);
        }
    }
    return result;
}

bool SettingsTypeForBufferState::getSinglePlot3d(QScatter3DSeries &outPlot, RSystem &rSystem, Investigator *algorithm, const ParametersSet &parametersSet) const
{
    (void) parametersSet;

    bool result = false;
    float x, y, z;

    if (functionalParameterZ == ParameterType::OfferedTrafficPerAS)
    {

        foreach(decimal a, rSystem.getAvailableAperAU())
        {
            z = static_cast<float>(a);

            for (int n=0; n <= rSystem.getModel().getBuffer().V(); n++)
            {
                x = n;

                const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, a);
                double yTmp=0;

                if ((*singlePoint)->read(yTmp, qos, n))
                {
                    if (yTmp > 0)
                        result = true;
                    y = static_cast<float>(yTmp);
                    QScatterDataItem tmp(QVector3D(x, y, z));
                    outPlot.dataProxy()->addItem(tmp);                }
            }
        }
    }

    if (functionalParameterZ == ParameterType::BufferState)
    {
        for (int n=0; n<=rSystem.getModel().getBuffer().V(); n++)
        {
            z = n;

            foreach(decimal a, rSystem.getAvailableAperAU())
            {
                x = static_cast<float>(a);

                const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, a);

                double yTmp=0;

                if ((*singlePoint)->read(yTmp, qos, n))
                {
                    if (yTmp > 0)
                        result = true;
                    y = static_cast<float>(yTmp);
                    QScatterDataItem tmp(QVector3D(x, y, z));
                    outPlot.dataProxy()->addItem(tmp);                }
            }
        }
    }
    return result;
}

QList<ParametersSet> SettingsTypeForBufferState::getParametersList(const ModelSystem &system, const QList<decimal> &aOfPerAU) const
{
    QList<ParametersSet> result;
    int n;
    decimal a;

    switch(functionalParameterX)
    {
    case ParameterType::OfferedTrafficPerAS:
    {
        switch(functionalParameterZ)
        {
        case ParameterType::None:
            for (n=0; n <= system.getBuffer().V(); n++)
            {
                ParametersSet item;
                item.bufferState = n;
                item.a = -1;
                result.append(item);
            }
            break;

        case ParameterType::BufferState:
        {
            ParametersSet item;
            item.bufferState = -1;
            item.a = -1;
            result.append(item);
            break;
        }

        case ParameterType::OfferedTrafficPerAS:
            qFatal("Funstional parameters for X and Z axis are the same");

        default:
            qFatal("Wrong functional parameter 2");
        }
        break;
    }

    case ParameterType::BufferState:
    {
        switch(functionalParameterZ)
        {
        case ParameterType::None:
        {
            foreach (a, aOfPerAU)
            {
                ParametersSet item;
                item.bufferState = -1;
                item.a = a;
                result.append(item);
            }
            break;
        }

        case ParameterType::OfferedTrafficPerAS:
        {
            ParametersSet item;
            item.bufferState = -1;
            item.a = -1;
            result.append(item);
            break;
        }

        case ParameterType::BufferState:
            qFatal("Funstional parameters for X and Z axis are the same");

        default:
            qFatal("Wrong functional parameter 2");
        }
        break;
    }

    default:
        qFatal("Wrong functional parameter");
    }
    return result;
}

SettingsTypeForClassAndSystemState::SettingsTypeForClassAndSystemState(TypeForClassAndSystemState qos, QString name, QString shortName): Settings (name, shortName), qos(qos)
{
    dependencyParameters.append(ParameterType::SystemState);
    dependencyParameters.append(ParameterType::TrafficClass);
    dependencyParameters.append(ParameterType::OfferedTrafficPerAS);

    functionalParameterX  = ParameterType::SystemState;
    functionalParameterZ  = ParameterType::None;
    additionalParameter[0] = ParameterType::OfferedTrafficPerAS;
    additionalParameter[1] = ParameterType::TrafficClass;
}

bool SettingsTypeForClassAndSystemState::getSinglePlot(QLineSeries *outPlot, QPair<double, double> &yMinAndMax, RSystem &rSystem, Investigator *algorithm, const ParametersSet &parametersSet, bool linearScale) const
{
    bool result = false;

    outPlot->clear();
    if (functionalParameterX == ParameterType::OfferedTrafficPerAS)
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
                yMinAndMax.first = qMin<double>(yMinAndMax.first, y);
                yMinAndMax.second = qMax<double>(yMinAndMax.second, y);
            }
        }
    }

    if (functionalParameterX == ParameterType::SystemState)
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
                yMinAndMax.first = qMin<double>(yMinAndMax.first, y);
                yMinAndMax.second = qMax<double>(yMinAndMax.second, y);
            }
        }
    }

    if (functionalParameterX == ParameterType::TrafficClass)
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
                yMinAndMax.first = qMin<double>(yMinAndMax.first, y);
                yMinAndMax.second = qMax<double>(yMinAndMax.second, y);
            }
        }
    }
    return result;
}

bool SettingsTypeForClassAndSystemState::getSinglePlot(QVector<double> &outPlot, RSystem &rSystem, Investigator *algorithm, const ParametersSet &parametersSet) const
{
    bool result = false;

    outPlot.clear();
    if (functionalParameterX == ParameterType::OfferedTrafficPerAS)
    {
        foreach(decimal a, rSystem.getAvailableAperAU())
        {
            const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, a);
            double y=0;
            if ((*singlePoint)->read(y, qos, parametersSet.classIndex, parametersSet.systemState))
            {
                if (y>0)
                    result = true;
            }
            outPlot.append(y);
        }
    }

    if (functionalParameterX == ParameterType::SystemState)
    {
        const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, parametersSet.a);
        for (int n=0; n<=rSystem.getModel().V(); n++)
        {
            double y=0;
            if ((*singlePoint)->read(y, qos, parametersSet.classIndex, n))
            {
                if ((y > 0))
                    result = true;
            }
            outPlot.append(y);
        }
    }

    if (functionalParameterX == ParameterType::TrafficClass)
    {
        const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, parametersSet.a);
        for (int i=0; i<rSystem.getModel().m(); i++)
        {
            double y=0;
            if ((*singlePoint)->read(y, qos, i, parametersSet.systemState))
            {
                if ((y > 0))
                    result = true;
            }
            outPlot.append(y);
        }
    }
    return result;
}

bool SettingsTypeForClassAndSystemState::getSinglePlot3d(QScatter3DSeries &outPlot, RSystem &rSystem, Investigator *algorithm, const ParametersSet &parametersSet) const
{
    bool result = false;
    float x, y, z;

    if (functionalParameterZ == ParameterType::OfferedTrafficPerAS)
    {
        foreach(decimal a, rSystem.getAvailableAperAU())
        {
            const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, a);
            z = static_cast<float>(a);

            QVector<double> newRow;
            if (functionalParameterX == ParameterType::SystemState)
            {
                for(int n = 0; n <= rSystem.getModel().V(); n++)
                {
                    x = n;

                    double yTmp=0;
                    if ((*singlePoint)->read(yTmp, qos, parametersSet.classIndex, n))
                    {
                        if (yTmp > 0)
                            result = true;
                        y = static_cast<float>(yTmp);
                        QScatterDataItem tmp(QVector3D(x, y, z));
                        outPlot.dataProxy()->addItem(tmp);
                    }
                }
            }
            if (functionalParameterX == ParameterType::TrafficClass)
            {
                for(int i = 0; i < rSystem.getModel().m(); i++)
                {
                    x = i;

                    double yTmp=0;
                    if ((*singlePoint)->read(yTmp, qos, i, parametersSet.systemState))
                    {
                        if (yTmp > 0)
                            result = true;
                        y = static_cast<float>(yTmp);
                        QScatterDataItem tmp(QVector3D(x, y, z));
                        outPlot.dataProxy()->addItem(tmp);
                    }
                }
            }
        }
    }

    if (functionalParameterZ == ParameterType::SystemState)
    {
        for (int n=0; n<=rSystem.getModel().V(); n++)
        {
            z = n;

            if (functionalParameterX == ParameterType::OfferedTrafficPerAS)
            {
                foreach(decimal a, rSystem.getAvailableAperAU())
                {
                    const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, a);
                    x = static_cast<float>(a);

                    double yTmp=0;
                    if ((*singlePoint)->read(yTmp, qos, parametersSet.classIndex, n))
                    {
                        if (yTmp > 0)
                            result = true;
                        y = static_cast<float>(yTmp);
                        QScatterDataItem tmp(QVector3D(x, y, z));
                        outPlot.dataProxy()->addItem(tmp);
                    }
                }
            }

            if (functionalParameterX == ParameterType::TrafficClass)
            {
                const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, parametersSet.a);
                for (int i=0; i < rSystem.getModel().m(); i++)
                {
                    x = i;

                    double yTmp=0;
                    if ((*singlePoint)->read(yTmp, qos, i, n))
                    {
                        if (yTmp > 0)
                            result = true;
                        y = static_cast<float>(yTmp);
                        QScatterDataItem tmp(QVector3D(x, y, z));
                        outPlot.dataProxy()->addItem(tmp);
                    }
                }
            }
        }
    }

    if (functionalParameterZ == ParameterType::TrafficClass)
    {
        for (int i=0; i<rSystem.getModel().m(); i++)
        {
            z = i;

            if (functionalParameterX == ParameterType::OfferedTrafficPerAS)
            {
                foreach(decimal a, rSystem.getAvailableAperAU())
                {
                    const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, a);
                    x = static_cast<float>(a);

                    double yTmp=0;
                    if ((*singlePoint)->read(yTmp, qos, i, parametersSet.systemState))
                    {
                        if (yTmp > 0)
                            result = true;
                        y = static_cast<float>(yTmp);
                        QScatterDataItem tmp(QVector3D(x, y, z));
                        outPlot.dataProxy()->addItem(tmp);
                    }
                }
            }

            if (functionalParameterX == ParameterType::SystemState)
            {
                const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, parametersSet.a);
                for (int n=0; n<= rSystem.getModel().V(); n++)
                {
                    x = n;

                    double yTmp=0;
                    if ((*singlePoint)->read(yTmp, qos, i, n))
                    {
                        if (yTmp > 0)
                            result = true;
                        y = static_cast<float>(yTmp);
                        QScatterDataItem tmp(QVector3D(x, y, z));
                        outPlot.dataProxy()->addItem(tmp);
                    }
                }
            }
        }
    }
    return result;
}

QList<ParametersSet> SettingsTypeForClassAndSystemState::getParametersList(const ModelSystem &system, const QList<decimal> &aOfPerAU) const
{
    QList<ParametersSet> result;
    int i;
    int n;
    switch (functionalParameterX)
    {
    case ParameterType::OfferedTrafficPerAS:
    {
        switch(functionalParameterZ)
        {
        case ParameterType::None:
        {
            for (i=0; i < system.m(); i++)
            {
                for (n=0; n <= system.V(); n++)
                {
                    ParametersSet item;
                    item.systemState = n;
                    item.classIndex = i;
                    result.append(item);
                }
            }
            break;
        }

        case ParameterType::SystemState:
        {
            for (i=0; i < system.m(); i++)
            {
                ParametersSet item;
                item.systemState = -1;
                item.classIndex = i;
                result.append(item);
            }
            break;
        }

        case ParameterType::OfferedTrafficPerAS:
            qFatal("Funstional parameters for X and Z axis are the same");

        default:
            qFatal("Wrong functional parameter 2");
        }
        break;
    }
    case ParameterType::SystemState:
    {
        switch(functionalParameterZ)
        {
        case ParameterType::None:
        {
            foreach (decimal a, aOfPerAU)
            {
                for (i=0; i < system.m(); i++)
                {
                    ParametersSet item;
                    item.systemState = -1;
                    item.classIndex = i;
                    item.a = a;
                    result.append(item);
                }
            }
            break;
        }
        case ParameterType::TrafficClass:
        {
            for (int n=0; n <= system.V(); n++)
            {
                ParametersSet item;
                item.systemState = n;
                item.classIndex = -1;
                result.append(item);
            }
            break;
        }

        case ParameterType::SystemState:
            qFatal("Funstional parameters for X and Z axis are the same");

        default:
            qFatal("Wrong functional parameter 2");

        }
        break;
    }

    case ParameterType::TrafficClass:
        switch(functionalParameterZ)
        {
        case ParameterType::None:
        {
            foreach (decimal a, aOfPerAU)
            {
                for (n=0; n <= system.V(); n++)
                {
                    ParametersSet item;
                    item.classIndex = -1;
                    item.systemState = n;
                    item.a = a;
                    result.append(item);
                }
            }
            break;
        }

        case ParameterType::SystemState:
        {
            foreach (decimal a, aOfPerAU)
            {
                ParametersSet item;
                item.systemState = -1;
                item.classIndex = -1;
                item.a = a;
                result.append(item);
            }
            break;
        }

        case ParameterType::TrafficClass:
            qFatal("Funstional parameters for X and Z axis are the same");

        default:
            qFatal("Wrong functional parameter 2");

        }
        break;

    default:
        qFatal("Wrong functional parameter");
    }
    return result;
}

SettingsTypeForClassAndServerState::SettingsTypeForClassAndServerState(TypeForClassAndServerState qos, QString name, QString shortName): Settings (name, shortName), qos(qos)
{
    dependencyParameters.append(ParameterType::ServerState);
    dependencyParameters.append(ParameterType::TrafficClass);
    dependencyParameters.append(ParameterType::OfferedTrafficPerAS);

    functionalParameterX  = ParameterType::ServerState;
    functionalParameterZ  = ParameterType::None;
    additionalParameter[0] = ParameterType::OfferedTrafficPerAS;
    additionalParameter[1] = ParameterType::TrafficClass;
    additionalParameter[2] = ParameterType::None;
}

bool SettingsTypeForClassAndServerState::getSinglePlot(QLineSeries *outPlot, QPair<double, double> &yMinAndMax, RSystem &rSystem, Investigator *algorithm, const ParametersSet &parametersSet, bool linearScale) const
{
    bool result = false;

    outPlot->clear();
    if (functionalParameterX == ParameterType::OfferedTrafficPerAS)
    {
        foreach(decimal a, rSystem.getAvailableAperAU())
        {
            const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, a);

            double y=0;
            if ((*singlePoint)->read(y, qos, parametersSet.classIndex, parametersSet.serverState))
            {
                if ((y > 0) || linearScale)
                {
                    *outPlot<<QPointF(static_cast<double>(a), y);
                    result = true;
                }
                yMinAndMax.first = qMin<double>(yMinAndMax.first, y);
                yMinAndMax.second = qMax<double>(yMinAndMax.second, y);
            }
        }
    }

    if (functionalParameterX == ParameterType::SystemState)
    {
        const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, parametersSet.a);
        for (int n=0; n<=rSystem.getModel().getServer().V(); n++)
        {
            double y=0;

            if ((*singlePoint)->read(y, qos, parametersSet.classIndex, n))
            {
                if ((y > 0) || linearScale)
                {
                    *outPlot<<QPointF(n, y);
                    result = true;
                }
                yMinAndMax.first = qMin<double>(yMinAndMax.first, y);
                yMinAndMax.second = qMax<double>(yMinAndMax.second, y);
            }
        }
    }

    if (functionalParameterX == ParameterType::TrafficClass)
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
                yMinAndMax.first = qMin<double>(yMinAndMax.first, y);
                yMinAndMax.second = qMax<double>(yMinAndMax.second, y);
            }
        }
    }
    return result;
}

bool SettingsTypeForClassAndServerState::getSinglePlot(QVector<double> &outPlot, RSystem &rSystem, Investigator *algorithm, const ParametersSet &parametersSet) const
{
    bool result = false;

    outPlot.clear();
    if (functionalParameterX == ParameterType::OfferedTrafficPerAS)
    {
        foreach(decimal a, rSystem.getAvailableAperAU())
        {
            const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, a);

            double y=0;
            if ((*singlePoint)->read(y, qos, parametersSet.classIndex, parametersSet.serverState))
            {
                if (y>0)
                    result = true;
            }
            outPlot.append(y);
        }
    }

    if (functionalParameterX == ParameterType::SystemState)
    {
        const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, parametersSet.a);
        for (int n=0; n<=rSystem.getModel().getServer().V(); n++)
        {
            double y=0;

            if ((*singlePoint)->read(y, qos, parametersSet.classIndex, n))
            {
                if (y>0)
                    result = true;
            }
            outPlot.append(y);
        }
    }

    if (functionalParameterX == ParameterType::TrafficClass)
    {
        const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, parametersSet.a);
        for (int i=0; i<rSystem.getModel().m(); i++)
        {
            double y=0;

            if ((*singlePoint)->read(y, qos, i, parametersSet.serverState))
            {
                if (y > 0)
                    result = true;
            }
            outPlot.append(y);
        }
    }
    return result;
}

bool SettingsTypeForClassAndServerState::getSinglePlot3d(QScatter3DSeries &outPlot, RSystem &rSystem, Investigator *algorithm, const ParametersSet &parametersSet) const
{
    bool result = false;
    float x, y, z;

    if (functionalParameterZ == ParameterType::OfferedTrafficPerAS)
    {
        foreach(decimal a, rSystem.getAvailableAperAU())
        {
            const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, a);
            z = static_cast<float>(a);

            if (functionalParameterX == ParameterType::ServerState)
            {
                for(int n = 0; n <= rSystem.getModel().getServer().V(); n++)
                {
                    x = n;

                    double yTmp=0;
                    if ((*singlePoint)->read(yTmp, qos, parametersSet.classIndex, n))
                    {
                        if (yTmp > 0)
                            result = true;
                        y = static_cast<float>(yTmp);
                        QScatterDataItem tmp(QVector3D(x, y, z));
                        outPlot.dataProxy()->addItem(tmp);
                    }
                }
            }
            if (functionalParameterX == ParameterType::TrafficClass)
            {
                for(int i = 0; i < rSystem.getModel().m(); i++)
                {
                    x = i;

                    double yTmp=0;
                    if ((*singlePoint)->read(yTmp, qos, i, parametersSet.systemState))
                    {
                        if (yTmp > 0)
                            result = true;
                        y = static_cast<float>(yTmp);
                        QScatterDataItem tmp(QVector3D(x, y, z));
                        outPlot.dataProxy()->addItem(tmp);
                    }
                }
            }
        }
    }

    if (functionalParameterZ == ParameterType::ServerState)
    {
        for (int n=0; n<=rSystem.getModel().getServer().V(); n++)
        {
            z = n;

            if (functionalParameterX == ParameterType::OfferedTrafficPerAS)
            {
                foreach(decimal a, rSystem.getAvailableAperAU())
                {
                    const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, a);
                    x = static_cast<float>(a);

                    double yTmp=0;
                    if ((*singlePoint)->read(yTmp, qos, parametersSet.classIndex, n))
                    {
                        if (yTmp > 0)
                            result = true;
                        y = static_cast<float>(yTmp);
                        QScatterDataItem tmp(QVector3D(x, y, z));
                        outPlot.dataProxy()->addItem(tmp);
                    }
                }
            }

            if (functionalParameterX == ParameterType::TrafficClass)
            {
                const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, parametersSet.a);
                for (int i=0; i < rSystem.getModel().m(); i++)
                {
                    x = i;

                    double yTmp=0;
                    if ((*singlePoint)->read(yTmp, qos, i, n))
                    {
                        if (yTmp > 0)
                            result = true;
                        y = static_cast<float>(yTmp);
                        QScatterDataItem tmp(QVector3D(x, y, z));
                        outPlot.dataProxy()->addItem(tmp);
                    }
                }
            }
        }
    }

    if (functionalParameterZ == ParameterType::TrafficClass)
    {
        for (int i=0; i < rSystem.getModel().m(); i++)
        {
            z = i;
            if (functionalParameterX == ParameterType::OfferedTrafficPerAS)
            {
                foreach(decimal a, rSystem.getAvailableAperAU())
                {
                    const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, a);
                    x = static_cast<float>(a);

                    double yTmp=0;
                    if ((*singlePoint)->read(yTmp, qos, i, parametersSet.systemState))
                    {
                        if (yTmp > 0)
                            result = true;
                        y = static_cast<float>(yTmp);
                        QScatterDataItem tmp(QVector3D(x, y, z));
                        outPlot.dataProxy()->addItem(tmp);
                    }
                }
            }

            if (functionalParameterX == ParameterType::ServerState)
            {
                const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, parametersSet.a);
                for (int n=0; n <= rSystem.getModel().getServer().V(); n++)
                {
                    x = n;

                    double yTmp=0;
                    if ((*singlePoint)->read(yTmp, qos, i, n))
                    {
                        if (yTmp > 0)
                            result = true;
                        y = static_cast<float>(yTmp);
                        QScatterDataItem tmp(QVector3D(x, y, z));
                        outPlot.dataProxy()->addItem(tmp);
                    }
                }
            }
        }
    }
    return result;
}

QList<ParametersSet> SettingsTypeForClassAndServerState::getParametersList(const ModelSystem &system, const QList<decimal> &aOfPerAU) const
{
    QList<ParametersSet> result;

    int i;
    int n;
    decimal a;
    switch (functionalParameterX)
    {
    case ParameterType::OfferedTrafficPerAS:
        switch(functionalParameterZ)
        {
        case ParameterType::None:
            for (i=0; i < system.m(); i++)
            {
                for (n=0; n <= system.getServer().V(); n++)
                {
                    ParametersSet item;
                    item.serverState = n;
                    item.classIndex = i;
                    result.append(item);
                }
            }
            break;

        case ParameterType::TrafficClass:
            for (n=0; n <= system.getServer().V(); n++)
            {
                ParametersSet item;
                item.serverState = n;
                item.classIndex = -1;
                result.append(item);
            }
            break;

        case ParameterType::ServerState:
            for (i=0; i < system.m(); i++)
            {
                ParametersSet item;
                item.serverState = -1;
                item.classIndex = i;
                result.append(item);
            }
            break;

        case ParameterType::OfferedTrafficPerAS:
            qFatal("Funstional parameters for X and Z axis are the same");

        default:
            qFatal("Wrong functional parameter 2");
        }
        break;

    case ParameterType::TrafficClass:
        switch(functionalParameterZ)
        {
        case ParameterType::None:
            foreach (a, aOfPerAU)
            {
                for (n=0; n <= system.getServer().V(); n++)
                {
                    ParametersSet item;
                    item.serverState = n;
                    item.classIndex = -1;
                    item.a = a;
                    result.append(item);
                }
            }
            break;

        case ParameterType::OfferedTrafficPerAS:
            for (n=0; n <= system.getServer().V(); n++)
            {
                ParametersSet item;
                item.serverState = n;
                item.classIndex = -1;
                item.a = -1;
                result.append(item);
            }
            break;


        case ParameterType::ServerState:
            foreach (a, aOfPerAU)
            {
                ParametersSet item;
                item.serverState = -1;
                item.classIndex = -1;
                item.a = a;
                result.append(item);
            }
            break;

        case ParameterType::TrafficClass:
            qFatal("Funstional parameters for X and Z axis are the same");

        default:
            qFatal("Wrong functional parameter 2");
        }
        break;

    case ParameterType::ServerState:
        switch(functionalParameterZ)
        {
        case ParameterType::None:
            foreach (a, aOfPerAU)
            {
                for (i=0; i < system.m(); i++)
                {
                    ParametersSet item;
                    item.serverState = -1;
                    item.classIndex = i;
                    item.a = a;
                    result.append(item);
                }
            }
            break;

        case ParameterType::OfferedTrafficPerAS:
            for (i=0; i < system.m(); i++)
            {
                ParametersSet item;
                item.serverState = -1;
                item.classIndex = i;
                item.a = -1;
                result.append(item);
            }
            break;

        case ParameterType::TrafficClass:
            foreach (a, aOfPerAU)
            {
                ParametersSet item;
                item.serverState = -1;
                item.classIndex = -1;
                item.a = a;
                result.append(item);
            }
            break;

        case ParameterType::ServerState:
            qFatal("Funstional parameters for X and Z axis are the same");

        default:
            qFatal("Wrong functional parameter 2");

        }
        break;

    default:
        qFatal("Wrong functional parameter");
    }
    return result;
}

SettingsTypeForClassAndBufferState::SettingsTypeForClassAndBufferState(TypeForClassAndBufferState qos, QString name, QString shortName): Settings (name, shortName), qos(qos)
{
    dependencyParameters.append(ParameterType::BufferState);
    dependencyParameters.append(ParameterType::TrafficClass);
    dependencyParameters.append(ParameterType::OfferedTrafficPerAS);

    functionalParameterX  = ParameterType::BufferState;
    functionalParameterZ  = ParameterType::None;
    additionalParameter[0] = ParameterType::OfferedTrafficPerAS;
    additionalParameter[1] = ParameterType::TrafficClass;
    additionalParameter[2] = ParameterType::None;
}

bool SettingsTypeForClassAndBufferState::getSinglePlot(QLineSeries *outPlot, QPair<double, double> &yMinAndMax, RSystem &rSystem, Investigator *algorithm, const ParametersSet &parametersSet, bool linearScale) const
{
    bool result = false;

    outPlot->clear();
    if (functionalParameterX == ParameterType::OfferedTrafficPerAS)
    {
        foreach(decimal a, rSystem.getAvailableAperAU())
        {
            const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, a);

            double y=0;
            if ((*singlePoint)->read(y, qos, parametersSet.classIndex, parametersSet.bufferState))
            {
                if ((y > 0) || linearScale)
                {
                    *outPlot<<QPointF(static_cast<double>(a), y);
                    result = true;
                }
                yMinAndMax.first = qMin<double>(yMinAndMax.first, y);
                yMinAndMax.second = qMax<double>(yMinAndMax.second, y);
            }
        }
    }

    if (functionalParameterX == ParameterType::SystemState)
    {
        const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, parametersSet.a);
        for (int n=0; n<=rSystem.getModel().getBuffer().V(); n++)
        {
            double y=0;

            if ((*singlePoint)->read(y, qos, parametersSet.classIndex, n))
            {
                if ((y > 0) || linearScale)
                {
                    *outPlot<<QPointF(n, y);
                    result = true;
                }
                yMinAndMax.first = qMin<double>(yMinAndMax.first, y);
                yMinAndMax.second = qMax<double>(yMinAndMax.second, y);
            }
        }
    }

    if (functionalParameterX == ParameterType::TrafficClass)
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
                yMinAndMax.first = qMin<double>(yMinAndMax.first, y);
                yMinAndMax.second = qMax<double>(yMinAndMax.second, y);
            }
        }
    }
    return result;
}

bool SettingsTypeForClassAndBufferState::getSinglePlot(QVector<double> &outPlot, RSystem &rSystem, Investigator *algorithm, const ParametersSet &parametersSet) const
{
    bool result = false;

    outPlot.clear();
    if (functionalParameterX == ParameterType::OfferedTrafficPerAS)
    {
        foreach(decimal a, rSystem.getAvailableAperAU())
        {
            const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, a);

            double y=0;
            if ((*singlePoint)->read(y, qos, parametersSet.classIndex, parametersSet.bufferState))
            {
                if ((y > 0))
                    result = true;
            }
            outPlot.append(y);
        }
    }

    if (functionalParameterX == ParameterType::SystemState)
    {
        const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, parametersSet.a);
        for (int n=0; n<=rSystem.getModel().getBuffer().V(); n++)
        {
            double y=0;

            if ((*singlePoint)->read(y, qos, parametersSet.classIndex, n))
            {
                if ((y > 0))
                    result = true;
            }
            outPlot.append(y);
        }
    }

    if (functionalParameterX == ParameterType::TrafficClass)
    {
        const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, parametersSet.a);
        for (int i=0; i<rSystem.getModel().m(); i++)
        {
            double y=0;

            if ((*singlePoint)->read(y, qos, i, parametersSet.bufferState))
            {
                if ((y > 0))
                    result = true;
            }
            outPlot.append(y);
        }
    }
    return result;
}

bool SettingsTypeForClassAndBufferState::getSinglePlot3d(QScatter3DSeries &outPlot, RSystem &rSystem, Investigator *algorithm, const ParametersSet &parametersSet) const
{
    bool result = false;
    float x, y, z;

    if (functionalParameterZ == ParameterType::OfferedTrafficPerAS)
    {
        foreach(decimal a, rSystem.getAvailableAperAU())
        {
            const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, a);
            z = static_cast<float>(a);

            if (functionalParameterX == ParameterType::BufferState)
            {
                for(int n = 0; n <= rSystem.getModel().getBuffer().V(); n++)
                {
                    x = n;

                    double yTmp=0;
                    if ((*singlePoint)->read(yTmp, qos, parametersSet.classIndex, n))
                    {
                        if (yTmp > 0)
                            result = true;
                        y = static_cast<float>(yTmp);
                        QScatterDataItem tmp(QVector3D(x, y, z));
                        outPlot.dataProxy()->addItem(tmp);
                    }
                }
            }
            if (functionalParameterX == ParameterType::TrafficClass)
            {
                for(int i = 0; i < rSystem.getModel().m(); i++)
                {
                    x = i;

                    double yTmp=0;
                    if ((*singlePoint)->read(yTmp, qos, i, parametersSet.systemState))
                    {
                        if (yTmp > 0)
                            result = true;
                        y = static_cast<float>(yTmp);
                        QScatterDataItem tmp(QVector3D(x, y, z));
                        outPlot.dataProxy()->addItem(tmp);
                    }
                }
            }
        }
    }

    if (functionalParameterZ == ParameterType::BufferState)
    {
        for (int n=0; n<=rSystem.getModel().getBuffer().V(); n++)
        {
            z = n;

            if (functionalParameterX == ParameterType::OfferedTrafficPerAS)
            {
                foreach(decimal a, rSystem.getAvailableAperAU())
                {
                    const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, a);
                    x = static_cast<float>(a);

                    double yTmp=0;
                    if ((*singlePoint)->read(yTmp, qos, parametersSet.classIndex, n))
                    {
                        if (yTmp > 0)
                            result = true;
                        y = static_cast<float>(yTmp);
                        QScatterDataItem tmp(QVector3D(x, y, z));
                        outPlot.dataProxy()->addItem(tmp);
                    }
                }
            }

            if (functionalParameterX == ParameterType::TrafficClass)
            {
                const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, parametersSet.a);
                for (int i=0; i < rSystem.getModel().m(); i++)
                {
                    x = i;

                    double yTmp=0;
                    if ((*singlePoint)->read(yTmp, qos, i, n))
                    {
                        if (yTmp > 0)
                            result = true;
                        y = static_cast<float>(yTmp);
                        QScatterDataItem tmp(QVector3D(x, y, z));
                        outPlot.dataProxy()->addItem(tmp);
                    }
                }
            }
        }
    }

    if (functionalParameterZ == ParameterType::TrafficClass)
    {
        for (int i=0; i<rSystem.getModel().m(); i++)
        {
            z = i;

            if (functionalParameterX == ParameterType::OfferedTrafficPerAS)
            {
                foreach(decimal a, rSystem.getAvailableAperAU())
                {
                    const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, a);
                    x = static_cast<float>(a);

                    double yTmp=0;
                    if ((*singlePoint)->read(yTmp, qos, i, parametersSet.systemState))
                    {
                        if (yTmp > 0)
                            result = true;
                        y = static_cast<float>(yTmp);
                        QScatterDataItem tmp(QVector3D(x, y, z));
                        outPlot.dataProxy()->addItem(tmp);
                    }
                }
            }

            if (functionalParameterX == ParameterType::BufferState)
            {
                const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, parametersSet.a);
                for (int n=0; n<= rSystem.getModel().getBuffer().V(); n++)
                {
                    x = n;

                    double yTmp=0;
                    if ((*singlePoint)->read(yTmp, qos, i, n))
                    {
                        if (yTmp > 0)
                            result = true;
                        y = static_cast<float>(yTmp);
                        QScatterDataItem tmp(QVector3D(x, y, z));
                        outPlot.dataProxy()->addItem(tmp);
                    }
                }
            }
        }
    }
    return result;
}

QList<ParametersSet> SettingsTypeForClassAndBufferState::getParametersList(const ModelSystem &system, const QList<decimal> &aOfPerAU) const
{
    QList<ParametersSet> result;
    int i;
    int n;
    decimal a;

    switch(functionalParameterX)
    {
    case ParameterType::OfferedTrafficPerAS:
        switch(functionalParameterZ)
        {
        case ParameterType::None:
            for (i=0; i < system.m(); i++)
            {
                for (n=0; n <= system.getBuffer().V(); n++)
                {
                    ParametersSet item;
                    item.bufferState = n;
                    item.classIndex = i;
                    result.append(item);
                }
            }
            break;

        case ParameterType::TrafficClass:
            for (n=0; n <= system.getBuffer().V(); n++)
            {
                ParametersSet item;
                item.bufferState = n;
                item.classIndex = -1;
                result.append(item);
            }
            break;

        case ParameterType::BufferState:
            for (i=0; i < system.m(); i++)
            {
                ParametersSet item;
                item.bufferState = -1;
                item.classIndex = i;
                result.append(item);
            }
            break;

        case ParameterType::OfferedTrafficPerAS:
            qFatal("Funstional parameters for X and Z axis are the same");

        default:
            qFatal("Wrong functional parameter 2");
        }
        break;

    case ParameterType::TrafficClass:
        switch(functionalParameterZ)
        {
        case ParameterType::None:
            foreach (a, aOfPerAU)
            {
                for (n=0; n <= system.getBuffer().V(); n++)
                {
                    ParametersSet item;
                    item.bufferState = n;
                    item.classIndex = -1;
                    item.a = a;
                    result.append(item);
                }
            }
            break;

        case ParameterType::OfferedTrafficPerAS:
            for (n=0; n <= system.getBuffer().V(); n++)
            {
                ParametersSet item;
                item.bufferState = n;
                item.classIndex = -1;
                item.a = -1;
                result.append(item);
            }
            break;

        case ParameterType::BufferState:
            foreach (a, aOfPerAU)
            {
                ParametersSet item;
                item.bufferState = -1;
                item.classIndex = -1;
                item.a = a;
                result.append(item);
            }
            break;
       case ParameterType::TrafficClass:
            qFatal("Funstional parameters for X and Z axis are the same");

        default:
            qFatal("Wrong functional parameter 2");
        }
        break;

    case ParameterType::BufferState:
        switch(functionalParameterZ)
        {
        case ParameterType::None:
            foreach (a, aOfPerAU)
            {
                for (i=0; i < system.m(); i++)
                {
                    ParametersSet item;
                    item.bufferState = -1;
                    item.classIndex = i;
                    item.a = a;
                    result.append(item);
                }
            }
            break;

        case ParameterType::OfferedTrafficPerAS:
            for (i=0; i < system.m(); i++)
            {
                ParametersSet item;
                item.bufferState = -1;
                item.classIndex = i;
                item.a = -1;
                result.append(item);
            }
            break;

        case ParameterType::TrafficClass:
            foreach (a, aOfPerAU)
            {
                ParametersSet item;
                item.bufferState = -1;
                item.classIndex = -1;
                item.a = a;
                result.append(item);
            }
            break;

        case ParameterType::BufferState:
            qFatal("Funstional parameters for X and Z axis are the same");

        default:
            qFatal("Wrong functional parameter 2");
        }
        break;

    default:
        qFatal("Qrong functional parameter");
    }
    return result;
}

SettingsForClassAndServerGroupsCombination::SettingsForClassAndServerGroupsCombination(TypeClasses_VsServerGroupsCombination qos, QString name, QString shortName): Settings (name, shortName), qos(qos)
{
    dependencyParameters.append(ParameterType::OfferedTrafficPerAS);
    dependencyParameters.append(ParameterType::TrafficClass);
    dependencyParameters.append(ParameterType::CombinationNumber);

    functionalParameterX  = ParameterType::OfferedTrafficPerAS;
    functionalParameterX  = ParameterType::None;
    additionalParameter[0] = ParameterType::TrafficClass;
    additionalParameter[1] = ParameterType::CombinationNumber;
    additionalParameter[2] = ParameterType::None;
}

bool SettingsForClassAndServerGroupsCombination::getSinglePlot(QLineSeries *outPlot, QPair<double, double> &yMinAndMax, RSystem &rSystem, Investigator *algorithm, const ParametersSet &parametersSet, bool linearScale) const
{
    bool result = false;

    TypeResourcess_VsServerGroupsCombination typeToRead;

    switch (qos)
    {
    case TypeClasses_VsServerGroupsCombination::AvailabilityInAllTheGroups:
        typeToRead = TypeResourcess_VsServerGroupsCombination::AvailabilityInAllTheGroups;
        break;

    case TypeClasses_VsServerGroupsCombination::AvailabilityInOneOrMoreGroups:
        typeToRead = TypeResourcess_VsServerGroupsCombination::AvailabilityInOneOrMoreGroups;
        break;

    case TypeClasses_VsServerGroupsCombination::InavailabilityInAllTheGroups:
        typeToRead = TypeResourcess_VsServerGroupsCombination::InavailabilityInAllTheGroups;
        break;
    }

    outPlot->clear();
    if (functionalParameterX == ParameterType::OfferedTrafficPerAS)
    {
        foreach(decimal a, rSystem.getAvailableAperAU())
        {
            const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, a);
            double x = static_cast<double>(a);
            double y=0;
            int t = rSystem.getModel().t(parametersSet.classIndex);

            if ((*singlePoint)->read(y, typeToRead, t, parametersSet.combinationNumber))
            {
                if ((y > 0) || linearScale)
                {
                    *outPlot<<QPointF(x, y);
                    result = true;
                }
                yMinAndMax.first = qMin<double>(yMinAndMax.first, y);
                yMinAndMax.second = qMax<double>(yMinAndMax.second, y);
            }
        }
    }

    if (functionalParameterX == ParameterType::NumberOfGroups)
    {
        int noOfCombinations = rSystem.getNoOfGroupsCombinations();

        const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, parametersSet.a);
        for (int n=0; n <= noOfCombinations; n++)
        {
            double y=0;
            int t = rSystem.getModel().t(parametersSet.classIndex);

            if ((*singlePoint)->read(y, typeToRead, t, n))
            {
                if ((y > 0) || linearScale)
                {
                    *outPlot<<QPointF(n, y);
                    result = true;
                }
                yMinAndMax.first = qMin<double>(yMinAndMax.first, y);
                yMinAndMax.second = qMax<double>(yMinAndMax.second, y);
            }
        }
    }

    if (functionalParameterX == ParameterType::TrafficClass)
    {
        const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, parametersSet.a);
        for (int i=0; i < rSystem.getModel().m(); i++)
        {
            double y=0;
            int t = rSystem.getModel().t(i);

            if ((*singlePoint)->read(y, typeToRead, t, parametersSet.combinationNumber))
            {
                if ((y > 0) || linearScale)
                {
                    *outPlot<<QPointF(i, y);
                    result = true;
                }
                yMinAndMax.first = qMin<double>(yMinAndMax.first, y);
                yMinAndMax.second = qMax<double>(yMinAndMax.second, y);
            }
        }
    }

    return result;
}

bool SettingsForClassAndServerGroupsCombination::getSinglePlot(QVector<double> &outPlot, RSystem &rSystem, Investigator *algorithm, const ParametersSet &parametersSet) const
{
    bool result = false;

    outPlot.clear();
    if (functionalParameterX == ParameterType::OfferedTrafficPerAS)
    {
        foreach(decimal a, rSystem.getAvailableAperAU())
        {
            const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, a);
            double y=0;
            int t = rSystem.getModel().t(parametersSet.classIndex);

            if ((*singlePoint)->read(y, TypeResourcess_VsServerGroupsCombination::InavailabilityInAllTheGroups, t, parametersSet.combinationNumber))
            {
                if (y > 0)
                    result = true;
            }
            outPlot.append(y);
        }
    }

    if (functionalParameterX == ParameterType::NumberOfGroups)
    {
        int noOfCombinations = rSystem.getNoOfGroupsCombinations();
        int t = rSystem.getModel().t(parametersSet.classIndex);
        const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, parametersSet.a);

        for (int n=0; n <= noOfCombinations; n++)
        {
            double y=0;

            if ((*singlePoint)->read(y, TypeResourcess_VsServerGroupsCombination::AvailabilityInAllTheGroups, t, n))
            {
                if (y > 0)
                    result = true;
            }
            outPlot.append(y);
        }
    }

    if (functionalParameterX == ParameterType::TrafficClass)
    {
        const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, parametersSet.a);
        for (int i=0; i < rSystem.getModel().m(); i++)
        {
            double y=0;
            int t = rSystem.getModel().t(i);

            if ((*singlePoint)->read(y, TypeResourcess_VsServerGroupsCombination::AvailabilityInAllTheGroups, t, parametersSet.combinationNumber))
            {
                if (y > 0)
                    result = true;
            }
            outPlot.append(y);
        }
    }

    return result;
}

bool SettingsForClassAndServerGroupsCombination::getSinglePlot3d(QScatter3DSeries &outPlot, RSystem &rSystem, Investigator *algorithm, const ParametersSet &parametersSet) const
{
    bool result = false;
    float x, y, z;

    if (functionalParameterZ == ParameterType::OfferedTrafficPerAS)
    {
        foreach(decimal a, rSystem.getAvailableAperAU())
        {
            const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, a);
            z = static_cast<float>(a);

            if (functionalParameterX == ParameterType::TrafficClass)
            {
                for (int i=0; i < rSystem.getModel().m(); i++)
                {
                    int t = rSystem.getModel().t(parametersSet.classIndex);
                    x = i;

                    double yTmp;
                    if ((*singlePoint)->read(yTmp, TypeResourcess_VsServerGroupsCombination::InavailabilityInAllTheGroups, t, parametersSet.numberOfGroups))
                    {
                        if (yTmp > 0)
                            result = true;
                        y = static_cast<float>(yTmp);
                        QScatterDataItem tmp(QVector3D(x, y, z));
                        outPlot.dataProxy()->addItem(tmp);
                    }
                }
            }
            if (functionalParameterX == ParameterType::CombinationNumber)
            {
                int t = rSystem.getModel().t(parametersSet.classIndex);
                int noOfCombinations = rSystem.getNoOfGroupsCombinations();

                for (int cn=0; cn<noOfCombinations; cn++)
                {
                    x = cn;

                    double yTmp;
                    if ((*singlePoint)->read(yTmp, TypeResourcess_VsServerGroupsCombination::InavailabilityInAllTheGroups, t, cn))
                    {
                        if (yTmp > 0)
                            result = true;
                        y = static_cast<float>(yTmp);
                        QScatterDataItem tmp(QVector3D(x, y, z));
                        outPlot.dataProxy()->addItem(tmp);
                    }
                }
            }
        }
    }

    if (functionalParameterZ == ParameterType::CombinationNumber)
    {
        int noOfCombinations = rSystem.getNoOfGroupsCombinations();

        for (int n=0; n <= noOfCombinations; n++)
        {
            z = n;

            if (functionalParameterX == ParameterType::OfferedTrafficPerAS)
            {
                int t = rSystem.getModel().t(parametersSet.classIndex);
                foreach (decimal a, rSystem.getAvailableAperAU())
                {
                    const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, parametersSet.a);
                    x = static_cast<float>(a);

                    double yTmp=0;
                    if ((*singlePoint)->read(yTmp, TypeResourcess_VsServerGroupsCombination::AvailabilityInAllTheGroups, t, n))
                    {
                        if (yTmp > 0)
                            result = true;
                        y = static_cast<float>(yTmp);
                        QScatterDataItem tmp(QVector3D(x, y, z));
                        outPlot.dataProxy()->addItem(tmp);
                    }
                }
            }

            if (functionalParameterX == ParameterType::TrafficClass)
            {
                const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, parametersSet.a);
                for (int i=0; i < rSystem.getModel().m(); i++)
                {
                    int t = rSystem.getModel().t(i);
                    x = i;

                    double yTmp;
                    if ((*singlePoint)->read(yTmp, TypeResourcess_VsServerGroupsCombination::AvailabilityInAllTheGroups, t, n))
                    {
                        if (yTmp > 0)
                            result = true;
                        y = static_cast<float>(yTmp);
                        QScatterDataItem tmp(QVector3D(x, y, z));
                        outPlot.dataProxy()->addItem(tmp);
                    }
                }
            }
        }
    }

    if (functionalParameterZ == ParameterType::TrafficClass)
    {
        for (int i=0; i < rSystem.getModel().m(); i++)
        {
            int t = rSystem.getModel().t(i);
            z = i;

            if (functionalParameterX == ParameterType::OfferedTrafficPerAS)
            {
                foreach (decimal a, rSystem.getAvailableAperAU())
                {
                    const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, a);
                    x = static_cast<float>(a);

                    double yTmp;
                    if ((*singlePoint)->read(yTmp, TypeResourcess_VsServerGroupsCombination::AvailabilityInAllTheGroups, t, parametersSet.combinationNumber))
                    {
                        if (yTmp > 0)
                            result = true;
                        y = static_cast<float>(yTmp);
                        QScatterDataItem tmp(QVector3D(x, y, z));
                        outPlot.dataProxy()->addItem(tmp);
                    }
                }
            }

            if (functionalParameterX == ParameterType::CombinationNumber)
            {
                const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, parametersSet.a);
                int noOfCOmbinations = rSystem.getNoOfGroupsCombinations();

                for (int cn=0; cn < noOfCOmbinations; cn++)
                {
                    x = cn;

                    double yTmp;
                    if ((*singlePoint)->read(yTmp, TypeResourcess_VsServerGroupsCombination::AvailabilityInAllTheGroups, t, cn))
                    {
                        if (yTmp > 0)
                            result = true;
                        y = static_cast<float>(yTmp);
                        QScatterDataItem tmp(QVector3D(x, y, z));
                        outPlot.dataProxy()->addItem(tmp);
                    }
                }
            }
        }
    }

    return result;
}

QList<ParametersSet> SettingsForClassAndServerGroupsCombination::getParametersList(const ModelSystem &system, const QList<decimal> &aOfPerAU) const
{
    QList<ParametersSet> result;

    QVector<QVector<int>> combinations = Utils::UtilsLAG::getPossibleCombinationsFinal(system.getServer().V());

    int noOfCOmbinations = combinations.length();
    int lstNoOfGroupInCombination = -1;

    int i;
    int combNo;
    decimal a;

    switch (functionalParameterX)
    {
    case ParameterType::OfferedTrafficPerAS:
        switch(functionalParameterZ)
        {
        case ParameterType::None:
            for (i=0; i < system.m(); i++)
            {
                for (combNo=0; combNo < noOfCOmbinations; combNo++)
                {
                    if ((system.getServer().schedulerAlg == ResourcessScheduler::Random) && combinations[combNo].length() == lstNoOfGroupInCombination)
                        continue;
                    lstNoOfGroupInCombination = combinations[combNo].length();

                    ParametersSet item;
                    item.combinationNumber = combNo;
                    item.classIndex = i;
                    result.append(item);
                }
            }
            break;


        case ParameterType::TrafficClass:
        {
            for (combNo=0; combNo < noOfCOmbinations; combNo++)
            {
                if ((system.getServer().schedulerAlg == ResourcessScheduler::Random) && combinations[combNo].length() == lstNoOfGroupInCombination)
                    continue;
                lstNoOfGroupInCombination = combinations[combNo].length();

                ParametersSet item;
                item.combinationNumber = combNo;
                item.classIndex = -1;
                result.append(item);
            }
            break;
        }

        case ParameterType::CombinationNumber:
            for (i=0; i < system.m(); i++)
            {
                ParametersSet item;
                item.combinationNumber = -1;
                item.classIndex = i;
                result.append(item);
            }
            break;

        case ParameterType::OfferedTrafficPerAS:
            qFatal("Funstional parameters for X and Z axis are the same");

        default:
            qFatal("Wrong functional parameter 2");
        }
        break;

    case ParameterType::TrafficClass:
        switch(functionalParameterZ)
        {
        case ParameterType::None:
            foreach (a, aOfPerAU)
            {
                for (combNo=0; combNo < noOfCOmbinations; combNo++)
                {
                    if ((system.getServer().schedulerAlg == ResourcessScheduler::Random) && combinations[combNo].length() == lstNoOfGroupInCombination)
                        continue;
                    lstNoOfGroupInCombination = combinations[combNo].length();

                    ParametersSet item;
                    item.combinationNumber = combNo;
                    item.classIndex = -1;
                    item.a = a;
                    result.append(item);
                }
            }
            break;

        case ParameterType::OfferedTrafficPerAS:
        {
            for (combNo=0; combNo < noOfCOmbinations; combNo++)
            {
                if ((system.getServer().schedulerAlg == ResourcessScheduler::Random) && combinations[combNo].length() == lstNoOfGroupInCombination)
                    continue;
                lstNoOfGroupInCombination = combinations[combNo].length();

                ParametersSet item;
                item.combinationNumber = combNo;
                item.classIndex = -1;
                item.a = -1;
                result.append(item);
            }
            break;
        }

        case ParameterType::CombinationNumber:
        {
            foreach (a, aOfPerAU)
            {
                ParametersSet item;
                item.combinationNumber = -1;
                item.classIndex = -1;
                item.a = a;
                result.append(item);
            }
            break;
        }

        case ParameterType::TrafficClass:
            qFatal("Funstional parameters for X and Z axis are the same");

        default:
            qFatal("Wrong functional parameter 2");
        }
        break;

    case ParameterType::CombinationNumber:
        switch(functionalParameterZ)
        {
        case ParameterType::None:
            foreach (a, aOfPerAU)
            {
                for (i=0; i < system.m(); i++)
                {
                    ParametersSet item;
                    item.combinationNumber = -1;
                    item.classIndex = i;
                    item.a = a;
                    result.append(item);
                }
            }
            break;

        case ParameterType::OfferedTrafficPerAS:
        {
            for (i=0; i < system.m(); i++)
            {
                ParametersSet item;
                item.combinationNumber = -1;
                item.classIndex = i;
                item.a = -1;
                result.append(item);
            }
            break;
        }

        case ParameterType::TrafficClass:
        {
            foreach (a, aOfPerAU)
            {
                ParametersSet item;
                item.combinationNumber = -1;
                item.classIndex = -1;
                item.a = a;
                result.append(item);
            }
            break;
        }

        case ParameterType::CombinationNumber:
            qFatal("Funstional parameters for X and Z axis are the same");

        default:
            qFatal("Wrong functional parameter 2");
        }
        break;

    default:
        qFatal("wrong functional parameter");

    }
    return result;
}

SettingsAvailableSubroupDistribution::SettingsAvailableSubroupDistribution(QString name, QString shortName): Settings (name, shortName)
{
    dependencyParameters.append(ParameterType::OfferedTrafficPerAS);
    dependencyParameters.append(ParameterType::TrafficClass);
    dependencyParameters.append(ParameterType::NumberOfGroups);

    functionalParameterX  = ParameterType::OfferedTrafficPerAS;
    functionalParameterZ  = ParameterType::None;
    additionalParameter[0] = ParameterType::TrafficClass;
    additionalParameter[1] = ParameterType::NumberOfGroups;
    additionalParameter[2] = ParameterType::None;
}

bool SettingsAvailableSubroupDistribution::getSinglePlot(QLineSeries *outPlot, QPair<double, double> &yMinAndMax, RSystem &rSystem, Investigator *algorithm, const ParametersSet &parametersSet, bool linearScale) const
{
    bool result = false;

    outPlot->clear();
    if (functionalParameterX == ParameterType::OfferedTrafficPerAS)
    {
        int t = rSystem.getModel().t(parametersSet.classIndex);
        foreach(decimal a, rSystem.getAvailableAperAU())
        {
            const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, a);
            double x = static_cast<double>(a);


            double y=0;
            if ((*singlePoint)->read(y, TypeForResourcessAndNumberOfServerGroups::AvailabilityOnlyInAllTheGroups, t, parametersSet.numberOfGroups))
            {
                if ((y > 0) || linearScale)
                {
                    *outPlot<<QPointF(x, y);
                    result = true;
                }
                yMinAndMax.first = qMin<double>(yMinAndMax.first, y);
                yMinAndMax.second = qMax<double>(yMinAndMax.second, y);
            }
        }
    }

    if (functionalParameterX == ParameterType::NumberOfGroups)
    {
        const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, parametersSet.a);
        int t = rSystem.getModel().t(parametersSet.classIndex);
        for (int k=0; k <= rSystem.getModel().getServer().V(); k++)
        {
            double y=0;

            if ((*singlePoint)->read(y, TypeForResourcessAndNumberOfServerGroups::AvailabilityOnlyInAllTheGroups, t, k))
            {
                if ((y > 0) || linearScale)
                {
                    *outPlot<<QPointF(k, y);
                    result = true;
                }
                yMinAndMax.first = qMin<double>(yMinAndMax.first, y);
                yMinAndMax.second = qMax<double>(yMinAndMax.second, y);
            }
        }
    }

    if (functionalParameterX == ParameterType::TrafficClass)
    {
        const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, parametersSet.a);
        for (int i=0; i < rSystem.getModel().m(); i++)
        {
            double y=0;
            int t = rSystem.getModel().t(i);

            if ((*singlePoint)->read(y, TypeForResourcessAndNumberOfServerGroups::AvailabilityOnlyInAllTheGroups, t, parametersSet.numberOfGroups))
            {
                if ((y > 0) || linearScale)
                {
                    *outPlot<<QPointF(i, y);
                    result = true;
                }
                yMinAndMax.first = qMin<double>(yMinAndMax.first, y);
                yMinAndMax.second = qMax<double>(yMinAndMax.second, y);
            }
        }
    }
    return result;
}

bool SettingsAvailableSubroupDistribution::getSinglePlot(QVector<double> &outPlot, RSystem &rSystem, Investigator *algorithm, const ParametersSet &parametersSet) const
{
    bool result = false;

    outPlot.clear();
    if (functionalParameterX == ParameterType::OfferedTrafficPerAS)
    {
        int t = rSystem.getModel().t(parametersSet.classIndex);
        foreach(decimal a, rSystem.getAvailableAperAU())
        {
            const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, a);

            double y=0;
            if ((*singlePoint)->read(y, TypeForResourcessAndNumberOfServerGroups::AvailabilityOnlyInAllTheGroups, t, parametersSet.numberOfGroups))
            {
                if (y > 0)
                    result = true;
            }
            outPlot.append(y);
        }
    }

    if (functionalParameterX == ParameterType::NumberOfGroups)
    {
        const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, parametersSet.a);
        int t = rSystem.getModel().t(parametersSet.classIndex);
        for (int k=0; k <= rSystem.getModel().getServer().k(); k++)
        {
            double y=0;

            if ((*singlePoint)->read(y, TypeForResourcessAndNumberOfServerGroups::AvailabilityOnlyInAllTheGroups, t, k))
            {
                if (y > 0)
                    result = true;
            }
            outPlot.append(y);
        }
    }

    if (functionalParameterX == ParameterType::TrafficClass)
    {
        const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, parametersSet.a);
        for (int i=0; i < rSystem.getModel().m(); i++)
        {
            double y=0;
            int t = rSystem.getModel().t(i);

            if ((*singlePoint)->read(y, TypeForResourcessAndNumberOfServerGroups::AvailabilityOnlyInAllTheGroups, t, parametersSet.numberOfGroups))
            {
                if (y > 0)
                    result = true;
            }
            outPlot.append(y);

        }
    }
    return result;
}

bool SettingsAvailableSubroupDistribution::getSinglePlot3d(QScatter3DSeries &outPlot, RSystem &rSystem, Investigator *algorithm, const ParametersSet &parametersSet) const
{
    //TODO Adam
    return false;
}

QList<ParametersSet> SettingsAvailableSubroupDistribution::getParametersList(const ModelSystem &system, const QList<decimal> &aOfPerAU) const
{
    QList<ParametersSet> result;
    int k;
    int i;
    decimal a;

    switch (functionalParameterX)
    {
    case ParameterType::OfferedTrafficPerAS:
        switch (functionalParameterZ)
        {
        case ParameterType::None:
            for (k=0; k <= system.getServer().V(); k++)
            {
                for (i=0; i < system.m(); i++)
                {
                    ParametersSet item;
                    item.numberOfGroups = k;
                    item.classIndex = i;
                    result.append(item);
                }
            }
            break;

        case ParameterType::NumberOfGroups:
            for (i=0; i < system.m(); i++)
            {
                ParametersSet item;
                item.classIndex = i;
                result.append(item);
            }
            break;

        case ParameterType::TrafficClass:
            for (k=0; k <= system.getServer().V(); k++)
            {
                ParametersSet item;
                item.numberOfGroups = k;
                result.append(item);
            }
            break;

        case ParameterType::OfferedTrafficPerAS:
            qFatal("Funstional parameters for X and Z axis are the same");

        default:
            qFatal("Wrong functional parameter 2");
        }
        break;

    case ParameterType::NumberOfGroups:
        switch (functionalParameterZ)
        {
        case ParameterType::None:
            foreach (a, aOfPerAU)
            {
                for (i=0; i < system.m(); i++)
                {
                    ParametersSet item;
                    item.numberOfGroups = -1;
                    item.classIndex = i;
                    item.a = a;
                    result.append(item);
                }
            }
            break;

        case ParameterType::OfferedTrafficPerAS:
            for (i=0; i < system.m(); i++)
            {
                ParametersSet item;
                item.numberOfGroups = -1;
                item.classIndex = i;
                result.append(item);
            }
            break;

        case ParameterType::TrafficClass:
            foreach (a, aOfPerAU)
            {
                ParametersSet item;
                item.numberOfGroups = -1;
                item.a = a;
                result.append(item);
            }
            break;

        case ParameterType::NumberOfGroups:
            qFatal("Funstional parameters for X and Z axis are the same");

        default:
            qFatal("Wrong functional parameter 2");
        }
        break;

    case ParameterType::TrafficClass:
        switch (functionalParameterZ)
        {
        case ParameterType::None:
            foreach (a, aOfPerAU)
            {
                for (k=0; k <= system.getServer().V(); k++)
                {
                    ParametersSet item;
                    item.numberOfGroups = k;
                    item.classIndex = -1;
                    item.a = a;
                    result.append(item);
                }
            }
            break;

        case ParameterType::OfferedTrafficPerAS:
            for (k=0; k <= system.getServer().V(); k++)
            {
                ParametersSet item;
                item.numberOfGroups = k;
                item.classIndex = -1;
                result.append(item);
            }
            break;

        case ParameterType::NumberOfGroups:
            foreach (a, aOfPerAU)
            {
                ParametersSet item;
                item.classIndex = -1;
                item.a = a;
                result.append(item);
            }
            break;

        case ParameterType::TrafficClass:
            qFatal("Funstional parameters for X and Z axis are the same");

        default:
            qFatal("Wrong functional parameter 2");
        }
        break;

    default:
        qFatal("Wrong functional parameter");
    }
    return result;
}

bool Settings::setFunctionalParameterX(ParameterType param)
{
    bool result = false;
    if (dependencyParameters.contains(param))
    {
        result = true;
        functionalParameterX = param;

        if (functionalParameterZ == param)
            functionalParameterZ = ParameterType::None;

        additionalParameter[0] = ParameterType::None;
        additionalParameter[1] = ParameterType::None;
        additionalParameter[1] = ParameterType::None;


        foreach (ParameterType tmpParam, this->dependencyParameters)
        {
            if (functionalParameterX == tmpParam)
                continue;

            if (functionalParameterZ == tmpParam)
                continue;

            if (additionalParameter[0] == ParameterType::None)
                additionalParameter[0] = tmpParam;
            else if (additionalParameter[1] == ParameterType::None)
                additionalParameter[1] = tmpParam;
            else
                additionalParameter[2] = tmpParam;

        }
    }
    return result;
}

bool Settings::setFunctionalParameterY(ParameterType param)
{
    bool result = false;
    if (dependencyParameters.contains(param))
    {
        result = true;
        functionalParameterZ = param;
        if (functionalParameterX == param)
            functionalParameterX = ParameterType::None;

        additionalParameter[0] = ParameterType::None;
        additionalParameter[1] = ParameterType::None;
        additionalParameter[1] = ParameterType::None;


        foreach (ParameterType tmpParam, this->dependencyParameters)
        {
            if (functionalParameterX == tmpParam)
                continue;

            if (functionalParameterZ == tmpParam)
                continue;

            if (additionalParameter[0] == ParameterType::None)
                additionalParameter[0] = tmpParam;
            else if (additionalParameter[1] == ParameterType::None)
                additionalParameter[1] = tmpParam;
            else
                additionalParameter[2] = tmpParam;
        }
    }
    return result;
}

double Settings::getXmin(RSystem &rSystem) const
{
    double result = -1;

    switch (functionalParameterX)
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

    switch (functionalParameterX)
    {
    case ParameterType::OfferedTrafficPerAS:
        result = rSystem.getMaxAperAU();
        break;

    case ParameterType::TrafficClass:
        result =  rSystem.getModel().m()-1;
        break;

    case ParameterType::NumberOfGroups:
        result = rSystem.getModel().getServer().k();
        break;

    case ParameterType::CombinationNumber:
        result = rSystem.getNoOfGroupsCombinations();
        break;

    case ParameterType::SystemState:
        result = rSystem.getModel().V();
        break;

    case ParameterType::ServerState:
        result = rSystem.getModel().getServer().V();
        break;

    case ParameterType::BufferState:
        result = rSystem.getModel().getBuffer().V();
        break;

    case ParameterType::None:
        break;
    }
    return result;
}

double Settings::getZmin(RSystem &rSystem) const
{
    double result = -1;

    switch (functionalParameterX)
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

double Settings::getZmax(RSystem &rSystem) const
{
    double result = -1;

    switch (functionalParameterZ)
    {
    case ParameterType::OfferedTrafficPerAS:
        result = rSystem.getMaxAperAU();
        break;

    case ParameterType::TrafficClass:
        result =  rSystem.getModel().m()-1;
        break;

    case ParameterType::NumberOfGroups:
        result = rSystem.getModel().getServer().k();
        break;

    case ParameterType::CombinationNumber:
        result = rSystem.getNoOfGroupsCombinations();
        break;

    case ParameterType::SystemState:
        result = rSystem.getModel().V();
        break;

    case ParameterType::ServerState:
        result = rSystem.getModel().getServer().V();
        break;

    case ParameterType::BufferState:
        result = rSystem.getModel().getBuffer().V();
        break;

    case ParameterType::None:
        break;
    }
    return result;
}

QString Settings::updateParameters(ParametersSet &outParameters, const QVariant &variant, ParameterType paramType, const ModelSystem &system, RSystem *resultsForSystem)
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
        result = QString("%1").arg(system.getTrClass(outParameters.classIndex).shortName());
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

void Settings::fillListWithParameters(QList<QVariant> &list, ParameterType paramType, const ModelSystem &system, QList<decimal> offeredTraffic)
{
    list.clear();
    QVariant tmpVariant;
    int i;
    switch (paramType)
    {
    case ParameterType::None:
        break;
    case ParameterType::TrafficClass:
        for (i=0; i<system.m(); i++)
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
        for (i=0; i<=system.V(); i++)
        {
            tmpVariant.setValue<int>(i);
            list.append(tmpVariant);
        }
        break;
    case ParameterType::ServerState:
        for (i=0; i<=system.getServer().V(); i++)
        {
            tmpVariant.setValue<int>(i);
            list.append(tmpVariant);
        }
        break;

    case ParameterType::BufferState:
        for (i=0; i<=system.getBuffer().V(); i++)
        {
            tmpVariant.setValue<int>(i);
            list.append(tmpVariant);
        }
        break;

    case ParameterType::NumberOfGroups:
        for (i=0; i<=system.getServer().k(); i++)
        {
            tmpVariant.setValue<int>(i);
            list.append(tmpVariant);
        }
        break;

    case ParameterType::CombinationNumber:
        int noOfComb = Utils::UtilsLAG::getPossibleCombinations(system.getServer().k()).length();
        for (i=0; i < noOfComb; i++)
        {
            tmpVariant.setValue<int>(i);
            list.append(tmpVariant);
        }
        break;
    }

}

QString Settings::getTypeValue(const ParametersSet &params, ParameterType type, const ModelSystem &system)
{
    QString result;
    QTextStream str;
    str.setString(&result, QIODevice::Append);

    switch (type)
    {
    case Results::ParameterType::TrafficClass:
        str<<system.getTrClass(params.classIndex).shortName();
        break;

    case Results::ParameterType::SystemState:
        str<<params.systemState;
        break;

    case Results::ParameterType::ServerState:
        str<<params.serverState;
        break;

    case Results::ParameterType::BufferState:
        str<<params.bufferState;
        break;

    case Results::ParameterType::CombinationNumber:
        str<<Utils::UtilsLAG::getCombinationString(Utils::UtilsLAG::getPossibleCombinationsFinal(system.getServer().k())[params.combinationNumber]);
        break;

    case Results::ParameterType::NumberOfGroups:
        str<<params.numberOfGroups;
        break;

    case Results::ParameterType::OfferedTrafficPerAS:
        str<<static_cast<double>(params.a);
        break;

    case Results::ParameterType::None:
        break;
    }
    str.flush();
    return result;
}

QString Settings::getParameterDescription(const ParametersSet &params, const ModelSystem &system)
{
    QString result;
    QTextStream str;
    str.setString(&result, QIODevice::Append);

    if (additionalParameter[0] != ParameterType::None)
        str<<getTypeValue(params, additionalParameter[0], system);

    if (additionalParameter[1] != ParameterType::None)
        str<<" "<<getTypeValue(params, additionalParameter[1], system);

    if (additionalParameter[2] != ParameterType::None)
        str<<" "<<getTypeValue(params, additionalParameter[2], system);

    str.flush();
    return result;
}

QString Settings::getParameterDescription(const ParametersSet &params, const ModelSystem &system, const QList<ParameterType> dontDescribeMe)
{
    QString result;
    QTextStream str;
    str.setString(&result, QIODevice::Append);

    if ((additionalParameter[0] != ParameterType::None) && !dontDescribeMe.contains(additionalParameter[0]))
        str<<getTypeValue(params, additionalParameter[0], system);

    if ((additionalParameter[1] != ParameterType::None) && !dontDescribeMe.contains(additionalParameter[1]))
        str<<" "<<getTypeValue(params, additionalParameter[1], system);

    if ((additionalParameter[2] != ParameterType::None) && !dontDescribeMe.contains(additionalParameter[2]))
        str<<" "<<getTypeValue(params, additionalParameter[2], system);

    str.flush();
    return result;
}

bool ParametersSet::operator<(const ParametersSet &rho) const
{
    bool result = false;

    if (a < rho.a)
        return a < rho.a;

    if (classIndex != rho.classIndex)
        return classIndex < rho.classIndex;

    if (systemState != rho.systemState)
        return systemState < rho.systemState;

    if (serverState != rho.serverState)
        return serverState < rho.serverState;

    if (bufferState != rho.bufferState)
        return bufferState < rho.bufferState;

    if (combinationNumber != rho.combinationNumber)
        return combinationNumber < rho.combinationNumber;

    if (numberOfGroups != rho.numberOfGroups)
        return numberOfGroups < rho.numberOfGroups;

    return result;
}

SettingsTypeForServerAndBufferState::SettingsTypeForServerAndBufferState(TypeForServerAngBufferState qos, QString name, QString shortName): Settings (name, shortName), qos(qos)
{
    dependencyParameters.append(ParameterType::ServerState);
    dependencyParameters.append(ParameterType::BufferState);
    dependencyParameters.append(ParameterType::OfferedTrafficPerAS);

    functionalParameterX  = ParameterType::ServerState;
    functionalParameterZ  = ParameterType::None;
    additionalParameter[0] = ParameterType::BufferState;
    additionalParameter[1] = ParameterType::OfferedTrafficPerAS;
    additionalParameter[2] = ParameterType::None;
}

bool SettingsTypeForServerAndBufferState::getSinglePlot(QLineSeries *outPlot, QPair<double, double> &yMinAndMax, RSystem &rSystem, Investigator *algorithm, const ParametersSet &parametersSet, bool linearScale) const
{
    bool result = false;

    outPlot->clear();
    if (functionalParameterX == ParameterType::OfferedTrafficPerAS)
    {
        foreach(decimal a, rSystem.getAvailableAperAU())
        {
            const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, a);

            double y=0;
            if ((*singlePoint)->read(y, qos, parametersSet.serverState, parametersSet.bufferState))
            {
                if ((y > 0) || linearScale)
                {
                    *outPlot<<QPointF(static_cast<double>(a), y);
                    result = true;
                }
                yMinAndMax.first = qMin<double>(yMinAndMax.first, y);
                yMinAndMax.second = qMax<double>(yMinAndMax.second, y);
            }
        }
    }

    if (functionalParameterX == ParameterType::ServerState)
    {
        const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, parametersSet.a);
        for (int n=0; n<=rSystem.getModel().getServer().V(); n++)
        {
            double y=0;

            if ((*singlePoint)->read(y, qos, n, parametersSet.bufferState))
            {
                if ((y > 0) || linearScale)
                {
                    *outPlot<<QPointF(n, y);
                    result = true;
                }
                yMinAndMax.first = qMin<double>(yMinAndMax.first, y);
                yMinAndMax.second = qMax<double>(yMinAndMax.second, y);
            }
        }
    }

    if (functionalParameterX == ParameterType::BufferState)
    {
        const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, parametersSet.a);
        for (int n=0; n<=rSystem.getModel().getServer().V(); n++)
        {
            double y=0;

            if ((*singlePoint)->read(y, qos, parametersSet.serverState, n))
            {
                if ((y > 0) || linearScale)
                {
                    *outPlot<<QPointF(n, y);
                    result = true;
                }
                yMinAndMax.first = qMin<double>(yMinAndMax.first, y);
                yMinAndMax.second = qMax<double>(yMinAndMax.second, y);
            }
        }
    }
    return result;
}

bool SettingsTypeForServerAndBufferState::getSinglePlot(QVector<double> &outPlot, RSystem &rSystem, Investigator *algorithm, const ParametersSet &parametersSet) const
{
    bool result = false;

    outPlot.clear();
    if (functionalParameterX == ParameterType::OfferedTrafficPerAS)
    {
        foreach(decimal a, rSystem.getAvailableAperAU())
        {
            const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, a);

            double y=0;
            if (functionalParameterZ == ParameterType::None)
            {
                if ((*singlePoint)->read(y, qos, parametersSet.serverState, parametersSet.bufferState))
                {
                    if (y>0)
                        result = true;
                }
                outPlot.append(y);
            }
            else if (functionalParameterZ == ParameterType::ServerState)
            {
                for (int n=0; n <= rSystem.getModel().getServer().V(); n++)
                {
                    if ((*singlePoint)->read(y, qos, n, parametersSet.bufferState))
                    {
                        if (y>0)
                            result = true;
                    }
                    outPlot.append(y);
                }
            }
            else if (functionalParameterZ == ParameterType::BufferState)
            {
                for (int n=0; n <= rSystem.getModel().getBuffer().V(); n++)
                {
                    if ((*singlePoint)->read(y, qos, parametersSet.serverState, n))
                    {
                        if (y>0)
                            result = true;
                    }
                    outPlot.append(y);
                }
            }
        }
    }

    if (functionalParameterX == ParameterType::ServerState)
    {
        for (int n=0; n<=rSystem.getModel().getServer().V(); n++)
        {
            double y=0;

            if (functionalParameterZ == ParameterType::OfferedTrafficPerAS)
            {
                foreach(decimal a, rSystem.getAvailableAperAU())
                {
                    const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, a);
                    if ((*singlePoint)->read(y, qos, n, parametersSet.bufferState))
                    {
                        if (y>0)
                            result = true;
                    }
                    outPlot.append(y);
                }
            }
            else
            {
                const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, parametersSet.a);

                if (functionalParameterZ == ParameterType::None)
                {
                    if ((*singlePoint)->read(y, qos, n, parametersSet.bufferState))
                    {
                        if (y>0)
                            result = true;
                    }
                    outPlot.append(y);
                }
                else if (functionalParameterZ == ParameterType::BufferState)
                {
                    for (int nb=0; nb<=rSystem.getModel().getBuffer().V(); nb++)
                    {
                        if ((*singlePoint)->read(y, qos, n, nb))
                        {
                            if (y>0)
                                result = true;
                        }
                        outPlot.append(y);
                    }
                }
            }
        }
    }

    if (functionalParameterX == ParameterType::BufferState)
    {
        for (int n=0; n<=rSystem.getModel().getBuffer().V(); n++)
        {
            double y=0;

            if (functionalParameterZ == ParameterType::OfferedTrafficPerAS)
            {
                foreach(decimal a, rSystem.getAvailableAperAU())
                {
                    const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, a);
                    if ((*singlePoint)->read(y, qos, parametersSet.serverState, n))
                    {
                        if (y>0)
                            result = true;
                    }
                    outPlot.append(y);
                }
            }
            else
            {
                const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, parametersSet.a);

                if (functionalParameterZ == ParameterType::None)
                {
                    if ((*singlePoint)->read(y, qos, parametersSet.serverState, n))
                    {
                        if (y>0)
                            result = true;
                    }
                    outPlot.append(y);
                }
                else if (functionalParameterZ == ParameterType::ServerState)
                {
                    for (int ns=0; ns<=rSystem.getModel().getServer().V(); ns++)
                    {
                        if ((*singlePoint)->read(y, qos, ns, n))
                        {
                            if (y>0)
                                result = true;
                        }
                        outPlot.append(y);
                    }
                }
            }
        }
    }
    return result;
}

bool SettingsTypeForServerAndBufferState::getSinglePlot3d(QScatter3DSeries &outPlot, RSystem &rSystem, Investigator *algorithm, const ParametersSet &parametersSet) const
{
    bool result = false;
    float x, y, z;

    if (functionalParameterZ == ParameterType::OfferedTrafficPerAS)
    {
        foreach(decimal a, rSystem.getAvailableAperAU())
        {
            const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, a);
            z = static_cast<float>(a);

            if (functionalParameterX == ParameterType::ServerState)
            {
                for (int n=0; n<=rSystem.getModel().getServer().V(); n++)
                {
                    x = n;

                    double yTmp;
                    if ((*singlePoint)->read(yTmp, qos, n, parametersSet.bufferState))
                    {
                        if (yTmp > 0)
                            result = true;
                        y = static_cast<float>(yTmp);
                        QScatterDataItem tmp(QVector3D(x, y, z));
                        outPlot.dataProxy()->addItem(tmp);
                    }
                }
            }

            if (functionalParameterX == ParameterType::BufferState)
            {
                for (int n=0; n<=rSystem.getModel().getBuffer().V(); n++)
                {
                    x = n;

                    double yTmp;
                    if ((*singlePoint)->read(yTmp, qos, parametersSet.serverState, n))
                    {
                        if (yTmp > 0)
                            result = true;
                        y = static_cast<float>(yTmp);
                        QScatterDataItem tmp(QVector3D(x, y, z));
                        outPlot.dataProxy()->addItem(tmp);
                    }
                }
            }
        }
    }

    if (functionalParameterZ == ParameterType::ServerState)
    {
        for (int n=0; n<=rSystem.getModel().getServer().V(); n++)
        {
            z = n;

            if (functionalParameterX == ParameterType::OfferedTrafficPerAS)
            {
                foreach (decimal a, rSystem.getAvailableAperAU())
                {
                    const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, a);
                    x = static_cast<float>(a);

                    double yTmp=0;
                    if ((*singlePoint)->read(yTmp, qos, n, parametersSet.bufferState))
                    {
                        if (yTmp > 0)
                            result = true;
                        y = static_cast<float>(yTmp);
                        QScatterDataItem tmp(QVector3D(x, y, z));
                        outPlot.dataProxy()->addItem(tmp);
                    }
                }
            }

            if (functionalParameterX == ParameterType::BufferState)
            {
                const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, parametersSet.a);

                for (int nb=0; nb <= rSystem.getModel().getBuffer().V(); nb++)
                {
                    x = nb;

                    double yTmp=0;
                    if ((*singlePoint)->read(yTmp, qos, n, nb))
                    {
                        if (yTmp > 0)
                            result = true;
                        y = static_cast<float>(yTmp);
                        QScatterDataItem tmp(QVector3D(x, y, z));
                        outPlot.dataProxy()->addItem(tmp);
                    }
                }
            }
        }
    }

    if (functionalParameterZ == ParameterType::BufferState)
    {
        for (int n=0; n<=rSystem.getModel().getBuffer().V(); n++)
        {
            z = n;

            if (functionalParameterX == ParameterType::OfferedTrafficPerAS)
            {
                foreach (decimal a, rSystem.getAvailableAperAU())
                {
                    const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, a);
                    x = static_cast<float>(a);

                    double yTmp=0;
                    if ((*singlePoint)->read(yTmp, qos, parametersSet.serverState, n))
                    {
                        if (yTmp > 0)
                            result = true;
                        y = static_cast<float>(yTmp);
                        QScatterDataItem tmp(QVector3D(x, y, z));
                        outPlot.dataProxy()->addItem(tmp);
                    }
                }
            }

            if (functionalParameterX == ParameterType::ServerState)
            {
                const RInvestigator *singlePoint = rSystem.getInvestigationResults(algorithm, parametersSet.a);
                for (int ns=0; ns <= rSystem.getModel().getServer().V(); ns++)
                {
                    x = ns;

                    double yTmp = 0;
                    if ((*singlePoint)->read(yTmp, qos, ns, n))
                    {
                        if (yTmp > 0)
                            result = true;
                        y = static_cast<float>(yTmp);
                        QScatterDataItem tmp(QVector3D(x, y, z));
                        outPlot.dataProxy()->addItem(tmp);
                    }
                }
            }
        }
    }
    return result;
}

QList<ParametersSet> SettingsTypeForServerAndBufferState::getParametersList(const ModelSystem &system, const QList<decimal> &aOfPerAU) const
{
    QList<ParametersSet> result;
    int ns, nb;
    decimal a;

    switch(functionalParameterX)
    {
    case ParameterType::OfferedTrafficPerAS:
        switch(functionalParameterZ)
        {
        case ParameterType::None:
            for (ns=0; ns <= system.getServer().V(); ns++)
            {
                for (nb=0; nb <= system.getBuffer().V(); nb++)
                {
                    ParametersSet item;
                    item.serverState = ns;
                    item.bufferState = nb;
                    item.a = -1;
                    result.append(item);
                }
            }
            break;
        case ParameterType::ServerState:
            for (nb=0; nb <= system.getBuffer().V(); nb++)
            {
                ParametersSet item;
                item.serverState = -1;
                item.bufferState = nb;
                item.a = -1;
                result.append(item);
            }
            break;

        case ParameterType::BufferState:
            for (ns=0; ns <= system.getServer().V(); ns++)
            {
                ParametersSet item;
                item.serverState = ns;
                item.bufferState = -1;
                item.a = -1;
                result.append(item);
            }
            break;

        case ParameterType::OfferedTrafficPerAS:
            qFatal("Funstional parameters for X and Z axis are the same");

        default:
            qFatal("Wrong functional parameter 2");
        }
        break;


    case ParameterType::ServerState:
        switch(functionalParameterZ)
        {
        case ParameterType::None:
            for (nb=0; nb <= system.getBuffer().V(); nb++)
            {
                foreach (a, aOfPerAU)
                {
                    ParametersSet item;
                    item.serverState = -1;
                    item.bufferState = nb;
                    item.a = a;
                    result.append(item);
                }
            }
            break;

        case ParameterType::OfferedTrafficPerAS:
            for (nb=0; nb <= system.getBuffer().V(); nb++)
            {
                ParametersSet item;
                 item.serverState = -1;
                item.bufferState = nb;
                item.a = -1;
                result.append(item);
            }
            break;

        case ParameterType::BufferState:
            foreach (a, aOfPerAU)
            {
                ParametersSet item;
                item.serverState = -1;
                item.bufferState = -1;
                item.a = a;
                result.append(item);
            }
            break;

        case ParameterType::ServerState:
            qFatal("Funstional parameters for X and Z axis are the same");

        default:
            qFatal("Wrong functional parameter 2");
        }
        break;

    case ParameterType::BufferState:
        switch(functionalParameterZ)
        {
        case ParameterType::None:
            for (ns=0; ns <= system.getServer().V(); ns++)
            {
                foreach (a, aOfPerAU)
                {
                    ParametersSet item;
                    item.serverState = ns;
                    item.bufferState = -1;
                    item.a = a;
                    result.append(item);
                }
            }
            break;

        case ParameterType::OfferedTrafficPerAS:
            for (ns=0; ns <= system.getServer().V(); ns++)
            {
                ParametersSet item;
                item.serverState = ns;
                item.bufferState = -1;
                item.a = -1;
                result.append(item);
            }
            break;

        case ParameterType::ServerState:
            foreach (a, aOfPerAU)
            {
                ParametersSet item;
                item.serverState = -1;
                item.bufferState = -1;
                item.a = a;
                result.append(item);
            }
            break;


        case ParameterType::BufferState:
            qFatal("Funstional parameters for X and Z axis are the same");

        default:
            qFatal("Wrong functional parameter 2");
        }
        break;

    default:
        qFatal("Wrong functional parameter");
    }
    return result;
}

} //namespace Results
