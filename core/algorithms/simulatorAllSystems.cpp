#include <qglobal.h>
#include <qmath.h>
#include <qstring.h>

#include "utils/lag.h"
#include "utils/utilsMisc.h"
#include "simulatorAllSystems.h"


namespace Algorithms
{

SimulatorAll::SimulatorAll() : Simulator()
{
    myQoS_Set << Results::Type::OccupancyDistributionServerAndBuffer;
    myQoS_Set << Results::Type::AllSugbrupsInGivenCombinationNotAvailableForCallsOfGivenClass;
    myQoS_Set << Results::Type::AvailableSubroupDistribution;
    myQoS_Set << Results::Type::AllSugbrupsInGivenCombinationAvailableForCallsOfGivenClass;

    system = nullptr;
}

bool SimulatorAll::possible(const ModelSystem &system) const
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

void SimulatorAll::calculateSystem(
        const ModelSystem &system
      , double a
      , RInvestigator *results
      , SimulationParameters *simParameters)
{
    prepareTemporaryData(system, a);


    System *simData = new System(system);
    Engine *engine = new Engine(simData);

    engine->initialize(a, system.getTotalAt(), system.getServer().V());

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
#ifdef QT_DEBUG
    callThatIsInService->proc->state    = ProcAll::ProcState::SENDING_DATA;
#endif
    callThatIsInService->proc->execute  = callThatIsInService->trEndedFun;
    callThatIsInService->proc->callData = callThatIsInService;

    callThatIsInService->proc->time     = callThatIsInService->plannedServiceTime;
    if (callThatIsInService->reqAS != callThatIsInService->server.allocatedAU)
    {
        callThatIsInService->proc->time*= callThatIsInService->reqAS;
        callThatIsInService->proc->time/= callThatIsInService->server.allocatedAU;
    }

    agenda->addProcess(callThatIsInService->proc);
}

void SimulatorAll::Engine::updateServiceTime(SimulatorAll::Call *uCall)
{
    double dataToTransfer = uCall->plannedServiceTime * uCall->reqAS - uCall->DUtransfered;
    double remainingTime = uCall->proc->time = dataToTransfer / uCall->server.allocatedAU;
    agenda->changeProcessWakeUpTime(uCall->proc, remainingTime);
}

void SimulatorAll::Engine::reuseProcess(ProcAll *proc)
{
#ifdef QT_DEBUG
    proc->setUseless();
#endif
    agenda->reuseProcess(proc);
}

void SimulatorAll::Engine::addProcess(ProcAll *proc)
{
#ifdef QT_DEBUG
    if (proc->time < 0)
        qFatal("Negative time value");
    if (proc->callData->classIdx > system->par.m())
        qFatal("Wrong class idx");
#endif
    agenda->addProcess(proc);
}

void SimulatorAll::Engine::removeProcess(ProcAll *proc)
{
#ifdef QT_DEBUG
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
    result->server.groupIndex     = -1;
    result->server.allocatedAU    = 0;
    result->buffer.groupIndex     = -1;
    result->buffer.allocatedAU    = 0;

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
    result->server.allocatedAU    = 0;
    result->server.groupIndex     = -1;
    result->buffer.allocatedAU    = 0;
    result->buffer.groupIndex     = -1;

    result->complementaryCall = nullptr;

    return result;
}
#define FOLDINGEND }

SimulatorAll::System::System(const ModelSystem &system) : par(system), state(system)
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
    for(int i=0; i<system->par.m(); i++)
    {
        const ModelTrClass &tmpClass = system->par.getTrClass(i);
        ProcAll::initialize(this, &tmpClass, i, a, sumPropAt, V);
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
    buffer->writesResultsOfSingleExperiment(singleResults, simulationTime);

    int V  = server->getV() + buffer->getV();
    int Vs = server->getV();
    int Vb = buffer->getV();

    const int &m  = par.m();

    int max_t = 0;

    for (int i=0; i<m; i++)
    {
        int t = par.t(i);
        max_t = qMax(t, max_t);
    }

/// TypeForClass
    for (int i=0; i<m; i++)
    {
        int t = par.t(i);

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

            int t = par.t(i);

            stateDurationTime = (n-t >= 0) ? statistics->getTimeStatistics(n-t).occupancyTime : 0;

            stateDurationTime = (n+t <= server->getV()) ? statistics->getTimeStatistics(n+t).occupancyTime : 0;

            stateDurationTime =  statistics->getTimeStatistics(n).occupancyTime;

        }
        assert(totalStatesDurationTime < simulationTime*1.1);
    }
/// TypeForServerAndBufferState
    for (int ns=0; ns<=Vs; ns++)
    {
       for (int nb=0; nb<=Vb; nb++)
       {
           double stateDurationTime = statistics->getTimeStatistics(ns, nb).occupancyTime;
           double p = stateDurationTime / simulationTime;
           singleResults.write(TypeForServerAngBufferState::StateProbability, p, ns, nb);
       }
    }
}



bool SimulatorAll::System::serveNewCall(SimulatorAll::Call *newCall)
{
    int serverGroupNumber;
    int bufferGroupNumber;
    int maxServerSpace;

    bool isPlaceInServer = server->findAS(newCall->reqAS, serverGroupNumber);
    bool isPlaceInBuffer;
    SystemPolicy sysPolicy = par.getBufferPolicy();
    bool result = false;


    newCall->buffer.allocatedAU = 0;
    newCall->server.allocatedAU = 0;


    if (isPlaceInServer && buffer->get_n() == 0)
    {
        result = true;
        addCall(newCall);
        server->addCall(newCall, serverGroupNumber);
        engine->prepareCallToService(newCall);
    }
    else
    {
        switch (sysPolicy)
        {
        case SystemPolicy::NoBuffer:
            finishCall(newCall, false);
            break;

        case SystemPolicy::dFIFO:
            if (true == (isPlaceInBuffer = buffer->findAS(newCall->reqAS, bufferGroupNumber)))
            {
                result = true;
                addCall(newCall);
                buffer->addCall(newCall, serverGroupNumber);
            }
            else
            {
                finishCall(newCall, false);
            }
            break;

        case SystemPolicy::cFifo:
            maxServerSpace = server->getMaxNumberOfAsInSingleGroup();
            isPlaceInBuffer = buffer->findAS(newCall->reqAS - maxServerSpace, bufferGroupNumber);

            if (isPlaceInBuffer)
            {
                result = true;
                addCall(newCall);
                if ( maxServerSpace > 0)
                {
                    server->addCallPartially(newCall, maxServerSpace);
                    buffer->addCall(newCall, bufferGroupNumber);
                    engine->prepareCallToService(newCall);
                }
                else
                {
                    buffer->addCall(newCall, bufferGroupNumber);
                }
            }
            else
            {
                finishCall(newCall, false);
            }
            break;

        case SystemPolicy::qFIFO:
            if (isPlaceInServer)
            {
                result = true;
                addCall(newCall);
                server->addCall(newCall, serverGroupNumber);
                engine->prepareCallToService(newCall);
            }
            else
            {
                if (true == (isPlaceInBuffer = buffer->findAS(newCall->reqAS, bufferGroupNumber)))
                {
                    result = true;
                    addCall(newCall);
                    buffer->addCall(newCall, bufferGroupNumber);
                }
                else
                {
                    finishCall(newCall, false);
                }
            }
            break;

        case SystemPolicy::SD_FIFO:
            qFatal("Not implemented");
            break;
        }
    }

    return result;
}

void SimulatorAll::System::endCallService(SimulatorAll::Call *call)
{
    if (call->server.allocatedAU > 0)
        server->removeCall(call);

    if (call->buffer.allocatedAU > 0)
        buffer->removeCall(call);

    removeCall(call);

    finishCall(call, true);                  //Update Simulation engine

    if (buffer->get_n())
        serveCallsInEque();
}

void SimulatorAll::System::addCall(SimulatorAll::Call *nCall)
{
    calls.append(nCall);

    state.n+= nCall->reqAS;
    state.n_i[nCall->classIdx]+= nCall->reqAS;
#ifdef QT_DEBUG
    if (state.n > par.V())
        qFatal("Too big state %d, system capacity is no of calls in system %d", state.n, par.V());

    if (state.n < calls.length())
        qFatal("Wrong state: %d, no of calls in system %d", state.n, calls.length());
#endif
}

void SimulatorAll::System::removeCall(SimulatorAll::Call *rCall)
{
#ifdef QT_DEBUG
    int noOfRemovedCalls = calls.removeAll(rCall);
    if (noOfRemovedCalls != 1)
        qFatal("Can't find a call on a list with calls %d. %d calls was removed", rCall->classIdx, noOfRemovedCalls);
#else
    calls.removeAll(rCall);
#endif

    state.n-= rCall->reqAS;
    state.n_i[rCall->classIdx]-= rCall->reqAS;
#ifdef QT_DEBUG
    if (state.n < 0)
        qFatal("Negative state %d, no of calls in system %d", state.n, calls.length());

    if (state.n < calls.length())
        qFatal("Wrong state: %d, no of calls in system %d", state.n, calls.length());
#endif
}

void SimulatorAll::System::serveCallsInEque()
{
    switch (par.getBufferPolicy())
    {
    case SystemPolicy::NoBuffer:
        serveCallsInEqueSpDisabled();
        break;

    case SystemPolicy::SD_FIFO:
        serveCallsInEqueSpSdFifo();
        break;

    case SystemPolicy::cFifo:
        serveCallsInEqueSpCFifo();
        break;

    case SystemPolicy::dFIFO:
        serveCallsInEque_DFifo();
        break;

    case SystemPolicy::qFIFO:
        serveCallsInEqueSpQFifo();
        break;
    }
}

void SimulatorAll::System::serveCallsInEqueSpSdFifo()
{
    qFatal("Not implemented");
    //TODO Adam: implement it
}

int Algorithms::SimulatorAll::System::serveCallsInEque_DFifo()
{
    Call *tmpCall;

    int groupNumber;
    int noOfFreeAsInSever;
    int noOfMovedCalls = 0;

    while (((noOfFreeAsInSever = server->getNoOfFreeAS()) > 0) && (nullptr != (tmpCall = buffer->showFirstCall())))
    {
        if (server->findAS(tmpCall->reqAS, groupNumber))
        {
            tmpCall = buffer->popCall();
            bool addResult = server->addCall(tmpCall, groupNumber);
            assert(addResult);

            engine->prepareCallToService(tmpCall);

            noOfMovedCalls++;
        }
        else
            break;
    }
    return noOfMovedCalls;
}

void SimulatorAll::System::serveCallsInEqueSpQFifo()
{
    qFatal("Not implemented");
    //TODO Adam: implement it
}

void Algorithms::SimulatorAll::System::serveCallsInEqueSpCFifo()
{
    qFatal("Not implemented");
    //TODO Adam: implement it
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


    statistics->collectPre(this->par, time, n_s, n_b, nMs_s, nMs_b, nKs_s, nKs_b);

    server->statsColectPre(this->par, time);
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

void SimulatorAll::Server::statsColectPre(const ModelSystem &system, double time)
{
    statistics->collectPre(system, time, state.n, state.n_i, state.n_k);
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

bool SimulatorAll::CommonRes::findAS(int noOfAUs, int& groupNo) const
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

SimulatorAll::Server::Server(System *system): CommonRes (system, system->par.getServer())
{   
    statistics = new ServerStatistics(system->par);
}


bool SimulatorAll::Server::addCall(SimulatorAll::Call *call, int groupNumber)
{
    bool result;
    if (true == (result = (state.subgroupFreeAUs[groupNumber] >= call->reqAS)))
    {
        call->server.groupIndex = groupNumber;
        call->server.allocatedAU = call->reqAS;
        state.addCall(call, call->server);
        calls.append(call);
    }
    return result;
}

bool SimulatorAll::Server::addCallPartially(SimulatorAll::Call *call, int noOfAs)
{
    return false;
}

bool SimulatorAll::Server::addCallPartially(SimulatorAll::Call *call, int noOfAs, int groupNumber)
{
    return false;
}

void SimulatorAll::Server::removeCall(SimulatorAll::Call *call)
{
    state.removeCall(call, call->server);
    call->server.allocatedAU = 0;
#ifdef QT_DEBUG
    int noOfRemovedCalls = calls.removeAll(call);
    assert(noOfRemovedCalls == 1);
    if (noOfRemovedCalls != 1)
        qFatal("Con't remove call from server. Implementation error. No of removed calls %d", noOfRemovedCalls);
#else
    calls.removeOne(call);
#endif
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
            singleResults.write(TypeForServerState::IntensityNewCallOutOffered, result, n);

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

int SimulatorAll::CommonRes::getMaxNumberOfAsInSingleGroup() const
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

#ifdef QT_DEBUG
    this->state = ProcAll::ProcState::WAITING_FOR_NEW_CALL;
#endif
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

#ifdef QT_DEBUG
    this->state = ProcAll::ProcState::WAITING_FOR_NEW_CALL;
#endif
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

#ifdef QT_DEBUG
    if (proc->callData->classIdx > system->par.m())
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

#ifdef QT_DEBUG
    newProc->state = ProcAll::ProcState::WAITING_FOR_NEW_CALL;
#endif
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

#ifdef QT_DEBUG
    if (proc->callData->classIdx > system->par.m())
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

#ifdef QT_DEBUG
        if (newProc->callData->classIdx > system->par.m())
            qFatal("Wrong class idx");
        newProc->state = ProcAll::ProcState::WAITING_FOR_NEW_CALL;
#endif
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

#ifdef QT_DEBUG
    if (proc->callData->classIdx > system->par.m())
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

#ifdef QT_DEBUG
    procNewCall->state = ProcAll::ProcState::WAITING_FOR_NEW_CALL;
#endif
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
#ifdef QT_DEBUG
        newProc->state = ProcAll::ProcState::WAITING_FOR_NEW_CALL;
#endif
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
#ifdef QT_DEBUG
    if (proc->callData->classIdx > system->par.m())
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

#ifdef QT_DEBUG
    if (proc->callData->classIdx > system->par.m())
        qFatal("Wrong class idx");
    newProc->state = ProcAll::ProcState::WAITING_FOR_NEW_CALL;
#endif
    system->endCallService(proc->callData);

    proc->callData = nullptr;
    system->engine->reuseProcess(proc);

    //Adding new process with state Waiting for new call

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

#ifdef QT_DEBUG
    if (proc->state == ProcState::USELESS)\
        qFatal("Wrong Process state");
#endif
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
    DUtransfered += (time * server.allocatedAU);
}
#define FOLDINEND }

#define FOLDINGSTART { // buffer

SimulatorAll::Buffer::Buffer(SimulatorAll::System *system): CommonRes (system, system->par.getBuffer())
{
    statistics = new BufferStatistics(system->par);
}

void SimulatorAll::Buffer::addCall(SimulatorAll::Call *nCall, int groupNo)
{
    nCall->buffer.groupIndex = groupNo;
    nCall->buffer.allocatedAU = nCall->reqAS - nCall->server.allocatedAU;
    state.addCall(nCall, nCall->buffer);
    calls.append(nCall);
#ifdef QT_DEBUG
    if (vTotal < state.n)
        qFatal("Implementation error in simulator. Buffer has wrong state %d. Buffer capacity id %d", state.n, vTotal);

    if (system->par.getBuffer().V(groupNo) != state.n_k[groupNo] + state.subgroupFreeAUs[groupNo])
        qFatal("mplementation error in simulator. Wrong group in buffer. Group %d capacity %d. Group state %d occupied, %d free"
          , groupNo, system->par.getBuffer().V(groupNo), state.n_k[groupNo], state.subgroupFreeAUs[groupNo]);
#endif
}

void SimulatorAll::Buffer::removeCall(SimulatorAll::Call *rCall)
{
    state.removeCall(rCall, rCall->buffer);
    rCall->buffer.allocatedAU = 0;
#ifdef QT_DEBUG
    int noOfRemovedCalls = calls.removeAll(rCall);

    if (noOfRemovedCalls != 1)
        qFatal("Implementation error in simulator. Can't remove call from buffer. %d calls was removed", noOfRemovedCalls);
    if (system->par.getBuffer().V(rCall->buffer.groupIndex) != state.n_k[rCall->buffer.groupIndex] + state.subgroupFreeAUs[rCall->buffer.groupIndex]);
        qFatal("Implementation error in simulator. Wrong buffer state after removing call");
#else
    calls.removeOne(rCall);
#endif
}

SimulatorAll::Call *SimulatorAll::Buffer::popCall()
{
    Call *result = calls.takeFirst();
    state.removeCall(result, result->buffer);
    result->buffer.allocatedAU = 0;

    return result;
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

void SimulatorAll::Buffer::writesResultsOfSingleExperiment(RSingle &singleResults, double simulationTime)
{
#warning "Implement it"
//    statistics->

}


SimulatorAll::CommonRes::CommonRes(SimulatorAll::System *system, const ModelResourcess &res)
    : system(system)
    , scheduler(res.schedulerAlg)
    , vTotal(res.V())
    , vMax(res.vMax())
    , k(res.k())
    , m(system->par.m())
{
    state.n = 0;
    state.n_k.resize(k);
    state.n_i.resize(m);

    state.subgroupFreeAUs.resize(k);
    state.subgroupSequence.resize(k);
    for (int j=0; j<k; j++)
    {
        state.subgroupFreeAUs[j] = res.V(j);
        state.subgroupSequence[j] = j;
        state.n_k[j] = 0;
    }
    for (int i=0; i<m; i++)
    {
        state.n_i[i] = 0;
    }
}

SimulatorAll::CommonRes::~CommonRes()
{

}

#define FOLDINEND }

 }
// namespace Algorithms
