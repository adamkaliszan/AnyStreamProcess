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

void SimulatorAll::Engine::reuseCall(SimulatorAll::Call *callToReuse)
{
    uselessCalls.push(callToReuse);
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
    QList<int> indexesToOccupy;
    bool isPlace = server->findAS(newCall->reqAS, groupNumber, indexesToOccupy);

    if (isPlace)
    {
        engine->prepareCallToService(newCall);

        calls.append(newCall);

        newCall->allocatedAS    = newCall->reqAS;
        server->addCall(newCall, newCall->reqAS, groupNumber, indexesToOccupy, true);

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

    statistics->collectPre(time, server->get_n(), buffer->get_n(), server->getMicroStates(), buffer->getMicroStates());
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
    return (state <= V) ? statistics->getTimeStatistics(state).occupancyTime : 0;
}

#define FOLDINGEND }


bool SimulatorAll::Server::findAS(int noOfAUs, int& groupNo, QList<int>& asIndexes
        ,  GroupResourcessAllocationlgorithm groupResourcessAllocationlgorithm
                ) const
{
    bool result = false;
    int groupNoTmp;

    switch (scheduler)
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


void SimulatorAll::Server::addCall(Call *call, int noOfAS, int groupNo, const QList<int>& asIndexes, bool newCall)
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
        if (calls.length() > this->V)
            qFatal("To many calls on server's list");
#endif
    }
}

void SimulatorAll::Server::removeCall(SimulatorAll::Call *call)
{
    groups[call->groupIndex]->removeCall(call);
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
  , V(system->systemData->vk_s())
  , vMax(system->systemData->v_sMax())
  , k(system->systemData->k_s())
  , m(system->systemData->m())
  , n(0)
{   


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

    groupSequence.resize(k);
    for (int j=0; j<k; j++)
        groupSequence[j] = j;

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

    for (int combinationNo=0; combinationNo < statistics->combinationList.length(); combinationNo++)
    {
        for (int n=0; n<=vMax; n++)
        {
            singleResults.write(TypeStateForServerGroupsCombination::FreeAUsInBestGroup
                                , statistics->freeAUsInBestGroupInCombination[combinationNo][n]/simulationTime
                                , n, combinationNo);

            singleResults.write(TypeStateForServerGroupsCombination::FreeAUsInEveryGroup
                                , statistics->freeAUsInWorstGroupInCombination[combinationNo][n]/simulationTime
                                , n, combinationNo);



            singleResults.write(TypeStateForServerGroupsCombination::AvailabilityOnlyInAllTheGroups
                                , statistics->availabilityOnlyInAllGroupsInCombination[combinationNo][n]/simulationTime
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
            singleResults.write(TypeClassForServerGroupsCombination::SerPossibilityOnlyInAllTheSubgroups, statistics->availabilityOnlyInAllGroupsInCombination[combinationNo][t]/simulationTime, classNo, combinationNo);
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
        int tempAvailability = groups[groupNo]->getNoOfFreeAUs(true);
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

#define FOLDINGSTART { // Allocation Unit
SimulatorAll::AU::AU(int m)
    : m(m)
    , servicedCall(nullptr)
{
    occupancyTimesPerClass.resize(m);
}

bool SimulatorAll::AU::isFree() const
{
    return (servicedCall == nullptr);
}

void SimulatorAll::AU::increaseOccupancyTime(double time)
{
    if (servicedCall != nullptr)
        occupancyTimesPerClass[servicedCall->classIdx] += time;
}

void SimulatorAll::AU::addCall(Call *newCall)
{
#ifndef DO_NOT_USE_SECUTIRY_CHECKS
    if (servicedCall != nullptr)
        qFatal("Can't add a call of class with index %d if there is a call of class with index %d in service",newCall->classIdx, newCall->classIdx);
#endif
    servicedCall = newCall;
}

bool SimulatorAll::AU::isThisCallHere(Call *comparedCall)
{
    return (servicedCall == comparedCall);
}

void SimulatorAll::AU::removeCall()
{
    servicedCall = nullptr;
}

#define FOLDINGSTART { //Statistics
void SimulatorAll::AU::statsClear()
{
    occupancyTime = 0;

    for (int i=0; i<m; i++)
        occupancyTimesPerClass[i] = 0;
}

void SimulatorAll::AU::statsColectPre(double time)
{
    if (this->servicedCall)
    {
        occupancyTimesPerClass[servicedCall->classIdx] += time;
        this->occupancyTime += time;
    }
}

void SimulatorAll::AU::statsCollectPost(int classIdx)
{
    (void) classIdx;
}

#define FOLDINEND }

#define FOLDINEND }

#define FOLDINGSTART { // Group
SimulatorAll::Group::Group(int v, int m): m(m), v(v), n(0)
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

SimulatorAll::Group::Group(const SimulatorAll::Group &rho)
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

int SimulatorAll::Group::findAS(int noOfAUs
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
        //break;
    }
    return result;
}

int SimulatorAll::Group::findMaxAS(SimulatorAll::GroupResourcessAllocationlgorithm groupResourcessAllocationlgorithm) const
{
    int result = 0;

    switch (groupResourcessAllocationlgorithm)
    {
    case GroupResourcessAllocationlgorithm::NonContinuous:
        result = v-n;
        break;
    default:
        qFatal("Not implemented");
        //break;
    }

    return result;
}

void SimulatorAll::Group::removeCall(SimulatorAll::Call *endedCall)
{
    n-=endedCall->reqAS;

    for (int idx=0; idx<v; idx++)
        if (allocationUnits[idx]->isThisCallHere(endedCall))
            allocationUnits[idx]->removeCall();
}

void SimulatorAll::Group::addCall(SimulatorAll::Call *newCall, const QList<int>& asIndexes)
{
    n+=newCall->reqAS;
    foreach (int index, asIndexes)
    {
       allocationUnits[index]->addCall(newCall);
    }
}

void SimulatorAll::Group::statsClear()
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

void SimulatorAll::Group::statsColectPre(double time)
{
    for (int resNo=0; resNo < v; resNo++)
        allocationUnits[resNo]->statsColectPre(time);

    timePerState[n] += time;
    for (int i=0; i<m; i++)
        workoutPerClassAndState[i][n] += (time *  n_i[i]);
}

void SimulatorAll::Group::statsCollectPost(int classIdx)
{
    for (int resNo=0; resNo < v; resNo++)
        allocationUnits[resNo]->statsCollectPost(classIdx);
}

int SimulatorAll::Group::getNoOfFreeAUs(bool considerAllocationAlgorithm)
{
    return (!considerAllocationAlgorithm) ? v-n : findMaxAS();
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
