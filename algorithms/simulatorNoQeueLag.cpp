#include "simulatorNoQeueLag.h"
#include <qstring.h>
#include <qglobal.h>
#include <utils/lag.h>
#include "utils/utilsMisc.h"
#include <qmath.h>


namespace Algorithms
{

SimulatorNoQeueLag::SimulatorNoQeueLag() : simulator()
{
    myQoS_Set << Results::Type::BlockingProbability;
    myQoS_Set << Results::Type::OccupancyDistribution;
    myQoS_Set << Results::Type::LossProbability;
    myQoS_Set << Results::Type::AvailableSubroupDistribution;

    system = nullptr;
}

bool SimulatorNoQeueLag::possible(const ModelSyst *system) const
{
    if (system->vk_b() > 0)
        return false;
    return simulator::possible(system);
}

void SimulatorNoQeueLag::calculateSystem(const ModelSyst *system
      , double a
      , RInvestigator *results
      , SimulationParameters *simParameters)
{
    prepareTemporaryData(system, a);


    System *simData = new System(system, simParameters->noOfSeries);
    simData->initialize(a, system->totalAt(), system->vk_s());

    int seed = 1024;

    int noOfSeries = simParameters->noOfSeries;

    for (int serNo=0; serNo<noOfSeries; serNo++)
    {
        seed = qrand();
        simData->statsDisable();
        if (serNo == 0)
        {
            int noOfLostCalls = simParameters->noOfLostCalls/simParameters->spaceOnStart;
            int noOfServCalls = simParameters->noOfServicedCalls/simParameters->spaceOnStart;
            simData->doSimExperiment(noOfLostCalls, seed, noOfServCalls);
        }
        else
        {
            int noOfLostCalls = simParameters->noOfLostCalls/simParameters->spaceBetweenSeries;
            int noOfServCalls = simParameters->noOfServicedCalls/simParameters->spaceBetweenSeries;
            simData->doSimExperiment(noOfLostCalls, seed, noOfServCalls);
        }
        simData->statsEnable(serNo);
        simData->doSimExperiment(simParameters->noOfLostCalls, seed, simParameters->noOfServicedCalls);
        simData->writesResultsOfSingleExperiment((*results)[serNo]);
        qDebug("simulation experiment no %d", serNo+1);
    }
    delete simData;
    //emit this->sigCalculationDone();
}

SimulatorNoQeueLag::System::System(const ModelSyst *system, int noOfSeries)
    : m(system->m())
    , V(system->V())
    , n(0)
    , old_n(0)
    , results(system->m(), system->vk_s(), system->vk_b(), noOfSeries)
{
    n_i.resize(m);

    systemData = system;

    agenda = new SimulatorDataCollection<ProcNoQeueLag>();
    server = new Server(this);

    timesPerClassAndState.resize(system->m());
    timesPerState.resize(V+1);

    eventsPerClass.resize(system->m());
    eventsPerState.resize(system->vk_s()+system->vk_b()+1);
    eventsPerClassAndState.resize(system->m());

    for (int i=0; i<m; i++)
    {
        timesPerClassAndState[i].resize(V+1);
        eventsPerClassAndState[i].resize(V+1);
    }
}

SimulatorNoQeueLag::System::~System()
{
    delete agenda;
    delete server;

    foreach(Call *tmp, callsInSystem)
        delete tmp;

    while (uselessCalls.length())
    {
        Call *tmp = uselessCalls.pop();
        delete tmp;
    }
}

void SimulatorNoQeueLag::System::initialize(double a, int sumPropAt, int V)
{
    for(int i=0; i<systemData->m(); i++)
    {
        const ModelTrClass *tmpClass = systemData->getClass(i);
        ProcNoQeueLag::initialize(this, tmpClass, i, a, sumPropAt, V);
    }
}

void SimulatorNoQeueLag::System::doSimExperiment(int numberOfLostCall, int seed, int numberOfServicedCalls)
{
    qsrand(seed);
    this->totalNumberOfLostCalls = 0;
    this->totalNumberOfServicedCalls = 0;
    if (numberOfServicedCalls == 0)
    {
        while(totalNumberOfLostCalls < numberOfLostCall)
        {
            ProcNoQeueLag *proc = takeFirstProcess();
            statsCollectPre(proc->time);
            proc->execute(proc, this);
            statsCollectPost(classIdx);
        }
    }
    else
    {
        if(totalNumberOfLostCalls > 0)
        {
            while(totalNumberOfLostCalls < numberOfLostCall && totalNumberOfServicedCalls < numberOfServicedCalls)
            {
                ProcNoQeueLag *proc = takeFirstProcess();
                statsCollectPre(proc->time);
                proc->execute(proc, this);
                statsCollectPost(classIdx);
            }
        }
        else
        {
            while(totalNumberOfServicedCalls < numberOfServicedCalls)
            {
                ProcNoQeueLag *proc = takeFirstProcess();
                statsCollectPre(proc->time);
                proc->execute(proc, this);
                statsCollectPost(classIdx);
            }
        }
    }
}

void SimulatorNoQeueLag::System::writesResultsOfSingleExperiment(RSingle& singleResults)
{
    server->writesResultsOfSingleExperiment(singleResults, results._simulationTime);

    int Vs = server->getV();
    int V  = server->getV();
    int m  = systemData->m();

    int max_t = 0;

    for (int i=0; i<m; i++)
    {
        int t = systemData->getClass(i)->t();
        max_t = qMax(t, max_t);
    }

    for (int i=0; i<m; i++)
    {
        int t = systemData->getClass(i)->t();
        double E = 0;

        for (int n_server = qMax(0, (V-t+1)); n_server<=V; n_server++)
            E+=timesPerState[n_server].availabilityTime;
        E /=results._simulationTime;

        if (E < 0)
            qFatal("Wrong E");
        results.act_E[i] = E;
        singleResults.write(TypeForClass::BlockingProbability, E, i);

        results.act_t[i] /= results.act_noOfServicedCalls[i];

        results.act_tS[i] /= results.act_noOfServicedCalls[i];

        results.act_tServer[i] /=results.act_noOfServicedCalls[i];
        results.act_tPlanedServer[i] /=results.act_noOfServicedCalls[i];


        for (int n=0; n<=Vs; n++)
        {
            double x;

            x = server->resourceUtilization(i, n)/server->getTimeOfState(n);
            results.act_LOC_server_yt[i][n] = x;
            singleResults.write(TypeForClassAndServerState::Usage, x, i, n);

            x = timesPerClassAndState[i][n].occupancyTime /getOccupancyTimeOfState(n);
            results.act_SYS_yt[i][n] = x;
        }
    }

    for (int n=0; n<=V; n++)
    {
        double tmpStateDurationTime = getOccupancyTimeOfState(n);

        double occupancyTime =  getOccupancyTimeOfState(n) / results._simulationTime;
        results.act_trDistribSys[n] = occupancyTime;
        singleResults.write(TypeForSystemState::StateProbability, occupancyTime, n);

        results.act_intOutNew[n] = static_cast<double>(getOutNew(n)) / tmpStateDurationTime;
        results.act_intOutEnd[n] = static_cast<double>(getOutEnd(n)) / tmpStateDurationTime;

        results.act_noOutNew[n] = getOutNew(n);
        results.act_noOutEnd[n] = getOutEnd(n);


        for (int i=0; i<m; i++)
        {
            int t = this->systemData->getClass(i)->t();

            tmpStateDurationTime = (n-t >= 0) ? getOccupancyTimeOfState(n-t) : 0;
            results.act_intInNewSC[i][n] = (tmpStateDurationTime > 0) ? (static_cast<double>(getInNewSC(n, i)) / tmpStateDurationTime) : 0;

            tmpStateDurationTime = (n+t <= server->getV()) ? getOccupancyTimeOfState(n+t) : 0;
            results.act_intInEndSC[i][n] = (tmpStateDurationTime > 0) ? (static_cast<double>(getInEndSC(n, i)) / tmpStateDurationTime) : 0;

            tmpStateDurationTime = getOccupancyTimeOfState(n);
            results.act_intOutNewSC[i][n] = static_cast<double>(getOutNewSC(n, i)) / tmpStateDurationTime;
            results.act_intOutEndSC[i][n] = static_cast<double>(getOutEndSC(n, i)) / tmpStateDurationTime;

            results.act_noInNewSC[i][n]   = getInNewSC(n, i);
            results.act_noInEndSC[i][n]   = getInEndSC(n, i);
            results.act_noOutNewSC[i][n]  = getOutNewSC(n, i);
            results.act_noOutEndSC[i][n]  = getOutEndSC(n, i);
        }
    }

    for (int n=0; n<=Vs; n++)
    {
        double stateProbability = server->getTimeOfState(n) / results._simulationTime;
        results.act_trDistribSys[n] = stateProbability;
        singleResults.write(TypeForSystemState::StateProbability, stateProbability, n);

        results.act_intInNew[n]  = 0;
        results.act_intInEnd[n]  = 0;

        results.act_noInNew[n]  = 0;
        results.act_noInEnd[n]  = 0;
        for (int i=0; i<m; i++)
        {
            results.act_intInNew[n]  += results.act_intInNewSC[i][n];
            results.act_intInEnd[n]  += results.act_intInEndSC[i][n];

            results.act_noInNew[n]   += getInNewSC(n, i);
            results.act_noInEnd[n]   += getInEndSC(n, i);
        }
        results.act_trDistribServ[n] = server->statsGetOccupancyTimeOfState(n) / results._simulationTime;
    }
}

int SimulatorNoQeueLag::System::getServerNumberOfFreeAS()
{
    return server->getNoOfFreeAS();
}

bool SimulatorNoQeueLag::System::serveNewCall(SimulatorNoQeueLag::Call *newCall)
{
#ifndef DO_NOT_USE_SECUTIRY_CHECKS

#endif
    classIdx = newCall->classIdx;


    int groupNumber;
    QList<int> indexesToOccupy;
    bool isPlace = server->findAS(newCall->reqAS, groupNumber, indexesToOccupy);

    if (isPlace)
    {
        callsInSystem.append(newCall);

        newCall->allocatedAS    = newCall->reqAS;
        server->addCall(newCall, newCall->reqAS, groupNumber, indexesToOccupy, true);
        newCall->proc           = agenda->getNewProcess();
        newCall->proc->state    = ProcNoQeueLag::SENDING_DATA;
        newCall->proc->execute  = newCall->trEndedFun;
        newCall->proc->callData = newCall;
        newCall->proc->time     = newCall->plannedServiceTime;
        agenda->addProcess(newCall->proc);

        n += newCall->reqAS;
        n_i[static_cast<int>(newCall->classIdx)] += newCall->reqAS;

        return true;
    }
    else
    {
        FinishCall(newCall, false);
        return false;
    }
}

void SimulatorNoQeueLag::System::endTransmission(SimulatorNoQeueLag::Call *call)
{
    classIdx = call->classIdx;


    removeCallFromServer(call);
    callsInSystem.removeAll(call);


    n-=call->reqAS;
    FinishCall(call, true);
}

void SimulatorNoQeueLag::System::addProcess(ProcNoQeueLag *proc)
{
#ifndef DO_NOT_USE_SECUTIRY_CHECKS
    if (proc->time < 0)
        qFatal("Negative time value");
    if (proc->callData->classIdx > this->systemData->m())
        qFatal("Wrong class idx");
#endif
    agenda->addProcess(proc);
}

void SimulatorNoQeueLag::System::removeProcess(ProcNoQeueLag *proc)
{
#ifndef DO_NOT_USE_SECUTIRY_CHECKS
    if (proc->time < 0)
        qFatal("Negative time value");
#endif
    agenda->removeProcess(&proc);
}

SimulatorNoQeueLag::Call *SimulatorNoQeueLag::System::getNewCall(SimulatorNoQeueLag::Call *parent)
{
    Call *result = _getNewCall();

    result->fillData(parent);
    result->DUtransfered          = 0;
    result->timeOnSystem          = 0;
    result->timeOnServer          = 0;
    result->allocatedAS           = 0;

    result->complementaryCall = nullptr;

    return result;
}

SimulatorNoQeueLag::Call *SimulatorNoQeueLag::System::getNewCall(
          const ModelTrClass *trClass
        , int classIdx
        , double IncE
        )
{
    double IncD = IncE * IncE /  trClass->getIncommingExPerDx();
    double ServE = 1.0 / trClass->getMu();
    double ServD = ServE * ServE / trClass->getServiceExPerDx();

    double incDeltaT = sqrt(12*IncD);
    double inc_tMin = IncE-0.5*incDeltaT;
    double inc_tMax = IncE+0.5*incDeltaT;


    double serviceDeltaT = sqrt(12*ServD);
    double service_tMin = ServE-0.5*serviceDeltaT;
    double service_tMax = ServE+0.5*serviceDeltaT;


    Call *result = _getNewCall();

    result->reqAS = trClass->t();
    result->trClass = trClass;
    result->classIdx = classIdx;
    result->sourceE = IncE;
    result->sourceD = IncD;

    result->serviceE = ServE;
    result->serviceD = ServD;

    switch (trClass->callServStr())
    {
    case ModelTrClass::StreamType::Uniform:
        result->serviceTmin = service_tMin;
        result->serviceTmax = service_tMax;
        break;
    default:
        result->serviceTmin = 0;
        result->serviceTmax = -1;
        break;
    }

    switch (trClass->newCallStr())
    {
    case ModelTrClass::StreamType::Uniform:
        result->incommingTmin = inc_tMin;
        result->incommingTmax = inc_tMax;

        break;
    default:
        result->incommingTmin = 0;
        result->incommingTmax = -1;
        break;
    }


    result->DUtransfered          = 0;
    result->timeOnSystem          = 0;
    result->timeOnServer          = 0;
    result->allocatedAS           = 0;

    result->complementaryCall = nullptr;

    return result;
}

SimulatorNoQeueLag::Call *SimulatorNoQeueLag::System::_getNewCall()
{
    static int noOfCreatedCalls = 0;
    if(uselessCalls.length())
        return uselessCalls.pop();
    noOfCreatedCalls++;
    return new Call;
}

void SimulatorNoQeueLag::System::reuseCall(SimulatorNoQeueLag::Call *callToReuse)
{
    uselessCalls.push(callToReuse);
}

void SimulatorNoQeueLag::System::reuseProcess(ProcNoQeueLag *proc)
{
    proc->idx = 0;
    proc->setUseless();

    agenda->reuseProcess(proc);
}

void SimulatorNoQeueLag::System::removeCallFromServer(SimulatorNoQeueLag::Call *call)
{
    server->removeCall(call);
}

void SimulatorNoQeueLag::System::FinishCall(SimulatorNoQeueLag::Call *call, bool acceptedToService)
{
#ifndef DO_NOT_USE_SECUTIRY_CHECKS
    if (call->classIdx > 100)
        qFatal("Wrong class idx");
#endif
    results.act_noOfCalls[call->classIdx]++;

    if (!acceptedToService)
    {
        results.act_noOfLostCallsBeforeQeue[call->classIdx]++;
        totalNumberOfLostCalls++;
    }
    else
    {
        totalNumberOfServicedCalls++;
        results.act_tS[call->classIdx] += call->timeOnSystem;
        results.act_tServer[call->classIdx] += call->timeOnServer;
        results.act_tPlanedServer[call->classIdx] += call->plannedServiceTime;
        results.act_noOfServicedCalls[call->classIdx] ++;
    }
    reuseCall(call);
}

void SimulatorNoQeueLag::System::cancellScheduledCall(SimulatorNoQeueLag::Call *call)
{
    reuseCall(call);
}

int SimulatorNoQeueLag::System::getMaxNumberOfAsInSingleGroup()
{
    return server->getMaxNumberOfAsInSingleGroup();
}


#define FOLDINGSTART { //Statistics

void SimulatorNoQeueLag::System::statsCollectPre(double time)
{
    old_n = n;
    results._simulationTime +=time;

    foreach(Call *tmpCall, callsInSystem)
    {
        tmpCall->collectTheStats(time);
    }
    server->statsColectPre(time);

    for (int i=0; i<m; i++)
    {
        timesPerClassAndState[i][n].occupancyTime += time*(server->n_i[i]);
        server->resourceUtilizationByClassInState[i][n] += time*(server->n_i[i]);
    }
    timesPerState[n].occupancyTime += time;

    int maxCallRequirement = getMaxNumberOfAsInSingleGroup();
    timesPerState[V-maxCallRequirement].availabilityTime += time;
}

void SimulatorNoQeueLag::System::statsCollectPost(int classIdx)
{
    if (n == old_n) //nowe zgłoszenie zostało odrzucone
    {
        eventsPerClass[classIdx].outNewOffered++;
        eventsPerClass[classIdx].outNewLost++;

        eventsPerState[old_n].outNewOffered++;
        eventsPerState[old_n].outNewLost++;
        eventsPerClassAndState[classIdx][old_n].outNewOffered++;
        eventsPerClassAndState[classIdx][old_n].outNewLost++;

        return;
    }
    if (n < old_n) //zakończenie obsługi zgłoszenia
    {
        eventsPerClass[classIdx].outEnd++;
        eventsPerClass[classIdx].inEnd++;

        eventsPerState[old_n].outEnd++;
        eventsPerClassAndState[classIdx][old_n].outEnd++;

        eventsPerState[n].inEnd++;
        eventsPerClassAndState[classIdx][n].inEnd++;

        return;
    }

    if (n > old_n) //przyjęcie do obsługi zgłoszenia
    {
        eventsPerClass[classIdx].outNewOffered++;
        eventsPerClass[classIdx].outNew++;
        eventsPerClass[classIdx].inNew++;

        eventsPerState[old_n].outNewOffered++;
        eventsPerState[old_n].outNew++;

        eventsPerClassAndState[classIdx][old_n].outNewOffered++;
        eventsPerClassAndState[classIdx][old_n].outNew++;

        eventsPerState[n].inNew++;
        eventsPerClassAndState[classIdx][n].inNew++;

        return;
    }

    server->statsCollectPost(classIdx);
}

void SimulatorNoQeueLag::System::statsClear()
{
    for (int i=0; i<m; i++)
        for (int n=0; n<=V; n++)
            eventsPerClassAndState[i][n].statsClear();

    for (int n=0; n<=V; n++)
    {
        eventsPerState[n].statsClear();
        timesPerState[n].statsClear();
    }
    server->statsClear();
}

void SimulatorNoQeueLag::System::statsEnable(int serNo)
{
    statsClear();

    for (int i=0; i<m; i++)
    {
        eventsPerClass[i].statsClear();
        for (int n=0; n<=V; n++)
        {
            timesPerClassAndState[i][n].statsClear();
            eventsPerClassAndState[i][n].statsClear();
        }
    }
    for (int n=0; n<=V; n++)
    {
        timesPerState[n].statsClear();
        eventsPerState[n].statsClear();
    }

    results.enableStatisticscollection(serNo);
}

void SimulatorNoQeueLag::System::statsDisable()
{
    results.disableStatisticCollection();
}

void SimulatorNoQeueLag::Server::statsClear()
{
    for (int n=0; n<=V; n++)
        timePerState[n].statsClear();

    for (int groupNo=0; groupNo < k; groupNo++)
    {
        groups[groupNo]->statsClear();    
    }

    for (int combinatinNo=0; combinatinNo < (int)combinationList.length(); combinatinNo++)
    {
        freeAUsInWorstGroupInCombination[combinatinNo].fill(0, vMax+1);
        freeAUsInBestGroupInCombination[combinatinNo].fill(0, vMax+1);
        availabilityOnlyInAllGroupsInCombination[combinatinNo].fill(0, vMax+1);
        availabilityInAllGroupsInCombination[combinatinNo].fill(0, vMax+1);
        inavailabilityInAllGroupsInCombination[combinatinNo].fill(0, vMax+1);
    }

    for (int noOfGroups=0; noOfGroups <= k; noOfGroups++)
    {
        availabilityTimeInGroupSet[noOfGroups].fill(0, vMax+1);
        availabilityTimeOnlyInExactNoOfGroups[noOfGroups].fill(0, vMax+1);
    }
}

void SimulatorNoQeueLag::Server::statsColectPre(double time)
{
    for (int i=0; i<m; i++)
    {
        resourceUtilizationByClass[i]          += (time*n_i[i]);
        resourceUtilizationByClassInState[i][n]+= (time*n_i[i]);
    }
    timePerState[n].occupancyTime += time;

    int maxCallRequirement = getMaxNumberOfAsInSingleGroup();
    timePerState[V-maxCallRequirement].availabilityTime += time;

    foreach(Call *tmpCall, this->calls)
        tmpCall->IncrTimeInServer(time);

    for (int groupNo=0; groupNo < k; groupNo++)
        groups[groupNo]->statsColectPre(time);
    statsColectPreGroupsAvailability(time);
}

void SimulatorNoQeueLag::Server::statsColectPreGroupsAvailability(double time)
{
    // Uaktualnianie informacji o liczbie dostępnych zasobów w danej grupie
    tmpAvailabilityInGroups.resize(k);
    for (int tmpK=0; tmpK<k; tmpK++)
        tmpAvailabilityInGroups[tmpK] = groups[tmpK]->getNoOfFreeAUs(true);

    // Zapis statystyk dla kombinacji podgrup
    for (int combinatinNo=0; combinatinNo < (int)combinationList.length(); combinatinNo++)
    {
        int minAvailability = V;
        int maxAvailability = 0;
        int complementaryMaxAvailability = -1;
        foreach (int groupNo, combinationList[combinatinNo].first)
        {
            minAvailability = qMin<int>(minAvailability, tmpAvailabilityInGroups[groupNo]);
            maxAvailability = qMax<int>(maxAvailability, tmpAvailabilityInGroups[groupNo]);
        }
        foreach (int complGroupNo, combinationList[combinatinNo].second)
        {
            complementaryMaxAvailability
                    = qMax<int>(complementaryMaxAvailability, tmpAvailabilityInGroups[complGroupNo]);
        }

        freeAUsInWorstGroupInCombination[combinatinNo][minAvailability]+= time;
        freeAUsInBestGroupInCombination[combinatinNo][maxAvailability]+= time;

        for (int n=minAvailability; n>complementaryMaxAvailability; n--)
            availabilityOnlyInAllGroupsInCombination[combinatinNo][n]+= time;

        for (int n=minAvailability; n>=0; n--)
            availabilityInAllGroupsInCombination[combinatinNo][n]+= time;

        for (int n=maxAvailability+1; n<=vMax; n++)
            inavailabilityInAllGroupsInCombination[combinatinNo][n]+= time;
    }

    // Zapis statystyk dla określonej liczby dowolnych podgrup


    availabilityTimeInGroupSet[0][0] += time;

    for (int availableResourcess=0; availableResourcess <= vMax; availableResourcess++)
    {
        int noOfGroups = 0;

        for (int i=0; i<k; i++)
        {
            if (tmpAvailabilityInGroups[i] >= availableResourcess)
                noOfGroups++;
        }
        for (int i=0; i<=noOfGroups; i++)
            availabilityTimeInGroupSet[i][availableResourcess]+= time;
        availabilityTimeOnlyInExactNoOfGroups[noOfGroups][availableResourcess] += time;
    }
}

void SimulatorNoQeueLag::Server::statsCollectPost(int classIdx)
{
    for (int groupNo=0; groupNo < k; groupNo++)
        groups[groupNo]->statsCollectPost(classIdx);
}

double SimulatorNoQeueLag::Server::statsGetWorkoutPerClassAndState(int i, int n) const
{
    return resourceUtilizationByClassInState[i][n];
}

double SimulatorNoQeueLag::Server::statsGetOccupancyTimeOfState(int state) const
{
    return (state <= V) ? timePerState[state].occupancyTime : 0;
}

double SimulatorNoQeueLag::Server::statsGetFAGequivalentAvailabilityTimeOfState(int state) const
{
    return (state <= V) ? timePerState[state].availabilityTime : 0;
}

#define FOLDINGEND }


bool SimulatorNoQeueLag::Server::findAS(int noOfAUs, int& groupNo, QList<int>& asIndexes
        , GroupResourcessAllocationlgorithm groupResourcessAllocationlgorithm
                ) const
{
    bool result = false;
    int groupNoTmp;

    switch (subgroupScheduler)
    {
    case ServerResourcessScheduler::Random:
        Utils::UtilsMisc::suffle(groupSequence);
        foreach (groupNoTmp, groupSequence)
        {
            groupNo = groupNoTmp;
            if (groups[static_cast<int>(groupNo)]->findAS(noOfAUs, asIndexes, groupResourcessAllocationlgorithm) >= noOfAUs)
            {
                result = true;
                break;
            }
        }
        break;

    case ServerResourcessScheduler::Sequencial:
        foreach (groupNoTmp, groupSequence)
        {
            groupNo = groupNoTmp;
            if (groups[static_cast<int>(groupNo)]->findAS(noOfAUs, asIndexes, groupResourcessAllocationlgorithm) >= noOfAUs)
            {
                result = true;
                break;
            }
        }
        break;
    }
    if (result)
    {
        int noOfFreeAus = groups[static_cast<int>(groupNo)]->getNoOfFreeAUs();
        assert (noOfFreeAus >= noOfAUs);
    }
    return result;
}


void SimulatorNoQeueLag::Server::addCall(Call *call, int noOfAS, int groupNo, const QList<int>& asIndexes, bool newCall)
{
    call->groupIndex = groupNo;
    n += noOfAS;
    assert(V >= n);
    n_i[call->classIdx] += noOfAS;

    groups[call->groupIndex]->addCall(call, asIndexes);

#ifndef DO_NOT_USE_SECUTIRY_CHECKS
    if (call->reqAS < call->allocatedAS)
        qFatal("More AS is allocated then it is required");
    if (noOfAS > call->allocatedAS)
        qFatal("Wrong argument value");
#endif
    if (newCall)
    {
        calls.append(call);
#ifndef DO_NOT_USE_SECUTIRY_CHECKS
        if ((int)calls.length() > this->V)
            qFatal("To many calls on server's list");
#endif
    }
}

void SimulatorNoQeueLag::Server::removeCall(SimulatorNoQeueLag::Call *call)
{
    groups[call->groupIndex]->removeCall(call);
    calls.removeAll(call);
    n_i[call->classIdx] -=call->allocatedAS;
    n -=call->allocatedAS;
}

double SimulatorNoQeueLag::Server::resourceUtilization(int classNumber, int stateNo) const
{
    return resourceUtilizationByClassInState[classNumber][stateNo];
}

double SimulatorNoQeueLag::Server::getTimeOfState(int stateNo) const
{
    return timePerState[stateNo].occupancyTime;
}

SimulatorNoQeueLag::Server::Server(System *system)
    : system(system)
    , subgroupScheduler(system->systemData->getGroupsSchedulerAlgorithm())
    , V(system->systemData->vk_s())
    , vMax(system->systemData->v_sMax())
    , k(system->systemData->k_s())
    , m(system->systemData->m())
    , n(0)
{
    combinationList = Utils::UtilsLAG::getPossibleCombinations(k);
    freeAUsInWorstGroupInCombination.resize(combinationList.length());
    freeAUsInBestGroupInCombination.resize(combinationList.length());
    availabilityOnlyInAllGroupsInCombination.resize(combinationList.length());
    availabilityInAllGroupsInCombination.resize(combinationList.length());
    inavailabilityInAllGroupsInCombination.resize(combinationList.length());

    for (int combinationNo=0; combinationNo<combinationList.length(); combinationNo++)
    {
        freeAUsInWorstGroupInCombination[combinationNo].fill(0, vMax+1);
        freeAUsInBestGroupInCombination[combinationNo].fill(0, vMax+1);
        availabilityOnlyInAllGroupsInCombination[combinationNo].fill(0, vMax+1);
        availabilityInAllGroupsInCombination[combinationNo].fill(0, vMax+1);
        inavailabilityInAllGroupsInCombination[combinationNo].fill(0, vMax+1);
    }

    availabilityTimeInGroupSet.resize(k+1);
    availabilityTimeOnlyInExactNoOfGroups.resize(k+1);
    for (int noOfNotConsideredGroups=0; noOfNotConsideredGroups <= k; noOfNotConsideredGroups++)
    {
        availabilityTimeInGroupSet[noOfNotConsideredGroups].fill(0, vMax+1);
        availabilityTimeOnlyInExactNoOfGroups[noOfNotConsideredGroups].fill(0, vMax+1);
    }

    groups.resize(k);
    int groupNo = 0;
    for (int grType=0; grType<system->systemData->k_sType(); grType++)
    {
        for (int subGroupNo=0; subGroupNo< system->systemData->k_s(grType); subGroupNo++)
        {
            groups[groupNo] = QSharedDataPointer<Group>(new Group (system->systemData->v_s(grType), system->systemData->m()));
            groupNo++;
        }
    }

    n_i.resize(m);

    timePerState.resize(V+1);

    resourceUtilizationByClassInState.resize(m);
    resourceUtilizationByClass.resize(m);
    for (int classIdx=0; classIdx<m; classIdx++)
        resourceUtilizationByClassInState[classIdx].resize(V+1);

    groupSequence.resize(k);
    for (int j=0; j<k; j++)
        groupSequence[j] = j;
}

/*
simulatorNoQeueLag::Server::Server(const simulatorNoQeueLag::Server& rho)
    : V(rho.V)
    , k(rho.k)
    , m(rho.m)
    , n(rho.n)
{

}
*/
SimulatorNoQeueLag::Server::~Server()
{
    n = 0;
}

void SimulatorNoQeueLag::Server::writesResultsOfSingleExperiment(RSingle &singleResults, double simulationTime)
{
    for (int noOfgroups=0; noOfgroups<=k; noOfgroups++)
    {
        for (int n=0; n<=vMax; n++)
        {
            double result;
            result = availabilityTimeInGroupSet[noOfgroups][n]/simulationTime;
            singleResults.write(TypeResourcess_VsNumberOfServerGroups::AvailabilityInAllTheGroups, result, n, noOfgroups);

            result = availabilityTimeOnlyInExactNoOfGroups[noOfgroups][n]/simulationTime;
            singleResults.write(TypeResourcess_VsNumberOfServerGroups::AvailabilityOnlyInAllTheGroups, result, n, noOfgroups);
        }
        for (int i=0; i < m; i++)
        {
            double result = 0;

            int n = system->systemData->getClass(i)->t();

            result = availabilityTimeInGroupSet[noOfgroups][n]/simulationTime;
            singleResults.write(TypeClassForServerBestGroupsSet::ServPossibilityOnlyInAllTheSubgroups, result, i, noOfgroups);
        }
    }

    for (int combinationNo=0; combinationNo < (int) combinationList.length(); combinationNo++)
    {
        for (int n=0; n<=vMax; n++)
        {
            singleResults.write(TypeResourcess_VsServerGroupsCombination::FreeAUsInBestGroup
                                , freeAUsInBestGroupInCombination[combinationNo][n]/simulationTime
                                , n, combinationNo);

            singleResults.write(TypeResourcess_VsServerGroupsCombination::FreeAUsInEveryGroup
                                , freeAUsInWorstGroupInCombination[combinationNo][n]/simulationTime
                                , n, combinationNo);

            singleResults.write(TypeResourcess_VsServerGroupsCombination::AvailabilityOnlyInAllTheGroups
                                , availabilityOnlyInAllGroupsInCombination[combinationNo][n]/simulationTime
                                , n, combinationNo);

            singleResults.write(TypeResourcess_VsServerGroupsCombination::AvailabilityInAllTheGroups
                                , availabilityInAllGroupsInCombination[combinationNo][n]/simulationTime
                                , n, combinationNo);

            singleResults.write(TypeResourcess_VsServerGroupsCombination::InavailabilityInAllTheGroups
                                , inavailabilityInAllGroupsInCombination[combinationNo][n]/simulationTime
                                , n, combinationNo);
        }
    }
}

int SimulatorNoQeueLag::Server::getMaxNumberOfAsInSingleGroup()
{
    int result = 0;

    for (int groupNo=0; groupNo<k; groupNo++)
    {
        int tempAvailability = groups[groupNo]->getNoOfFreeAUs(true);
        result = (tempAvailability > result) ? tempAvailability : result;
    }
    return result;
}


void ProcNoQeueLag::initialize(
        SimulatorNoQeueLag::System *system
        , const ModelTrClass *trClass
        , int classIdx
        , double a
        , int sumPropAt
        , int V
        )
{
    ProcNoQeueLag *newEvent;
    int callNo = 0;

    switch(trClass->srcType())
    {
    case ModelTrClass::SourceType::Independent:
        newEvent = system->getNewProcess();
        switch (trClass->newCallStr())
        {
        case ModelTrClass::StreamType::Poisson:
            switch (trClass->callServStr())
            {
            case ModelTrClass::StreamType::Poisson:
                newEvent->initializeIndepMM(system, trClass, classIdx, a, sumPropAt, V);
                break;
            case ModelTrClass::StreamType::Uniform:
                newEvent->initializeIndepMU(system, trClass, classIdx, a, sumPropAt, V);
                break;
            case ModelTrClass::StreamType::Normal:
                newEvent->initializeIndepMN(system, trClass, classIdx, a, sumPropAt, V);
                break;
            case ModelTrClass::StreamType::Gamma:
                newEvent->initializeIndepMG(system, trClass, classIdx, a, sumPropAt, V);
                break;
            case ModelTrClass::StreamType::Pareto:
                newEvent->initializeIndepMP(system, trClass, classIdx, a, sumPropAt, V);
                break;
            }
            break;
        case ModelTrClass::StreamType::Uniform:
            switch (trClass->callServStr())
            {
            case ModelTrClass::StreamType::Poisson:
                newEvent->initializeIndepUM(system, trClass, classIdx, a, sumPropAt, V);
                break;
            case ModelTrClass::StreamType::Uniform:
                newEvent->initializeIndepUU(system, trClass, classIdx, a, sumPropAt, V);
                break;
            case ModelTrClass::StreamType::Normal:
                newEvent->initializeIndepUN(system, trClass, classIdx, a, sumPropAt, V);
                break;
            case ModelTrClass::StreamType::Gamma:
                newEvent->initializeIndepUG(system, trClass, classIdx, a, sumPropAt, V);
                break;
            case ModelTrClass::StreamType::Pareto:
                newEvent->initializeIndepUP(system, trClass, classIdx, a, sumPropAt, V);
                break;
            }
            break;

        case ModelTrClass::StreamType::Normal:
            switch (trClass->callServStr())
            {
            case ModelTrClass::StreamType::Poisson:
                newEvent->initializeIndepNM(system, trClass, classIdx, a, sumPropAt, V);
                break;
            case ModelTrClass::StreamType::Uniform:
                newEvent->initializeIndepNU(system, trClass, classIdx, a, sumPropAt, V);
                break;
            case ModelTrClass::StreamType::Normal:
                newEvent->initializeIndepNN(system, trClass, classIdx, a, sumPropAt, V);
                break;
            case ModelTrClass::StreamType::Gamma:
                newEvent->initializeIndepNG(system, trClass, classIdx, a, sumPropAt, V);
                break;
            case ModelTrClass::StreamType::Pareto:
                newEvent->initializeIndepNP(system, trClass, classIdx, a, sumPropAt, V);
                break;
            }
            break;

        case ModelTrClass::StreamType::Gamma:
            switch (trClass->callServStr())
            {
            case ModelTrClass::StreamType::Poisson:
                newEvent->initializeIndepGM(system, trClass, classIdx, a, sumPropAt, V);
                break;
            case ModelTrClass::StreamType::Uniform:
                newEvent->initializeIndepGU(system, trClass, classIdx, a, sumPropAt, V);
                break;
            case ModelTrClass::StreamType::Normal:
                newEvent->initializeIndepGN(system, trClass, classIdx, a, sumPropAt, V);
                break;
            case ModelTrClass::StreamType::Gamma:
                newEvent->initializeIndepGG(system, trClass, classIdx, a, sumPropAt, V);
                break;
            case ModelTrClass::StreamType::Pareto:
                newEvent->initializeIndepGP(system, trClass, classIdx, a, sumPropAt, V);
                break;
            }
            break;

        case ModelTrClass::StreamType::Pareto:
            switch (trClass->callServStr())
            {
            case ModelTrClass::StreamType::Poisson:
                newEvent->initializeIndepPM(system, trClass, classIdx, a, sumPropAt, V);
                break;
            case ModelTrClass::StreamType::Uniform:
                newEvent->initializeIndepPU(system, trClass, classIdx, a, sumPropAt, V);
                break;
            case ModelTrClass::StreamType::Normal:
                newEvent->initializeIndepPN(system, trClass, classIdx, a, sumPropAt, V);
                break;
            case ModelTrClass::StreamType::Gamma:
                newEvent->initializeIndepPG(system, trClass, classIdx, a, sumPropAt, V);
                break;
            case ModelTrClass::StreamType::Pareto:
                newEvent->initializeIndepPP(system, trClass, classIdx, a, sumPropAt, V);
                break;
            }
            break;
        }
        break;


    case ModelTrClass::SourceType::DependentMinus:
        for (callNo = 0; callNo<trClass->s(); callNo++)
        {
            newEvent = system->getNewProcess();
            switch(trClass->newCallStr())
            {
            case ModelTrClass::StreamType::Poisson:
                switch(trClass->callServStr())
                {
                case ModelTrClass::StreamType::Poisson:
                    newEvent->initializeDepMinusMM(system, trClass, classIdx, a, sumPropAt, V);
                    break;
                case ModelTrClass::StreamType::Uniform:
                    newEvent->initializeDepMinusMU(system, trClass, classIdx, a, sumPropAt, V);
                    break;
                case ModelTrClass::StreamType::Normal:
                    newEvent->initializeDepMinusMN(system, trClass, classIdx, a, sumPropAt, V);
                    break;
                case ModelTrClass::StreamType::Gamma:
                    newEvent->initializeDepMinusMG(system, trClass, classIdx, a, sumPropAt, V);
                    break;
                case ModelTrClass::StreamType::Pareto:
                    newEvent->initializeDepMinusMP(system, trClass, classIdx, a, sumPropAt, V);
                    break;
                }
                break;

            case ModelTrClass::StreamType::Uniform:
                switch(trClass->callServStr())
                {
                case ModelTrClass::StreamType::Poisson:
                    newEvent->initializeDepMinusUM(system, trClass, classIdx, a, sumPropAt, V);
                    break;
                case ModelTrClass::StreamType::Uniform:
                    newEvent->initializeDepMinusUU(system, trClass, classIdx, a, sumPropAt, V);
                    break;
                case ModelTrClass::StreamType::Normal:
                    newEvent->initializeDepMinusUN(system, trClass, classIdx, a, sumPropAt, V);
                    break;
                case ModelTrClass::StreamType::Gamma:
                    newEvent->initializeDepMinusUG(system, trClass, classIdx, a, sumPropAt, V);
                    break;
                case ModelTrClass::StreamType::Pareto:
                    newEvent->initializeDepMinusUP(system, trClass, classIdx, a, sumPropAt, V);
                    break;
                }
                break;

            case ModelTrClass::StreamType::Normal:
                switch(trClass->callServStr())
                {
                case ModelTrClass::StreamType::Poisson:
                    newEvent->initializeDepMinusNM(system, trClass, classIdx, a, sumPropAt, V);
                    break;
                case ModelTrClass::StreamType::Uniform:
                    newEvent->initializeDepMinusNU(system, trClass, classIdx, a, sumPropAt, V);
                    break;
                case ModelTrClass::StreamType::Normal:
                    newEvent->initializeDepMinusNN(system, trClass, classIdx, a, sumPropAt, V);
                    break;
                case ModelTrClass::StreamType::Gamma:
                    newEvent->initializeDepMinusNG(system, trClass, classIdx, a, sumPropAt, V);
                    break;
                case ModelTrClass::StreamType::Pareto:
                    newEvent->initializeDepMinusNP(system, trClass, classIdx, a, sumPropAt, V);
                    break;
                }
                break;

            case ModelTrClass::StreamType::Gamma:
                switch(trClass->callServStr())
                {
                case ModelTrClass::StreamType::Poisson:
                    newEvent->initializeDepMinusGM(system, trClass, classIdx, a, sumPropAt, V);
                    break;
                case ModelTrClass::StreamType::Uniform:
                    newEvent->initializeDepMinusGU(system, trClass, classIdx, a, sumPropAt, V);
                    break;
                case ModelTrClass::StreamType::Normal:
                    newEvent->initializeDepMinusGN(system, trClass, classIdx, a, sumPropAt, V);
                    break;
                case ModelTrClass::StreamType::Gamma:
                    newEvent->initializeDepMinusGG(system, trClass, classIdx, a, sumPropAt, V);
                    break;
                case ModelTrClass::StreamType::Pareto:
                    newEvent->initializeDepMinusGP(system, trClass, classIdx, a, sumPropAt, V);
                    break;
                }
                break;

            case ModelTrClass::StreamType::Pareto:
                switch(trClass->callServStr())
                {
                case ModelTrClass::StreamType::Poisson:
                    newEvent->initializeDepMinusPM(system, trClass, classIdx, a, sumPropAt, V);
                    break;
                case ModelTrClass::StreamType::Uniform:
                    newEvent->initializeDepMinusPU(system, trClass, classIdx, a, sumPropAt, V);
                    break;
                case ModelTrClass::StreamType::Normal:
                    newEvent->initializeDepMinusPN(system, trClass, classIdx, a, sumPropAt, V);
                    break;
                case ModelTrClass::StreamType::Gamma:
                    newEvent->initializeDepMinusPG(system, trClass, classIdx, a, sumPropAt, V);
                    break;
                case ModelTrClass::StreamType::Pareto:
                    newEvent->initializeDepMinusPP(system, trClass, classIdx, a, sumPropAt, V);
                    break;
                }
                break;
            }
        }
        break;

    case ModelTrClass::SourceType::DependentPlus:
        for (callNo = 0; callNo<trClass->s(); callNo++)
        {
            newEvent = system->getNewProcess();
            switch(trClass->newCallStr())
            {
            case ModelTrClass::StreamType::Poisson:
                switch(trClass->callServStr())
                {
                case ModelTrClass::StreamType::Poisson:
                    newEvent->initializeDepPlusMM(system, trClass, classIdx, a, sumPropAt, V);
                    break;
                case ModelTrClass::StreamType::Uniform:
                    newEvent->initializeDepPlusMU(system, trClass, classIdx, a, sumPropAt, V);
                    break;
                case ModelTrClass::StreamType::Normal:
                    newEvent->initializeDepPlusMN(system, trClass, classIdx, a, sumPropAt, V);
                    break;
                case ModelTrClass::StreamType::Gamma:
                    newEvent->initializeDepPlusMG(system, trClass, classIdx, a, sumPropAt, V);
                    break;
                case ModelTrClass::StreamType::Pareto:
                    newEvent->initializeDepPlusMP(system, trClass, classIdx, a, sumPropAt, V);
                    break;
                }
                break;

            case ModelTrClass::StreamType::Uniform:
                switch(trClass->callServStr())
                {
                case ModelTrClass::StreamType::Poisson:
                    newEvent->initializeDepPlusUM(system, trClass, classIdx, a, sumPropAt, V);
                    break;
                case ModelTrClass::StreamType::Uniform:
                    newEvent->initializeDepPlusUU(system, trClass, classIdx, a, sumPropAt, V);
                    break;
                case ModelTrClass::StreamType::Normal:
                    newEvent->initializeDepPlusUN(system, trClass, classIdx, a, sumPropAt, V);
                    break;
                case ModelTrClass::StreamType::Gamma:
                    newEvent->initializeDepPlusUG(system, trClass, classIdx, a, sumPropAt, V);
                    break;
                case ModelTrClass::StreamType::Pareto:
                    newEvent->initializeDepPlusUP(system, trClass, classIdx, a, sumPropAt, V);
                    break;
                }
                break;

            case ModelTrClass::StreamType::Normal:
                switch(trClass->callServStr())
                {
                case ModelTrClass::StreamType::Poisson:
                    newEvent->initializeDepPlusNM(system, trClass, classIdx, a, sumPropAt, V);
                    break;
                case ModelTrClass::StreamType::Uniform:
                    newEvent->initializeDepPlusNU(system, trClass, classIdx, a, sumPropAt, V);
                    break;
                case ModelTrClass::StreamType::Normal:
                    newEvent->initializeDepPlusNN(system, trClass, classIdx, a, sumPropAt, V);
                    break;
                case ModelTrClass::StreamType::Gamma:
                    newEvent->initializeDepPlusNG(system, trClass, classIdx, a, sumPropAt, V);
                    break;
                case ModelTrClass::StreamType::Pareto:
                    newEvent->initializeDepPlusNP(system, trClass, classIdx, a, sumPropAt, V);
                    break;
                }
                break;

            case ModelTrClass::StreamType::Gamma:
                switch(trClass->callServStr())
                {
                case ModelTrClass::StreamType::Poisson:
                    newEvent->initializeDepPlusGM(system, trClass, classIdx, a, sumPropAt, V);
                    break;
                case ModelTrClass::StreamType::Uniform:
                    newEvent->initializeDepPlusGU(system, trClass, classIdx, a, sumPropAt, V);
                    break;
                case ModelTrClass::StreamType::Normal:
                    newEvent->initializeDepPlusGN(system, trClass, classIdx, a, sumPropAt, V);
                    break;
                case ModelTrClass::StreamType::Gamma:
                    newEvent->initializeDepPlusGG(system, trClass, classIdx, a, sumPropAt, V);
                    break;
                case ModelTrClass::StreamType::Pareto:
                    newEvent->initializeDepPlusGP(system, trClass, classIdx, a, sumPropAt, V);
                    break;
                }
                break;

            case ModelTrClass::StreamType::Pareto:
                switch(trClass->callServStr())
                {
                case ModelTrClass::StreamType::Poisson:
                    newEvent->initializeDepPlusPM(system, trClass, classIdx, a, sumPropAt, V);
                    break;
                case ModelTrClass::StreamType::Uniform:
                    newEvent->initializeDepPlusPU(system, trClass, classIdx, a, sumPropAt, V);
                    break;
                case ModelTrClass::StreamType::Normal:
                    newEvent->initializeDepPlusPN(system, trClass, classIdx, a, sumPropAt, V);
                    break;
                case ModelTrClass::StreamType::Gamma:
                    newEvent->initializeDepPlusPG(system, trClass, classIdx, a, sumPropAt, V);
                    break;
                case ModelTrClass::StreamType::Pareto:
                    newEvent->initializeDepPlusPP(system, trClass, classIdx, a, sumPropAt, V);
                    break;
                }
                break;
            }
        }
        break;
    }
}

void ProcNoQeueLag::initializeIndependent(SimulatorNoQeueLag::System *system
        , const ModelTrClass *trClass
        , int classIdx, double a
        , int sumPropAt
        , int V
        , double (*funTimeNewCall)(double, double)
        , void (*funNewCall)(ProcNoQeueLag *, SimulatorNoQeueLag::System *))
{
    double IncE = 1.0 / trClass->intensityNewCallTotal(a, V, sumPropAt);
    callData = system->getNewCall(trClass, classIdx, IncE);
    callData->trEndedFun = ProcNoQeueLag::transmisionEndedIndependent;

    this->state = ProcNoQeueLag::WAITING_FOR_NEW_CALL;
    this->execute = funNewCall;
    this->time = funTimeNewCall(this->callData->sourceE, this->callData->sourceD);
    system->addProcess(this);
}

void ProcNoQeueLag::ProcNoQeueLag::initializeDependent(SimulatorNoQeueLag::System *system
        , const ModelTrClass *trClass, int classIdx
        , double a
        , int sumPropAt
        , int V
        , double (*funTimeNewCall)(double, double)
        , void (*funNewCall)(ProcNoQeueLag *, SimulatorNoQeueLag::System *)
        , void (*funEndCall)(ProcNoQeueLag *, SimulatorNoQeueLag::System *))
{
    double A = a*V*trClass->propAt()/sumPropAt / trClass->t();

    double sign = 1;
    switch (trClass->srcType())
    {
    case ModelTrClass::SourceType::DependentMinus:
        sign = -1;
        break;
    case ModelTrClass::SourceType::DependentPlus:
        sign = 1;
        break;
    default:
        sign = 0;
        break;
    }

    double IncE = (double)(trClass->s() + sign * A) / trClass->intensityNewCallTotal(a, V, sumPropAt);
    callData = system->getNewCall(trClass, classIdx, IncE);
    callData->trEndedFun = funEndCall;

    this->state = ProcNoQeueLag::WAITING_FOR_NEW_CALL;
    this->execute = funNewCall;
    this->time = funTimeNewCall(this->callData->sourceE, this->callData->sourceD);
    system->addProcess(this);
}

void ProcNoQeueLag::newCallIndep(
          ProcNoQeueLag *proc
        , SimulatorNoQeueLag::System *system
        , double (*funTimeNewCall)(double, double)
        , double (*funTimeOfService)(double, double)
        , void (*funNewCall)(ProcNoQeueLag *proc, SimulatorNoQeueLag::System *system))
{
#ifndef DO_NOT_USE_SECUTIRY_CHECKS
    if (proc->callData->classIdx > system->systemData->m())
        qFatal("Wrong class idx");
#endif
    SimulatorNoQeueLag::Call *callData = proc->callData;
    callData->plannedServiceTime       = funTimeOfService(callData->serviceE, callData->serviceD);
    callData->reqAS                    = proc->callData->reqAS;
    callData->DUmessageSize            = callData->reqAS * callData->plannedServiceTime;
    callData->proc                     = nullptr;

    system->serveNewCall(callData);

    //Adding new process with state Waiting for new call
    ProcNoQeueLag *newProc = proc;
    newProc->callData = system->getNewCall(proc->callData);

    newProc->state = ProcNoQeueLag::WAITING_FOR_NEW_CALL;
    newProc->execute = funNewCall;
    newProc->time = funTimeNewCall(newProc->callData->sourceE, newProc->callData->sourceD);
    system->addProcess(newProc);
}

void ProcNoQeueLag::newCallDepMinus(
          ProcNoQeueLag *proc
        , SimulatorNoQeueLag::System *system
        , double (*funTimeNewCall)(double, double)
        , double (*funTimeOfService)(double, double)
        , void (*funNewCall)(ProcNoQeueLag *, SimulatorNoQeueLag::System *))
{
#ifndef DO_NOT_USE_SECUTIRY_CHECKS
    if (proc->callData->classIdx > system->systemData->m())
        qFatal("Wrong class idx");
#endif
    SimulatorNoQeueLag::Call *callData = proc->callData;
    callData->plannedServiceTime       = funTimeOfService(callData->serviceE, callData->serviceD);
    callData->reqAS                    = proc->callData->reqAS;
    callData->DUmessageSize            = callData->reqAS * callData->plannedServiceTime;
    callData->proc                     = nullptr;


    if (system->serveNewCall(callData) == false)
    {
        ProcNoQeueLag *newProc = system->getNewProcess();
        newProc->callData = system->getNewCall(callData);

    #ifndef DO_NOT_USE_SECUTIRY_CHECKS
        if (newProc->callData->classIdx > system->systemData->m())
            qFatal("Wrong class idx");
    #endif
        newProc->state = ProcNoQeueLag::WAITING_FOR_NEW_CALL;
        newProc->execute = funNewCall;
        newProc->time = funTimeNewCall(newProc->callData->sourceE, newProc->callData->sourceD);
        system->addProcess(newProc);
    }

    system->reuseProcess(proc);
}

void ProcNoQeueLag::newCallDepPlus(
          ProcNoQeueLag *proc
        , SimulatorNoQeueLag::System *system
        , double (*funTimeNewCall)(double, double)
        , double (*funTimeOfService)(double, double)
        , void (*funNewCall)(ProcNoQeueLag *, SimulatorNoQeueLag::System *))
{
#ifndef DO_NOT_USE_SECUTIRY_CHECKS
    if (proc->callData->classIdx > system->systemData->m())
        qFatal("Wrong class idx");
#endif
    SimulatorNoQeueLag::Call *callData           = proc->callData;
    SimulatorNoQeueLag::Call *parentServicedCall = callData->complementaryCall;

    callData->plannedServiceTime = funTimeOfService(callData->serviceE, callData->serviceD);
    callData->reqAS              = proc->callData->reqAS;
    callData->DUmessageSize      = callData->reqAS * callData->plannedServiceTime;
    callData->proc               = nullptr;

    ProcNoQeueLag *procNewCall = system->getNewProcess();
    procNewCall->callData = system->getNewCall(callData);
    procNewCall->callData->proc = procNewCall;

    if (parentServicedCall)
    {
        procNewCall->callData->complementaryCall = parentServicedCall;
        parentServicedCall->complementaryCall = procNewCall->callData;
        callData->complementaryCall = nullptr;
    }

    procNewCall->state = ProcNoQeueLag::WAITING_FOR_NEW_CALL;
    procNewCall->execute = funNewCall;
    procNewCall->time = funTimeNewCall(procNewCall->callData->sourceE, procNewCall->callData->sourceD);
    system->addProcess(procNewCall);

    if (system->serveNewCall(callData) == true)
    {
        ProcNoQeueLag *newProc = system->getNewProcess();
        newProc->callData = system->getNewCall(callData);
        newProc->callData->proc = newProc;
        callData->complementaryCall = newProc->callData;
        newProc->callData->complementaryCall = callData;

        newProc->state = ProcNoQeueLag::WAITING_FOR_NEW_CALL;
        newProc->execute = funNewCall;
        newProc->time = funTimeNewCall(newProc->callData->sourceE, newProc->callData->sourceD);
        system->addProcess(newProc);
    }
    system->reuseProcess(proc);
}

void ProcNoQeueLag::transmisionEndedIndependent(ProcNoQeueLag *proc, SimulatorNoQeueLag::System *system)
{
#ifndef DO_NOT_USE_SECUTIRY_CHECKS
    if (proc->callData->classIdx > system->systemData->m())
        qFatal("Wrong class idx");
#endif
    system->endTransmission(proc->callData);

    proc->callData = NULL;
    system->reuseProcess(proc);
}

void ProcNoQeueLag::transmisionEndedDependentMinus(
          ProcNoQeueLag *proc
        , SimulatorNoQeueLag::System *system
        , double (*funTimeNewCall)(double, double)
        , void (*funNewCall)(ProcNoQeueLag *, SimulatorNoQeueLag::System *))
{
    ProcNoQeueLag *newProc = system->getNewProcess();
    newProc->callData = system->getNewCall(proc->callData);

#ifndef DO_NOT_USE_SECUTIRY_CHECKS
    if (proc->callData->classIdx > system->systemData->m())
        qFatal("Wrong class idx");
#endif
    system->endTransmission(proc->callData);

    proc->callData = NULL;
    system->reuseProcess(proc);

    //Adding new process with state Waiting for new call
    newProc->state = ProcNoQeueLag::WAITING_FOR_NEW_CALL;
    newProc->execute = funNewCall;
    newProc->time = funTimeNewCall(newProc->callData->sourceE, newProc->callData->sourceD);
    system->addProcess(newProc);
}

void ProcNoQeueLag::transmisionEndedDependentPlus(ProcNoQeueLag *proc, SimulatorNoQeueLag::System *system)
{
    SimulatorNoQeueLag::Call *scheduledCall = proc->callData->complementaryCall;

    if (scheduledCall->complementaryCall != proc->callData)
        qFatal("Wrong relations between Pascal sourcess");

    if (proc->state == PROC_STATE::USELESS)\
        qFatal("Wrong Process state");
    system->endTransmission(proc->callData);

    system->removeProcess(scheduledCall->proc);
    system->cancellScheduledCall(scheduledCall);

    proc->callData = NULL;
    system->reuseProcess(proc);
}


void SimulatorNoQeueLag::Call::fillData(SimulatorNoQeueLag::Call *src)
{
    reqAS = src->reqAS;
    trClass = src->trClass;
    classIdx = src->classIdx;
    sourceE = src->sourceE;
    sourceD = src->sourceD;
    incommingTmin = src->incommingTmin;
    incommingTmax = src->incommingTmax;
    serviceE= src->serviceE;
    serviceD= src->serviceD;
    serviceTmin = src->serviceTmin;
    serviceTmax = src->serviceTmax;
    trEndedFun = src->trEndedFun;
}

void SimulatorNoQeueLag::Call::IncrTimeInServer(double time)
{
    timeOnServer +=time;
}

void SimulatorNoQeueLag::Call::collectTheStats(double time)
{
    timeOnSystem +=time;
    DUtransfered += (time * allocatedAS);
}

#define FOLDINGSTART { // Allocation Unit
SimulatorNoQeueLag::AU::AU(int m)
    : m(m)
    , servicedCall(nullptr)
{
    occupancyTimesPerClass.resize(m);
}

bool SimulatorNoQeueLag::AU::isFree() const
{
    return (servicedCall == nullptr);
}

void SimulatorNoQeueLag::AU::increaseOccupancyTime(double time)
{
    if (servicedCall != nullptr)
        occupancyTimesPerClass[servicedCall->classIdx] += time;
}

void SimulatorNoQeueLag::AU::addCall(Call *newCall)
{
#ifndef DO_NOT_USE_SECUTIRY_CHECKS
    if (servicedCall != nullptr)
        qFatal("Can't add a call of class with index %d if there is a call of class with index %d in service",newCall->classIdx, newCall->classIdx);
#endif
    servicedCall = newCall;
}

bool SimulatorNoQeueLag::AU::isThisCallHere(Call *comparedCall)
{
    return (servicedCall == comparedCall);
}

void SimulatorNoQeueLag::AU::removeCall()
{
    servicedCall = nullptr;
}

#define FOLDINGSTART { //Statistics
void SimulatorNoQeueLag::AU::statsClear()
{
    occupancyTime = 0;

    for (int i=0; i<m; i++)
        occupancyTimesPerClass[i] = 0;
}

void SimulatorNoQeueLag::AU::statsColectPre(double time)
{
    if (this->servicedCall)
    {
        occupancyTimesPerClass[servicedCall->classIdx] += time;
        this->occupancyTime += time;
    }
}

void SimulatorNoQeueLag::AU::statsCollectPost(int classIdx)
{
    (void) classIdx;
}

#define FOLDINEND }

#define FOLDINEND }

#define FOLDINGSTART { // Group
SimulatorNoQeueLag::Group::Group(int v, int m): m(m), v(v), n(0)
{
    allocationUnits.resize(v);
    for (int n=0; n<v; n++)
        allocationUnits[n] = new AU(m);

    timePerState.resize(v+1);

    workoutPerClassAndState.resize(m);
    n_i.resize(m);
    for (int i=0; i<m; i++)
    {
        workoutPerClassAndState[i].resize(v+1);
    }
}

SimulatorNoQeueLag::Group::Group(const SimulatorNoQeueLag::Group &rho)
    : QSharedData(rho)
    , m(rho.m)
    , v(rho.v)
    , n(rho.n)
{
    allocationUnits = rho.allocationUnits;
    for (int n=0; n<v; n++)
        allocationUnits[n] = new AU(m);

    timePerState = rho.timePerState;
    workoutPerClassAndState = rho.workoutPerClassAndState;
}

int SimulatorNoQeueLag::Group::findAS(int noOfAUs
                                       , QList<int> &ausToOccupy
                                       , GroupResourcessAllocationlgorithm groupResourcessAllocationlgorithm
                                       ) const
{
    ausToOccupy.clear();
    int result = 0;
    int nx = 0;

    switch (groupResourcessAllocationlgorithm)
    {
    case GroupResourcessAllocationlgorithm::NonContinuous:
        result = v-n;
        if (result >= noOfAUs)
        {
            for (nx=0, result=0; nx<v && result < noOfAUs; nx++)
                if (allocationUnits[nx]->isFree())
                {
                    ausToOccupy.append(nx);
                    result++;
                }
        }
        break;
    default:
        qFatal("Not implemented");
        break;
    }
    return result;
}

int SimulatorNoQeueLag::Group::findMaxAS(SimulatorNoQeueLag::GroupResourcessAllocationlgorithm groupResourcessAllocationlgorithm) const
{
    int result = 0;

    switch (groupResourcessAllocationlgorithm)
    {
    case GroupResourcessAllocationlgorithm::NonContinuous:
        result = v-n;
        break;
    default:
        qFatal("Not implemented");
        break;
    }

    return result;
}

void SimulatorNoQeueLag::Group::removeCall(SimulatorNoQeueLag::Call *endedCall)
{
    n-=endedCall->reqAS;

    for (int idx=0; idx<v; idx++)
        if (allocationUnits[idx]->isThisCallHere(endedCall))
            allocationUnits[idx]->removeCall();
}

void SimulatorNoQeueLag::Group::addCall(SimulatorNoQeueLag::Call *newCall, const QList<int>& asIndexes)
{
    n+=newCall->reqAS;
    foreach (int index, asIndexes)
    {
       allocationUnits[index]->addCall(newCall);
    }
}

void SimulatorNoQeueLag::Group::statsClear()
{
    for (int resNo=0; resNo < v; resNo++)
    {
        allocationUnits[resNo]->statsClear();
    }

    for (int n=0; n<=v; n++)
    {
        timePerState[n] = 0;
        for (int i=0; i<m; i++)
            workoutPerClassAndState[i][n] = 0;
    }
}

void SimulatorNoQeueLag::Group::statsColectPre(double time)
{
    for (int resNo=0; resNo < v; resNo++)
        allocationUnits[resNo]->statsColectPre(time);

    timePerState[n] += time;
    for (int i=0; i<m; i++)
        workoutPerClassAndState[i][n] += (time *  n_i[i]);
}

void SimulatorNoQeueLag::Group::statsCollectPost(int classIdx)
{
    for (int resNo=0; resNo < v; resNo++)
        allocationUnits[resNo]->statsCollectPost(classIdx);
}

int SimulatorNoQeueLag::Group::getNoOfFreeAUs(bool considerAllocationAlgorithm)
{
    return (!considerAllocationAlgorithm) ? v-n : findMaxAS();
}

#define FOLDINEND }

SimulatorNoQeueLag::EvenStatistics::EvenStatistics()
    : inNew(0)
    , inEnd(0)
    , outNew(0)
    , outNewOffered(0)
    , outNewLost(0)
    , outEnd(0)
{ }

void SimulatorNoQeueLag::EvenStatistics::statsClear()
{
    memset(this, 0, sizeof(class EvenStatistics));
}

SimulatorNoQeueLag::TimeStatisticsState::TimeStatisticsState()
    : availabilityTime(0)
    , occupancyTime(0)
{ }

void SimulatorNoQeueLag::TimeStatisticsState::statsClear()
{
    availabilityTime = 0;
    occupancyTime = 0;
}

} // namespace Algorithms
