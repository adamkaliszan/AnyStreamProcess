#include "simulatorAllSystems.h"
#include <qstring.h>
#include <qglobal.h>
#include <utils/lag.h>
#include "utils/utilsMisc.h"
#include <qmath.h>


namespace Algorithms
{

SimulatorAll::SimulatorAll() : Simulator()
{
    myQoS_Set << Results::Type::AllSugbrupsInGivenCombinationNotAvailableForCallsOfGivenClass;
    myQoS_Set << Results::Type::AvailableSubroupDistribution;
    myQoS_Set << Results::Type::AllSugbrupsInGivenCombinationAvailableForCallsOfGivenClass;

    system = nullptr;
}

bool SimulatorAll::possible(const ModelCreator *system) const
{
    return Simulator::possible(system);
}

StatisticEventType SimulatorAll::SimEvent2statEvent(SimulatorAll::EventType simEvent)
{
    switch (simEvent)
    {
    case EventType::callServiceEnded:
    case EventType::callServiceEndedAndBufferDequeued:
        return StatisticEventType::callServiceEnded;

    case EventType::newCallAccepted:
    case EventType::newCallEnqued:
        return StatisticEventType::newCallAccepted;

    case EventType::newCallRejected:
        return StatisticEventType::newCallRejected;
    }
    return StatisticEventType::newCallRejected;
}

void SimulatorAll::calculateSystem(const ModelCreator *system
                                   , double a
                                   , RInvestigator *results
                                   , SimulationParameters *simParameters)
{
    prepareTemporaryData(system, a);


    System *simData = new System(system);
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
        simData->statsEnable();
        double simulationTime = engine->doSimExperiment(simParameters->noOfLostCalls, seed, simParameters->noOfServicedCalls);
        simData->writesResultsOfSingleExperiment((*results)[serNo], simulationTime);
        qDebug("universal simulation experiment no %d", serNo+1);
    }
    delete simData;
    delete engine;
    //emit this->sigCalculationDone();
}

#define FOLDINGSTART { //Agenda
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
    callThatIsInService->proc->state    = ProcAll::ProcState::SENDING_DATA;
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
    if (proc->callData->classIdx > system->par.m)
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
#define FOLDINGEND }

SimulatorAll::System::System(const ModelCreator *system) : par(system), state(system)
{
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
    for(int i=0; i<system->par.m; i++)
    {
        const ModelTrClass *tmpClass = system->par.data->getClass(i);
        ProcAll::initialize(this, tmpClass, i, a, sumPropAt, V);
    }
}

double SimulatorAll::Engine::doSimExperiment(int numberOfLostCall, unsigned int seed, int numberOfServicedCalls)
{
    double simulationTime = 0;
    double deltaTime;
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

    EventType event;

    if (numberOfServicedCalls == 0)
    {
        while(totalNumberOfLostCalls < numberOfLostCall)
        {
            ProcAll *proc = takeFirstProcess();
            deltaTime = proc->time;
            assert(deltaTime>=0);
            system->statsCollectPre(deltaTime);
            simulationTime+= deltaTime;
            classIdx = proc->callData->classIdx;
            event = proc->execute(proc, system);
            system->statsCollectPost(classIdx, event);
        }
    }
    else
    {
        if(totalNumberOfLostCalls > 0)
        {
            while(totalNumberOfLostCalls < numberOfLostCall && totalNumberOfServicedCalls < numberOfServicedCalls)
            {
                ProcAll *proc = takeFirstProcess();
                deltaTime = proc->time;
                assert(deltaTime>=0);
                system->statsCollectPre(deltaTime);
                simulationTime += deltaTime;
                classIdx = proc->callData->classIdx;
                event = proc->execute(proc, system);
                system->statsCollectPost(classIdx, event);
            }
        }
        else
        {
            while(totalNumberOfServicedCalls < numberOfServicedCalls)
            {
                ProcAll *proc = takeFirstProcess();
                deltaTime = proc->time;
                assert(deltaTime>=0);
                system->statsCollectPre(deltaTime);
                simulationTime += deltaTime;
                classIdx = proc->callData->classIdx;
                event = proc->execute(proc, system);
                system->statsCollectPost(classIdx, event);
            }
        }
    }
    return simulationTime;
}

void SimulatorAll::System::writesResultsOfSingleExperiment(RSingle& singleResults, double simulationTime)
{
    server->writesResultsOfSingleExperiment(singleResults, simulationTime);

    int V  = server->getV() + buffer->getV();
    const int &m  = par.m;

    int max_t = 0;

    for (int i=0; i<m; i++)
    {
        int t = par.t_i[i];
        max_t = qMax(t, max_t);
    }

/// TypeForClass
    for (int i=0; i<m; i++)
    {
        int t = par.t_i[i];

    /// BlockingProbability
        double E = 0;

        for (int n = qMax(0, (V-t+1)); n<=V; n++)
            E+=statistics->getTimeStatistics(n).occupancyTime;
        E/= simulationTime;

        if (E < 0)
            qFatal("Wrong E");
        singleResults.write(TypeForClass::BlockingProbability, E, i);

    /// LossProbability
        if (statistics->getEventStatisticsSC(i).outNewOffered > 0)
        {
            double B = (statistics->getEventStatisticsSC(i).outNewOffered - statistics->getEventStatisticsSC(i).outNewAcceptedByServer);
            B/= statistics->getEventStatisticsSC(i).outNewOffered;

            if (B < 0)
                qFatal("Wrong B");
            singleResults.write(TypeForClass::LossProbability, B, i);
        }

    /// AvarageNumbersOfCallsInSystem
        double y = 0;
        for (int n=0; n <= V; n++)
            y+= statistics->getTimeStatisticsSC(i, n).occupancyUtilization * statistics->getTimeStatistics(n).occupancyTime;
        y/= simulationTime;
        singleResults.write(TypeForClass::AvarageNumbersOfCallsInSystem, y, i);

    /// CongestionTraffic
        y=0;
        for (int n=0; n <= V; n++)
            y+= n*statistics->getTimeStatistics(n).occupancyTime;
        y/=simulationTime;
        singleResults.write(TypeForClass::CongestionTraffic, y, i);

//        AvarageNumbersOfCallsInServer,  /// Avarage number of calls in server
//        AvarageNumbersOfCallsInBuffer,  /// Avarage number of calls in buffer. Only for systems with buffer

    }
/// TypeForSystemState
    double totalStatesDurationTime = 0;
    for (int n=0; n<=V; n++)
    {
        double stateDurationTime = statistics->getTimeStatistics(n).occupancyTime;
        totalStatesDurationTime += stateDurationTime;

    /// StateProbability
        double p = stateDurationTime / simulationTime;
        assert(p <= 1);
        singleResults.write(TypeForSystemState::StateProbability, p, n);

    /// IntensityNewCallOutOffered,
        p = statistics->getEventStatistics(n).outNewOffered;
        p/= stateDurationTime;
        singleResults.write(TypeForSystemState::IntensityNewCallOutOffered, p, n);

    /// IntensityNewCallOutAccepted
        p = statistics->getEventStatistics(n).outNewAcceptedByServer;
        p/= stateDurationTime;
        singleResults.write(TypeForSystemState::IntensityNewCallOutAccepted, p, n);

    /// IntensityEndCallOut,
        p = statistics->getEventStatistics(n).outEnd;
        p/= stateDurationTime;
        singleResults.write(TypeForSystemState::IntensityEndCallOut, p, n);

    /// IntensityNewCallIn,
        p = statistics->getEventStatistics(n).inNew;
        p/= stateDurationTime;
        singleResults.write(TypeForSystemState::IntensityNewCallIn, p, n);

    /// IntensityEndCallIn
        p = statistics->getEventStatistics(n).inEnd;
        p/= stateDurationTime;
        singleResults.write(TypeForSystemState::IntensityEndCallIn, p, n);

///TypeForClassAndSystemState
        for (int i=0; i<m; i++)
        {

    /// UsageForSystem
            p = statistics->getTimeStatisticsSC(i, n).occupancyUtilization;
            p/= stateDurationTime;
            singleResults.write(TypeForClassAndSystemState::UsageForSystem, p, i, n);

    /// UsageForServer
            p = statistics->getTimeStatisticsSC(i, n).occupancyUtilizationServer;
            p/= stateDurationTime;
            singleResults.write(TypeForClassAndSystemState::UsageForServer, p, i, n);

    /// UsageForBuffer
            p = statistics->getTimeStatisticsSC(i, n).occupancyUtilizationBuffer;
            p/= stateDurationTime;
            singleResults.write(TypeForClassAndSystemState::UsageForBuffer, p, i, n);

    /// CAC_ProbabilityForSystem
            if (statistics->getEventStatisticsSC(i, n).outNewOffered > 0)
            {
                p = (statistics->getEventStatisticsSC(i, n).outNewAcceptedByServer + statistics->getEventStatisticsSC(i, n).outNewAcceptedByBuffer);
                p/= statistics->getEventStatisticsSC(i, n).outNewOffered;
                singleResults.write(TypeForClassAndSystemState::CAC_ProbabilityForSystem, p, i, n);
            }

    /// CAC_ProbabilityForServer
            if (statistics->getEventStatisticsSC(i, n).outNewOffered > 0)
            {
                p = statistics->getEventStatisticsSC(i, n).outNewAcceptedByServer / statistics->getEventStatisticsSC(i, n).outNewOffered;
                singleResults.write(TypeForClassAndSystemState::CAC_ProbabilityForServer, p, i, n);
            }

    /// CAC_ProbabilityForQueue
            if (statistics->getEventStatisticsSC(i, n).outNewOffered - statistics->getEventStatisticsSC(i, n).outNewAcceptedByServer > 0)
            {
                p = statistics->getEventStatisticsSC(i, n).outNewAcceptedByBuffer;
                p/= (statistics->getEventStatisticsSC(i, n).outNewOffered - statistics->getEventStatisticsSC(i, n).outNewAcceptedByServer);
                singleResults.write(TypeForClassAndSystemState::CAC_ProbabilityForQueue, p, i, n);
            }

            if (stateDurationTime > 0)
            {
    /// OfferedNewCallIntensityOutForSystem
                p = statistics->getEventStatisticsSC(i, n).outNewOffered / stateDurationTime;
                singleResults.write(TypeForClassAndSystemState::OfferedNewCallIntensityOutForSystem, p, i, n);

    /// OfferedNewCallIntensityOutForServer
                p = statistics->getEventStatisticsSC(i, n).outNewOffered / stateDurationTime;
                singleResults.write(TypeForClassAndSystemState::OfferedNewCallIntensityOutForServer, p, i, n);

    /// OfferedNewCallIntensityOutForQueue
                p = statistics->getEventStatisticsSC(i, n).outNewOffered - statistics->getEventStatisticsSC(i, n).outNewAcceptedByServer;
                p/= stateDurationTime;
                singleResults.write(TypeForClassAndSystemState::OfferedNewCallIntensityOutForQueue, p, i, n);

    /// RealNewCallIntensityOutForSystem
                p = (statistics->getEventStatisticsSC(i, n).outNewAcceptedByServer + statistics->getEventStatisticsSC(i, n).outNewAcceptedByBuffer);
                p/= stateDurationTime;
                singleResults.write(TypeForClassAndSystemState::RealNewCallIntensityOutForSystem, p, i, n);

                // RealNewCallIntensityOutForServer
                // RealNewCallIntensityOutForQueue

    /// NewCallIntensityInForSystem
                p = statistics->getEventStatisticsSC(i, n).inNew / stateDurationTime;
                singleResults.write(TypeForClassAndSystemState::NewCallIntensityInForSystem, p, i, n);

                // NewCallIntensityInForServer
                // NewCallIntensityInForQueue

    /// EndCallIntensityOutForSystem,
                p = statistics->getEventStatisticsSC(i, n).outEnd / stateDurationTime;
                singleResults.write(TypeForClassAndSystemState::EndCallIntensityOutForSystem, p, i, n);

                // EndCallIntensityOutForServer
                // EndCallIntensityOutForQueue

    /// EndCallIntensityInForSystem
                p = statistics->getEventStatisticsSC(i, n).inEnd / stateDurationTime;
                singleResults.write(TypeForClassAndSystemState::EndCallIntensityInForSystem, p, i, n);

                // EndCallIntensityInForServer
                // EndCallIntensityInForQueue

            }


            int t = par.t_i[i];

            stateDurationTime = (n-t >= 0) ? statistics->getTimeStatistics(n-t).occupancyTime : 0;

            stateDurationTime = (n+t <= server->getV()) ? statistics->getTimeStatistics(n+t).occupancyTime : 0;

            stateDurationTime =  statistics->getTimeStatistics(n).occupancyTime;

        }
        assert(totalStatesDurationTime < simulationTime*1.1);
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

        state.n += newCall->reqAS;
        state.n_i[static_cast<int>(newCall->classIdx)] += newCall->reqAS;

        return true;
    }
    else
    {
        if (0)
        {
            serveCallsInEque();
            return true;
        }
        else
        {
            finishCall(newCall, false);
            return false;
        }
    }
}

void SimulatorAll::System::endCallService(SimulatorAll::Call *call)
{
    removeCallFromServer(call);
    calls.removeAll(call);

    state.n-=call->reqAS;
    finishCall(call, true);
}

void SimulatorAll::System::removeCallFromServer(SimulatorAll::Call *call)
{
    server->removeCall(call);
    serveCallsInEque();
}

void SimulatorAll::System::removeCallFromBuffer(SimulatorAll::Call *call)
{
    buffer->removeCall(call);
}

void SimulatorAll::System::finishCall(SimulatorAll::Call *call, bool acceptedToService)
{
#ifndef DO_NOT_USE_SECUTIRY_CHECKS
    if (call->classIdx > 100)
        qFatal("Wrong class idx");
#endif

    if (!acceptedToService)
    {
        engine->notifyLostCall();
    }
    else
    {
        engine->notifyServicedCall();
    }
    engine->reuseCall(call);

}

void SimulatorAll::System::serveCallsInEque()
{
    Call *tmpCall;
    int numberOfAvailableAS;

    numberOfAvailableAS = server->getNoOfFreeAS();
    while(numberOfAvailableAS > 0)
    {
#ifndef DO_NOT_USE_SECUTIRY_CHECKS
        buffer->consistencyCheck();
#endif
        tmpCall = buffer->getNextCall();
        if (tmpCall == nullptr)
            break;

//TODO        if (buffer->par.scheduler != BufferResourcessScheduler::Continuos && tmpCall->reqAS > numberOfAvailableAS)
//            break;

        int maxResToAll = tmpCall->reqAS - tmpCall->allocatedAS;
#ifndef DO_NOT_USE_SECUTIRY_CHECKS
        if (buffer->state.n < (int)maxResToAll)
            qFatal("Wrong number of max ressourcess to allocate");
#endif
        bool newCall = (tmpCall->allocatedAS == 0);
        int noOfAS_toAllocate =qMin(numberOfAvailableAS, (int) maxResToAll);
        tmpCall->allocatedAS += noOfAS_toAllocate;

        double newTime = (tmpCall->DUmessageSize-tmpCall->DUtransfered)/tmpCall->allocatedAS;

//TODO         server->addCall(tmpCall, noOfAS_toAllocate, newCall);

        if (newCall)
        {
            tmpCall->proc = engine->getNewProcess();
            tmpCall->proc->time = newTime;
            tmpCall->proc->callData = tmpCall;
//TODO            tmpCall->proc->state = ProcAll::PROC_STATE::SENDING_DATA;
            tmpCall->proc->execute = tmpCall->trEndedFun;
            engine->addProcess(tmpCall->proc);
        }
        else
        {
//TODO            engine->changeProcessWakeUpTime(tmpCall->proc, newTime);
        }
        buffer->takeCall(tmpCall, noOfAS_toAllocate);


        numberOfAvailableAS = server->getNoOfFreeAS();

#ifndef DO_NOT_USE_SECUTIRY_CHECKS
        buffer->consistencyCheck();
#endif
    }
}


#define FOLDINGSTART { //Statistics

void SimulatorAll::System::statsCollectPre(double time)
{
    state.old_n = state.n;

    foreach(Call *tmpCall, calls)
    {
        tmpCall->collectTheStats(time);
    }

    int n_s = server->state.n;
    int n_b = buffer->get_n();

    const QVector<int> &nMs_s = server->getMicroStates();
    const QVector<int> &nMs_b = buffer->getMicroStates();

    const QVector<int> &nKs_s = server->getOccupancyOfTheGroups();
    const QVector<int> &nKs_b = buffer->getOccupancyOfTheGroups();


    statistics->collectPre(this->par.data, time, n_s, n_b, nMs_s, nMs_b, nKs_s, nKs_b);

    server->statsColectPre(this->par.data, time);
    buffer->statsColectPre(time);
}

void SimulatorAll::System::statsCollectPost(int classIdx, EventType eventSim)
{
    statistics->collectPost(classIdx, state.old_n, state.n, SimulatorAll::SimEvent2statEvent(eventSim));
    server->statsCollectPost(classIdx, state.old_n, state.n, eventSim);
    buffer->statsCollectPost(classIdx, state.old_n, state.n);
}

void SimulatorAll::System::statsClear()
{
    statistics->clear();
    server->statsClear();
    buffer->statsClear();
}

void SimulatorAll::System::statsEnable()
{
    statsClear();
}

void SimulatorAll::System::statsDisable()
{
}

void SimulatorAll::Server::statsClear()
{
    statistics->clear();
}

void SimulatorAll::Server::statsColectPre(const ModelCreator *mSystem, double time)
{
    statistics->collectPre(mSystem, time, state.n, state.n_i, state.n_k);

    statsColectPreGroupsAvailability(time);
}

void SimulatorAll::Server::statsColectPreGroupsAvailability(double time)
{
    (void) time;
}

void SimulatorAll::Server::statsCollectPost(int classIdx, int old_n, int n, SimulatorAll::EventType simEvent)
{
    StatisticEventType statEvent = SimulatorAll::SimEvent2statEvent(simEvent);
    statistics->collectPost(classIdx, old_n, n, statEvent);
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
    case ResourcessScheduler::Random:
        Utils::UtilsMisc::suffle(state.subgroupSequence);
        foreach (groupNoTmp, state.subgroupSequence)
        {
            groupNo = groupNoTmp;
            if (state.subgroupFreeAUs[groupNoTmp] >= noOfAUs)
            {
                result = true;
                break;
            }
        }
        break;

    case ResourcessScheduler::Sequencial:
        foreach (groupNoTmp, state.subgroupSequence)
        {
            groupNo = groupNoTmp;
            if (state.subgroupFreeAUs[groupNoTmp] >= noOfAUs)
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
    state.n += noOfAS;
    assert(vTotal >= state.n);
    state.n_i[call->classIdx] += noOfAS;
    state.n_k[groupNo] += noOfAS;
    state.subgroupFreeAUs[groupNo] -=noOfAS;


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
    this->state.subgroupFreeAUs[call->groupIndex] +=call->allocatedAS;
    calls.removeAll(call);
    state.n -=call->allocatedAS;
    state.n_i[call->classIdx]-= call->allocatedAS;
    state.n_k[call->groupIndex]-= call->allocatedAS;
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
  , scheduler(system->par.data->getGroupsSchedulerAlgorithm())
  , vTotal(system->par.vk_sb)
  , vMax(system->par.data->v_sMax())
  , k(system->par.data->k_s())
  , m(system->par.m)
{   
    state.n = 0;
    state.n_k.resize(k);
    state.n_i.resize(m);

    state.subgroupFreeAUs.resize(k);
    state.subgroupSequence.resize(k);
    for (int j=0; j<k; j++)
    {
        state.subgroupFreeAUs[j] = system->par.data->getConstSyst().vs[j];
        state.subgroupSequence[j] = j;
    }

    statistics = new ServerStatistics(system->par.data);
}

SimulatorAll::Server::~Server()
{
    state.n = 0;
}

void SimulatorAll::Server::writesResultsOfSingleExperiment(RSingle &singleResults, double simulationTime)
{
    double result;

/// TypeForServerState
    for (int n=0; n<=this->getV(); n++)
    {
        double stateTime = statistics->getTimeStatistics(n).occupancyTime;
        result = stateTime/simulationTime;
        singleResults.write(TypeForServerState::StateProbability, result, n);

        if (stateTime > 0)
        {
            result = statistics->getEventStatistics(n).outNewOffered/stateTime;
            singleResults.write(TypeForServerState::IntensityNewCallOut, result, n);

            result = statistics->getEventStatistics(n).inNew/stateTime;
            singleResults.write(TypeForServerState::IntensityNewCallIn, result, n);

            result = statistics->getEventStatistics(n).outEnd/stateTime;
            singleResults.write(TypeForServerState::IntensityEndCallOut, result, n);

            result = statistics->getEventStatistics(n).inEnd/stateTime;
            singleResults.write(TypeForServerState::IntensityEndCallIn, result, n);
        }
    }

/// TypeForClassAndServerState
    for (int n=0; n<=this->getV(); n++)
    {
        double stateTime = statistics->getTimeStatistics(n).occupancyTime;
        if (stateTime > 0)
        {
            for (int classNo=0; classNo < m; classNo++)
            {
                result = statistics->getTimeStatisticsSC(classNo, n).occupancyUtilization/stateTime;
                singleResults.write(TypeForClassAndServerState::Usage, result, classNo, n);

                result = statistics->getEventStatisticsSC(classNo, n).outNewOffered/stateTime;
                singleResults.write(TypeForClassAndServerState::OfferedNewCallIntensityOut, result, classNo, n);

                result = statistics->getEventStatisticsSC(classNo, n).outNewAcceptedByServer/stateTime;
                singleResults.write(TypeForClassAndServerState::RealNewCallIntensityOut, result, classNo, n);

                result = statistics->getEventStatisticsSC(classNo, n).inNew/stateTime;
                singleResults.write(TypeForClassAndServerState::NewCallIntensityIn, result, classNo, n);

                result = statistics->getEventStatisticsSC(classNo, n).outEnd/stateTime;
                singleResults.write(TypeForClassAndServerState::EndCallIntensityOut, result, classNo, n);

                result = statistics->getEventStatisticsSC(classNo, n).inEnd/stateTime;
                singleResults.write(TypeForClassAndServerState::EndCallIntensityIn, result, classNo, n);
            }
        }
        for (int classNo=0; classNo < m; classNo++)
        {
            if (statistics->getEventStatisticsSC(classNo, n).outNewOffered > 0)
            {
                result = statistics->getEventStatisticsSC(classNo, n).outNewAcceptedByServer / statistics->getEventStatisticsSC(classNo, n).outNewOffered;
                singleResults.write(TypeForClassAndServerState::CAC_Probability, result, classNo, n);
            }
        }
    }

/// TypeResourcess_VsServerGroupsCombination
    for (int combinationNo=0; combinationNo < statistics->getNoOfSets(); combinationNo++)
    {
        for (int n=0; n<=vMax; n++)
        {
            singleResults.write(TypeResourcess_VsServerGroupsCombination::AvailabilityInOneOrMoreGroups
              , statistics->getTimeGroupComb(combinationNo, n).oneOrMoreInCombinationAvailable / simulationTime, n, combinationNo);

            singleResults.write(TypeResourcess_VsServerGroupsCombination::AvailabilityInAllTheGroups
              , statistics->getTimeGroupComb(combinationNo, n).allInCombinationAvailable / simulationTime, n, combinationNo);

            singleResults.write(TypeResourcess_VsServerGroupsCombination::InavailabilityInAllTheGroups
              , statistics->getTimeGroupComb(combinationNo, n).allInCombinationUnavailable / simulationTime, n, combinationNo);
        }
    }

/// TypeForResourcessAndNumberOfServerGroups
    for (int noOfgroups=0; noOfgroups<=k; noOfgroups++)
    {
        for (int n=0; n <= vMax; n++)
        {
            result = statistics->getTimeGroupSet(noOfgroups, n).allInSetAvailableAllOutsideSetUnavailable/simulationTime;
            singleResults.write(TypeForResourcessAndNumberOfServerGroups::AvailabilityOnlyInAllTheGroups, result, n, noOfgroups);

            result = statistics->getTimeGroupSet(noOfgroups, n).allInSetAvailable/simulationTime;
            singleResults.write(TypeForResourcessAndNumberOfServerGroups::AvailabilityInAllTheGroups, result, n, noOfgroups);

            result = statistics->getTimeGroupSet(noOfgroups, n).allUnavailable/simulationTime;
            singleResults.write(TypeForResourcessAndNumberOfServerGroups::InavailabilityInAllTheGroups, result, n, noOfgroups);
        }
    }
}

int SimulatorAll::Server::getMaxNumberOfAsInSingleGroup()
{
    int result = 0;

    for (int groupNo=0; groupNo<k; groupNo++)
    {
        int tempAvailability = state.subgroupFreeAUs[groupNo];
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
  , SimulatorAll::EventType (*funNewCall)(ProcAll *, SimulatorAll::System *)
)
{
    double IncE = 1.0 / trClass->intensityNewCallTotal(a, static_cast<size_t>(V), sumPropAt);
    callData = system->getNewCall(trClass, classIdx, IncE);
    callData->trEndedFun = ProcAll::callServiceEndedIndependent;

    this->state = ProcAll::ProcState::WAITING_FOR_NEW_CALL;
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
  , SimulatorAll::EventType (*funNewCall)(ProcAll *, SimulatorAll::System *)
  , SimulatorAll::EventType (*funEndCall)(ProcAll *, SimulatorAll::System *)
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

    this->state = ProcAll::ProcState::WAITING_FOR_NEW_CALL;
    this->execute = funNewCall;
    this->time = funTimeNewCall(this->callData->sourceE, this->callData->sourceD);
    engine->addProcess(this);
}

SimulatorAll::EventType ProcAll::newCallIndep(
    ProcAll *proc
  , SimulatorAll::System *system
  , double (*funTimeNewCall)(double, double)
  , double (*funTimeOfService)(double, double)
  , SimulatorAll::EventType (*funNewCall)(ProcAll *proc, SimulatorAll::System *system)
)
{
    SimulatorAll::EventType result;

#ifndef DO_NOT_USE_SECUTIRY_CHECKS
    if (proc->callData->classIdx > system->par.m)
        qFatal("Wrong class idx");
#endif
    SimulatorAll::Call *callData = proc->callData;
    callData->plannedServiceTime       = funTimeOfService(callData->serviceE, callData->serviceD);
    callData->reqAS                    = proc->callData->reqAS;
    callData->DUmessageSize            = callData->reqAS * callData->plannedServiceTime;
    callData->proc                     = nullptr;

    result = system->serveNewCall(callData) ? SimulatorAll::EventType::newCallAccepted : SimulatorAll::EventType::newCallRejected;

    //Adding new process with state Waiting for new call
    ProcAll *newProc = proc;
    newProc->callData = system->engine->getNewCall(proc->callData);

    newProc->state = ProcAll::ProcState::WAITING_FOR_NEW_CALL;
    newProc->execute = funNewCall;
    newProc->time = funTimeNewCall(newProc->callData->sourceE, newProc->callData->sourceD);
    system->engine->addProcess(newProc);

    return result; //TODO uwzględnić bufor
}

SimulatorAll::EventType ProcAll::newCallDepMinus(
    ProcAll *proc
  , SimulatorAll::System *system
  , double (*funTimeNewCall)(double, double)
  , double (*funTimeOfService)(double, double)
  , SimulatorAll::EventType (*funNewCall)(ProcAll *, SimulatorAll::System *)
)
{
    SimulatorAll::EventType result;

#ifndef DO_NOT_USE_SECUTIRY_CHECKS
    if (proc->callData->classIdx > system->par.m)
        qFatal("Wrong class idx");
#endif
    SimulatorAll::Call *callData = proc->callData;
    callData->plannedServiceTime       = funTimeOfService(callData->serviceE, callData->serviceD);
    callData->reqAS                    = proc->callData->reqAS;
    callData->DUmessageSize            = callData->reqAS * callData->plannedServiceTime;
    callData->proc                     = nullptr;


    if (system->serveNewCall(callData) == false)
    {
        result = SimulatorAll::EventType::newCallRejected;
        ProcAll *newProc = system->engine->getNewProcess();
        newProc->callData = system->engine->getNewCall(callData);

    #ifndef DO_NOT_USE_SECUTIRY_CHECKS
        if (newProc->callData->classIdx > system->par.m)
            qFatal("Wrong class idx");
    #endif
        newProc->state = ProcAll::ProcState::WAITING_FOR_NEW_CALL;
        newProc->execute = funNewCall;
        newProc->time = funTimeNewCall(newProc->callData->sourceE, newProc->callData->sourceD);
        system->engine->addProcess(newProc);
    }
    else
    {
        result = SimulatorAll::EventType::newCallAccepted; //TODO uwzględnić bufor
    }

    system->engine->reuseProcess(proc);

    return result;
}

SimulatorAll::EventType ProcAll::newCallDepPlus(ProcAll *proc
  , SimulatorAll::System *system
  , double (*funTimeNewCall)(double, double)
  , double (*funTimeOfService)(double, double)
  , SimulatorAll::EventType (*funNewCall)(ProcAll *, SimulatorAll::System *)
)
{
    SimulatorAll::EventType result;

#ifndef DO_NOT_USE_SECUTIRY_CHECKS
    if (proc->callData->classIdx > system->par.m)
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

    procNewCall->state = ProcAll::ProcState::WAITING_FOR_NEW_CALL;
    procNewCall->execute = funNewCall;
    procNewCall->time = funTimeNewCall(procNewCall->callData->sourceE, procNewCall->callData->sourceD);
    system->engine->addProcess(procNewCall);

    if (system->serveNewCall(callData) == true)
    {
        result = SimulatorAll::EventType::newCallAccepted;//TODO uwzględnić bufor
        ProcAll *newProc = system->engine->getNewProcess();
        newProc->callData = system->engine->getNewCall(callData);
        newProc->callData->proc = newProc;
        callData->complementaryCall = newProc->callData;
        newProc->callData->complementaryCall = callData;

        newProc->state = ProcAll::ProcState::WAITING_FOR_NEW_CALL;
        newProc->execute = funNewCall;
        newProc->time = funTimeNewCall(newProc->callData->sourceE, newProc->callData->sourceD);
        system->engine->addProcess(newProc);
    }
    else
    {
        result = SimulatorAll::EventType::newCallRejected;
    }

    system->engine->reuseProcess(proc);

    return result;
}

SimulatorAll::EventType ProcAll::callServiceEndedIndependent(ProcAll *proc, SimulatorAll::System *system)
{
#ifndef DO_NOT_USE_SECUTIRY_CHECKS
    if (proc->callData->classIdx > system->par.m)
        qFatal("Wrong class idx");
#endif
    system->endCallService(proc->callData);

    proc->callData = nullptr;
    system->engine->reuseProcess(proc);

    return SimulatorAll::EventType::callServiceEnded;
}

SimulatorAll::EventType ProcAll::callServiceEndedDependentMinus(ProcAll *proc
  , SimulatorAll::System *system
  , double (*funTimeNewCall)(double, double)
  , SimulatorAll::EventType(*funNewCall)(ProcAll *, SimulatorAll::System *)
)
{
    ProcAll *newProc = system->engine->getNewProcess();
    newProc->callData = system->engine->getNewCall(proc->callData);

#ifndef DO_NOT_USE_SECUTIRY_CHECKS
    if (proc->callData->classIdx > system->par.m)
        qFatal("Wrong class idx");
#endif
    system->endCallService(proc->callData);

    proc->callData = nullptr;
    system->engine->reuseProcess(proc);

    //Adding new process with state Waiting for new call
    newProc->state = ProcAll::ProcState::WAITING_FOR_NEW_CALL;
    newProc->execute = funNewCall;
    newProc->time = funTimeNewCall(newProc->callData->sourceE, newProc->callData->sourceD);
    system->engine->addProcess(newProc);

    return SimulatorAll::EventType::callServiceEnded;
}

SimulatorAll::EventType ProcAll::callServiceEndedDependentPlus(ProcAll *proc, SimulatorAll::System *system)
{
    SimulatorAll::Call *scheduledCall = proc->callData->complementaryCall;

    if (scheduledCall->complementaryCall != proc->callData)
        qFatal("Wrong relations between Pascal sourcess");

    if (proc->state == ProcState::USELESS)\
        qFatal("Wrong Process state");
    system->endCallService(proc->callData);

    system->cancellScheduledCall(scheduledCall);

    proc->callData = nullptr;
    system->engine->reuseProcess(proc);

    return SimulatorAll::EventType::callServiceEnded;
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

SimulatorAll::Buffer::Buffer(SimulatorAll::System *system) : par(system), state(system)
{
    statistics = new BufferStatistics(system->par.data);
}

void SimulatorAll::Buffer::statsClear()
{
    statistics->clear();
}

void SimulatorAll::Buffer::statsColectPre(double time)
{
    (void) time;
}

void SimulatorAll::Buffer::statsCollectPost(int classIdx, int old_n, int n)
{
    (void) classIdx;
    (void) old_n;
    (void) n;
}

void SimulatorAll::Buffer::takeCall(SimulatorAll::Call *call, int noOfAS)
{
    //TODO
}

void SimulatorAll::Buffer::removeCall(SimulatorAll::Call *first)
{
    (void) first;
}

SimulatorAll::Call *SimulatorAll::Buffer::getNextCall()
{
    return nullptr;//TODO
}

SimulatorAll::System::Parameters::Parameters(const ModelCreator *data)
  : m(data->m())
  , vk_sb(data->V())
  , vk_s(data->vk_s())
  , vk_b(data->vk_b())
  , t_i(data->getConstSyst().t)
  , data(data)
{
}

#define FOLDINEND }









} // namespace Algorithms
