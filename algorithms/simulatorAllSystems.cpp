#include "simulatorAllSystems.h"
#include <qstring.h>
#include <qglobal.h>
#include <utils/lag.h>
#include "utils/utilsMisc.h"
#include <qmath.h>


namespace Algorithms
{

SimulatorAll::Engine::Engine(SimulatorAll::System *system): system(system)
{
    system->engine = this;
    agenda = new SimulatorDataCollection<ProcAll>();
}

SimulatorAll::Engine::~Engine()
{
    while (uselessCalls.length())
    {
        Call *tmp = uselessCalls.pop();
        delete tmp;
    }
    delete agenda;
}

SimulatorAll::Call *SimulatorAll::Engine::_getNewCall()
{
    static int noOfCreatedCalls = 0;
    if(uselessCalls.length())
        return uselessCalls.pop();
    noOfCreatedCalls++;
    return new Call;
}

void SimulatorAll::Engine::prepareCallToService(SimulatorAll::Call *callThatIsInService)
{
    callThatIsInService->proc           = agenda->getNewProcess();
    callThatIsInService->proc->state    = ProcAll::SENDING_DATA;
    callThatIsInService->proc->execute  = callThatIsInService->trEndedFun;
    callThatIsInService->proc->callData = callThatIsInService;
    callThatIsInService->proc->time     = callThatIsInService->plannedServiceTime;
    agenda->addProcess(callThatIsInService->proc);

}

void SimulatorAll::Engine::reuseProcess(ProcAll *proc)
{
    proc->idx = 0;
    proc->setUseless();

    agenda->reuseProcess(proc);
}

void SimulatorAll::Engine::addProcess(ProcAll *proc)
{
#ifndef DO_NOT_USE_SECUTIRY_CHECKS
    if (proc->time < 0)
        qFatal("Negative time value");
    if (proc->callData->classIdx > system->systemData->m())
        qFatal("Wrong class idx");
#endif
    agenda->addProcess(proc);
}

void SimulatorAll::Engine::removeProcess(ProcAll *proc)
{
#ifndef DO_NOT_USE_SECUTIRY_CHECKS
    if (proc->time < 0)
        qFatal("Negative time value");
#endif
    agenda->removeProcess(&proc);
}

SimulatorAll::Call *SimulatorAll::Engine::getNewCall(SimulatorAll::Call *parent)
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

SimulatorAll::Call *SimulatorAll::Engine::getNewCall(
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

SimulatorAll::SimulatorAll() : simulator()
{
    myQoS_Set << Results::Type::AllSugbrupsInGivenCombinationAndClassAvailable;
    myQoS_Set << Results::Type::AvailableSubroupDistribution;

    system = nullptr;
}

bool SimulatorAll::possible(const ModelSyst *system) const
{
    return simulator::possible(system);
}

void SimulatorAll::calculateSystem(const ModelSyst *system
      , double a
      , RInvestigator *results
      , SimulationParameters *simParameters)
{
    prepareTemporaryData(system, a);


    System *simData = new System(system, simParameters->noOfSeries);

    Engine *engine = new Engine(simData);

    engine->initialize(a, system->totalAt(), system->vk_s());

    unsigned int seed = 1024;

    int noOfSeries = simParameters->noOfSeries;

    for (int serNo=0; serNo<noOfSeries; serNo++)
    {
        seed = static_cast<unsigned int>(qrand());
        simData->statsDisable();
        if (serNo == 0)
        {
            int noOfLostCalls = simParameters->noOfLostCalls/simParameters->spaceOnStart;
            int noOfServCalls = simParameters->noOfServicedCalls/simParameters->spaceOnStart;
            engine->doSimExperiment(noOfLostCalls, seed, noOfServCalls);
        }
        else
        {
            int noOfLostCalls = simParameters->noOfLostCalls/simParameters->spaceBetweenSeries;
            int noOfServCalls = simParameters->noOfServicedCalls/simParameters->spaceBetweenSeries;
            engine->doSimExperiment(noOfLostCalls, seed, noOfServCalls);
        }
        simData->statsEnable(serNo);
        engine->doSimExperiment(simParameters->noOfLostCalls, seed, simParameters->noOfServicedCalls);
        simData->writesResultsOfSingleExperiment((*results)[serNo]);
        qDebug("universal simulation experiment no %d", serNo+1);
    }
    delete simData;
    delete engine;
    //emit this->sigCalculationDone();
}

SimulatorAll::System::System(const ModelSyst *system, int noOfSeries)
    : m(system->m())
    , vk_sb(system->V())
    , vk_s(system->vk_s())
    , vk_b(system->vk_b())
    , results(system->m(), system->vk_s(), system->vk_b(), noOfSeries)
    , n(0)
    , old_n(0)
{
    n_i.resize(m);

    systemData = system;


    server = new Server(this);
    buffer = new Buffer(this);


    statistics = new SystemStatistics(system);
}

SimulatorAll::System::~System()
{
    foreach(Call *tmp, calls)
        delete tmp;

    delete server;
}

void SimulatorAll::Engine::initialize(double a, int sumPropAt, int V)
{
    for(int i=0; i<system->systemData->m(); i++)
    {
        const ModelTrClass *tmpClass = system->systemData->getClass(i);
        ProcAll::initialize(this, tmpClass, i, a, sumPropAt, V);
    }
}

void SimulatorAll::Engine::doSimExperiment(int numberOfLostCall, unsigned int seed, int numberOfServicedCalls)
{
    int classIdx;
#define DO_SIM_EXP \
    ProcAll *proc = takeFirstProcess();\
    system->statsCollectPre(proc->time);\
    classIdx = proc->callData->classIdx;\
    proc->execute(proc, system);\
    system->statsCollectPost(classIdx);\
#undef DO_SIM_EXP
    qsrand(seed);
    totalNumberOfLostCalls = 0;
    totalNumberOfServicedCalls = 0;

    if (numberOfServicedCalls == 0)
    {
        while(totalNumberOfLostCalls < numberOfLostCall)
        {
            ProcAll *proc = takeFirstProcess();
            system->statsCollectPre(proc->time);
            classIdx = proc->callData->classIdx;
            proc->execute(proc, system);
            system->statsCollectPost(classIdx);
        }
    }
    else
    {
        if(totalNumberOfLostCalls > 0)
        {
            while(totalNumberOfLostCalls < numberOfLostCall && totalNumberOfServicedCalls < numberOfServicedCalls)
            {
                ProcAll *proc = takeFirstProcess();
                system->statsCollectPre(proc->time);
                classIdx = proc->callData->classIdx;
                proc->execute(proc, system);
                system->statsCollectPost(classIdx);
            }
        }
        else
        {
            while(totalNumberOfServicedCalls < numberOfServicedCalls)
            {
                ProcAll *proc = takeFirstProcess();
                system->statsCollectPre(proc->time);
                classIdx = proc->callData->classIdx;
                proc->execute(proc, system);
                system->statsCollectPost(classIdx);
            }
        }
    }
}

void SimulatorAll::System::writesResultsOfSingleExperiment(RSingle& singleResults)
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

        for (int n = qMax(0, (V-t+1)); n<=V; n++)
            E+=statistics->getTimeStatistics(n).occupancyTime;
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

            x = statistics->getTimeStatisticsSC(i,n).occupancyUtilization /statistics->getTimeStatistics(n).occupancyTime;
            results.act_SYS_yt[i][n] = x;
        }
    }

    for (int n=0; n<=V; n++)
    {
        double tmpstateDurationTime = statistics->getTimeStatistics(n).occupancyTime;

        double occupancyTime =  statistics->getTimeStatistics(n).occupancyTime / results._simulationTime;
        results.act_trDistribSys[n] = occupancyTime;
        singleResults.write(TypeForSystemState::StateProbability, occupancyTime, n);

        results.act_intOutNew[n] = static_cast<double>(statistics->getEventStatistics(n).outNewOffered) / tmpstateDurationTime;
        results.act_intOutEnd[n] = static_cast<double>(statistics->getEventStatistics(n).outEnd) / tmpstateDurationTime;

        results.act_noOutNew[n] = statistics->getEventStatistics(n).outNewAccepted;
        results.act_noOutEnd[n] = statistics->getEventStatistics(n).outEnd;


        for (int i=0; i<m; i++)
        {
            int t = this->systemData->getClass(i)->t();

            tmpstateDurationTime = (n-t >= 0) ? statistics->getTimeStatistics(n-t).occupancyTime : 0;
            results.act_intInNewSC[i][n] = (tmpstateDurationTime > 0) ? (static_cast<double>(statistics->getEventStatisticsSC(i, n).inNew) / tmpstateDurationTime) : 0;

            tmpstateDurationTime = (n+t <= server->getV()) ? statistics->getTimeStatistics(n+t).occupancyTime : 0;
            results.act_intInEndSC[i][n] = (tmpstateDurationTime > 0) ? (static_cast<double>(statistics->getEventStatisticsSC(i, n).inEnd) / tmpstateDurationTime) : 0;

            tmpstateDurationTime =  statistics->getTimeStatistics(n).occupancyTime;
            results.act_intOutNewSC[i][n] = static_cast<double>(statistics->getEventStatisticsSC(i, n).outNewOffered) / tmpstateDurationTime;
            results.act_intOutEndSC[i][n] = static_cast<double>(statistics->getEventStatisticsSC(i, n).outEnd) / tmpstateDurationTime;

            results.act_noInNewSC[i][n]   = statistics->getEventStatisticsSC(i, n).inNew;
            results.act_noInEndSC[i][n]   = statistics->getEventStatisticsSC(i, n).inEnd;
            results.act_noOutNewSC[i][n]  = statistics->getEventStatisticsSC(i, n).outNewAccepted;
            results.act_noOutEndSC[i][n]  = statistics->getEventStatisticsSC(i, n).outEnd;
        }
    }

    for (int n=0; n<=Vs; n++)
    {//TODO use servers statistics
        double stateProbability = server->getTimeOfState(n) / results._simulationTime;
        results.act_trDistribSys[n] = stateProbability;
        singleResults.write(TypeForSystemState::StateProbability, stateProbability, n);

        results.act_intInNew[n] = 0;
        results.act_intInEnd[n] = 0;

        results.act_noInNew[n]  = 0;
        results.act_noInEnd[n]  = 0;
        for (int i=0; i<m; i++)
        {
            results.act_intInNew[n]  += results.act_intInNewSC[i][n];
            results.act_intInEnd[n]  += results.act_intInEndSC[i][n];
        //    results.act_noInNew[n]   += getInNewSC(n, i);
        //TODO use servers statistics   results.act_noInEnd[n]   += getInEndSC(n, i);
        }
        results.act_trDistribServ[n] = server->statsGetOccupancyTimeOfState(n) / results._simulationTime;
    }
}



bool SimulatorAll::System::serveNewCall(SimulatorAll::Call *newCall)
{
#ifndef DO_NOT_USE_SECUTIRY_CHECKS

#endif
    int groupNumber;
     bool isPlace = server->findAS(newCall->reqAS, groupNumber);

    if (isPlace)
    {
        engine->prepareCallToService(newCall);

        calls.append(newCall);

        newCall->allocatedAS    = newCall->reqAS;
        server->addCall(newCall, newCall->reqAS, groupNumber, true);

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

void SimulatorAll::System::endCallService(SimulatorAll::Call *call)
{
    removeCallFromServer(call);
    calls.removeAll(call);

    n-=call->reqAS;
    FinishCall(call, true);
}

void SimulatorAll::System::removeCallFromServer(SimulatorAll::Call *call)
{
    server->removeCall(call);
}

void SimulatorAll::System::removeCallFromBuffer(SimulatorAll::Call *call)
{
    buffer->removeCall(call);
}

void SimulatorAll::System::FinishCall(SimulatorAll::Call *call, bool acceptedToService)
{
#ifndef DO_NOT_USE_SECUTIRY_CHECKS
    if (call->classIdx > 100)
        qFatal("Wrong class idx");
#endif
    results.act_noOfCalls[call->classIdx]++;

    if (!acceptedToService)
    {
        engine->notifyLostCall();
        results.act_noOfLostCallsBeforeQeue[call->classIdx]++;
    }
    else
    {
        engine->notifyServicedCall();
        results.act_tS[call->classIdx] += call->timeOnSystem;
        results.act_tServer[call->classIdx] += call->timeOnServer;
        results.act_tPlanedServer[call->classIdx] += call->plannedServiceTime;
        results.act_noOfServicedCalls[call->classIdx] ++;
    }
    engine->reuseCall(call);
}



#define FOLDINGSTART { //Statistics

void SimulatorAll::System::statsCollectPre(double time)
{
    old_n = n;
    results._simulationTime +=time;

    foreach(Call *tmpCall, calls)
    {
        tmpCall->collectTheStats(time);
    }

    int n_s = server->get_n();
    int n_b = buffer->get_n();
    const QVector<int> &nMs_s = server->getMicroStates();
    const QVector<int> &nMs_b = buffer->getMicroStates();

    statistics->collectPre(time, n_s, n_b, nMs_s, nMs_b);
    server->statsColectPre(time);
    buffer->statsColectPre(time);
}

void SimulatorAll::System::statsCollectPost(int classIdx)
{
    statistics->collectPost(classIdx, old_n, n);
    server->statsCollectPost(classIdx, old_n, n);
}

void SimulatorAll::System::statsClear()
{
    statistics->clear();
    server->statsClear();
    buffer->statsClear();
}

void SimulatorAll::System::statsEnable(int serNo)
{
    statsClear();
    results.enableStatisticscollection(serNo);
}

void SimulatorAll::System::statsDisable()
{
    results.disableStatisticCollection();
}

void SimulatorAll::Server::statsClear()
{
    statistics->clear();
}

void SimulatorAll::Server::statsColectPre(double time)
{
    statistics->collectPre(time, n, n_i);

    statsColectPreGroupsAvailability(time);
}

void SimulatorAll::Server::statsColectPreGroupsAvailability(double time)
{

}

void SimulatorAll::Server::statsCollectPost(int classIdx, int old_n, int n)
{
    statistics->collectPost(classIdx, old_n, n);
}

double SimulatorAll::Server::statsGetWorkoutPerClassAndState(int i, int n) const
{
    return statistics->getTimeStatisticsSC(i, n).occupancyUtilization;
}

double SimulatorAll::Server::statsGetOccupancyTimeOfState(int state) const
{
    return (state <= vTotal) ? statistics->getTimeStatistics(state).occupancyTime : 0;
}

#define FOLDINGEND }


bool SimulatorAll::Server::findAS(int noOfAUs, int& groupNo) const
{
    bool result = false;
    int groupNoTmp;

    switch (scheduler)
    {
    case ServerResourcessScheduler::Random:
        Utils::UtilsMisc::suffle(subgroupSequence);
        foreach (groupNoTmp, subgroupSequence)
        {
            groupNo = groupNoTmp;
            if (subgroupFreeAUs[groupNoTmp] >= noOfAUs)
            {
                result = true;
                break;
            }
        }
        break;

    case ServerResourcessScheduler::Sequencial:
        foreach (groupNoTmp, subgroupSequence)
        {
            groupNo = groupNoTmp;
            if (subgroupFreeAUs[groupNoTmp] >= noOfAUs)
            {
                result = true;
                break;
            }
        }
        break;
    }
    return result;
}


void SimulatorAll::Server::addCall(Call *call, int noOfAS, int groupNo, bool newCall)
{
    call->groupIndex = groupNo;
    n += noOfAS;
    assert(vTotal >= n);
    n_i[call->classIdx] += noOfAS;
    n_k[groupNo] += noOfAS;
    subgroupFreeAUs[groupNo] -=noOfAS;


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
        if (calls.length() > this->vTotal)
            qFatal("To many calls on server's list");
#endif
    }
}

void SimulatorAll::Server::removeCall(SimulatorAll::Call *call)
{
    this->subgroupFreeAUs[call->groupIndex] +=call->allocatedAS;
    calls.removeAll(call);
    n_i[call->classIdx] -=call->allocatedAS;
    n -=call->allocatedAS;
}

double SimulatorAll::Server::resourceUtilization(int classNumber, int stateNo) const
{
    return statistics->getTimeStatisticsSC(classNumber, stateNo).occupancyUtilization;
}

double SimulatorAll::Server::getTimeOfState(int stateNo) const
{
    return statistics->getTimeStatistics(stateNo).occupancyTime;
}

SimulatorAll::Server::Server(System *system)
  : system(system)
  , scheduler(system->systemData->getGroupsSchedulerAlgorithm())
  , vTotal(system->systemData->vk_s())
  , vMax(system->systemData->v_sMax())
  , k(system->systemData->k_s())
  , m(system->systemData->m())
  , n(0)
{   
    n_k.resize(k);
    n_i.resize(m);

    subgroupFreeAUs.resize(k);
    subgroupSequence.resize(k);
    for (int j=0; j<k; j++)
    {
        subgroupFreeAUs[j] = system->systemData->v_s(j);
        subgroupSequence[j] = j;
    }

    statistics = new ServerStatistics(system->systemData);
}

SimulatorAll::Server::~Server()
{
    n = 0;
}

void SimulatorAll::Server::writesResultsOfSingleExperiment(RSingle &singleResults, double simulationTime)
{
    for (int noOfgroups=0; noOfgroups<=k; noOfgroups++)
    {
        for (int n=0; n<=vMax; n++)
        {
            double result;
            result = statistics->availabilityTimeInGroupSet[noOfgroups][n]/simulationTime;
            singleResults.write(TypeStateForServerGroupsSet::AvailabilityOnlyInAllTheGroups, result, n, noOfgroups);

            result = statistics->availabilityTimeOnlyInExactNoOfGroups[noOfgroups][n]/simulationTime;
            singleResults.write(TypeStateForServerGroupsSet::AvailabilityOnlyInAllTheGroups, result, n, noOfgroups);
        }
        for (int i=0; i < m; i++)
        {
            double result = 0;

            int n = system->systemData->getClass(i)->t();

            result = statistics->availabilityTimeInGroupSet[noOfgroups][n]/simulationTime;
            singleResults.write(TypeClassForServerBestGroupsSet::ServPossibilityOnlyInAllTheSubgroups, result, i, noOfgroups);

            result = statistics->availabilityTimeOnlyInExactNoOfGroups[noOfgroups][n]/simulationTime;
            singleResults.write(TypeClassForServerExactGroupsSet::ServPossibilityOnlyInAllTheSubgroups, result, i, noOfgroups);
        }
    }

    for (int combinationNo=0; combinationNo < statistics->getNoOfSets(); combinationNo++)
    {
        for (int n=0; n<=vMax; n++)
        {
            singleResults.write(TypeStateForServerGroupsCombination::FreeAUsInBestGroup
                                , statistics->getTimeGroupSet(combinationNo, n).atLeastOneInetAvailable /simulationTime
                                , n, combinationNo);

            singleResults.write(TypeStateForServerGroupsCombination::FreeAUsInEveryGroup
                                , statistics->freeAUsInWorstGroupInCombination[combinationNo][n]/simulationTime
                                , n, combinationNo);



            singleResults.write(TypeStateForServerGroupsCombination::AvailabilityOnlyInAllTheGroups
                                , statistics->getTimeGroupSet(combinationNo, n).allInSetAvailableAllOutsideSetUnavailable /simulationTime
                                , n, combinationNo);

            singleResults.write(TypeStateForServerGroupsCombination::AvailabilityInAllTheGroups
                                , statistics->availabilityInAllGroupsInCombination[combinationNo][n]/simulationTime
                                , n, combinationNo);

            singleResults.write(TypeStateForServerGroupsCombination::InavailabilityInAllTheGroups
                                , statistics->inavailabilityInAllGroupsInCombination[combinationNo][n]/simulationTime
                                , n, combinationNo);


        }

        for (int classNo=0; classNo<m; classNo++)
        {
            int t = this->system->systemData->getClass(classNo)->t();
            singleResults.write(TypeClassForServerGroupsCombination::SerPossibilityOnlyInAllTheSubgroups, statistics->getTimeBestGroupSetSC(combinationNo, classNo).allInSetAvailableAllOutsideSetUnavailable/simulationTime, classNo, combinationNo);
            singleResults.write(TypeClassForServerGroupsCombination::SerPossibilityInAllTheSubgroups,     statistics->availabilityInAllGroupsInCombination[combinationNo][t]/simulationTime, classNo, combinationNo);
            singleResults.write(TypeClassForServerGroupsCombination::SerImpossibilityInAllTheSubgroups,   statistics->inavailabilityInAllGroupsInCombination[combinationNo][t]/simulationTime, classNo, combinationNo);
        }
    }
}

int SimulatorAll::Server::getMaxNumberOfAsInSingleGroup()
{
    int result = 0;

    for (int groupNo=0; groupNo<k; groupNo++)
    {
        int tempAvailability = subgroupFreeAUs[groupNo];
        result = (tempAvailability > result) ? tempAvailability : result;
    }
    return result;
}


void ProcAll::initialize(
    SimulatorAll::Engine *engine
  , const ModelTrClass *trClass
  , int classIdx
  , double a
  , int sumPropAt
  , int V
)
{
    ProcAll *newEvent;
    int callNo = 0;

    switch(trClass->srcType())
    {
    case ModelTrClass::SourceType::Independent:
        newEvent = engine->getNewProcess();
        switch (trClass->newCallStr())
        {
        case ModelTrClass::StreamType::Poisson:
            switch (trClass->callServStr())
            {
            case ModelTrClass::StreamType::Poisson:
                newEvent->initializeIndepMM(engine, trClass, classIdx, a, sumPropAt, V);
                break;
            case ModelTrClass::StreamType::Uniform:
                newEvent->initializeIndepMU(engine, trClass, classIdx, a, sumPropAt, V);
                break;
            case ModelTrClass::StreamType::Normal:
                newEvent->initializeIndepMN(engine, trClass, classIdx, a, sumPropAt, V);
                break;
            case ModelTrClass::StreamType::Gamma:
                newEvent->initializeIndepMG(engine, trClass, classIdx, a, sumPropAt, V);
                break;
            case ModelTrClass::StreamType::Pareto:
                newEvent->initializeIndepMP(engine, trClass, classIdx, a, sumPropAt, V);
                break;
            }
            break;
        case ModelTrClass::StreamType::Uniform:
            switch (trClass->callServStr())
            {
            case ModelTrClass::StreamType::Poisson:
                newEvent->initializeIndepUM(engine, trClass, classIdx, a, sumPropAt, V);
                break;
            case ModelTrClass::StreamType::Uniform:
                newEvent->initializeIndepUU(engine, trClass, classIdx, a, sumPropAt, V);
                break;
            case ModelTrClass::StreamType::Normal:
                newEvent->initializeIndepUN(engine, trClass, classIdx, a, sumPropAt, V);
                break;
            case ModelTrClass::StreamType::Gamma:
                newEvent->initializeIndepUG(engine, trClass, classIdx, a, sumPropAt, V);
                break;
            case ModelTrClass::StreamType::Pareto:
                newEvent->initializeIndepUP(engine, trClass, classIdx, a, sumPropAt, V);
                break;
            }
            break;

        case ModelTrClass::StreamType::Normal:
            switch (trClass->callServStr())
            {
            case ModelTrClass::StreamType::Poisson:
                newEvent->initializeIndepNM(engine, trClass, classIdx, a, sumPropAt, V);
                break;
            case ModelTrClass::StreamType::Uniform:
                newEvent->initializeIndepNU(engine, trClass, classIdx, a, sumPropAt, V);
                break;
            case ModelTrClass::StreamType::Normal:
                newEvent->initializeIndepNN(engine, trClass, classIdx, a, sumPropAt, V);
                break;
            case ModelTrClass::StreamType::Gamma:
                newEvent->initializeIndepNG(engine, trClass, classIdx, a, sumPropAt, V);
                break;
            case ModelTrClass::StreamType::Pareto:
                newEvent->initializeIndepNP(engine, trClass, classIdx, a, sumPropAt, V);
                break;
            }
            break;

        case ModelTrClass::StreamType::Gamma:
            switch (trClass->callServStr())
            {
            case ModelTrClass::StreamType::Poisson:
                newEvent->initializeIndepGM(engine, trClass, classIdx, a, sumPropAt, V);
                break;
            case ModelTrClass::StreamType::Uniform:
                newEvent->initializeIndepGU(engine, trClass, classIdx, a, sumPropAt, V);
                break;
            case ModelTrClass::StreamType::Normal:
                newEvent->initializeIndepGN(engine, trClass, classIdx, a, sumPropAt, V);
                break;
            case ModelTrClass::StreamType::Gamma:
                newEvent->initializeIndepGG(engine, trClass, classIdx, a, sumPropAt, V);
                break;
            case ModelTrClass::StreamType::Pareto:
                newEvent->initializeIndepGP(engine, trClass, classIdx, a, sumPropAt, V);
                break;
            }
            break;

        case ModelTrClass::StreamType::Pareto:
            switch (trClass->callServStr())
            {
            case ModelTrClass::StreamType::Poisson:
                newEvent->initializeIndepPM(engine, trClass, classIdx, a, sumPropAt, V);
                break;
            case ModelTrClass::StreamType::Uniform:
                newEvent->initializeIndepPU(engine, trClass, classIdx, a, sumPropAt, V);
                break;
            case ModelTrClass::StreamType::Normal:
                newEvent->initializeIndepPN(engine, trClass, classIdx, a, sumPropAt, V);
                break;
            case ModelTrClass::StreamType::Gamma:
                newEvent->initializeIndepPG(engine, trClass, classIdx, a, sumPropAt, V);
                break;
            case ModelTrClass::StreamType::Pareto:
                newEvent->initializeIndepPP(engine, trClass, classIdx, a, sumPropAt, V);
                break;
            }
            break;
        }
        break;


    case ModelTrClass::SourceType::DependentMinus:
        for (callNo = 0; callNo<trClass->s(); callNo++)
        {
            newEvent = engine->getNewProcess();
            switch(trClass->newCallStr())
            {
            case ModelTrClass::StreamType::Poisson:
                switch(trClass->callServStr())
                {
                case ModelTrClass::StreamType::Poisson:
                    newEvent->initializeDepMinusMM(engine, trClass, classIdx, a, sumPropAt, V);
                    break;
                case ModelTrClass::StreamType::Uniform:
                    newEvent->initializeDepMinusMU(engine, trClass, classIdx, a, sumPropAt, V);
                    break;
                case ModelTrClass::StreamType::Normal:
                    newEvent->initializeDepMinusMN(engine, trClass, classIdx, a, sumPropAt, V);
                    break;
                case ModelTrClass::StreamType::Gamma:
                    newEvent->initializeDepMinusMG(engine, trClass, classIdx, a, sumPropAt, V);
                    break;
                case ModelTrClass::StreamType::Pareto:
                    newEvent->initializeDepMinusMP(engine, trClass, classIdx, a, sumPropAt, V);
                    break;
                }
                break;

            case ModelTrClass::StreamType::Uniform:
                switch(trClass->callServStr())
                {
                case ModelTrClass::StreamType::Poisson:
                    newEvent->initializeDepMinusUM(engine, trClass, classIdx, a, sumPropAt, V);
                    break;
                case ModelTrClass::StreamType::Uniform:
                    newEvent->initializeDepMinusUU(engine, trClass, classIdx, a, sumPropAt, V);
                    break;
                case ModelTrClass::StreamType::Normal:
                    newEvent->initializeDepMinusUN(engine, trClass, classIdx, a, sumPropAt, V);
                    break;
                case ModelTrClass::StreamType::Gamma:
                    newEvent->initializeDepMinusUG(engine, trClass, classIdx, a, sumPropAt, V);
                    break;
                case ModelTrClass::StreamType::Pareto:
                    newEvent->initializeDepMinusUP(engine, trClass, classIdx, a, sumPropAt, V);
                    break;
                }
                break;

            case ModelTrClass::StreamType::Normal:
                switch(trClass->callServStr())
                {
                case ModelTrClass::StreamType::Poisson:
                    newEvent->initializeDepMinusNM(engine, trClass, classIdx, a, sumPropAt, V);
                    break;
                case ModelTrClass::StreamType::Uniform:
                    newEvent->initializeDepMinusNU(engine, trClass, classIdx, a, sumPropAt, V);
                    break;
                case ModelTrClass::StreamType::Normal:
                    newEvent->initializeDepMinusNN(engine, trClass, classIdx, a, sumPropAt, V);
                    break;
                case ModelTrClass::StreamType::Gamma:
                    newEvent->initializeDepMinusNG(engine, trClass, classIdx, a, sumPropAt, V);
                    break;
                case ModelTrClass::StreamType::Pareto:
                    newEvent->initializeDepMinusNP(engine, trClass, classIdx, a, sumPropAt, V);
                    break;
                }
                break;

            case ModelTrClass::StreamType::Gamma:
                switch(trClass->callServStr())
                {
                case ModelTrClass::StreamType::Poisson:
                    newEvent->initializeDepMinusGM(engine, trClass, classIdx, a, sumPropAt, V);
                    break;
                case ModelTrClass::StreamType::Uniform:
                    newEvent->initializeDepMinusGU(engine, trClass, classIdx, a, sumPropAt, V);
                    break;
                case ModelTrClass::StreamType::Normal:
                    newEvent->initializeDepMinusGN(engine, trClass, classIdx, a, sumPropAt, V);
                    break;
                case ModelTrClass::StreamType::Gamma:
                    newEvent->initializeDepMinusGG(engine, trClass, classIdx, a, sumPropAt, V);
                    break;
                case ModelTrClass::StreamType::Pareto:
                    newEvent->initializeDepMinusGP(engine, trClass, classIdx, a, sumPropAt, V);
                    break;
                }
                break;

            case ModelTrClass::StreamType::Pareto:
                switch(trClass->callServStr())
                {
                case ModelTrClass::StreamType::Poisson:
                    newEvent->initializeDepMinusPM(engine, trClass, classIdx, a, sumPropAt, V);
                    break;
                case ModelTrClass::StreamType::Uniform:
                    newEvent->initializeDepMinusPU(engine, trClass, classIdx, a, sumPropAt, V);
                    break;
                case ModelTrClass::StreamType::Normal:
                    newEvent->initializeDepMinusPN(engine, trClass, classIdx, a, sumPropAt, V);
                    break;
                case ModelTrClass::StreamType::Gamma:
                    newEvent->initializeDepMinusPG(engine, trClass, classIdx, a, sumPropAt, V);
                    break;
                case ModelTrClass::StreamType::Pareto:
                    newEvent->initializeDepMinusPP(engine, trClass, classIdx, a, sumPropAt, V);
                    break;
                }
                break;
            }
        }
        break;

    case ModelTrClass::SourceType::DependentPlus:
        for (callNo = 0; callNo<trClass->s(); callNo++)
        {
            newEvent = engine->getNewProcess();
            switch(trClass->newCallStr())
            {
            case ModelTrClass::StreamType::Poisson:
                switch(trClass->callServStr())
                {
                case ModelTrClass::StreamType::Poisson:
                    newEvent->initializeDepPlusMM(engine, trClass, classIdx, a, sumPropAt, V);
                    break;
                case ModelTrClass::StreamType::Uniform:
                    newEvent->initializeDepPlusMU(engine, trClass, classIdx, a, sumPropAt, V);
                    break;
                case ModelTrClass::StreamType::Normal:
                    newEvent->initializeDepPlusMN(engine, trClass, classIdx, a, sumPropAt, V);
                    break;
                case ModelTrClass::StreamType::Gamma:
                    newEvent->initializeDepPlusMG(engine, trClass, classIdx, a, sumPropAt, V);
                    break;
                case ModelTrClass::StreamType::Pareto:
                    newEvent->initializeDepPlusMP(engine, trClass, classIdx, a, sumPropAt, V);
                    break;
                }
                break;

            case ModelTrClass::StreamType::Uniform:
                switch(trClass->callServStr())
                {
                case ModelTrClass::StreamType::Poisson:
                    newEvent->initializeDepPlusUM(engine, trClass, classIdx, a, sumPropAt, V);
                    break;
                case ModelTrClass::StreamType::Uniform:
                    newEvent->initializeDepPlusUU(engine, trClass, classIdx, a, sumPropAt, V);
                    break;
                case ModelTrClass::StreamType::Normal:
                    newEvent->initializeDepPlusUN(engine, trClass, classIdx, a, sumPropAt, V);
                    break;
                case ModelTrClass::StreamType::Gamma:
                    newEvent->initializeDepPlusUG(engine, trClass, classIdx, a, sumPropAt, V);
                    break;
                case ModelTrClass::StreamType::Pareto:
                    newEvent->initializeDepPlusUP(engine, trClass, classIdx, a, sumPropAt, V);
                    break;
                }
                break;

            case ModelTrClass::StreamType::Normal:
                switch(trClass->callServStr())
                {
                case ModelTrClass::StreamType::Poisson:
                    newEvent->initializeDepPlusNM(engine, trClass, classIdx, a, sumPropAt, V);
                    break;
                case ModelTrClass::StreamType::Uniform:
                    newEvent->initializeDepPlusNU(engine, trClass, classIdx, a, sumPropAt, V);
                    break;
                case ModelTrClass::StreamType::Normal:
                    newEvent->initializeDepPlusNN(engine, trClass, classIdx, a, sumPropAt, V);
                    break;
                case ModelTrClass::StreamType::Gamma:
                    newEvent->initializeDepPlusNG(engine, trClass, classIdx, a, sumPropAt, V);
                    break;
                case ModelTrClass::StreamType::Pareto:
                    newEvent->initializeDepPlusNP(engine, trClass, classIdx, a, sumPropAt, V);
                    break;
                }
                break;

            case ModelTrClass::StreamType::Gamma:
                switch(trClass->callServStr())
                {
                case ModelTrClass::StreamType::Poisson:
                    newEvent->initializeDepPlusGM(engine, trClass, classIdx, a, sumPropAt, V);
                    break;
                case ModelTrClass::StreamType::Uniform:
                    newEvent->initializeDepPlusGU(engine, trClass, classIdx, a, sumPropAt, V);
                    break;
                case ModelTrClass::StreamType::Normal:
                    newEvent->initializeDepPlusGN(engine, trClass, classIdx, a, sumPropAt, V);
                    break;
                case ModelTrClass::StreamType::Gamma:
                    newEvent->initializeDepPlusGG(engine, trClass, classIdx, a, sumPropAt, V);
                    break;
                case ModelTrClass::StreamType::Pareto:
                    newEvent->initializeDepPlusGP(engine, trClass, classIdx, a, sumPropAt, V);
                    break;
                }
                break;

            case ModelTrClass::StreamType::Pareto:
                switch(trClass->callServStr())
                {
                case ModelTrClass::StreamType::Poisson:
                    newEvent->initializeDepPlusPM(engine, trClass, classIdx, a, sumPropAt, V);
                    break;
                case ModelTrClass::StreamType::Uniform:
                    newEvent->initializeDepPlusPU(engine, trClass, classIdx, a, sumPropAt, V);
                    break;
                case ModelTrClass::StreamType::Normal:
                    newEvent->initializeDepPlusPN(engine, trClass, classIdx, a, sumPropAt, V);
                    break;
                case ModelTrClass::StreamType::Gamma:
                    newEvent->initializeDepPlusPG(engine, trClass, classIdx, a, sumPropAt, V);
                    break;
                case ModelTrClass::StreamType::Pareto:
                    newEvent->initializeDepPlusPP(engine, trClass, classIdx, a, sumPropAt, V);
                    break;
                }
                break;
            }
        }
        break;
    }
}

void ProcAll::initializeIndependent(
    SimulatorAll::Engine *system
  , const ModelTrClass *trClass
  , int classIdx, double a
  , int sumPropAt
  , int V
  , double (*funTimeNewCall)(double, double)
  , void (*funNewCall)(ProcAll *, SimulatorAll::System *)
)
{
    double IncE = 1.0 / trClass->intensityNewCallTotal(a, static_cast<size_t>(V), sumPropAt);
    callData = system->getNewCall(trClass, classIdx, IncE);
    callData->trEndedFun = ProcAll::callServiceEndedIndependent;

    this->state = ProcAll::WAITING_FOR_NEW_CALL;
    this->execute = funNewCall;
    this->time = funTimeNewCall(this->callData->sourceE, this->callData->sourceD);
    system->addProcess(this);
}

void ProcAll::ProcAll::initializeDependent(
    SimulatorAll::Engine *engine
  , const ModelTrClass *trClass, int classIdx
  , double a
  , int sumPropAt
  , int V
  , double (*funTimeNewCall)(double, double)
  , void (*funNewCall)(ProcAll *, SimulatorAll::System *)
  , void (*funEndCall)(ProcAll *, SimulatorAll::System *)
)
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

    double IncE = (trClass->s() + sign * A) / trClass->intensityNewCallTotal(a, static_cast<size_t>(V), sumPropAt);
    callData = engine->getNewCall(trClass, classIdx, IncE);
    callData->trEndedFun = funEndCall;

    this->state = ProcAll::WAITING_FOR_NEW_CALL;
    this->execute = funNewCall;
    this->time = funTimeNewCall(this->callData->sourceE, this->callData->sourceD);
    engine->addProcess(this);
}

void ProcAll::newCallIndep(
    ProcAll *proc
  , SimulatorAll::System *system
  , double (*funTimeNewCall)(double, double)
  , double (*funTimeOfService)(double, double)
  , void (*funNewCall)(ProcAll *proc, SimulatorAll::System *system)
)
{
#ifndef DO_NOT_USE_SECUTIRY_CHECKS
    if (proc->callData->classIdx > system->systemData->m())
        qFatal("Wrong class idx");
#endif
    SimulatorAll::Call *callData = proc->callData;
    callData->plannedServiceTime       = funTimeOfService(callData->serviceE, callData->serviceD);
    callData->reqAS                    = proc->callData->reqAS;
    callData->DUmessageSize            = callData->reqAS * callData->plannedServiceTime;
    callData->proc                     = nullptr;

    system->serveNewCall(callData);

    //Adding new process with state Waiting for new call
    ProcAll *newProc = proc;
    newProc->callData = system->engine->getNewCall(proc->callData);

    newProc->state = ProcAll::WAITING_FOR_NEW_CALL;
    newProc->execute = funNewCall;
    newProc->time = funTimeNewCall(newProc->callData->sourceE, newProc->callData->sourceD);
    system->engine->addProcess(newProc);
}

void ProcAll::newCallDepMinus(
    ProcAll *proc
  , SimulatorAll::System *system
  , double (*funTimeNewCall)(double, double)
  , double (*funTimeOfService)(double, double)
  , void (*funNewCall)(ProcAll *, SimulatorAll::System *)
)
{
#ifndef DO_NOT_USE_SECUTIRY_CHECKS
    if (proc->callData->classIdx > system->systemData->m())
        qFatal("Wrong class idx");
#endif
    SimulatorAll::Call *callData = proc->callData;
    callData->plannedServiceTime       = funTimeOfService(callData->serviceE, callData->serviceD);
    callData->reqAS                    = proc->callData->reqAS;
    callData->DUmessageSize            = callData->reqAS * callData->plannedServiceTime;
    callData->proc                     = nullptr;


    if (system->serveNewCall(callData) == false)
    {
        ProcAll *newProc = system->engine->getNewProcess();
        newProc->callData = system->engine->getNewCall(callData);

    #ifndef DO_NOT_USE_SECUTIRY_CHECKS
        if (newProc->callData->classIdx > system->systemData->m())
            qFatal("Wrong class idx");
    #endif
        newProc->state = ProcAll::WAITING_FOR_NEW_CALL;
        newProc->execute = funNewCall;
        newProc->time = funTimeNewCall(newProc->callData->sourceE, newProc->callData->sourceD);
        system->engine->addProcess(newProc);
    }

    system->engine->reuseProcess(proc);
}

void ProcAll::newCallDepPlus(
    ProcAll *proc
  , SimulatorAll::System *system
  , double (*funTimeNewCall)(double, double)
  , double (*funTimeOfService)(double, double)
  , void (*funNewCall)(ProcAll *, SimulatorAll::System *)
)
{
#ifndef DO_NOT_USE_SECUTIRY_CHECKS
    if (proc->callData->classIdx > system->systemData->m())
        qFatal("Wrong class idx");
#endif
    SimulatorAll::Call *callData           = proc->callData;
    SimulatorAll::Call *parentServicedCall = callData->complementaryCall;

    callData->plannedServiceTime = funTimeOfService(callData->serviceE, callData->serviceD);
    callData->reqAS              = proc->callData->reqAS;
    callData->DUmessageSize      = callData->reqAS * callData->plannedServiceTime;
    callData->proc               = nullptr;

    ProcAll *procNewCall = system->engine->getNewProcess();
    procNewCall->callData = system->engine->getNewCall(callData);
    procNewCall->callData->proc = procNewCall;

    if (parentServicedCall)
    {
        procNewCall->callData->complementaryCall = parentServicedCall;
        parentServicedCall->complementaryCall = procNewCall->callData;
        callData->complementaryCall = nullptr;
    }

    procNewCall->state = ProcAll::WAITING_FOR_NEW_CALL;
    procNewCall->execute = funNewCall;
    procNewCall->time = funTimeNewCall(procNewCall->callData->sourceE, procNewCall->callData->sourceD);
    system->engine->addProcess(procNewCall);

    if (system->serveNewCall(callData) == true)
    {
        ProcAll *newProc = system->engine->getNewProcess();
        newProc->callData = system->engine->getNewCall(callData);
        newProc->callData->proc = newProc;
        callData->complementaryCall = newProc->callData;
        newProc->callData->complementaryCall = callData;

        newProc->state = ProcAll::WAITING_FOR_NEW_CALL;
        newProc->execute = funNewCall;
        newProc->time = funTimeNewCall(newProc->callData->sourceE, newProc->callData->sourceD);
        system->engine->addProcess(newProc);
    }
    system->engine->reuseProcess(proc);
}

void ProcAll::callServiceEndedIndependent(ProcAll *proc, SimulatorAll::System *system)
{
#ifndef DO_NOT_USE_SECUTIRY_CHECKS
    if (proc->callData->classIdx > system->systemData->m())
        qFatal("Wrong class idx");
#endif
    system->endCallService(proc->callData);

    proc->callData = nullptr;
    system->engine->reuseProcess(proc);
}

void ProcAll::callServiceEndedDependentMinus(
    ProcAll *proc
  , SimulatorAll::System *system
  , double (*funTimeNewCall)(double, double)
  , void (*funNewCall)(ProcAll *, SimulatorAll::System *)
)
{
    ProcAll *newProc = system->engine->getNewProcess();
    newProc->callData = system->engine->getNewCall(proc->callData);

#ifndef DO_NOT_USE_SECUTIRY_CHECKS
    if (proc->callData->classIdx > system->systemData->m())
        qFatal("Wrong class idx");
#endif
    system->endCallService(proc->callData);

    proc->callData = nullptr;
    system->engine->reuseProcess(proc);

    //Adding new process with state Waiting for new call
    newProc->state = ProcAll::WAITING_FOR_NEW_CALL;
    newProc->execute = funNewCall;
    newProc->time = funTimeNewCall(newProc->callData->sourceE, newProc->callData->sourceD);
    system->engine->addProcess(newProc);
}

void ProcAll::callServiceEndedDependentPlus(ProcAll *proc, SimulatorAll::System *system)
{
    SimulatorAll::Call *scheduledCall = proc->callData->complementaryCall;

    if (scheduledCall->complementaryCall != proc->callData)
        qFatal("Wrong relations between Pascal sourcess");

    if (proc->state == PROC_STATE::USELESS)\
        qFatal("Wrong Process state");
    system->endCallService(proc->callData);

    system->cancellScheduledCall(scheduledCall);

    proc->callData = nullptr;
    system->engine->reuseProcess(proc);
}


void SimulatorAll::Call::fillData(SimulatorAll::Call *src)
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

void SimulatorAll::Call::IncrTimeInServer(double time)
{
    timeOnServer +=time;
}

void SimulatorAll::Call::collectTheStats(double time)
{
    timeOnSystem +=time;
    DUtransfered += (time * allocatedAS);
}


#define FOLDINEND }

#define FOLDINGSTART { // buffer

SimulatorAll::Buffer::Buffer(SimulatorAll::System *system) : system(system), V(system->vk_b), m(system->m), firstCall(nullptr), n(0)
{
    numberOfCalls.resize(m);
    numberOfAS.resize(m);

    avgNumberOfCalls.resize(m);
    n_i.resize(m);


    occupancyTimes.resize(V+1);
    numberOfCalls.resize(m);
    numberOfAS.resize(m);
    avgNumberOfCalls.resize(m);
    AStime_ofOccupiedAS_byClassI_inStateN.resize(m);
    {
        for (int nb=0; nb<=V; nb++)
            AStime_ofOccupiedAS_byClassI_inStateN[nb].resize(m);
    }
}

void SimulatorAll::Buffer::statsClear()
{

}

void SimulatorAll::Buffer::statsColectPre(double time)
{

}

void SimulatorAll::Buffer::removeCall(SimulatorAll::Call *first)
{

}

#define FOLDINEND }









} // namespace Algorithms
