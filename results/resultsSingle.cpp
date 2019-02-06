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
    Vs = system->vk_s();

    dataGeneral.clear();
    dataPerClasses.fill(DataForClasses(), m);
    dataPerSystemState.fill(DataForStates(), V+1);
    dataPerServerState.fill(DataForStates(), Vs+1);
    dataPerBufferState.fill(DataForStates(), V-Vs+1);
    dataPerServerAndBufferState.fill(DataForStates(), (Vs+1) * (V-Vs+1));
    dataPerClassAndServerState.fill(DataForClassesAndState(), (Vs+1)*m);
    dataPerClassAndQueueState.fill(DataForClassesAndState(), (V-Vs+1)*m);
    dataPerClassAndSystemStateForSystem.fill(DataForClassesAndState(), (V+1)*m);
    dataPerClassAndSystemStateForServer.fill(DataForClassesAndState(), (V+1)*m);
    dataPerClassAndSystemStateForBuffer.fill(DataForClassesAndState(), (V+1)*m);
    dataPerGroupCombination.fill(DataPerGroups(vMax+1, m), ::Utils::UtilsLAG::getPossibleCombinations(system->k_s()).length());
    dataPerExactGroupNumber.fill(DataPerGroups(vMax+1, m), (system->k_s())+1);
}

RSingle &RSingle::write(TypeGeneral type, double value)
{
    switch (type)
    {
    case TypeGeneral::SystemUtilization:
        dataGeneral.systemUtilization = value;
        break;

    case TypeGeneral::ServerUtilization:
        dataGeneral.serverUtilization = value;
        break;

    case TypeGeneral::BufferUtilization:
        dataGeneral.bufferUtilization = value;
        break;

    case TypeGeneral::TotalTime:
        dataGeneral.totalTime = value;
        break;

    case TypeGeneral::ServiceTime:
        dataGeneral.serviceTime = value;
        break;

    case TypeGeneral::WaitingTime:
        dataGeneral.waitingTime = value;
        break;

    }
    return *this;
}

bool RSingle::read(double &result, TypeGeneral type) const
{
    switch (type)
    {
    case TypeGeneral::SystemUtilization:
        result = dataGeneral.systemUtilization;
        break;

    case TypeGeneral::ServerUtilization:
        result = dataGeneral.serverUtilization;
        break;

    case TypeGeneral::BufferUtilization:
        result = dataGeneral.bufferUtilization;
        break;

    case TypeGeneral::TotalTime:
        result = dataGeneral.totalTime;
        break;

    case TypeGeneral::ServiceTime:
        result = dataGeneral.serviceTime;
        break;

    case TypeGeneral::WaitingTime:
        result = dataGeneral.waitingTime;
        break;
    }
    return true;
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

    case TypeForClass::AvarageNumbersOfCallsInSystem:
        dataPerClasses[classNo].avarageNumbersOfCallsInSystem = value;
        break;

    case TypeForClass::AvarageNumbersOfCallsInServer:
        dataPerClasses[classNo].avarageNumbersOfCallsInServer = value;
        break;

    case TypeForClass::AvarageNumbersOfCallsInBuffer:
        dataPerClasses[classNo].avarageNumbersOfCallsInBuffer = value;
        break;
    }
    return *this;
}

bool RSingle::read(double &result, TypeForClass type, int classNo) const
{
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

    case TypeForClass::AvarageNumbersOfCallsInSystem:
        result = dataPerClasses[classNo].avarageNumbersOfCallsInSystem;
        break;

    case TypeForClass::AvarageNumbersOfCallsInServer:
        result = dataPerClasses[classNo].avarageNumbersOfCallsInServer;
        break;

    case TypeForClass::AvarageNumbersOfCallsInBuffer:
        result = dataPerClasses[classNo].avarageNumbersOfCallsInBuffer;
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

    case TypeForSystemState::IntensityNewCallOutOffered:
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

    case TypeForSystemState::IntensityNewCallOutOffered:
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

RSingle &RSingle::write(TypeForBufferState type, double value, int queueState)
{
    switch (type)
    {
    case TypeForBufferState::StateProbability:
        dataPerBufferState[queueState].probability = value;
        break;

    case TypeForBufferState::IntensityNewCallIn:
        dataPerSystemState[queueState].newCallInIntensity = value;
        break;

    case TypeForBufferState::IntensityEndCallIn:
        dataPerSystemState[queueState].endCallInIntensity = value;
        break;

    case TypeForBufferState::IntensityNewCallOut:
        dataPerSystemState[queueState].newCallOutIntensity = value;
        break;

    case TypeForBufferState::IntensityEndCallOut:
        dataPerSystemState[queueState].endCallOutIntensity = value;
        break;
    }
    return *this;
}

bool RSingle::read(double &result, TypeForBufferState type, int queueState) const
{
    switch (type)
    {
    case TypeForBufferState::StateProbability:
        result = dataPerBufferState[queueState].probability;
        break;

    case TypeForBufferState::IntensityNewCallIn:
        result = dataPerSystemState[queueState].newCallInIntensity;
        break;

    case TypeForBufferState::IntensityEndCallIn:
        result = dataPerSystemState[queueState].endCallInIntensity;
        break;

    case TypeForBufferState::IntensityNewCallOut:
        result = dataPerSystemState[queueState].newCallOutIntensity;
        break;

    case TypeForBufferState::IntensityEndCallOut:
        result = dataPerSystemState[queueState].endCallOutIntensity;
        break;
    }
    return true;
}

RSingle &RSingle::write(TypeForServerAngBufferState type, double value, int serverState, int bufferState)
{
    int index = serverState + (Vs+1) * bufferState;
    switch (type)
    {
    case TypeForServerAngBufferState::StateProbability:
        dataPerServerAndBufferState[index].probability = value;
        break;
    }
    return *this;
}

bool RSingle::read(double &result, TypeForServerAngBufferState type, int serverState, int bufferState) const
{
    int index = serverState + (Vs+1) * bufferState;

    switch (type)
    {
    case TypeForServerAngBufferState::StateProbability:
        result = dataPerServerAndBufferState[index].probability;
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

bool RSingle::read(double &result, TypeForClassAndServerState type, int classNo, int serverState) const
{
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

RSingle &RSingle::write(TypeForClassAndBufferState type, double value, int classNo, int queueState)
{
    int index = classNo * (V-Vs+1) + queueState;
    switch(type)
    {
    case TypeForClassAndBufferState::RealNewCallIntensityOut:
        dataPerClassAndQueueState[index].realNewCallIntensityOut = value;
        break;

    case TypeForClassAndBufferState::OfferedNewCallIntensityOut:
        dataPerClassAndQueueState[index].offeredCallIntensityOut = value;
        break;

    case TypeForClassAndBufferState::EndCallIntensityOut:
        dataPerClassAndQueueState[index].endCallIntensityOut = value;
        break;

    case TypeForClassAndBufferState::NewCallIntensityIn:
        dataPerClassAndQueueState[index].newCallIntensityIn = value;
        break;

    case TypeForClassAndBufferState::EndCallIntensityIn:
        dataPerClassAndQueueState[index].endCallIntensityIn = value;
        break;

    case TypeForClassAndBufferState::CAC_Probability:
        dataPerClassAndQueueState[index].cac_probability = value;
        break;

    case TypeForClassAndBufferState::Usage:
        dataPerClassAndQueueState[index].utilization = value;
        break;
    }
    return *this;
}

bool RSingle::read(double &result, TypeForClassAndBufferState type, int classNo, int queueState) const
{
    int index = classNo * (V-Vs+1) + queueState;

    switch(type)
    {
    case TypeForClassAndBufferState::RealNewCallIntensityOut:
        result = dataPerClassAndQueueState[index].realNewCallIntensityOut;
        break;

    case TypeForClassAndBufferState::OfferedNewCallIntensityOut:
        result = dataPerClassAndQueueState[index].offeredCallIntensityOut;
        break;

    case TypeForClassAndBufferState::EndCallIntensityOut:
        result = dataPerClassAndQueueState[index].endCallIntensityOut;
        break;

    case TypeForClassAndBufferState::NewCallIntensityIn:
        result = dataPerClassAndQueueState[index].newCallIntensityIn;
        break;

    case TypeForClassAndBufferState::EndCallIntensityIn:
        result = dataPerClassAndQueueState[index].endCallIntensityIn;
        break;

    case TypeForClassAndBufferState::CAC_Probability:
        result = dataPerClassAndQueueState[index].cac_probability;
        break;

    case TypeForClassAndBufferState::Usage:
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
        dataPerClassAndSystemStateForBuffer[index].realNewCallIntensityOut = value;
        break;

    case TypeForClassAndSystemState::RealNewCallIntensityOutForSystem:
        dataPerClassAndSystemStateForSystem[index].realNewCallIntensityOut = value;
        break;

    case TypeForClassAndSystemState::OfferedNewCallIntensityOutForServer:
        dataPerClassAndSystemStateForServer[index].offeredCallIntensityOut = value;
        break;

    case TypeForClassAndSystemState::OfferedNewCallIntensityOutForQueue:
        dataPerClassAndSystemStateForBuffer[index].offeredCallIntensityOut = value;
        break;

    case TypeForClassAndSystemState::OfferedNewCallIntensityOutForSystem:
        dataPerClassAndSystemStateForSystem[index].offeredCallIntensityOut = value;
        break;

    case TypeForClassAndSystemState::EndCallIntensityOutForServer:
        dataPerClassAndSystemStateForServer[index].endCallIntensityOut = value;
        break;

    case TypeForClassAndSystemState::EndCallIntensityOutForQueue:
        dataPerClassAndSystemStateForBuffer[index].endCallIntensityOut = value;
        break;

    case TypeForClassAndSystemState::EndCallIntensityOutForSystem:
        dataPerClassAndSystemStateForSystem[index].endCallIntensityOut = value;
        break;

    case TypeForClassAndSystemState::NewCallIntensityInForServer:
        dataPerClassAndSystemStateForServer[index].newCallIntensityIn = value;
        break;

    case TypeForClassAndSystemState::NewCallIntensityInForQueue:
        dataPerClassAndSystemStateForBuffer[index].newCallIntensityIn = value;
        break;

    case TypeForClassAndSystemState::NewCallIntensityInForSystem:
        dataPerClassAndSystemStateForSystem[index].newCallIntensityIn = value;
        break;

    case TypeForClassAndSystemState::EndCallIntensityInForServer:
        dataPerClassAndSystemStateForServer[index].endCallIntensityIn = value;
        break;

    case TypeForClassAndSystemState::EndCallIntensityInForQueue:
        dataPerClassAndSystemStateForBuffer[index].endCallIntensityIn = value;
        break;

    case TypeForClassAndSystemState::EndCallIntensityInForSystem:
        dataPerClassAndSystemStateForSystem[index].endCallIntensityIn = value;
        break;

    case TypeForClassAndSystemState::CAC_ProbabilityForServer:
        dataPerClassAndSystemStateForServer[index].cac_probability = value;
        break;

    case TypeForClassAndSystemState::CAC_ProbabilityForQueue:
        dataPerClassAndSystemStateForBuffer[index].cac_probability = value;
        break;

    case TypeForClassAndSystemState::CAC_ProbabilityForSystem:
        dataPerClassAndSystemStateForSystem[index].cac_probability = value;
        break;

    case TypeForClassAndSystemState::UsageForServer:
        dataPerClassAndSystemStateForServer[index].utilization = value;
        break;

    case TypeForClassAndSystemState::UsageForBuffer:
        dataPerClassAndSystemStateForBuffer[index].utilization = value;
        break;

    case TypeForClassAndSystemState::UsageForSystem:
        dataPerClassAndSystemStateForSystem[index].utilization = value;
        break;
    }
    return *this;
}

bool RSingle::read(double &result, TypeForClassAndSystemState type, int classNo, int systemState) const
{
    int index = classNo * (V+1) + systemState;

    switch(type)
    {
    case TypeForClassAndSystemState::RealNewCallIntensityOutForServer:
        result = dataPerClassAndSystemStateForServer[index].realNewCallIntensityOut;
        break;

    case TypeForClassAndSystemState::RealNewCallIntensityOutForQueue:
        result = dataPerClassAndSystemStateForBuffer[index].realNewCallIntensityOut;
        break;

    case TypeForClassAndSystemState::RealNewCallIntensityOutForSystem:
        result = dataPerClassAndSystemStateForSystem[index].realNewCallIntensityOut;
        break;

    case TypeForClassAndSystemState::OfferedNewCallIntensityOutForServer:
        result = dataPerClassAndSystemStateForServer[index].offeredCallIntensityOut;
        break;

    case TypeForClassAndSystemState::OfferedNewCallIntensityOutForQueue:
        result = dataPerClassAndSystemStateForBuffer[index].offeredCallIntensityOut;
        break;

    case TypeForClassAndSystemState::OfferedNewCallIntensityOutForSystem:
        result = dataPerClassAndSystemStateForSystem[index].offeredCallIntensityOut;
        break;

    case TypeForClassAndSystemState::EndCallIntensityOutForServer:
        result = dataPerClassAndSystemStateForServer[index].endCallIntensityOut;
        break;

    case TypeForClassAndSystemState::EndCallIntensityOutForQueue:
        result = dataPerClassAndSystemStateForBuffer[index].endCallIntensityOut;
        break;

    case TypeForClassAndSystemState::EndCallIntensityOutForSystem:
        result = dataPerClassAndSystemStateForSystem[index].endCallIntensityOut;
        break;

    case TypeForClassAndSystemState::NewCallIntensityInForServer:
        result = dataPerClassAndSystemStateForServer[index].newCallIntensityIn;
        break;

    case TypeForClassAndSystemState::NewCallIntensityInForQueue:
        result = dataPerClassAndSystemStateForBuffer[index].newCallIntensityIn;
        break;

    case TypeForClassAndSystemState::NewCallIntensityInForSystem:
        result = dataPerClassAndSystemStateForSystem[index].newCallIntensityIn;
        break;

    case TypeForClassAndSystemState::EndCallIntensityInForServer:
        result = dataPerClassAndSystemStateForServer[index].endCallIntensityIn;
        break;

    case TypeForClassAndSystemState::EndCallIntensityInForQueue:
        result = dataPerClassAndSystemStateForBuffer[index].endCallIntensityIn;
        break;

    case TypeForClassAndSystemState::EndCallIntensityInForSystem:
        result = dataPerClassAndSystemStateForSystem[index].endCallIntensityIn;
        break;

    case TypeForClassAndSystemState::CAC_ProbabilityForServer:
        result = dataPerClassAndSystemStateForServer[index].cac_probability;
        break;

    case TypeForClassAndSystemState::CAC_ProbabilityForQueue:
        result = dataPerClassAndSystemStateForBuffer[index].cac_probability;
        break;

    case TypeForClassAndSystemState::CAC_ProbabilityForSystem:
        result = dataPerClassAndSystemStateForSystem[index].cac_probability;
        break;

    case TypeForClassAndSystemState::UsageForServer:
        result = dataPerClassAndSystemStateForServer[index].utilization;
        break;

    case TypeForClassAndSystemState::UsageForBuffer:
        result = dataPerClassAndSystemStateForBuffer[index].utilization;
        break;

    case TypeForClassAndSystemState::UsageForSystem:
        result = dataPerClassAndSystemStateForSystem[index].utilization;
        break;
    }
    return true;
}

RSingle &RSingle::write(TypeResourcess_VsServerGroupsCombination type, double value, int numberOfResourcess, int groupCombinationIndex)
{
    switch(type)
    {
    case TypeResourcess_VsServerGroupsCombination::AvailabilityInOneOrMoreGroups:
        dataPerGroupCombination[groupCombinationIndex].availabilityProbabilities[numberOfResourcess].freeInBestGroup = value;
        break;

    case TypeResourcess_VsServerGroupsCombination::AvailabilityInAllTheGroups:
        dataPerGroupCombination[groupCombinationIndex].availabilityProbabilities[numberOfResourcess].availabilityInAllTheGroups = value;
        break;

    case TypeResourcess_VsServerGroupsCombination::InavailabilityInAllTheGroups:
        dataPerGroupCombination[groupCombinationIndex].availabilityProbabilities[numberOfResourcess].inavailabilityInAllTheGroups = value;
        break;
    }
    return *this;
}

bool RSingle::read(double &result, TypeResourcess_VsServerGroupsCombination type, int numberOfResourcess, int groupCombinationIndex) const
{
    switch(type)
    {
    case TypeResourcess_VsServerGroupsCombination::AvailabilityInOneOrMoreGroups:
        result = dataPerGroupCombination[groupCombinationIndex].availabilityProbabilities[numberOfResourcess].freeInBestGroup;
        break;

    case TypeResourcess_VsServerGroupsCombination::AvailabilityInAllTheGroups:
        result = dataPerGroupCombination[groupCombinationIndex].availabilityProbabilities[numberOfResourcess].availabilityInAllTheGroups;
        break;

    case TypeResourcess_VsServerGroupsCombination::InavailabilityInAllTheGroups:
        result = dataPerGroupCombination[groupCombinationIndex].availabilityProbabilities[numberOfResourcess].inavailabilityInAllTheGroups;
        break;
    }
    return true;
}

RSingle &RSingle::write(TypeForResourcessAndNumberOfServerGroups type, double value, int numberOfResourcess, int numberOfGroups)
{
    switch(type)
    {
    case TypeForResourcessAndNumberOfServerGroups::AvailabilityOnlyInAllTheGroups:
        dataPerExactGroupNumber[numberOfGroups].availabilityProbabilities[numberOfResourcess].availabilityOnlyInAllTheGroups = value;
        break;
    case TypeForResourcessAndNumberOfServerGroups::AvailabilityInAllTheGroups:
        dataPerExactGroupNumber[numberOfGroups].availabilityProbabilities[numberOfResourcess].availabilityInAllTheGroups = value;
        break;
    case TypeForResourcessAndNumberOfServerGroups::InavailabilityInAllTheGroups:
        dataPerExactGroupNumber[numberOfGroups].availabilityProbabilities[numberOfResourcess].inavailabilityInAllTheGroups = value;
        break;
    }
    return *this;
}

bool RSingle::read(double &result, TypeForResourcessAndNumberOfServerGroups type, int numberOfResourcess, int numberOfGroups) const
{
    switch(type)
    {
    case TypeForResourcessAndNumberOfServerGroups::AvailabilityOnlyInAllTheGroups:
        result = dataPerExactGroupNumber[numberOfGroups].availabilityProbabilities[numberOfResourcess].availabilityOnlyInAllTheGroups;
        break;
    case TypeForResourcessAndNumberOfServerGroups::AvailabilityInAllTheGroups:
        result = dataPerExactGroupNumber[numberOfGroups].availabilityProbabilities[numberOfResourcess].availabilityInAllTheGroups;
        break;

    case TypeForResourcessAndNumberOfServerGroups::InavailabilityInAllTheGroups:
        result = dataPerExactGroupNumber[numberOfGroups].availabilityProbabilities[numberOfResourcess].inavailabilityInAllTheGroups;
        break;
    }
    return true;
}

RSingle::DataGeneral &RSingle::DataGeneral::operator+=(const RSingle::DataGeneral &rho)
{
    systemUtilization+= rho.systemUtilization;
    serverUtilization+= rho.serverUtilization;
    bufferUtilization+= rho.bufferUtilization;
    totalTime        += rho.totalTime;
    serviceTime      += rho.serviceTime;
    waitingTime      += rho.waitingTime;

    return *this;
}

RSingle::DataGeneral RSingle::DataGeneral::operator-(const RSingle::DataGeneral &rho) const
{
    RSingle::DataGeneral result;
    result.systemUtilization = systemUtilization + rho.serverUtilization;
    result.serverUtilization = serverUtilization + rho.serverUtilization;
    result.bufferUtilization = bufferUtilization + rho.bufferUtilization;
    result.totalTime         = totalTime         + rho.totalTime;
    result.serviceTime       = serviceTime       + rho.serviceTime;
    result.waitingTime       = waitingTime       + rho.waitingTime;

    return result;
}

RSingle::DataGeneral RSingle::DataGeneral::operator^(double rho) const
{
    RSingle::DataGeneral result;
    result.systemUtilization = qPow(systemUtilization, rho);
    result.serverUtilization = qPow(serverUtilization, rho);
    result.bufferUtilization = qPow(bufferUtilization, rho);
    result.totalTime         = qPow(totalTime,         rho);
    result.serviceTime       = qPow(serviceTime,       rho);
    result.waitingTime       = qPow(waitingTime,       rho);

    return *this;
}

RSingle::DataGeneral RSingle::DataGeneral::operator*(const RSingle::DataGeneral &rho) const
{
    RSingle::DataGeneral result;
    result.systemUtilization = systemUtilization * rho.serverUtilization;
    result.serverUtilization = serverUtilization * rho.serverUtilization;
    result.bufferUtilization = bufferUtilization * rho.bufferUtilization;
    result.totalTime         = totalTime         * rho.totalTime;
    result.serviceTime       = serviceTime       * rho.serviceTime;
    result.waitingTime       = waitingTime       * rho.waitingTime;

    return result;
}

RSingle::DataGeneral &RSingle::DataGeneral::operator/=(double rho)
{
    systemUtilization/= rho;
    serverUtilization/= rho;
    bufferUtilization/= rho;
    totalTime        /= rho;
    serviceTime      /= rho;
    waitingTime      /= rho;

    return *this;
}

RSingle::DataGeneral &RSingle::DataGeneral::operator*=(double rho)
{
    systemUtilization*= rho;
    serverUtilization*= rho;
    bufferUtilization*= rho;
    totalTime        *= rho;
    serviceTime      *= rho;
    waitingTime      *= rho;

    return *this;
}

void RSingle::DataGeneral::clear()
{
    systemUtilization = 0;
    serverUtilization = 0;
    bufferUtilization = 0;
    totalTime         = 0;
    serviceTime       = 0;
    waitingTime       = 0;
}

RSingle& RSingle::operator+=(const RSingle &rho)
{
    Utils::addElementsToFirst<RSingle::DataForClasses>(dataPerClasses, rho.dataPerClasses);
    Utils::addElementsToFirst<RSingle::DataForStates>(dataPerSystemState, rho.dataPerSystemState);
    Utils::addElementsToFirst<RSingle::DataForStates>(dataPerServerState, rho.dataPerServerState);
    Utils::addElementsToFirst<RSingle::DataForStates>(dataPerBufferState, rho.dataPerBufferState);
    Utils::addElementsToFirst<RSingle::DataForStates>(dataPerServerAndBufferState, rho.dataPerServerAndBufferState);
    Utils::addElementsToFirst<DataForClassesAndState>(dataPerClassAndServerState, rho.dataPerClassAndServerState);
    Utils::addElementsToFirst<DataForClassesAndState>(dataPerClassAndQueueState, rho.dataPerClassAndQueueState);
    Utils::addElementsToFirst<DataForClassesAndState>(dataPerClassAndSystemStateForServer, rho.dataPerClassAndSystemStateForServer);
    Utils::addElementsToFirst<DataForClassesAndState>(dataPerClassAndSystemStateForBuffer, rho.dataPerClassAndSystemStateForBuffer);
    Utils::addElementsToFirst<DataForClassesAndState>(dataPerClassAndSystemStateForSystem, rho.dataPerClassAndSystemStateForSystem);
    Utils::addElementsToFirst<RSingle::DataPerGroups>(dataPerGroupCombination, rho.dataPerGroupCombination);
    Utils::addElementsToFirst<RSingle::DataPerGroups>(dataPerExactGroupNumber, rho.dataPerExactGroupNumber);
    return *this;
}

RSingle RSingle::operator-(const RSingle &rho) const
{
    RSingle result = *this;
    result.dataPerClasses                      = Utils::subtractElements<RSingle::DataForClasses>(dataPerClasses, rho.dataPerClasses);
    result.dataPerSystemState                  = Utils::subtractElements<RSingle::DataForStates>(dataPerSystemState, rho.dataPerSystemState);
    result.dataPerServerState                  = Utils::subtractElements<RSingle::DataForStates>(dataPerServerState, rho.dataPerServerState);
    result.dataPerBufferState                  = Utils::subtractElements<RSingle::DataForStates>(dataPerBufferState, rho.dataPerBufferState);
    result.dataPerServerAndBufferState         = Utils::subtractElements<RSingle::DataForStates>(dataPerServerAndBufferState, rho.dataPerServerAndBufferState);
    result.dataPerClassAndServerState          = Utils::subtractElements<DataForClassesAndState>(dataPerClassAndServerState, rho.dataPerClassAndServerState);
    result.dataPerClassAndQueueState           = Utils::subtractElements<DataForClassesAndState>(dataPerClassAndQueueState, rho.dataPerClassAndQueueState);
    result.dataPerClassAndSystemStateForServer = Utils::subtractElements<DataForClassesAndState>(dataPerClassAndSystemStateForServer, rho.dataPerClassAndSystemStateForServer);
    result.dataPerClassAndSystemStateForBuffer  = Utils::subtractElements<DataForClassesAndState>(dataPerClassAndSystemStateForBuffer,  rho.dataPerClassAndSystemStateForBuffer);
    result.dataPerClassAndSystemStateForSystem = Utils::subtractElements<DataForClassesAndState>(dataPerClassAndSystemStateForSystem, rho.dataPerClassAndSystemStateForSystem);
    result.dataPerGroupCombination             = Utils::subtractElements<RSingle::DataPerGroups> (this->dataPerGroupCombination, rho.dataPerGroupCombination);
    result.dataPerExactGroupNumber             = Utils::subtractElements<RSingle::DataPerGroups> (this->dataPerExactGroupNumber, rho.dataPerExactGroupNumber);
    return result;
}

RSingle RSingle::operator^(double rho) const
{
    RSingle result = *this;
    result.dataPerClasses                      = Utils::powerElementTempl<DataForClasses>(result.dataPerClasses, rho);
    result.dataPerSystemState                  = Utils::powerElementTempl<RSingle::DataForStates>(dataPerSystemState, rho);
    result.dataPerServerState                  = Utils::powerElementTempl<RSingle::DataForStates>(dataPerServerState, rho);
    result.dataPerBufferState                  = Utils::powerElementTempl<RSingle::DataForStates>(dataPerBufferState, rho);
    result.dataPerServerAndBufferState         = Utils::powerElementTempl<RSingle::DataForStates>(dataPerServerAndBufferState, rho);
    result.dataPerClassAndServerState          = Utils::powerElementTempl<DataForClassesAndState>(dataPerClassAndServerState, rho);
    result.dataPerClassAndQueueState           = Utils::powerElementTempl<DataForClassesAndState>(dataPerClassAndQueueState, rho);
    result.dataPerClassAndSystemStateForServer = Utils::powerElementTempl<DataForClassesAndState>(dataPerClassAndSystemStateForServer, rho);
    result.dataPerClassAndSystemStateForBuffer  = Utils::powerElementTempl<DataForClassesAndState>(dataPerClassAndSystemStateForBuffer, rho);
    result.dataPerClassAndSystemStateForSystem = Utils::powerElementTempl<DataForClassesAndState>(dataPerClassAndSystemStateForSystem, rho);
    result.dataPerGroupCombination             = Utils::powerElementTempl<RSingle::DataPerGroups> (this->dataPerGroupCombination, rho);
    result.dataPerExactGroupNumber             = Utils::powerElementTempl<RSingle::DataPerGroups> (this->dataPerExactGroupNumber, rho);
    return result;
}

RSingle RSingle::operator*(const RSingle &rho)
{
    RSingle result = *this;
    result.dataPerClasses                      = Utils::multiplyElement<RSingle::DataForClasses>(dataPerClasses, rho.dataPerClasses);
    result.dataPerSystemState                  = Utils::multiplyElement<RSingle::DataForStates>(dataPerSystemState, rho.dataPerSystemState);
    result.dataPerServerState                  = Utils::multiplyElement<RSingle::DataForStates>(dataPerServerState, rho.dataPerServerState);
    result.dataPerBufferState                  = Utils::multiplyElement<RSingle::DataForStates>(dataPerBufferState, rho.dataPerBufferState);
    result.dataPerServerAndBufferState         = Utils::multiplyElement<RSingle::DataForStates>(dataPerServerAndBufferState, rho.dataPerServerAndBufferState);
    result.dataPerClassAndServerState          = Utils::multiplyElement<DataForClassesAndState>(dataPerClassAndServerState, rho.dataPerClassAndServerState);
    result.dataPerClassAndQueueState           = Utils::multiplyElement<DataForClassesAndState>(dataPerClassAndQueueState, rho.dataPerClassAndQueueState);
    result.dataPerClassAndSystemStateForServer = Utils::multiplyElement<DataForClassesAndState>(dataPerClassAndSystemStateForServer, rho.dataPerClassAndSystemStateForServer);
    result.dataPerClassAndSystemStateForBuffer  = Utils::multiplyElement<DataForClassesAndState>(dataPerClassAndSystemStateForBuffer, rho.dataPerClassAndSystemStateForBuffer);
    result.dataPerClassAndSystemStateForSystem = Utils::multiplyElement<DataForClassesAndState>(dataPerClassAndSystemStateForSystem, rho.dataPerClassAndSystemStateForSystem);
    result.dataPerGroupCombination             = Utils::multiplyElement<RSingle::DataPerGroups> (this->dataPerGroupCombination, rho.dataPerGroupCombination);
    return result;
}

RSingle &RSingle::operator/=(double rho)
{
    Utils::divideElementToFirst<RSingle::DataForClasses>(dataPerClasses, rho);
    Utils::divideElementToFirst<RSingle::DataForStates>(dataPerSystemState, rho);
    Utils::divideElementToFirst<RSingle::DataForStates>(dataPerServerState, rho);
    Utils::divideElementToFirst<RSingle::DataForStates>(dataPerBufferState, rho);
    Utils::divideElementToFirst<RSingle::DataForStates>(dataPerServerAndBufferState, rho);
    Utils::divideElementToFirst<RSingle::DataForClassesAndState>(dataPerClassAndServerState, rho);
    Utils::divideElementToFirst<RSingle::DataForClassesAndState>(dataPerClassAndQueueState, rho);
    Utils::divideElementToFirst<RSingle::DataForClassesAndState>(dataPerClassAndSystemStateForServer, rho);
    Utils::divideElementToFirst<RSingle::DataForClassesAndState>(dataPerClassAndSystemStateForBuffer, rho);
    Utils::divideElementToFirst<RSingle::DataForClassesAndState>(dataPerClassAndSystemStateForSystem, rho);
    Utils::divideElementToFirst<RSingle::DataPerGroups>(dataPerGroupCombination, rho);
    Utils::divideElementToFirst<RSingle::DataPerGroups>(dataPerExactGroupNumber, rho);
    return *this;
}

RSingle &RSingle::operator*=(double rho)
{
    Utils::multiplyElementToFirst<RSingle::DataForClasses>(dataPerClasses, rho);
    Utils::multiplyElementToFirst<RSingle::DataForStates>(dataPerSystemState, rho);
    Utils::multiplyElementToFirst<RSingle::DataForStates>(dataPerServerState, rho);
    Utils::multiplyElementToFirst<RSingle::DataForStates>(dataPerBufferState, rho);
    Utils::multiplyElementToFirst<RSingle::DataForStates>(dataPerServerAndBufferState, rho);
    Utils::multiplyElementToFirst<RSingle::DataForClassesAndState>(dataPerClassAndServerState, rho);
    Utils::multiplyElementToFirst<RSingle::DataForClassesAndState>(dataPerClassAndQueueState, rho);
    Utils::multiplyElementToFirst<RSingle::DataForClassesAndState>(dataPerClassAndSystemStateForServer, rho);
    Utils::multiplyElementToFirst<RSingle::DataForClassesAndState>(dataPerClassAndSystemStateForBuffer, rho);
    Utils::multiplyElementToFirst<RSingle::DataForClassesAndState>(dataPerClassAndSystemStateForSystem, rho);
    Utils::multiplyElementToFirst<RSingle::DataPerGroups>(dataPerGroupCombination, rho);
    Utils::multiplyElementToFirst<RSingle::DataPerGroups>(dataPerExactGroupNumber, rho);
    return *this;
}

void RSingle::sqrt()
{ 
    Utils::sqrtTemplate<DataForClasses>(this->dataPerClasses);
    Utils::sqrtTemplate<RSingle::DataForStates>(dataPerSystemState);
    Utils::sqrtTemplate<RSingle::DataForStates>(dataPerServerState);
    Utils::sqrtTemplate<RSingle::DataForStates>(dataPerBufferState);
    Utils::sqrtTemplate<RSingle::DataForStates>(dataPerServerAndBufferState);
    Utils::sqrtTemplate<RSingle::DataForClassesAndState>(dataPerClassAndServerState);
    Utils::sqrtTemplate<RSingle::DataForClassesAndState>(dataPerClassAndQueueState);
    Utils::sqrtTemplate<RSingle::DataForClassesAndState>(dataPerClassAndSystemStateForServer);
    Utils::sqrtTemplate<RSingle::DataForClassesAndState>(dataPerClassAndSystemStateForBuffer);
    Utils::sqrtTemplate<RSingle::DataForClassesAndState>(dataPerClassAndSystemStateForSystem);
    Utils::sqrtTemplate<RSingle::DataPerGroups>(dataPerGroupCombination);
    Utils::sqrtTemplate<RSingle::DataPerGroups>(dataPerExactGroupNumber);
}

void RSingle::clear()
{
    dataPerClasses.fill(DataForClasses());
    dataPerSystemState.fill(DataForStates());
    dataPerServerState.fill(DataForStates());
    dataPerBufferState.fill(DataForStates());
    dataPerServerAndBufferState.fill(DataForStates());;
    dataPerClassAndServerState.fill(DataForClassesAndState());
    dataPerClassAndQueueState.fill(DataForClassesAndState());
    dataPerClassAndSystemStateForServer.fill(DataForClassesAndState());
    dataPerClassAndSystemStateForBuffer.fill(DataForClassesAndState());
    dataPerClassAndSystemStateForSystem.fill(DataForClassesAndState());
    dataPerGroupCombination.fill(DataPerGroups(vMax+1, m));
    dataPerExactGroupNumber.fill(DataPerGroups(vMax+1, m));
}

RSingle::DataForClasses &RSingle::DataForClasses::operator+=(const RSingle::DataForClasses &rho)
{
    blockingProbability          +=rho.blockingProbability;
    lossProbability              +=rho.lossProbability;
    congestionTraffic            +=rho.congestionTraffic;
    avarageNumbersOfCallsInSystem+=rho.avarageNumbersOfCallsInSystem;
    avarageNumbersOfCallsInServer+=rho.avarageNumbersOfCallsInServer;
    avarageNumbersOfCallsInBuffer+=rho.avarageNumbersOfCallsInBuffer;

    return *this;
}

RSingle::DataForClasses RSingle::DataForClasses::operator-(const RSingle::DataForClasses &rho) const
{
    RSingle::DataForClasses result;

    result.blockingProbability           = blockingProbability - rho.blockingProbability;
    result.lossProbability               = lossProbability - rho.lossProbability;
    result.congestionTraffic             = congestionTraffic - rho.congestionTraffic;
    result.avarageNumbersOfCallsInSystem = avarageNumbersOfCallsInSystem - rho.avarageNumbersOfCallsInSystem;
    result.avarageNumbersOfCallsInServer = avarageNumbersOfCallsInServer - rho.avarageNumbersOfCallsInServer;
    result.avarageNumbersOfCallsInBuffer = avarageNumbersOfCallsInBuffer - rho.avarageNumbersOfCallsInBuffer;
    return result;
}

RSingle::DataForClasses RSingle::DataForClasses::operator^(double rho) const
{
    RSingle::DataForClasses result;

    result.blockingProbability           = qPow(this->blockingProbability, rho);
    result.lossProbability               = qPow(this->lossProbability, rho);
    result.congestionTraffic             = qPow(this->congestionTraffic, rho);
    result.avarageNumbersOfCallsInSystem = qPow(this->avarageNumbersOfCallsInSystem, rho);
    result.avarageNumbersOfCallsInServer = qPow(this->avarageNumbersOfCallsInServer, rho);
    result.avarageNumbersOfCallsInBuffer = qPow(this->avarageNumbersOfCallsInBuffer, rho);

    return result;
}

RSingle::DataForClasses RSingle::DataForClasses::operator*(const RSingle::DataForClasses &rho) const
{
    RSingle::DataForClasses result;

    result.blockingProbability           = this->blockingProbability * rho.blockingProbability;
    result.lossProbability               = this->lossProbability * rho.lossProbability;
    result.congestionTraffic             = this->congestionTraffic * rho.congestionTraffic;
    result.avarageNumbersOfCallsInSystem = this->avarageNumbersOfCallsInSystem * rho.avarageNumbersOfCallsInSystem;
    result.avarageNumbersOfCallsInServer = this->avarageNumbersOfCallsInServer * rho.avarageNumbersOfCallsInServer;
    result.avarageNumbersOfCallsInBuffer = this->avarageNumbersOfCallsInBuffer * rho.avarageNumbersOfCallsInBuffer;
    return result;
}

RSingle::DataForClasses &RSingle::DataForClasses::operator/=(double rho)
{
    blockingProbability           /= rho;
    lossProbability               /= rho;
    congestionTraffic             /= rho;
    avarageNumbersOfCallsInSystem /= rho;
    avarageNumbersOfCallsInServer /= rho;
    avarageNumbersOfCallsInBuffer /= rho;
    return *this;
}

RSingle::DataForClasses &RSingle::DataForClasses::operator*=(double rho)
{
    blockingProbability           *= rho;
    lossProbability               *= rho;
    congestionTraffic             *= rho;
    avarageNumbersOfCallsInSystem *=rho;
    avarageNumbersOfCallsInServer *=rho;
    avarageNumbersOfCallsInBuffer *=rho;
    return *this;
}

RSingle::DataForClasses& RSingle::DataForClasses::sqrt()
{
    blockingProbability           = qSqrt(blockingProbability);
    lossProbability               = qSqrt(lossProbability);
    congestionTraffic             = qSqrt(congestionTraffic);
    avarageNumbersOfCallsInSystem = qSqrt(avarageNumbersOfCallsInSystem);
    avarageNumbersOfCallsInServer = qSqrt(avarageNumbersOfCallsInServer);
    avarageNumbersOfCallsInBuffer = qSqrt(avarageNumbersOfCallsInBuffer);
    return *this;
}

RSingle::DataForClasses RSingle::DataForClasses::pow(double rho) const
{
    RSingle::DataForClasses result;
    result.blockingProbability           = qPow(blockingProbability, rho);
    result.lossProbability               = qPow(lossProbability, rho);
    result.congestionTraffic             = qPow(congestionTraffic, rho);
    result.avarageNumbersOfCallsInSystem = qPow(avarageNumbersOfCallsInSystem, rho);
    result.avarageNumbersOfCallsInServer = qPow(avarageNumbersOfCallsInServer, rho);
    result.avarageNumbersOfCallsInBuffer = qPow(avarageNumbersOfCallsInBuffer, rho);
    return result;
}

void RSingle::DataForClasses::clear()
{
    blockingProbability           = 0;
    lossProbability               = 0;
    congestionTraffic             = 0;
    avarageNumbersOfCallsInSystem = 0;
    avarageNumbersOfCallsInServer = 0;
    avarageNumbersOfCallsInBuffer = 0;
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
