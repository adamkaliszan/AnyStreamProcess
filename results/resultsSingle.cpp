#include <qmath.h>


#include "resultsSingle.h"
#include "resultsSingleGroup.h"
#include "resultsUtils.h"
#include "utils/lag.h"
#include <QtCharts>

namespace Results
{

RSingle::RSingle(): m(0), vMax(0), V(0), Vs(0) { }

void RSingle::init(const ModelSyst *system)
{
    m = system->m();
    vMax = system->v_sMax();
    V = system->V();
    Vs = system->V_s();

    dataPerClasses.fill(DataForClasses(), m);
    dataPerServerState.fill(DataForStates(), Vs+1);
    dataPerQueueState.fill(DataForStates(), V-Vs+1);
    dataPerSystemState.fill(DataForStates(), V+1);
    dataPerClassAndServerState.fill(DataForClassesAndState(), (Vs+1)*m);
    dataPerClassAndQueueState.fill(DataForClassesAndState(), (V-Vs+1)*m);
    dataPerClassAndSystemStateForSystem.fill(DataForClassesAndState(), (V+1)*m);
    dataPerGroupCombination.fill(DataPerGroups(vMax+1, m), ::Utils::UtilsLAG::getPossibleCombinations(system->Ks()).length());
    dataPerBestGroups.fill(DataPerGroups(vMax+1, m), (system->Ks())+1);
    dataPerExactGroupNumber.fill(DataPerGroups(vMax+1, m), (system->Ks())+1);
}

RSingle &RSingle::write(TypeForClass type, double value, int classNo)
{
    switch (type)
    {
    case TypeForClass::BlockingProbability:
        dataPerClasses[classNo].blockingProbability = value;
        break;

    case TypeForClass::LossProbability:
        dataPerClasses[classNo].lossProbability = value;
        break;

    case TypeForClass::CongestionTraffic:
        dataPerClasses[classNo].congestionTraffic = value;
        break;

    }
    return *this;
}

bool RSingle::read(double &result, TypeForClass type, int classNo) const
{
    if (false)
        return false;

    switch (type)
    {
    case TypeForClass::BlockingProbability:
        result = dataPerClasses[classNo].blockingProbability;
        break;

    case TypeForClass::LossProbability:
        result = dataPerClasses[classNo].lossProbability;
        break;

    case TypeForClass::CongestionTraffic:
        result = dataPerClasses[classNo].congestionTraffic;
        break;
    }
    return true;
}

RSingle &RSingle::write(TypeForSystemState type, double value, int systemState)
{
    switch (type)
    {
    case TypeForSystemState::StateProbability:
        dataPerSystemState[systemState].probability = value;
        break;

    case TypeForSystemState::IntensityNewCallIn:
        dataPerSystemState[systemState].newCallInIntensity = value;
        break;

    case TypeForSystemState::IntensityEndCallIn:
        dataPerSystemState[systemState].endCallInIntensity = value;
        break;

    case TypeForSystemState::IntensityNewCallOut:
        dataPerSystemState[systemState].newCallOutIntensity = value;
        break;

    case TypeForSystemState::IntensityEndCallOut:
        dataPerSystemState[systemState].endCallOutIntensity = value;
        break;
    }
    return *this;
}

bool RSingle::read(double &result, TypeForSystemState type, int systemState) const
{
    if (false)
        return false;
    switch (type)
    {
    case TypeForSystemState::StateProbability:
        result = dataPerSystemState[systemState].probability;
        break;

    case TypeForSystemState::IntensityNewCallIn:
        result = dataPerSystemState[systemState].newCallInIntensity;
        break;

    case TypeForSystemState::IntensityEndCallIn:
        result = dataPerSystemState[systemState].endCallInIntensity;
        break;

    case TypeForSystemState::IntensityNewCallOut:
        result = dataPerSystemState[systemState].newCallOutIntensity;
        break;

    case TypeForSystemState::IntensityEndCallOut:
        result = dataPerSystemState[systemState].endCallOutIntensity;
        break;
    }
    return true;
}

RSingle &RSingle::write(TypeForServerState type, double value, int serverState)
{
    switch (type)
    {
    case TypeForServerState::StateProbability:
        dataPerServerState[serverState].probability = value;
        break;

    case TypeForServerState::IntensityNewCallIn:
        dataPerSystemState[serverState].newCallInIntensity = value;
        break;

    case TypeForServerState::IntensityEndCallIn:
        dataPerSystemState[serverState].endCallInIntensity = value;
        break;

    case TypeForServerState::IntensityNewCallOut:
        dataPerSystemState[serverState].newCallOutIntensity = value;
        break;

    case TypeForServerState::IntensityEndCallOut:
        dataPerSystemState[serverState].endCallOutIntensity = value;
        break;
    }
    return *this;
}

bool RSingle::read(double &result, TypeForServerState type, int serverState) const
{
    if (false)
        return false;

    switch (type)
    {
    case TypeForServerState::StateProbability:
        result = dataPerServerState[serverState].probability;
        break;

    case TypeForServerState::IntensityNewCallIn:
        result = dataPerSystemState[serverState].newCallInIntensity;
        break;

    case TypeForServerState::IntensityEndCallIn:
        result = dataPerSystemState[serverState].endCallInIntensity;
        break;

    case TypeForServerState::IntensityNewCallOut:
        result = dataPerSystemState[serverState].newCallOutIntensity;
        break;

    case TypeForServerState::IntensityEndCallOut:
        result = dataPerSystemState[serverState].endCallOutIntensity;
        break;
    }
    return true;
}

RSingle &RSingle::write(TypeForQueueState type, double value, int queueState)
{
    switch (type)
    {
    case TypeForQueueState::StateProbability:
        dataPerQueueState[queueState].probability = value;
        break;

    case TypeForQueueState::IntensityNewCallIn:
        dataPerSystemState[queueState].newCallInIntensity = value;
        break;

    case TypeForQueueState::IntensityEndCallIn:
        dataPerSystemState[queueState].endCallInIntensity = value;
        break;

    case TypeForQueueState::IntensityNewCallOut:
        dataPerSystemState[queueState].newCallOutIntensity = value;
        break;

    case TypeForQueueState::IntensityEndCallOut:
        dataPerSystemState[queueState].endCallOutIntensity = value;
        break;
    }
    return *this;
}

bool RSingle::read(double &result, TypeForQueueState type, int queueState) const
{
    if (false)
        return false;

    switch (type)
    {
    case TypeForQueueState::StateProbability:
        result = dataPerQueueState[queueState].probability;
        break;

    case TypeForQueueState::IntensityNewCallIn:
        result = dataPerSystemState[queueState].newCallInIntensity;
        break;

    case TypeForQueueState::IntensityEndCallIn:
        result = dataPerSystemState[queueState].endCallInIntensity;
        break;

    case TypeForQueueState::IntensityNewCallOut:
        result = dataPerSystemState[queueState].newCallOutIntensity;
        break;

    case TypeForQueueState::IntensityEndCallOut:
        result = dataPerSystemState[queueState].endCallOutIntensity;
        break;
    }
    return true;
}

RSingle &RSingle::write(TypeForClassAndServerState type, double value, int classNo, int serverState)
{
    int index = classNo * (Vs+1) + serverState;
    switch(type)
    {
    case TypeForClassAndServerState::RealNewCallIntensityOut:
        dataPerClassAndServerState[index].realNewCallIntensityOut = value;
        break;

    case TypeForClassAndServerState::OfferedNewCallIntensityOut:
        dataPerClassAndServerState[index].offeredCallIntensityOut = value;
        break;

    case TypeForClassAndServerState::EndCallIntensityOut:
        dataPerClassAndServerState[index].endCallIntensityOut = value;
        break;

    case TypeForClassAndServerState::NewCallIntensityIn:
        dataPerClassAndServerState[index].newCallIntensityIn = value;
        break;

    case TypeForClassAndServerState::EndCallIntensityIn:
        dataPerClassAndServerState[index].endCallIntensityIn = value;
        break;

    case TypeForClassAndServerState::CAC_Probability:
        dataPerClassAndServerState[index].cac_probability = value;
        break;

    case TypeForClassAndServerState::Usage:
        dataPerClassAndServerState[index].utilization = value;
        break;
    }
    return *this;
}

bool RSingle::read(double &result, TypeForClassAndServerState type, int classNo, int serverState)
{
    if (false)
        return false;
    int index = classNo * (Vs+1) + serverState;

    switch(type)
    {
    case TypeForClassAndServerState::RealNewCallIntensityOut:
        result = dataPerClassAndServerState[index].realNewCallIntensityOut;
        break;

    case TypeForClassAndServerState::OfferedNewCallIntensityOut:
        result = dataPerClassAndServerState[index].offeredCallIntensityOut;
        break;

    case TypeForClassAndServerState::EndCallIntensityOut:
        result = dataPerClassAndServerState[index].endCallIntensityOut;
        break;

    case TypeForClassAndServerState::NewCallIntensityIn:
        result = dataPerClassAndServerState[index].newCallIntensityIn;
        break;

    case TypeForClassAndServerState::EndCallIntensityIn:
        result = dataPerClassAndServerState[index].endCallIntensityIn;
        break;

    case TypeForClassAndServerState::CAC_Probability:
        result = dataPerClassAndServerState[index].cac_probability;
        break;

    case TypeForClassAndServerState::Usage:
        result = dataPerClassAndServerState[index].utilization;
        break;
    }
    return true;
}

RSingle &RSingle::write(TypeForClassAndQueueState type, double value, int classNo, int queueState)
{
    int index = classNo * (V-Vs+1) + queueState;
    switch(type)
    {
    case TypeForClassAndQueueState::RealNewCallIntensityOut:
        dataPerClassAndQueueState[index].realNewCallIntensityOut = value;
        break;

    case TypeForClassAndQueueState::OfferedNewCallIntensityOut:
        dataPerClassAndQueueState[index].offeredCallIntensityOut = value;
        break;

    case TypeForClassAndQueueState::EndCallIntensityOut:
        dataPerClassAndQueueState[index].endCallIntensityOut = value;
        break;

    case TypeForClassAndQueueState::NewCallIntensityIn:
        dataPerClassAndQueueState[index].newCallIntensityIn = value;
        break;

    case TypeForClassAndQueueState::EndCallIntensityIn:
        dataPerClassAndQueueState[index].endCallIntensityIn = value;
        break;

    case TypeForClassAndQueueState::CAC_Probability:
        dataPerClassAndQueueState[index].cac_probability = value;
        break;

    case TypeForClassAndQueueState::Usage:
        dataPerClassAndQueueState[index].utilization = value;
        break;
    }
    return *this;
}

bool RSingle::read(double &result, TypeForClassAndQueueState type, int classNo, int queueState)
{
    if (false)
        return false;
    int index = classNo * (V-Vs+1) + queueState;

    switch(type)
    {
    case TypeForClassAndQueueState::RealNewCallIntensityOut:
        result = dataPerClassAndQueueState[index].realNewCallIntensityOut;
        break;

    case TypeForClassAndQueueState::OfferedNewCallIntensityOut:
        result = dataPerClassAndQueueState[index].offeredCallIntensityOut;
        break;

    case TypeForClassAndQueueState::EndCallIntensityOut:
        result = dataPerClassAndQueueState[index].endCallIntensityOut;
        break;

    case TypeForClassAndQueueState::NewCallIntensityIn:
        result = dataPerClassAndQueueState[index].newCallIntensityIn;
        break;

    case TypeForClassAndQueueState::EndCallIntensityIn:
        result = dataPerClassAndQueueState[index].endCallIntensityIn;
        break;

    case TypeForClassAndQueueState::CAC_Probability:
        result = dataPerClassAndQueueState[index].cac_probability;
        break;

    case TypeForClassAndQueueState::Usage:
        result = dataPerClassAndQueueState[index].utilization;
        break;
    }
    return true;
}

RSingle &RSingle::write(TypeForClassAndSystemState type, double value, int classNo, int systemState)
{
    int index = classNo * (V + 1) + systemState;
    switch(type)
    {
    case TypeForClassAndSystemState::RealNewCallIntensityOutForServer:
        dataPerClassAndSystemStateForServer[index].realNewCallIntensityOut = value;
        break;

    case TypeForClassAndSystemState::RealNewCallIntensityOutForQueue:
        dataPerClassAndSystemStateForQueue[index].realNewCallIntensityOut = value;
        break;

    case TypeForClassAndSystemState::RealNewCallIntensityOutForSystem:
        dataPerClassAndSystemStateForSystem[index].realNewCallIntensityOut = value;
        break;

    case TypeForClassAndSystemState::OfferedNewCallIntensityOutForServer:
        dataPerClassAndSystemStateForServer[index].offeredCallIntensityOut = value;
        break;

    case TypeForClassAndSystemState::OfferedNewCallIntensityOutForQueue:
        dataPerClassAndSystemStateForQueue[index].offeredCallIntensityOut = value;
        break;

    case TypeForClassAndSystemState::OfferedNewCallIntensityOutForSystem:
        dataPerClassAndSystemStateForSystem[index].offeredCallIntensityOut = value;
        break;

    case TypeForClassAndSystemState::EndCallIntensityOutForServer:
        dataPerClassAndSystemStateForServer[index].endCallIntensityOut = value;
        break;

    case TypeForClassAndSystemState::EndCallIntensityOutForQueue:
        dataPerClassAndSystemStateForQueue[index].endCallIntensityOut = value;
        break;

    case TypeForClassAndSystemState::EndCallIntensityOutForSystem:
        dataPerClassAndSystemStateForSystem[index].endCallIntensityOut = value;
        break;

    case TypeForClassAndSystemState::NewCallIntensityInForServer:
        dataPerClassAndSystemStateForServer[index].newCallIntensityIn = value;
        break;

    case TypeForClassAndSystemState::NewCallIntensityInForQueue:
        dataPerClassAndSystemStateForQueue[index].newCallIntensityIn = value;
        break;

    case TypeForClassAndSystemState::NewCallIntensityInForSystem:
        dataPerClassAndSystemStateForSystem[index].newCallIntensityIn = value;
        break;

    case TypeForClassAndSystemState::EndCallIntensityInForServer:
        dataPerClassAndSystemStateForServer[index].endCallIntensityIn = value;
        break;

    case TypeForClassAndSystemState::EndCallIntensityInForQueue:
        dataPerClassAndSystemStateForQueue[index].endCallIntensityIn = value;
        break;

    case TypeForClassAndSystemState::EndCallIntensityInForSystem:
        dataPerClassAndSystemStateForSystem[index].endCallIntensityIn = value;
        break;

    case TypeForClassAndSystemState::CAC_ProbabilityForServer:
        dataPerClassAndSystemStateForServer[index].cac_probability = value;
        break;

    case TypeForClassAndSystemState::CAC_ProbabilityForQueue:
        dataPerClassAndSystemStateForQueue[index].cac_probability = value;
        break;

    case TypeForClassAndSystemState::CAC_ProbabilityForSystem:
        dataPerClassAndSystemStateForSystem[index].cac_probability = value;
        break;

    case TypeForClassAndSystemState::UsageForServer:
        dataPerClassAndSystemStateForServer[index].utilization = value;
        break;

    case TypeForClassAndSystemState::UsageForQueue:
        dataPerClassAndSystemStateForQueue[index].utilization = value;
        break;

    case TypeForClassAndSystemState::UsageForSystem:
        dataPerClassAndSystemStateForSystem[index].utilization = value;
        break;
    }
    return *this;
}

bool RSingle::read(double &result, TypeForClassAndSystemState type, int classNo, int systemState)
{
    if (false)
        return false;
    int index = classNo * (V+1) + systemState;

    switch(type)
    {
    case TypeForClassAndSystemState::RealNewCallIntensityOutForServer:
        result = dataPerClassAndSystemStateForServer[index].realNewCallIntensityOut;
        break;

    case TypeForClassAndSystemState::RealNewCallIntensityOutForQueue:
        result = dataPerClassAndSystemStateForQueue[index].realNewCallIntensityOut;
        break;

    case TypeForClassAndSystemState::RealNewCallIntensityOutForSystem:
        result = dataPerClassAndSystemStateForSystem[index].realNewCallIntensityOut;
        break;

    case TypeForClassAndSystemState::OfferedNewCallIntensityOutForServer:
        result = dataPerClassAndSystemStateForServer[index].offeredCallIntensityOut;
        break;

    case TypeForClassAndSystemState::OfferedNewCallIntensityOutForQueue:
        result = dataPerClassAndSystemStateForQueue[index].offeredCallIntensityOut;
        break;

    case TypeForClassAndSystemState::OfferedNewCallIntensityOutForSystem:
        result = dataPerClassAndSystemStateForSystem[index].offeredCallIntensityOut;
        break;

    case TypeForClassAndSystemState::EndCallIntensityOutForServer:
        result = dataPerClassAndSystemStateForServer[index].endCallIntensityOut;
        break;

    case TypeForClassAndSystemState::EndCallIntensityOutForQueue:
        result = dataPerClassAndSystemStateForQueue[index].endCallIntensityOut;
        break;

    case TypeForClassAndSystemState::EndCallIntensityOutForSystem:
        result = dataPerClassAndSystemStateForSystem[index].endCallIntensityOut;
        break;

    case TypeForClassAndSystemState::NewCallIntensityInForServer:
        result = dataPerClassAndSystemStateForServer[index].newCallIntensityIn;
        break;

    case TypeForClassAndSystemState::NewCallIntensityInForQueue:
        result = dataPerClassAndSystemStateForQueue[index].newCallIntensityIn;
        break;

    case TypeForClassAndSystemState::NewCallIntensityInForSystem:
        result = dataPerClassAndSystemStateForSystem[index].newCallIntensityIn;
        break;

    case TypeForClassAndSystemState::EndCallIntensityInForServer:
        result = dataPerClassAndSystemStateForServer[index].endCallIntensityIn;
        break;

    case TypeForClassAndSystemState::EndCallIntensityInForQueue:
        result = dataPerClassAndSystemStateForQueue[index].endCallIntensityIn;
        break;

    case TypeForClassAndSystemState::EndCallIntensityInForSystem:
        result = dataPerClassAndSystemStateForSystem[index].endCallIntensityIn;
        break;

    case TypeForClassAndSystemState::CAC_ProbabilityForServer:
        result = dataPerClassAndSystemStateForServer[index].cac_probability;
        break;

    case TypeForClassAndSystemState::CAC_ProbabilityForQueue:
        result = dataPerClassAndSystemStateForQueue[index].cac_probability;
        break;

    case TypeForClassAndSystemState::CAC_ProbabilityForSystem:
        result = dataPerClassAndSystemStateForSystem[index].cac_probability;
        break;

    case TypeForClassAndSystemState::UsageForServer:
        result = dataPerClassAndSystemStateForServer[index].utilization;
        break;

    case TypeForClassAndSystemState::UsageForQueue:
        result = dataPerClassAndSystemStateForQueue[index].utilization;
        break;

    case TypeForClassAndSystemState::UsageForSystem:
        result = dataPerClassAndSystemStateForSystem[index].utilization;
        break;
    }
    return true;
}

RSingle &RSingle::write(TypeStateForServerGroupsCombination type, double value, int numberOfResourcess, int groupCombinationIndex)
{
    switch(type)
    {
    case TypeStateForServerGroupsCombination::FreeAUsInBestGroup:
        dataPerGroupCombination[groupCombinationIndex].availabilityProbabilities[numberOfResourcess].freeInBestGroup = value;
        break;

    case TypeStateForServerGroupsCombination::FreeAUsInEveryGroup:
        dataPerGroupCombination[groupCombinationIndex].availabilityProbabilities[numberOfResourcess].freeInAllTheGroups = value;
        break;

    case TypeStateForServerGroupsCombination::AvailabilityOnlyInAllTheGroups:
        dataPerGroupCombination[groupCombinationIndex].availabilityProbabilities[numberOfResourcess].availabilityOnlyInAllTheGroups = value;
        break;

    case TypeStateForServerGroupsCombination::AvailabilityInAllTheGroups:
        dataPerGroupCombination[groupCombinationIndex].availabilityProbabilities[numberOfResourcess].availabilityInAllTheGroups = value;
        break;

    case TypeStateForServerGroupsCombination::InavailabilityInAllTheGroups:
        dataPerGroupCombination[groupCombinationIndex].availabilityProbabilities[numberOfResourcess].inavailabilityInAllTheGroups = value;
        break;
    }
    return *this;
}

bool RSingle::read(double &result, TypeStateForServerGroupsCombination type, int numberOfResourcess, int groupCombinationIndex) const
{
    if (false)
        return false;

    switch(type)
    {
    case TypeStateForServerGroupsCombination::FreeAUsInBestGroup:
        result = dataPerGroupCombination[groupCombinationIndex].availabilityProbabilities[numberOfResourcess].freeInBestGroup;
        break;

    case TypeStateForServerGroupsCombination::FreeAUsInEveryGroup:
        result = dataPerGroupCombination[groupCombinationIndex].availabilityProbabilities[numberOfResourcess].freeInAllTheGroups;
        break;

    case TypeStateForServerGroupsCombination::AvailabilityOnlyInAllTheGroups:
        result = dataPerGroupCombination[groupCombinationIndex].availabilityProbabilities[numberOfResourcess].availabilityOnlyInAllTheGroups;
        break;

    case TypeStateForServerGroupsCombination::AvailabilityInAllTheGroups:
        result = dataPerGroupCombination[groupCombinationIndex].availabilityProbabilities[numberOfResourcess].availabilityInAllTheGroups;
        break;

    case TypeStateForServerGroupsCombination::InavailabilityInAllTheGroups:
        result = dataPerGroupCombination[groupCombinationIndex].availabilityProbabilities[numberOfResourcess].inavailabilityInAllTheGroups;
        break;
    }
    return true;
}

RSingle &RSingle::write(TypeClassForServerGroupsCombination type, double value, int classNumber, int groupCombinationIndex)
{
    switch(type)
    {
    case TypeClassForServerGroupsCombination::SerPossibilityInBestSubgroup:
        dataPerGroupCombination[groupCombinationIndex].availabilityClasses[classNumber].availabilityInBestSubgroup = value;
        break;

    case TypeClassForServerGroupsCombination::SerPossibilityOnlyInAllTheSubgroups:
        dataPerGroupCombination[groupCombinationIndex].availabilityClasses[classNumber].availabilityOnlyInAllSubgroups = value;
        break;

    case TypeClassForServerGroupsCombination::SerPossibilityInAllTheSubgroups:
        dataPerGroupCombination[groupCombinationIndex].availabilityClasses[classNumber].availabilityInAllSubgroups = value;
        break;

    case TypeClassForServerGroupsCombination::SerImpossibilityInAllTheSubgroups:
        dataPerGroupCombination[groupCombinationIndex].availabilityClasses[classNumber].inavailabilityInAllSubgroups = value;
        break;

    }
    return *this;
}

bool RSingle::read(double &result, TypeClassForServerGroupsCombination type, int classNumber, int groupCombinationIndex) const
{
    if (false)
        return false;

    switch(type)
    {
    case TypeClassForServerGroupsCombination::SerPossibilityInBestSubgroup:
        result = dataPerGroupCombination[groupCombinationIndex].availabilityClasses[classNumber].availabilityInBestSubgroup;
        break;

    case TypeClassForServerGroupsCombination::SerPossibilityOnlyInAllTheSubgroups:
        result = dataPerGroupCombination[groupCombinationIndex].availabilityClasses[classNumber].availabilityOnlyInAllSubgroups;
        break;

    case TypeClassForServerGroupsCombination::SerPossibilityInAllTheSubgroups:
        result = dataPerGroupCombination[groupCombinationIndex].availabilityClasses[classNumber].availabilityInAllSubgroups;
        break;

    case TypeClassForServerGroupsCombination::SerImpossibilityInAllTheSubgroups:
        result = dataPerGroupCombination[groupCombinationIndex].availabilityClasses[classNumber].inavailabilityInAllSubgroups;
        break;
    }
    return true;
}

RSingle &RSingle::write(TypeClassForServerBestGroupsSet type, double value, int classNumber, int numberOfGroups)
{
    switch(type)
    {
    case TypeClassForServerBestGroupsSet::ServPossibilityInBestSubgroup:
        dataPerBestGroups[numberOfGroups].availabilityClasses[classNumber].availabilityInBestSubgroup = value;
        break;
    case TypeClassForServerBestGroupsSet::ServPossibilityOnlyInAllTheSubgroups:
        dataPerBestGroups[numberOfGroups].availabilityClasses[classNumber].availabilityOnlyInAllSubgroups = value;
        break;
    case TypeClassForServerBestGroupsSet::ServPossibilityInAllTheSubgroups:
        dataPerBestGroups[numberOfGroups].availabilityClasses[classNumber].availabilityInAllSubgroups = value;
        break;
    case TypeClassForServerBestGroupsSet::ServImpossibilityInAllTheSubgroups:
        dataPerBestGroups[numberOfGroups].availabilityClasses[classNumber].inavailabilityInAllSubgroups = value;
        break;

    }
    return *this;
}

bool RSingle::read(double &result, TypeClassForServerBestGroupsSet type, int classNumber, int numberOfGroups) const
{
    if (false)
        return false;

    switch(type)
    {
    case TypeClassForServerBestGroupsSet::ServPossibilityInBestSubgroup:
        result = dataPerBestGroups[numberOfGroups].availabilityClasses[classNumber].availabilityInBestSubgroup;
        break;

    case TypeClassForServerBestGroupsSet::ServPossibilityOnlyInAllTheSubgroups:
        result = dataPerBestGroups[numberOfGroups].availabilityClasses[classNumber].availabilityOnlyInAllSubgroups;
        break;

    case TypeClassForServerBestGroupsSet::ServPossibilityInAllTheSubgroups:
        result = dataPerBestGroups[numberOfGroups].availabilityClasses[classNumber].availabilityInAllSubgroups;
        break;

    case TypeClassForServerBestGroupsSet::ServImpossibilityInAllTheSubgroups:
        result = dataPerBestGroups[numberOfGroups].availabilityClasses[classNumber].inavailabilityInAllSubgroups;
        break;
    }
    return result;
}

RSingle &RSingle::write(TypeStateForServerGroupsSet type, double value, int numberOfResourcess, int numberOfGroups)
{
    switch(type)
    {
    case TypeStateForServerGroupsSet::AvailabilityOnlyInAllTheGroups:
        dataPerExactGroupNumber[numberOfGroups].availabilityProbabilities[numberOfResourcess].availabilityOnlyInAllTheGroups = value;
        break;
    case TypeStateForServerGroupsSet::AvailabilityInAllTheGroups:
        dataPerExactGroupNumber[numberOfGroups].availabilityProbabilities[numberOfResourcess].availabilityInAllTheGroups = value;
        break;
    case TypeStateForServerGroupsSet::InavailabilityInAllTheGroups:
        dataPerExactGroupNumber[numberOfGroups].availabilityProbabilities[numberOfResourcess].inavailabilityInAllTheGroups = value;
        break;
    }
    return *this;
}

bool RSingle::read(double &result, TypeStateForServerGroupsSet type, int numberOfResourcess, int numberOfGroups) const
{
    if (false)
        return false;

    switch(type)
    {
    case TypeStateForServerGroupsSet::AvailabilityOnlyInAllTheGroups:
        result = dataPerExactGroupNumber[numberOfGroups].availabilityProbabilities[numberOfResourcess].availabilityOnlyInAllTheGroups;
        break;
    case TypeStateForServerGroupsSet::AvailabilityInAllTheGroups:
        result = dataPerExactGroupNumber[numberOfGroups].availabilityProbabilities[numberOfResourcess].availabilityInAllTheGroups;
        break;

    case TypeStateForServerGroupsSet::InavailabilityInAllTheGroups:
        result = dataPerExactGroupNumber[numberOfGroups].availabilityProbabilities[numberOfResourcess].inavailabilityInAllTheGroups;
        break;
    }
    return true;
}

RSingle &RSingle::write(TypeClassForServerExactGroupsSet type, double value, int classNumber, int numberOfGroups)
{
    switch(type)
    {
    case TypeClassForServerExactGroupsSet::ServPossibilityInBestSubgroup:
        dataPerExactGroupNumber[numberOfGroups].availabilityClasses[classNumber].availabilityInBestSubgroup = value;
        break;

    case TypeClassForServerExactGroupsSet::ServPossibilityOnlyInAllTheSubgroups:
        dataPerExactGroupNumber[numberOfGroups].availabilityClasses[classNumber].availabilityOnlyInAllSubgroups = value;
        break;

    case TypeClassForServerExactGroupsSet::ServPossibilityInAllTheSubgroups:
        dataPerExactGroupNumber[numberOfGroups].availabilityClasses[classNumber].availabilityInAllSubgroups = value;
        break;

    case TypeClassForServerExactGroupsSet::ServImpossibilityInAllTheSubgroups:
        dataPerExactGroupNumber[numberOfGroups].availabilityClasses[classNumber].inavailabilityInAllSubgroups = value;
        break;
    }
    return *this;
}

bool RSingle::read(double &result, TypeClassForServerExactGroupsSet type, int classNumber, int numberOfGroups) const
{
    if (false)
        return false;

    switch(type)
    {
    case TypeClassForServerExactGroupsSet::ServPossibilityInBestSubgroup:
        result = dataPerExactGroupNumber[numberOfGroups].availabilityClasses[classNumber].availabilityInBestSubgroup;
        break;
    case TypeClassForServerExactGroupsSet::ServPossibilityOnlyInAllTheSubgroups:
        result = dataPerExactGroupNumber[numberOfGroups].availabilityClasses[classNumber].availabilityOnlyInAllSubgroups;
        break;
    case TypeClassForServerExactGroupsSet::ServPossibilityInAllTheSubgroups:
        result = dataPerExactGroupNumber[numberOfGroups].availabilityClasses[classNumber].availabilityInAllSubgroups;
        break;

    case TypeClassForServerExactGroupsSet::ServImpossibilityInAllTheSubgroups:
        result = dataPerExactGroupNumber[numberOfGroups].availabilityClasses[classNumber].inavailabilityInAllSubgroups;
        break;

    }
    return result;
}

/*
RSingle RSingle::operator=(const RSingle &rho)
{
    RSingle result;
    result.dataPerBestGroups           = rho.dataPerBestGroups;
    result.dataPerClassAndServerState  = rho.dataPerClassAndServerState;
    result.dataPerClasses              = rho.dataPerClasses;
    result.dataPerExactGroupNumber     = rho.dataPerExactGroupNumber;
    result.dataPerGroupCombination     = rho.dataPerGroupCombination;
    result.dataPerServerState          = rho.dataPerServerState;
    result.dataPerSystemState          = rho.dataPerSystemState;

    return result;
}
*/

RSingle& RSingle::operator+=(const RSingle &rho)
{
    Utils::addElementsToFirst<RSingle::DataForClasses>(dataPerClasses, rho.dataPerClasses);
    Utils::addElementsToFirst<RSingle::DataForStates>(dataPerServerState, rho.dataPerServerState);
    Utils::addElementsToFirst<RSingle::DataForStates>(dataPerQueueState, rho.dataPerQueueState);
    Utils::addElementsToFirst<RSingle::DataForStates>(dataPerSystemState, rho.dataPerSystemState);
    Utils::addElementsToFirst<DataForClassesAndState>(dataPerClassAndServerState, rho.dataPerClassAndServerState);
    Utils::addElementsToFirst<DataForClassesAndState>(dataPerClassAndQueueState, rho.dataPerClassAndQueueState);
    Utils::addElementsToFirst<DataForClassesAndState>(dataPerClassAndSystemStateForServer, rho.dataPerClassAndSystemStateForServer);
    Utils::addElementsToFirst<DataForClassesAndState>(dataPerClassAndSystemStateForQueue, rho.dataPerClassAndSystemStateForQueue);
    Utils::addElementsToFirst<DataForClassesAndState>(dataPerClassAndSystemStateForSystem, rho.dataPerClassAndSystemStateForSystem);
    Utils::addElementsToFirst<RSingle::DataPerGroups>(dataPerGroupCombination, rho.dataPerGroupCombination);
    Utils::addElementsToFirst<RSingle::DataPerGroups>(dataPerBestGroups, rho.dataPerBestGroups);
    Utils::addElementsToFirst<RSingle::DataPerGroups>(dataPerExactGroupNumber, rho.dataPerExactGroupNumber);
    return *this;
}

RSingle RSingle::operator-(const RSingle &rho) const
{
    RSingle result = *this;
    result.dataPerClasses                      = Utils::subtractElements<RSingle::DataForClasses>(dataPerClasses, rho.dataPerClasses);
    result.dataPerServerState                  = Utils::subtractElements<RSingle::DataForStates>(dataPerServerState, rho.dataPerServerState);
    result.dataPerQueueState                   = Utils::subtractElements<RSingle::DataForStates>(dataPerQueueState, rho.dataPerQueueState);
    result.dataPerSystemState                  = Utils::subtractElements<RSingle::DataForStates>(dataPerSystemState, rho.dataPerSystemState);
    result.dataPerClassAndServerState          = Utils::subtractElements<DataForClassesAndState>(dataPerClassAndServerState, rho.dataPerClassAndServerState);
    result.dataPerClassAndQueueState           = Utils::subtractElements<DataForClassesAndState>(dataPerClassAndQueueState, rho.dataPerClassAndQueueState);
    result.dataPerClassAndSystemStateForServer = Utils::subtractElements<DataForClassesAndState>(dataPerClassAndSystemStateForServer, rho.dataPerClassAndSystemStateForServer);
    result.dataPerClassAndSystemStateForQueue  = Utils::subtractElements<DataForClassesAndState>(dataPerClassAndSystemStateForQueue,  rho.dataPerClassAndSystemStateForQueue);
    result.dataPerClassAndSystemStateForSystem = Utils::subtractElements<DataForClassesAndState>(dataPerClassAndSystemStateForSystem, rho.dataPerClassAndSystemStateForSystem);
    result.dataPerGroupCombination             = Utils::subtractElements<RSingle::DataPerGroups> (this->dataPerGroupCombination, rho.dataPerGroupCombination);
    result.dataPerBestGroups                   = Utils::subtractElements<RSingle::DataPerGroups> (this->dataPerBestGroups, rho.dataPerBestGroups);
    result.dataPerExactGroupNumber             = Utils::subtractElements<RSingle::DataPerGroups> (this->dataPerExactGroupNumber, rho.dataPerExactGroupNumber);
    return result;
}

RSingle RSingle::operator^(double rho) const
{
    RSingle result = *this;
    result.dataPerClasses                      = Utils::powerElementTempl<DataForClasses>(result.dataPerClasses, rho);
    result.dataPerServerState                  = Utils::powerElementTempl<RSingle::DataForStates>(dataPerServerState, rho);
    result.dataPerQueueState                   = Utils::powerElementTempl<RSingle::DataForStates>(dataPerQueueState, rho);
    result.dataPerSystemState                  = Utils::powerElementTempl<RSingle::DataForStates>(dataPerSystemState, rho);
    result.dataPerClassAndServerState          = Utils::powerElementTempl<DataForClassesAndState>(dataPerClassAndServerState, rho);
    result.dataPerClassAndQueueState           = Utils::powerElementTempl<DataForClassesAndState>(dataPerClassAndQueueState, rho);
    result.dataPerClassAndSystemStateForServer = Utils::powerElementTempl<DataForClassesAndState>(dataPerClassAndSystemStateForServer, rho);
    result.dataPerClassAndSystemStateForQueue  = Utils::powerElementTempl<DataForClassesAndState>(dataPerClassAndSystemStateForQueue, rho);
    result.dataPerClassAndSystemStateForSystem = Utils::powerElementTempl<DataForClassesAndState>(dataPerClassAndSystemStateForSystem, rho);
    result.dataPerGroupCombination             = Utils::powerElementTempl<RSingle::DataPerGroups> (this->dataPerGroupCombination, rho);
    result.dataPerBestGroups                   = Utils::powerElementTempl<RSingle::DataPerGroups> (this->dataPerBestGroups, rho);
    result.dataPerExactGroupNumber             = Utils::powerElementTempl<RSingle::DataPerGroups> (this->dataPerExactGroupNumber, rho);
    return result;
}

RSingle RSingle::operator*(const RSingle &rho)
{
    RSingle result = *this;
    result.dataPerClasses                      = Utils::multiplyElement<RSingle::DataForClasses>(dataPerClasses, rho.dataPerClasses);
    result.dataPerServerState                  = Utils::multiplyElement<RSingle::DataForStates>(dataPerServerState, rho.dataPerServerState);
    result.dataPerQueueState                   = Utils::multiplyElement<RSingle::DataForStates>(dataPerQueueState, rho.dataPerQueueState);
    result.dataPerSystemState                  = Utils::multiplyElement<RSingle::DataForStates>(dataPerSystemState, rho.dataPerSystemState);
    result.dataPerClassAndServerState          = Utils::multiplyElement<DataForClassesAndState>(dataPerClassAndServerState, rho.dataPerClassAndServerState);
    result.dataPerClassAndQueueState           = Utils::multiplyElement<DataForClassesAndState>(dataPerClassAndQueueState, rho.dataPerClassAndQueueState);
    result.dataPerClassAndSystemStateForServer = Utils::multiplyElement<DataForClassesAndState>(dataPerClassAndSystemStateForServer, rho.dataPerClassAndSystemStateForServer);
    result.dataPerClassAndSystemStateForQueue  = Utils::multiplyElement<DataForClassesAndState>(dataPerClassAndSystemStateForQueue, rho.dataPerClassAndSystemStateForQueue);
    result.dataPerClassAndSystemStateForSystem = Utils::multiplyElement<DataForClassesAndState>(dataPerClassAndSystemStateForSystem, rho.dataPerClassAndSystemStateForSystem);
    result.dataPerGroupCombination             = Utils::multiplyElement<RSingle::DataPerGroups> (this->dataPerGroupCombination, rho.dataPerGroupCombination);
    return result;
}

RSingle &RSingle::operator/=(double rho)
{
    Utils::divideElementToFirst<RSingle::DataForClasses>(dataPerClasses, rho);
    Utils::divideElementToFirst<RSingle::DataForStates>(dataPerServerState, rho);
    Utils::divideElementToFirst<RSingle::DataForStates>(dataPerQueueState, rho);
    Utils::divideElementToFirst<RSingle::DataForStates>(dataPerSystemState, rho);
    Utils::divideElementToFirst<RSingle::DataForClassesAndState>(dataPerClassAndServerState, rho);
    Utils::divideElementToFirst<RSingle::DataForClassesAndState>(dataPerClassAndQueueState, rho);
    Utils::divideElementToFirst<RSingle::DataForClassesAndState>(dataPerClassAndSystemStateForServer, rho);
    Utils::divideElementToFirst<RSingle::DataForClassesAndState>(dataPerClassAndSystemStateForQueue, rho);
    Utils::divideElementToFirst<RSingle::DataForClassesAndState>(dataPerClassAndSystemStateForSystem, rho);
    Utils::divideElementToFirst<RSingle::DataPerGroups>(dataPerGroupCombination, rho);
    Utils::divideElementToFirst<RSingle::DataPerGroups>(dataPerBestGroups, rho);
    Utils::divideElementToFirst<RSingle::DataPerGroups>(dataPerExactGroupNumber, rho);
    return *this;
}

RSingle &RSingle::operator*=(double rho)
{
    Utils::multiplyElementToFirst<RSingle::DataForClasses>(dataPerClasses, rho);
    Utils::multiplyElementToFirst<RSingle::DataForStates>(dataPerServerState, rho);
    Utils::multiplyElementToFirst<RSingle::DataForStates>(dataPerQueueState, rho);
    Utils::multiplyElementToFirst<RSingle::DataForStates>(dataPerSystemState, rho);
    Utils::multiplyElementToFirst<RSingle::DataForClassesAndState>(dataPerClassAndServerState, rho);
    Utils::multiplyElementToFirst<RSingle::DataForClassesAndState>(dataPerClassAndQueueState, rho);
    Utils::multiplyElementToFirst<RSingle::DataForClassesAndState>(dataPerClassAndSystemStateForServer, rho);
    Utils::multiplyElementToFirst<RSingle::DataForClassesAndState>(dataPerClassAndSystemStateForQueue, rho);
    Utils::multiplyElementToFirst<RSingle::DataForClassesAndState>(dataPerClassAndSystemStateForSystem, rho);
    Utils::multiplyElementToFirst<RSingle::DataPerGroups>(dataPerGroupCombination, rho);
    Utils::multiplyElementToFirst<RSingle::DataPerGroups>(dataPerBestGroups, rho);
    Utils::multiplyElementToFirst<RSingle::DataPerGroups>(dataPerExactGroupNumber, rho);
    return *this;
}

void RSingle::sqrt()
{ 
    Utils::sqrtTemplate<DataForClasses>(this->dataPerClasses);
    Utils::sqrtTemplate<RSingle::DataForStates>(dataPerServerState);
    Utils::sqrtTemplate<RSingle::DataForStates>(dataPerQueueState);
    Utils::sqrtTemplate<RSingle::DataForStates>(dataPerSystemState);
    Utils::sqrtTemplate<RSingle::DataForClassesAndState>(dataPerClassAndServerState);
    Utils::sqrtTemplate<RSingle::DataForClassesAndState>(dataPerClassAndQueueState);
    Utils::sqrtTemplate<RSingle::DataForClassesAndState>(dataPerClassAndSystemStateForServer);
    Utils::sqrtTemplate<RSingle::DataForClassesAndState>(dataPerClassAndSystemStateForQueue);
    Utils::sqrtTemplate<RSingle::DataForClassesAndState>(dataPerClassAndSystemStateForSystem);
    Utils::sqrtTemplate<RSingle::DataPerGroups>(dataPerGroupCombination);
    Utils::sqrtTemplate<RSingle::DataPerGroups>(dataPerBestGroups);
    Utils::sqrtTemplate<RSingle::DataPerGroups>(dataPerExactGroupNumber);
}

void RSingle::clear()
{
    dataPerClasses.fill(DataForClasses());
    dataPerGroupCombination.fill(DataPerGroups(vMax+1, m));
    dataPerBestGroups.fill(DataPerGroups(vMax+1, m));
    dataPerExactGroupNumber.fill(DataPerGroups(vMax+1, m));
}

RSingle::DataForClasses &RSingle::DataForClasses::operator+=(const RSingle::DataForClasses &rho)
{
    blockingProbability +=rho.blockingProbability;
    congestionTraffic +=rho.congestionTraffic;
    lossProbability +=rho.lossProbability;

    return *this;
}

RSingle::DataForClasses RSingle::DataForClasses::operator-(const RSingle::DataForClasses &rho) const
{
    RSingle::DataForClasses result;

    result.blockingProbability = blockingProbability - rho.blockingProbability;
    result.congestionTraffic = congestionTraffic - rho.congestionTraffic;
    result.lossProbability = lossProbability - rho.lossProbability;

    return result;
}

RSingle::DataForClasses RSingle::DataForClasses::operator^(double rho) const
{
    RSingle::DataForClasses result;

    result.blockingProbability = qPow(this->blockingProbability, rho);
    result.congestionTraffic = qPow(this->congestionTraffic, rho);
    result.lossProbability = qPow(this->lossProbability, rho);

    return result;
}

RSingle::DataForClasses RSingle::DataForClasses::operator*(const RSingle::DataForClasses &rho) const
{
    RSingle::DataForClasses result;

    result.blockingProbability = this->blockingProbability * rho.blockingProbability;
    result.congestionTraffic   = this->congestionTraffic * rho.congestionTraffic;
    result.lossProbability     = this->lossProbability * rho.lossProbability;

    return result;
}

RSingle::DataForClasses &RSingle::DataForClasses::operator/=(double rho)
{
    blockingProbability /= rho;
    congestionTraffic   /= rho;
    lossProbability     /= rho;

    return *this;
}

RSingle::DataForClasses &RSingle::DataForClasses::operator*=(double rho)
{
    blockingProbability *= rho;
    congestionTraffic   *= rho;
    lossProbability     *= rho;

    return *this;
}

RSingle::DataForClasses& RSingle::DataForClasses::sqrt()
{
    blockingProbability = qSqrt(blockingProbability);
    congestionTraffic   = qSqrt(congestionTraffic);
    lossProbability     = qSqrt(lossProbability);
    return *this;
}

RSingle::DataForClasses RSingle::DataForClasses::pow(double rho) const
{
    RSingle::DataForClasses result;
    result.blockingProbability = qPow(blockingProbability, rho);
    result.congestionTraffic   = qPow(congestionTraffic, rho);
    result.lossProbability     = qPow(lossProbability, rho);
    return result;
}

void RSingle::DataForClasses::clear()
{
    blockingProbability = 0;
    congestionTraffic   = 0;
    lossProbability     = 0;
}

RSingle::DataForStates &RSingle::DataForStates::operator+=(const RSingle::DataForStates &rho)
{
    probability         +=rho.probability;
    newCallInIntensity  +=rho.newCallInIntensity;
    endCallInIntensity  +=rho.endCallInIntensity;
    newCallOutIntensity +=rho.newCallOutIntensity;
    endCallOutIntensity +=rho.endCallOutIntensity;

    return *this;
}

RSingle::DataForStates RSingle::DataForStates::operator-(const RSingle::DataForStates &rho) const
{
    RSingle::DataForStates result;

    result.probability         = probability         -rho.probability;
    result.newCallInIntensity  = newCallInIntensity  -rho.newCallInIntensity;
    result.endCallInIntensity  = endCallInIntensity  -rho.endCallInIntensity;
    result.newCallOutIntensity = newCallOutIntensity -rho.newCallOutIntensity;
    result.endCallOutIntensity = endCallOutIntensity -rho.endCallOutIntensity;

    return result;
}

RSingle::DataForStates RSingle::DataForStates::operator^(double rho) const
{
    RSingle::DataForStates result;

    result.probability         = qPow(this->probability        , rho);
    result.newCallInIntensity  = qPow(this->newCallInIntensity , rho);
    result.endCallInIntensity  = qPow(this->endCallInIntensity , rho);
    result.newCallOutIntensity = qPow(this->newCallOutIntensity, rho);
    result.endCallOutIntensity = qPow(this->endCallOutIntensity, rho);

    return result;
}

RSingle::DataForStates RSingle::DataForStates::operator*(const RSingle::DataForStates &rho) const
{
    RSingle::DataForStates result;

    result.probability         = this->probability         * rho.probability;
    result.newCallInIntensity  = this->newCallInIntensity  * rho.newCallInIntensity;
    result.endCallInIntensity  = this->endCallInIntensity  * rho.endCallInIntensity;
    result.newCallOutIntensity = this->newCallOutIntensity * rho.newCallOutIntensity;
    result.endCallOutIntensity = this->endCallOutIntensity * rho.endCallOutIntensity;

    return result;
}

RSingle::DataForStates &RSingle::DataForStates::operator/=(double rho)
{
    probability         /= rho;
    newCallInIntensity  /= rho;
    endCallInIntensity  /= rho;
    newCallOutIntensity /= rho;
    endCallOutIntensity /= rho;

    return *this;
}

RSingle::DataForStates &RSingle::DataForStates::operator*=(double rho)
{
    probability         *= rho;
    newCallInIntensity  *= rho;
    endCallInIntensity  *= rho;
    newCallOutIntensity *= rho;
    endCallOutIntensity *= rho;

    return *this;
}

RSingle::DataForStates& RSingle::DataForStates::sqrt()
{
    probability         = qSqrt(probability);
    newCallInIntensity  = qSqrt(newCallInIntensity);
    endCallInIntensity  = qSqrt(endCallInIntensity);
    newCallOutIntensity = qSqrt(newCallOutIntensity);
    endCallOutIntensity = qSqrt(endCallOutIntensity);

    return *this;
}

RSingle::DataForStates RSingle::DataForStates::pow(double rho) const
{
    RSingle::DataForStates result;

    result.probability         = qPow(probability,         rho);
    result.newCallInIntensity  = qPow(newCallInIntensity,  rho);
    result.endCallInIntensity  = qPow(endCallInIntensity,  rho);
    result.newCallOutIntensity = qPow(newCallOutIntensity, rho);
    result.endCallOutIntensity = qPow(endCallOutIntensity, rho);

    return result;
}

void RSingle::DataForStates::clear()
{
    probability         = 0;
    newCallInIntensity  = 0;
    endCallInIntensity  = 0;
    newCallOutIntensity = 0;
    endCallOutIntensity = 0;
}

RSingle::DataPerGroups &RSingle::DataPerGroups::operator+=(const RSingle::DataPerGroups &rho)
{
    Utils::addElementsToFirst<AvailabilityClass>(this->availabilityClasses, rho.availabilityClasses);
    Utils::addElementsToFirst<AvailabilityAU>(this->availabilityProbabilities, rho.availabilityProbabilities);

    return *this;
}

RSingle::DataPerGroups RSingle::DataPerGroups::operator-(const RSingle::DataPerGroups &rho) const
{
    RSingle::DataPerGroups result;
    result.availabilityClasses = Utils::subtractElements<AvailabilityClass>(this->availabilityClasses, rho.availabilityClasses);
    result.availabilityProbabilities = Utils::subtractElements<AvailabilityAU>(this->availabilityProbabilities, rho.availabilityProbabilities);
    return result;
}

RSingle::DataPerGroups RSingle::DataPerGroups::operator^(double rho) const
{
    RSingle::DataPerGroups result;
    result.availabilityClasses = Utils::powerElementTempl<AvailabilityClass>(this->availabilityClasses, rho);
    result.availabilityProbabilities = Utils::powerElementTempl<AvailabilityAU>(this->availabilityProbabilities, rho);

    return result;
}

RSingle::DataPerGroups RSingle::DataPerGroups::operator*(const RSingle::DataPerGroups &rho) const
{
    RSingle::DataPerGroups result;
    result.availabilityClasses = Utils::multiplyElement<AvailabilityClass>(this->availabilityClasses, rho.availabilityClasses);
    result.availabilityProbabilities = Utils::multiplyElement<AvailabilityAU>(this->availabilityProbabilities, rho.availabilityProbabilities);
    return result;
}

RSingle::DataPerGroups &RSingle::DataPerGroups::operator/=(double rho)
{
    Utils::divideElementToFirst<AvailabilityClass>(this->availabilityClasses, rho);
    Utils::divideElementToFirst<AvailabilityAU>(this->availabilityProbabilities, rho);
    return *this;
}

RSingle::DataPerGroups &RSingle::DataPerGroups::operator*=(double rho)
{
    Utils::multiplyElementToFirst<AvailabilityClass>(this->availabilityClasses, rho);
    Utils::multiplyElementToFirst<AvailabilityAU>(this->availabilityProbabilities, rho);
    return *this;
}

RSingle::DataPerGroups& RSingle::DataPerGroups::sqrt()
{
    Utils::sqrtTemplate<AvailabilityClass>(availabilityClasses);
    Utils::sqrtTemplate<AvailabilityAU>(availabilityProbabilities);
    return *this;
}

RSingle::DataPerGroups RSingle::DataPerGroups::pow(double rho) const
{
    RSingle::DataPerGroups result = *this;
    result.availabilityClasses       = Utils::powerElementTempl<AvailabilityClass>(availabilityClasses, rho);
    result.availabilityProbabilities = Utils::powerElementTempl<AvailabilityAU>(availabilityProbabilities, rho);
    return result;
}

void RSingle::DataPerGroups::clear()
{
    availabilityClasses.fill(AvailabilityClass());
    availabilityProbabilities.fill(AvailabilityAU());
}


RSingle::DataForClassesAndState &RSingle::DataForClassesAndState::operator+=(const RSingle::DataForClassesAndState &rho)
{
    realNewCallIntensityOut+= rho.realNewCallIntensityOut;
    offeredCallIntensityOut+= rho.offeredCallIntensityOut;
    newCallIntensityIn     += rho.newCallIntensityIn;
    endCallIntensityOut    += rho.endCallIntensityOut;
    endCallIntensityIn     += rho.endCallIntensityIn;
    cac_probability        += rho.cac_probability;
    utilization            += rho.utilization;
    return *this;
}

RSingle::DataForClassesAndState RSingle::DataForClassesAndState::operator-(const RSingle::DataForClassesAndState &rho) const
{
    RSingle::DataForClassesAndState result;
    result.realNewCallIntensityOut = realNewCallIntensityOut - rho.realNewCallIntensityOut;
    result.offeredCallIntensityOut = offeredCallIntensityOut - rho.offeredCallIntensityOut;
    result.newCallIntensityIn      = newCallIntensityIn      - rho.newCallIntensityIn;
    result.endCallIntensityOut     = endCallIntensityOut     - rho.endCallIntensityOut;
    result.endCallIntensityIn      = endCallIntensityIn      - rho.endCallIntensityIn;
    result.cac_probability         = cac_probability         - rho.cac_probability;
    result.utilization             = utilization             - rho.utilization;
    return result;
}

RSingle::DataForClassesAndState RSingle::DataForClassesAndState::operator^(double rho) const
{
    RSingle::DataForClassesAndState result;
    result.realNewCallIntensityOut = qPow( realNewCallIntensityOut, rho);
    result.offeredCallIntensityOut = qPow(offeredCallIntensityOut, rho);
    result.newCallIntensityIn      = qPow(newCallIntensityIn, rho);
    result.endCallIntensityOut     = qPow(endCallIntensityOut, rho);
    result.endCallIntensityIn      = qPow(endCallIntensityIn, rho);
    result.cac_probability         = qPow(cac_probability, rho);
    result.utilization             = qPow(utilization, rho);
    return result;
}

RSingle::DataForClassesAndState RSingle::DataForClassesAndState::operator*(const RSingle::DataForClassesAndState &rho) const
{
    RSingle::DataForClassesAndState result;
    result.realNewCallIntensityOut = realNewCallIntensityOut * rho.realNewCallIntensityOut;
    result.offeredCallIntensityOut = offeredCallIntensityOut * rho.offeredCallIntensityOut;
    result.newCallIntensityIn      = newCallIntensityIn      * rho.newCallIntensityIn;
    result.endCallIntensityOut     = endCallIntensityOut     * rho.endCallIntensityOut;
    result.endCallIntensityIn      = endCallIntensityIn      * rho.endCallIntensityIn;
    result.cac_probability         = cac_probability         * rho.cac_probability;
    result.utilization             = utilization             * rho.utilization;
    return result;
}

RSingle::DataForClassesAndState &RSingle::DataForClassesAndState::operator/=(double rho)
{
    realNewCallIntensityOut/= rho;
    offeredCallIntensityOut/= rho;
    newCallIntensityIn     /= rho;
    endCallIntensityOut    /= rho;
    endCallIntensityIn     /= rho;
    cac_probability        /= rho;
    utilization            /= rho;
    return *this;
}

RSingle::DataForClassesAndState &RSingle::DataForClassesAndState::operator*=(double rho)
{
    realNewCallIntensityOut*= rho;
    offeredCallIntensityOut*= rho;
    newCallIntensityIn     *= rho;
    endCallIntensityOut    *= rho;
    endCallIntensityIn     *= rho;
    cac_probability        *= rho;
    utilization            *= rho;
    return *this;
}

RSingle::DataForClassesAndState &RSingle::DataForClassesAndState::sqrt()
{
    realNewCallIntensityOut = qSqrt( realNewCallIntensityOut);
    offeredCallIntensityOut = qSqrt(offeredCallIntensityOut);
    newCallIntensityIn      = qSqrt(newCallIntensityIn);
    endCallIntensityOut     = qSqrt(endCallIntensityOut);
    endCallIntensityIn      = qSqrt(endCallIntensityIn);
    cac_probability         = qSqrt(cac_probability);
    utilization             = qSqrt(utilization);
    return *this;
}

RSingle::DataForClassesAndState RSingle::DataForClassesAndState::pow(double rho) const
{
    RSingle::DataForClassesAndState result;
    result.realNewCallIntensityOut = qPow( realNewCallIntensityOut, rho);
    result.offeredCallIntensityOut = qPow(offeredCallIntensityOut, rho);
    result.newCallIntensityIn      = qPow(newCallIntensityIn, rho);
    result.endCallIntensityOut     = qPow(endCallIntensityOut, rho);
    result.endCallIntensityIn      = qPow(endCallIntensityIn, rho);
    result.cac_probability         = qPow(cac_probability, rho);
    result.utilization             = qPow(utilization, rho);
    return result;
}

void RSingle::DataForClassesAndState::clear()
{
    realNewCallIntensityOut = 0;
    offeredCallIntensityOut = 0;
    newCallIntensityIn      = 0;
    endCallIntensityOut     = 0;
    endCallIntensityIn      = 0;
    cac_probability         = 0;
    utilization             = 0;
}

} //namespace Results
