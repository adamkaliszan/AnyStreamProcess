#include "simulatorQeueSdFIFO.h"
#include <qstring.h>
#include <qglobal.h>

namespace Algorithms
{

SimulatorQeueSdFifo::SimulatorQeueSdFifo(): simulator(BufferResourcessScheduler::SD_FIFO) { }

bool SimulatorQeueSdFifo::possible(const ModelSyst *system) const
{
    if (system->vk_b() == 0)
        return false;
    return Investigator::possible(system);
}

QSet<Results::Type> SimulatorQeueSdFifo::getQoS_Set(const ModelSyst *system) const
{
    (void) system;
    QSet<Results::Type> result;

    result << Results::Type::BlockingProbability;
    result << Results::Type::OccupancyDistribution;

    return result;
}



void SimulatorQeueSdFifo::calculateSystem(const ModelSyst *system
      , double a
      , RInvestigator *results
      , SimulationParameters *simParameters
      )
{
    //if (this->isItTheSameSystem(system))
    //    return;

    this->system = system;
    System *simData = new System(system, simParameters->noOfSeries);
    simData->initialize(a, system->totalAt(), system->vk_s());

    int seed = 1024;

    int noOfSeries = simParameters->noOfSeries;

    for (int serNo=0; serNo<noOfSeries; serNo++)
    {
        seed = qrand();
        simData->disableStatisticCollection();
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
        simData->enableStatisticscollection(serNo);
        simData->doSimExperiment(simParameters->noOfLostCalls, seed, simParameters->noOfServicedCalls);
        simData->writesResultsOfSingleExperiment((*results)[serNo]);
        qDebug("simulation experiment no %d", serNo+1);
    }
    //emit this->sigCalculationDone();
}

SimulatorQeueSdFifo::System::System(const ModelSyst *system, int noOfSeries)
    : results(system->m(), system->vk_s(), system->vk_b(), noOfSeries)
    , systemData(system)
{
    this->agenda = new simulatorDataCollection<ProcQeueSdFifo>();
    server = new Server(system->vk_s(), system->vk_b(), this);
}

void SimulatorQeueSdFifo::System::initialize(double a, int sumPropAt, int V)
{
    for(int i=0; i<systemData->m(); i++)
    {
        const ModelTrClass *tmpClass = systemData->getClass(i);
        ProcQeueSdFifo::initialize(this, tmpClass, i, a, sumPropAt, V);
    }
}

void SimulatorQeueSdFifo::System::doSimExperiment(int numberOfLostCall, int seed, int numberOfServicedCalls=0)
{
    qsrand(seed);
    this->totalNumberOfLostCalls = 0;
    this->totalNumberOfServicedCalls = 0;

    if (numberOfServicedCalls == 0)
    {
        while(totalNumberOfLostCalls < numberOfLostCall)
        {
            ProcQeueSdFifo *proc = takeFirstProcess();
            collectTheStat(proc->time);
            proc->execute(proc, this);
        }
    }
    else
    {
        if(totalNumberOfLostCalls > 0)
        {
            while(totalNumberOfLostCalls < numberOfLostCall && totalNumberOfServicedCalls < numberOfServicedCalls)
            {
                ProcQeueSdFifo *proc = takeFirstProcess();
                collectTheStat(proc->time);
                proc->execute(proc, this);
            }
        }
        else
        {
            while(totalNumberOfServicedCalls < numberOfServicedCalls)
            {
                ProcQeueSdFifo *proc = takeFirstProcess();
                collectTheStat(proc->time);
                proc->execute(proc, this);
            }
        }
    }


/*
    if (numberOfLostCall != 0 && numberOfServicedCalls == 0)
    {
        while(totalNumberOfLostCalls < numberOfLostCall)
        {
            ProcQeueSdFifo *proc = takeFirstProcess();
            collectTheStat(proc->time);
            proc->execute(proc, this);
        }
        return;
    }

    if (numberOfLostCall != 0 && numberOfServicedCalls != 0)
    {
        while(totalNumberOfLostCalls < numberOfLostCall && totalNumberOfServicedCalls < numberOfServicedCalls)
        {
            ProcQeueSdFifo *proc = takeFirstProcess();
            collectTheStat(proc->time);
            proc->execute(proc, this);
        }
        return;
    }

    if (numberOfLostCall == 0 && numberOfServicedCalls != 0)
    {
        while(totalNumberOfServicedCalls < numberOfServicedCalls)
        {
            ProcQeueSdFifo *proc = takeFirstProcess();
            collectTheStat(proc->time);
            proc->execute(proc, this);
        }
        return;
    }
    */
}

void SimulatorQeueSdFifo::System::writesResultsOfSingleExperiment(Results::RSingle& singleRes)
{
    for (int i=0; i<this->systemData->m(); i++)
    {
        double E=0;
        results.act_E[i] = 0;
        for (int n=server->getV()+server->getVc() - systemData->getClass(i)->t() + 1; n<=server->getV() + server->getVc(); n++)
            E+= server->getOccupancyTimeOfState(n);
        E/= results._simulationTime;
        results.act_E[i] = E;
        singleRes.write(TypeForClass::BlockingProbability, E, i);

        results.act_y[i]  = server->getAvarageNumberOfCalls(i)/results._simulationTime;

        results.act_t[i] /= results.act_noOfServicedCalls[i];
        results.act_tS[i] /= results.act_noOfServicedCalls[i];

        for (int n=0; n<=server->getV() + server->getVc(); n++)
        {
            double x = server->getASSystemForClassInState(i, n);
            x/= server->getOccupancyTimeOfState(n);
            results.act_SYS_yt[i][n] = x;
        }
    }
    //Calculating avarqge "qeue" length. Avarage number of calls that cause, that system is under compression
    *results.act_Qlen = 0;
    for (int n=1; n<=server->getVc(); n++)
        *results.act_Qlen += n*server->getOccupancyTimeOfState(server->getV()+n);
    *results.act_Qlen /= results._simulationTime;

    for (int n=0; n<=server->getV() + server->getVc(); n++)
        results.act_trDistribSys[n] = (server->getOccupancyTimeOfState(n)/ results._simulationTime);
}

void SimulatorQeueSdFifo::System::serveNewCall(SimulatorQeueSdFifo::Call *newCall)
{
    bool compressionChanged;

    if (server->addCall(newCall, compressionChanged))
    {
        ProcQeueSdFifo *proc = getNewProcess();
        proc->callData = newCall;
        newCall->proc = proc;
        proc->execute = ProcQeueSdFifo::transmisionEnded;
        proc->state = ProcQeueSdFifo::SENDING_DATA;
        proc->time = (newCall->plannedServiceTime * newCall->compression);
        addProcess(proc);

        if (compressionChanged)
            this->changeServiceTimeOfAllTheCalls();
    }
    else
    {
        FinishCall(newCall, false);
    }
}

void SimulatorQeueSdFifo::System::addProcess(ProcQeueSdFifo *proc)
{
#ifndef DO_NOT_USE_SECUTIRY_CHECKS
    if (proc->time < 0)
        qFatal("Negative time value");
    if (proc->callData->classIdx > this->systemData->m())
        qFatal("Wrong class idx");
#endif
    agenda->addProcess(proc);
}

SimulatorQeueSdFifo::Call *SimulatorQeueSdFifo::System::getNewCall(SimulatorQeueSdFifo::Call *parent)
{
    Call *result = _getNewCall();
    result->fillData(parent);

    result->timeOnBuffer   = 0;
    result->timeOnSystem   = 0;
    return result;
}

SimulatorQeueSdFifo::Call *SimulatorQeueSdFifo::System::getNewCall(
          const ModelTrClass *trClass
        , int classIdx
        , double incE
        , double incD
        , double servE
        , double servD
        )
{
    Call *result = _getNewCall();

    result->reqAS = trClass->t();

    result->trClass = trClass;
    result->classIdx = classIdx;
    result->sourceE = incE;
    result->sourceD = incD;
    result->serviceE = servE,
    result->serviceD = servD;

    result->timeOnBuffer   = 0;
    result->timeOnSystem   = 0;

    return result;
}

SimulatorQeueSdFifo::Call *SimulatorQeueSdFifo::System::_getNewCall()
{
    if(uselessCalls.capacity())
        return uselessCalls.pop();
    return new Call;
}

void SimulatorQeueSdFifo::System::reuseCall(SimulatorQeueSdFifo::Call *callToReuse)
{
    uselessCalls.push(callToReuse);
}

void SimulatorQeueSdFifo::System::reuseProcess(ProcQeueSdFifo *proc)
{
    proc->idx = 0;
#ifndef DO_NOT_USE_SECUTIRY_CHECKS
    proc->setUseless();
#endif
    agenda->reuseProcess(proc);
}

void SimulatorQeueSdFifo::System::FinishCall(SimulatorQeueSdFifo::Call *call, bool acceptedToService)
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
        results.act_t[call->classIdx] += call->timeOnBuffer;
        results.act_t[call->classIdx] += call->timeOnBuffer;
        results.act_tS[call->classIdx] += call->timeOnSystem;
        results.act_noOfServicedCalls[call->classIdx] ++;
        totalNumberOfServicedCalls++;
    }
    reuseCall(call);
}

void SimulatorQeueSdFifo::System::changeServiceTimeOfAllTheCalls()
{
    foreach (Call *tmpCall, server->calls)
    {
        tmpCall->compression = server->getCompression();
        double timeToEnd = (tmpCall->plannedServiceTime - tmpCall->effectiveServiceTime) * tmpCall->compression;
        agenda->changeProcessWakeUpTime(tmpCall->proc, timeToEnd);
    }
}

void SimulatorQeueSdFifo::System::collectTheStat(double time)
{
    results._simulationTime +=time;

    foreach(Call *tmpCall, server->calls)
        tmpCall->collectTheStats(time);

    server->collectTheStats(time);
}

void SimulatorQeueSdFifo::System::enableStatisticscollection(int serNo)
{
    results.enableStatisticscollection(serNo);
    server->clearTheStats();
}

void SimulatorQeueSdFifo::System::disableStatisticCollection()
{
    results.disableStatisticCollection();
}

void SimulatorQeueSdFifo::Server::collectTheStats(double time)
{
#ifndef DO_NOT_USE_SECUTIRY_CHECKS
    if (n>V + Vc)
        qFatal("void SimulatorCompression::Server::collectTheStats: Wrong n: %d", n);
#endif
    occupancyTimes[n] += time;

    for (int i=0; i<system->systemData->m(); i++)
        AStime_ofOccupiedAS_byClassI_inStateN[i][n] +=(time * numberOfClassesInSystem[i]);
}

bool SimulatorQeueSdFifo::Server::addCall(Call *call, bool &compressionChanged)
{
    if (n+call->reqAS > V+Vc)
        return false;

    avarageNumberOfCalls[call->classIdx]++;
    n += call->reqAS;
    numberOfClassesInSystem[call->classIdx] +=call->reqAS;
    calls.append(call);
    double oldCompression = compression;
    compression = qMax(1.0, (double)n/(double)V);
    compressionChanged = (bool)(oldCompression != compression);
    call->compression = compression;
    return true;
}

void SimulatorQeueSdFifo::Server::removeCall(SimulatorQeueSdFifo::Call *call, bool &compressionChanged)
{
    avarageNumberOfCalls[call->classIdx]--;
    double oldCompression = compression;
    n -=call->reqAS;
    numberOfClassesInSystem[call->classIdx] -=call->reqAS;
    compression = qMax(1.0, (double)(n)/(double)(V));
    compressionChanged = (bool)(oldCompression != compression);
#ifndef DO_NOT_USE_SECUTIRY_CHECKS
    int numberOfRemovedCalls = calls.removeAll(call);
    if (numberOfRemovedCalls != 1)
        qFatal("Wrong number of removed calls %d", numberOfRemovedCalls);
#else
    calls.removeOne(call);
#endif
}

SimulatorQeueSdFifo::Server::Server(int numberOfAS, int compCapacity, System *system): system(system), V(numberOfAS), Vc(compCapacity), compression(0)
{
    occupancyTimes       = new double[V+Vc+1];
    avarageNumberOfCalls = new double[system->systemData->m()];

    AStime_ofOccupiedAS_byClassI_inStateN = new double*[system->systemData->m()];
    for (int i=0; i<system->systemData->m(); i++)
    {
        AStime_ofOccupiedAS_byClassI_inStateN[i] = new double[V+Vc+1];
        //bzero(AStime_ofOccupiedAS_byClassI_inStateN[i], (V+Vc+1)*sizeof(double));
    }
    bzero(occupancyTimes,       (V+Vc+1)*sizeof(double));
    bzero(avarageNumberOfCalls, (system->systemData->m())*sizeof(double));
    n = 0;

    numberOfClassesInSystem = new int[system->systemData->m()];
    bzero(numberOfClassesInSystem, system->systemData->m()*sizeof(int));
}

void ProcQeueSdFifo::initialize(
          SimulatorQeueSdFifo::System *system
        , const ModelTrClass *trClass
        , int classIdx
        , double a
        , int sumPropAt
        , int V
        )
{
    ProcQeueSdFifo *newEvent;
    newEvent = system->getNewProcess();

    switch(trClass->srcType())
    {
    case ModelTrClass::SourceType::Independent:
        switch (trClass->newCallStr())
        {
        case ModelTrClass::StreamType::Poisson:
            switch (trClass->callServStr())
            {
            case ModelTrClass::StreamType::Poisson:
                newEvent->initializeErlangMM(system, trClass, classIdx, a, sumPropAt, V);
                break;
            case ModelTrClass::StreamType::Uniform:
                newEvent->initializeErlangMU(system, trClass, classIdx, a, sumPropAt, V);
                break;
            case ModelTrClass::StreamType::Normal:
                newEvent->initializeErlangMN(system, trClass, classIdx, a, sumPropAt, V);
                break;
            case ModelTrClass::StreamType::Gamma:
            case ModelTrClass::StreamType::Pareto:
                qFatal("Not implemented");
            }
            break;
        case ModelTrClass::StreamType::Uniform:
            switch (trClass->callServStr())
            {
            case ModelTrClass::StreamType::Poisson:
                newEvent->initializeErlangUM(system, trClass, classIdx, a, sumPropAt, V);
                break;
            case ModelTrClass::StreamType::Uniform:
                newEvent->initializeErlangUU(system, trClass, classIdx, a, sumPropAt, V);
                break;
            case ModelTrClass::StreamType::Normal:
                newEvent->initializeErlangUN(system, trClass, classIdx, a, sumPropAt, V);
                break;
            case ModelTrClass::StreamType::Gamma:
            case ModelTrClass::StreamType::Pareto:
                qFatal("Not implemented");
            }
            break;
        case ModelTrClass::StreamType::Normal:
            switch (trClass->callServStr())
            {
            case ModelTrClass::StreamType::Poisson:
                newEvent->initializeErlangNM(system, trClass, classIdx, a, sumPropAt, V);
                break;
            case ModelTrClass::StreamType::Uniform:
                newEvent->initializeErlangNU(system, trClass, classIdx, a, sumPropAt, V);
                break;
            case ModelTrClass::StreamType::Normal:
                newEvent->initializeErlangNN(system, trClass, classIdx, a, sumPropAt, V);
                break;
            case ModelTrClass::StreamType::Gamma:
            case ModelTrClass::StreamType::Pareto:
                qFatal("Not implemented");
            }
            break;
        case ModelTrClass::StreamType::Gamma:
        case ModelTrClass::StreamType::Pareto:
            qFatal("Not implemented");
        }
        break;

    default:
        qDebug("Not supported clas type");
        break;
    }
}

void ProcQeueSdFifo::initializeErlang(
          SimulatorQeueSdFifo::System *system
        , const ModelTrClass *trClass
        , int idx
        , double a
        , int sumPropAt
        , int V
        , double (*funTimeNewCall)(double, double)
        , void (*funNewCall)(ProcQeueSdFifo *, SimulatorQeueSdFifo::System *)
        )
{
    double incEx = 1.0/trClass->intensityNewCallTotal(a, V, sumPropAt);
    double incD = incEx * incEx/trClass->getIncommingExPerDx();
    double servEx = 1.0/trClass->getMu();
    double servD = servEx * servEx/trClass->getServiceExPerDx();

    this->callData = system->getNewCall(trClass, idx, incEx, incD, servEx, servD);

    this->state = ProcQeueSdFifo::WAITING_FOR_NEW;
    this->execute = funNewCall;
    this->time = funTimeNewCall(this->callData->sourceE, this->callData->sourceD);
    system->addProcess(this);
}



void ProcQeueSdFifo::newCallErlang(ProcQeueSdFifo *proc, SimulatorQeueSdFifo::System *system,
                                   double (*funTimeNewCall)(double, double), double (*funTimeOfService)(double, double),
                                   void (*funNewCall)(ProcQeueSdFifo *, SimulatorQeueSdFifo::System *))
{
#ifndef DO_NOT_USE_SECUTIRY_CHECKS
    if (proc->callData->classIdx > system->systemData->m())
        qFatal("Wrong class idx");
#endif
    SimulatorQeueSdFifo::Call *callData = proc->callData;
    callData->plannedServiceTime   = funTimeOfService(callData->serviceE, callData->serviceD);
    callData->effectiveServiceTime = 0;
    callData->reqAS                = proc->callData->reqAS;

    callData->compression          = 1;
    system->serveNewCall(callData);

    //Adding new process with state Waiting for new call
    ProcQeueSdFifo *newProc = proc;
    newProc->callData = system->getNewCall(proc->callData);

    newProc->state = ProcQeueSdFifo::WAITING_FOR_NEW;
    newProc->execute = funNewCall;
    newProc->time = funTimeNewCall(newProc->callData->sourceE, newProc->callData->sourceD);
    system->addProcess(newProc);
}

/*
void ProcQeueSdFifo::newCallEngset(ProcQeueFifo *proc, SimulatorQeueFifo::System *system)
{
#ifndef DO_NOT_USE_SECUTIRY_CHECKS
    if (proc->callData->classIdx > system->systemData->m())
        qFatal("Wrong class idx");
#endif
    SimulatorQeueFifo::Call *callData = proc->callData;
    callData->plannedServiceTime = simulator::distrLambda(callData->serviceE);
    callData->reqAS              = proc->callData->reqAS;
    callData->DUmessageSize      = callData->reqAS * callData->plannedServiceTime;
    callData->proc               = nullptr;


    if (system->serveNewCall(callData) == false)
    {
        ProcQeueFifo *newProc = system->getNewProcess();
        newProc->callData = system->getNewCall(callData);

    #ifndef DO_NOT_USE_SECUTIRY_CHECKS
        if (newProc->callData->classIdx > system->systemData->m())
            qFatal("Wrong class idx");
    #endif
        newProc->state = ProcQeueFifo::WAITING_FOR_NEW_CALL;
        newProc->execute = ProcQeueFifo::newCallEngset;
        newProc->time = simulator::distrLambda(newProc->callData->sourceE);
        system->addProcess(newProc);
    }

    system->reuseProcess(proc);
}

void ProcQeueFifo::newCallEngsetMD(ProcQeueFifo *proc, SimulatorQeueFifo::System *system)
{
#ifndef DO_NOT_USE_SECUTIRY_CHECKS
    if (proc->callData->classIdx > system->systemData->m())
        qFatal("Wrong class idx");
#endif
    SimulatorQeueFifo::Call *callData = proc->callData;
    callData->plannedServiceTime = simulator::distrUniform(callData->serviceTmin, callData->serviceTmax);
    callData->reqAS              = proc->callData->reqAS;
    callData->DUmessageSize      = callData->reqAS * callData->plannedServiceTime;
    callData->proc               = nullptr;

    if (system->serveNewCall(callData) == false)
    {
        ProcQeueFifo *newProc = system->getNewProcess();
        newProc->callData = system->getNewCall(callData);

    #ifndef DO_NOT_USE_SECUTIRY_CHECKS
        if (newProc->callData->classIdx > system->systemData->m())
            qFatal("Wrong class idx");
    #endif
        newProc->state = ProcQeueFifo::WAITING_FOR_NEW_CALL;
        newProc->execute = ProcQeueFifo::newCallEngsetMD;
        newProc->time = simulator::distrLambda(newProc->callData->sourceE);
        system->addProcess(newProc);
    }

    system->reuseProcess(proc);
}

void ProcQeueFifo::newCallEngsetMN(ProcQeueFifo *proc, SimulatorQeueFifo::System *system)
{
#ifndef DO_NOT_USE_SECUTIRY_CHECKS
    if (proc->callData->classIdx > system->systemData->m())
        qFatal("Wrong class idx");
#endif
    SimulatorQeueFifo::Call *callData = proc->callData;
    callData->plannedServiceTime = simulator::distrLambda(callData->serviceE);
    callData->reqAS              = proc->callData->reqAS;
    callData->DUmessageSize      = callData->reqAS * callData->plannedServiceTime;
    callData->proc               = nullptr;


    if (system->serveNewCall(callData) == false)
    {
        ProcQeueFifo *newProc = system->getNewProcess();
        newProc->callData = system->getNewCall(callData);

    #ifndef DO_NOT_USE_SECUTIRY_CHECKS
        if (newProc->callData->classIdx > system->systemData->m())
            qFatal("Wrong class idx");
    #endif
        newProc->state = ProcQeueFifo::WAITING_FOR_NEW_CALL;
        newProc->execute = ProcQeueFifo::newCallEngsetMN;
        newProc->time = simulator::distrNormal(newProc->callData->sourceE, newProc->callData->sourceD);
        system->addProcess(newProc);
    }

    system->reuseProcess(proc);
}


void ProcQeueFifo::newCallPascal(ProcQeueFifo *proc, SimulatorQeueFifo::System *system)
{
#ifndef DO_NOT_USE_SECUTIRY_CHECKS
    if (proc->callData->classIdx > system->systemData->m())
        qFatal("Wrong class idx");
#endif
    SimulatorQeueFifo::Call *callData           = proc->callData;
    SimulatorQeueFifo::Call *parentServicedCall = callData->complementaryCall;

    callData->plannedServiceTime = simulator::distrLambda(callData->serviceE);
    callData->reqAS              = proc->callData->reqAS;
    callData->DUmessageSize      = callData->reqAS * callData->plannedServiceTime;
    callData->proc               = nullptr;

    ProcQeueFifo *procNewCall = system->getNewProcess();
    procNewCall->callData = system->getNewCall(callData);
    procNewCall->callData->proc = procNewCall;

    if (parentServicedCall)
    {
        procNewCall->callData->complementaryCall = parentServicedCall;
        parentServicedCall->complementaryCall = procNewCall->callData;
        callData->complementaryCall = nullptr;
    }

    procNewCall->state = ProcQeueFifo::WAITING_FOR_NEW_CALL;
    procNewCall->execute = ProcQeueFifo::newCallPascal;
    procNewCall->time = simulator::distrLambda(procNewCall->callData->sourceE);
    system->addProcess(procNewCall);

    if (system->serveNewCall(callData) == true)
    {
        ProcQeueFifo *newProc = system->getNewProcess();
        newProc->callData = system->getNewCall(callData);
        newProc->callData->proc = newProc;
        callData->complementaryCall = newProc->callData;
        newProc->callData->complementaryCall = callData;

        newProc->state = ProcQeueFifo::WAITING_FOR_NEW_CALL;
        newProc->execute = ProcQeueFifo::newCallPascal;
        newProc->time = simulator::distrLambda(newProc->callData->sourceE);
        system->addProcess(newProc);
    }
    system->reuseProcess(proc);
}

void ProcQeueFifo::newCallPascalMD(ProcQeueFifo *proc, SimulatorQeueFifo::System *system)
{
#ifndef DO_NOT_USE_SECUTIRY_CHECKS
    if (proc->callData->classIdx > system->systemData->m())
        qFatal("Wrong class idx");
#endif
    SimulatorQeueFifo::Call *callData           = proc->callData;
    SimulatorQeueFifo::Call *parentServicedCall = callData->complementaryCall;

    callData->plannedServiceTime = simulator::distrUniform(callData->serviceTmin, callData->serviceTmax);
    callData->reqAS              = proc->callData->reqAS;
    callData->DUmessageSize      = callData->reqAS * callData->plannedServiceTime;
    callData->proc               = nullptr;

    ProcQeueFifo *procNewCall = system->getNewProcess();
    procNewCall->callData = system->getNewCall(callData);
    procNewCall->callData->proc = procNewCall;

    if (parentServicedCall)
    {
        procNewCall->callData->complementaryCall = parentServicedCall;
        parentServicedCall->complementaryCall = procNewCall->callData;
        callData->complementaryCall = nullptr;
    }

    procNewCall->state = ProcQeueFifo::WAITING_FOR_NEW_CALL;
    procNewCall->execute = ProcQeueFifo::newCallPascalMD;
    procNewCall->time = simulator::distrLambda(procNewCall->callData->sourceE);
    system->addProcess(procNewCall);

    if (system->serveNewCall(callData) == true)
    {
        ProcQeueFifo *newProc = system->getNewProcess();
        newProc->callData = system->getNewCall(callData);
        newProc->callData->proc = newProc;
        callData->complementaryCall = newProc->callData;
        newProc->callData->complementaryCall = callData;

        newProc->state = ProcQeueFifo::WAITING_FOR_NEW_CALL;
        newProc->execute = ProcQeueFifo::newCallPascalMD;
        newProc->time = simulator::distrLambda(procNewCall->callData->sourceE);
        system->addProcess(newProc);
    }
    system->reuseProcess(proc);
}

void ProcQeueFifo::newCallPascalMN(ProcQeueFifo *proc, SimulatorQeueFifo::System *system)
{
#ifndef DO_NOT_USE_SECUTIRY_CHECKS
    if (proc->callData->classIdx > system->systemData->m())
        qFatal("Wrong class idx");
#endif
    SimulatorQeueFifo::Call *callData           = proc->callData;
    SimulatorQeueFifo::Call *parentServicedCall = callData->complementaryCall;

    callData->plannedServiceTime = simulator::distrNormal(callData->serviceE, callData->serviceE);
    callData->reqAS              = proc->callData->reqAS;
    callData->DUmessageSize      = callData->reqAS * callData->plannedServiceTime;
    callData->proc               = nullptr;

    ProcQeueFifo *procNewCall = system->getNewProcess();
    procNewCall->callData = system->getNewCall(callData);
    procNewCall->callData->proc = procNewCall;

    if (parentServicedCall)
    {
        procNewCall->callData->complementaryCall = parentServicedCall;
        parentServicedCall->complementaryCall = procNewCall->callData;
        callData->complementaryCall = nullptr;
    }

    procNewCall->state = ProcQeueFifo::WAITING_FOR_NEW_CALL;
    procNewCall->execute = ProcQeueFifo::newCallPascalMN;
    procNewCall->time = simulator::distrNormal(procNewCall->callData->sourceE, procNewCall->callData->sourceD);
    system->addProcess(procNewCall);

    if (system->serveNewCall(callData) == true)
    {
        ProcQeueFifo *newProc = system->getNewProcess();
        newProc->callData = system->getNewCall(callData);
        newProc->callData->proc = newProc;
        callData->complementaryCall = newProc->callData;
        newProc->callData->complementaryCall = callData;

        newProc->state = ProcQeueFifo::WAITING_FOR_NEW_CALL;
        newProc->execute = ProcQeueFifo::newCallPascalMN;
        newProc->time = simulator::distrNormal(newProc->callData->sourceE, newProc->callData->sourceD);
        system->addProcess(newProc);
    }
    system->reuseProcess(proc);
}
*/

void ProcQeueSdFifo::transmisionEnded(ProcQeueSdFifo *proc, SimulatorQeueSdFifo::System *system)
{
#ifndef DO_NOT_USE_SECUTIRY_CHECKS
    if (proc->callData->classIdx > system->systemData->m())
        qFatal("Wrong class idx");
#endif

    bool compressionChaned;
    system->server->removeCall(proc->callData, compressionChaned);
    system->FinishCall(proc->callData, true);

    if (compressionChaned)
        system->changeServiceTimeOfAllTheCalls();

    proc->callData = NULL;
    system->reuseProcess(proc);
}

void SimulatorQeueSdFifo::Call::collectTheStats(double time)
{
    timeOnSystem +=time;
    effectiveServiceTime +=(time/compression);
}

} // namespace Algorithms
