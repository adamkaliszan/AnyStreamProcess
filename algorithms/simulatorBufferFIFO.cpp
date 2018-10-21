#include <qstring.h>
#include <qglobal.h>
#include "simulatorBufferFIFO.h"
#include "results/resultsInvestigator.h"

namespace Algorithms
{

SimulatorBufferFifo::SimulatorBufferFifo(QueueServDiscipline qDisc): simulator(qDisc)
{
    myQoS_Set
       <<Results::Type::BlockingProbability
       <<Results::Type::OccupancyDistribution;
}

bool SimulatorBufferFifo::possible(ModelSyst *system)
{
    if (system->vk_b() == 0)
        return false;
    return Investigator::possible(system);
}

void SimulatorBufferFifo::calculateSystem(const ModelSyst *system
        , double a
        , RInvestigator *results
        , SimulationParameters *simParameters)
{
    //if (this->isItTheSameSystem(system))
    //    return;
    this->system = system;
    System *simData = new System(system, simParameters->noOfSeries);
    simData->initialize(a, system->totalAt(), system->vk_s());

    int seed = 1024;

    int noOfSeries = 10;

    for (int serNo=0; serNo<noOfSeries; serNo++)
    {
        seed = qrand();
        simData->disableStatisticCollection();
        if (serNo == 0)
            simData->doSimExperiment(simParameters->noOfLostCalls/simParameters->spaceOnStart, seed);
        else
            simData->doSimExperiment(simParameters->noOfLostCalls/simParameters->spaceBetweenSeries, seed);

        simData->enableStatisticscollection(serNo);
        simData->doSimExperiment(simParameters->noOfLostCalls, seed);
        simData->writesResultsOfSingleExperiment();
        simData->writeResults((*results)[serNo]);
        qDebug("simulation experiment no %d", serNo+1);
    }
    //emit this->sigCalculationDone();
}

SimulatorBufferFifo::System::System(const ModelSyst *system, int numberOfSeries): _simulationTime(0)
{
    avgNumberOfCalls = new double[system->m()];

    __act_yQ                      = new double[system->m()];
    __act_y                       = new double[system->m()];
    __act_t                       = new double[system->m()];
    __act_tS                      = new double[system->m()];
    __act_Qlen                    = new double;
    __act_E                       = new double[system->m()];
    __act_noOfLostCallsBeforeQeue = new int[system->m()];
    __act_noOfLostCallsInQeue     = new int[system->m()];
    __act_noOfCalls               = new int[system->m()];
    __act_noOfServicedCalls       = new int[system->m()];

    this->systemData = system;
    this->agenda = new simulatorDataCollection<ProcBufferFifo>();


    int32_t *v, *k, numberOfTypes;
    system->getLinkParameters(&k, &v, &numberOfTypes);
    server = new Server(system->vk_s(), this);
    delete []v;
    delete []k;

    buffer = new Buffer(system->vk_b(), system->vk_b() * system->T(), this);

    yQ   = new double*[numberOfSeries];
    y    = new double*[numberOfSeries];
    t    = new double*[numberOfSeries];
    tS   = new double*[numberOfSeries];
    Qlen = new double[numberOfSeries];
    E    = new double*[numberOfSeries];
    Y    = new double*[numberOfSeries];

    noOfLostCallsBeforeQeue = new int*[numberOfSeries];
    noOfLostCallsInQeue = new int*[numberOfSeries];
    noOfCalls = new int*[numberOfSeries];
    noOfServicedCalls = new int*[numberOfSeries];

    for (int serNo=0; serNo<numberOfSeries; serNo++)
    {
        yQ[serNo] = new double[system->m()];
        y [serNo] = new double[system->m()];
        t [serNo] = new double[system->m()];
        tS[serNo] = new double[system->m()];
        E [serNo] = new double[system->m()];
        Y [serNo] = new double[system->m()];

        noOfLostCallsBeforeQeue[serNo] = new int[system->m()];
        noOfLostCallsInQeue[serNo]     = new int[system->m()];
        noOfCalls[serNo]               = new int[system->m()];
        noOfServicedCalls[serNo]       = new int[system->m()];
    }
}

void SimulatorBufferFifo::System::initialize(double a, int sumPropAt, int V)
{
    for(int i=0; i<systemData->m(); i++)
    {
        const ModelTrClass *tmpClass = systemData->getClass(i);

        ProcBufferFifo::initialize(this, tmpClass, i, a, sumPropAt, V);
    }
}

void SimulatorBufferFifo::System::doSimExperiment(int numberOfLostCall, int seed)
{
    qsrand(seed);
    this->totalNumberOfLostCalls = 0;
    while(totalNumberOfLostCalls < numberOfLostCall)
    {
        ProcBufferFifo *proc = takeFirstProcess();

        collectTheStat(proc->time);      

        proc->execute(proc, this);
    }
}

void SimulatorBufferFifo::System::writeResults(RSingle &results)
{
    for (int i=0; i<systemData->m(); i++)
    {
        results.write(TypeForClass::BlockingProbability, act_E[i], i);
    }
}

void SimulatorBufferFifo::System::disableStatisticCollection()
{
    act_yQ    = __act_yQ;
    act_y     = __act_y;
    act_t     = __act_t;
    act_tS    = __act_tS;
    act_Qlen  = __act_Qlen;
    act_E     = __act_E;
    act_Y     = __act_Y;

    act_noOfLostCallsBeforeQeue = __act_noOfLostCallsBeforeQeue;
    act_noOfLostCallsInQeue     = __act_noOfLostCallsInQeue;
    act_noOfCalls               = __act_noOfCalls;
    act_noOfServicedCalls       = __act_noOfServicedCalls;

    *act_Qlen = 0;
    for (int i=0; i<this->systemData->m(); i++)
    {
        act_yQ[i] = 0;
        act_y[i]  = 0;
        act_t[i]  = 0;
        act_tS[i] = 0;
        act_E[i]  = 0;
        act_Y[i]  = 0;

        act_noOfLostCallsBeforeQeue[i] = 0;
        act_noOfLostCallsInQeue[i]     = 0;
        act_noOfCalls[i]               = 0;
        act_noOfServicedCalls[i]       = 0;
    }
}

void SimulatorBufferFifo::System::enableStatisticscollection(int serNo)
{   
    _simulationTime = 0;

    act_yQ   = yQ[serNo];
    act_y    = y[serNo];
    act_t    = t[serNo];
    act_tS   = tS[serNo];
    act_Qlen = &Qlen[serNo];
    act_E    = E[serNo];
    act_Y    = Y[serNo];

    act_noOfLostCallsBeforeQeue = noOfLostCallsBeforeQeue[serNo];
    act_noOfLostCallsInQeue     = noOfLostCallsInQeue[serNo];
    act_noOfCalls               = noOfCalls[serNo];
    act_noOfServicedCalls       = noOfServicedCalls[serNo];

    *act_Qlen = 0;
    for (int i=0; i<this->systemData->m(); i++)
    {
        act_yQ[i] = 0;
        act_y[i]  = 0;
        act_t[i]  = 0;
        act_tS[i] = 0;
        act_E[i]  = 0;
        act_Y[i]  = 0;

        act_noOfLostCallsBeforeQeue[i] = 0;
        act_noOfLostCallsInQeue[i]     = 0;
        act_noOfCalls[i]               = 0;
        act_noOfServicedCalls[i]       = 0;
    }

    clearTheStats();
}

void SimulatorBufferFifo::System::writesResultsOfSingleExperiment()
{
    for (int i=0; i<this->systemData->m(); i++)
    {
        for (int n=buffer->getV() - systemData->getClass(i)->t() + 1; n<=buffer->getV(); n++)
        {
            double tmp =buffer->getOccupancyTimeOfState(n);
            act_E[i] += tmp;
        }
        //TODO calculate Y

        act_E[i] /=_simulationTime;
        act_Y[i] /=_simulationTime;
        act_yQ[i] = buffer->getAvarageNumberOfCalls(i)/_simulationTime;
        act_y[i]  = avgNumberOfCalls[i]/_simulationTime;

        act_t[i] /= act_noOfServicedCalls[i];
        act_tS[i] /= act_noOfServicedCalls[i];
    }

    double ex = 0;
    double probSum = buffer->getOccupancyTimeOfState(0);
    for (int n=1; n<=buffer->getV(); n++)
    {
        ex += n*buffer->getOccupancyTimeOfState(n);
        probSum +=buffer->getOccupancyTimeOfState(n);
    }
    *act_Qlen = ex/probSum;
}

int SimulatorBufferFifo::System::getServerNumberOfFreeAS()
{
    return server->getNoOfMaxFreeAS();
}

double SimulatorBufferFifo::System::getQeueNumberOfFreeDU()
{
    return buffer->getNoOfFreeDU();
}

double SimulatorBufferFifo::System::getBufferNumberOfFreeAS()
{
    return buffer->getNoOfFreeAS();
}

void SimulatorBufferFifo::System::addCall(SimulatorBufferFifo::Call *newCall)
{
#ifndef DO_NOT_USE_SECUTIRY_CHECKS
    if (newCall->serverDataRate > getServerNumberOfFreeAS())
        qFatal("Call requires more AS in serwer then number of available AS");
    if (newCall->bufferDataRate > getBufferNumberOfFreeAS())
        qFatal("Call requires more AS in buffer then number of available AS");
    if (newCall->serverDataRate > newCall->reqAS)
        qFatal("Wrong server datarate");
#endif
    callsInSystem.append(newCall);

    if (newCall->serverDataRate)
        server->addCall(newCall);

    if (newCall->bufferAS)
        buffer->addCall(newCall);
}

void SimulatorBufferFifo::System::addProcess(ProcBufferFifo *proc)
{
#ifndef DO_NOT_USE_SECUTIRY_CHECKS
    if (proc->hasCallData(proc) == false)
        qFatal("Wrong process");
    if (proc->time < 0)
        qFatal("Negative time value");
    if (proc->callData->classIdx > this->systemData->m())
        qFatal("Wrong class idx");
#endif
    agenda->addProcess(proc);
}

void SimulatorBufferFifo::System::changeProcessWakeUpTime(ProcBufferFifo *proc, double newTime)
{
#ifndef DO_NOT_USE_SECUTIRY_CHECKS
    if (newTime < 0)
        qFatal("negative time ");
#endif
    agenda->changeProcessWakeUpTime(proc, newTime);
}

SimulatorBufferFifo::Call *SimulatorBufferFifo::System::getNewCall(SimulatorBufferFifo::Call *parent)
{
    Call *result = _getNewCall();
    result->fillData(parent);

    result->serverDataRate = 0;
    result->bufferDataRate = 0;
    result->noOfTransmittedDU = 0;
    result->noOfReceivedDU = 0;
    result->bufferDU = 0;
    result->timeOnBuffer = 0;
    result->timeOnSystem = 0;
    result->timeAndDU = 0;

    //result->procBufferOverflow = NULL;
    //result->procBufferEmpty = NULL;

    return result;
}

SimulatorBufferFifo::Call *SimulatorBufferFifo::System::getNewCall(
          const ModelTrClass *trClass
        , int classIdx
        , double sourceIntensity
        , double serviceIntensity
        )
{
    Call *result = _getNewCall();

    result->reqAS = trClass->t();

    result->trClass = trClass;
    result->classIdx = classIdx;
    result->sourceIntensity = sourceIntensity;
    result->serviceIntensity = serviceIntensity;

    result->serverDataRate = 0;
    result->bufferDataRate = 0;
    result->noOfTransmittedDU = 0;
    result->noOfReceivedDU = 0;
    result->bufferDU = 0;
    result->timeOnBuffer = 0;
    result->timeOnSystem = 0;
    result->timeAndDU = 0;

    //result->procBufferOverflow = NULL;
    //result->procBufferEmpty = NULL;

    return result;
}

SimulatorBufferFifo::Call *SimulatorBufferFifo::System::_getNewCall()
{
    if(uselessCalls.capacity())
        return uselessCalls.pop();
    return new Call;
}

void SimulatorBufferFifo::System::reuseCall(SimulatorBufferFifo::Call **callToReuse)
{
    (*callToReuse)->procCall = NULL;
    uselessCalls.push(*callToReuse);
    *callToReuse = NULL;
}

ProcBufferFifo *SimulatorBufferFifo::System::getNewProcess()
{
    return agenda->getNewProcess();
}

void SimulatorBufferFifo::System::reuseProcess(ProcBufferFifo **proc)
{
#ifndef DO_NOT_USE_SECUTIRY_CHECKS
    if ((*proc)->hasCallData())
        qFatal("Process has stil call data. Reuse call first");
#endif

    (*proc)->idx = 0;
    (*proc)->setUseless();

    agenda->reuseProcess(*proc);
    *proc = NULL;
}

void SimulatorBufferFifo::System::removeCallFromServer(SimulatorBufferFifo::Call *call)
{
    server->release(call->serverDataRate);
    call->serverDataRate = 0;

    serveCallsInEque();
}

void SimulatorBufferFifo::System::removeCallFromBuffer(Call *call)
{
#ifndef DO_NOT_USE_SECUTIRY_CHECKS
    if (call->bufferDU >= 0.001)
        qFatal("The call still has data in the buffer");
#endif

    buffer->removeCallAndReleaseRes(call);

    call->bufferAS = 0;
    call->bufferDataRate = 0;
    call->bufferDU = 0;
}

void SimulatorBufferFifo::System::FinishCall(SimulatorBufferFifo::Call **call, bool acceptedToService, bool servicedToTheEnd)
{
    Call *fCall = *call;

    //collectTheStat

#ifndef DO_NOT_USE_SECUTIRY_CHECKS
    if (fCall->classIdx > 100)
        qFatal("Wrong class idx");
#endif
    act_noOfCalls[fCall->classIdx]++;

    if (!acceptedToService)
    {
        act_noOfLostCallsBeforeQeue[fCall->classIdx]++;
        totalNumberOfLostCalls++;
    }
    else if (!servicedToTheEnd)
    {
        act_noOfLostCallsInQeue[fCall->classIdx]++;
        totalNumberOfLostCalls++;
    }
    else
    {
        act_t[fCall->classIdx] += fCall->timeOnBuffer;
        act_tS[fCall->classIdx] += fCall->timeOnSystem;
        act_noOfServicedCalls[fCall->classIdx] ++;
    }

    if (acceptedToService)
    {
#ifndef DO_NOT_USE_SECUTIRY_CHECKS
        int noOfMatchingCalls = callsInSystem.removeAll(fCall);
        if (noOfMatchingCalls != 1)
            qFatal("Wrong number of calls in system: %d", noOfMatchingCalls);
#else
        callsInSystem.removeAll(fCall);
#endif
    }

    if (servicedToTheEnd)
    {
        removeCallFromBuffer(fCall);
        removeCallFromServer(fCall);
    }
    (*call)->procCall = NULL;
    reuseCall(call);
}

ProcBufferFifo *SimulatorBufferFifo::System::takeFirstProcess()
{
    ProcBufferFifo *result = agenda->takeFirstProcess();
    return result;
}

void SimulatorBufferFifo::System::RemoveProcess(ProcBufferFifo **proc)
{
    (*proc)->removeCallData();
    agenda->removeProcess(proc);
}

void SimulatorBufferFifo::System::serveCallsInEque()
{
    Call *tmpCall;
    int numberOfAvailableAS;

    numberOfAvailableAS = server->getNoOfMaxFreeAS();
    while(numberOfAvailableAS > 0)
    {
        tmpCall = buffer->getNextCall();
        if (tmpCall == NULL)
            break;

        int newServerDataRate = qMin(numberOfAvailableAS+tmpCall->serverDataRate, tmpCall->reqAS);
        //int deltaServerDataRate =  newServerDataRate - tmpCall->serverDataRate;

        if (tmpCall->procCall->state == ProcBufferFifo::REC_DATA)
        {
            //qDebug("Increasing transmission spead by %d AS (%d -> %d / %d)", deltaServerDataRate, tmpCall->serverDataRate, newServerDataRate, tmpCall->reqAS);
            server->changeCallsResourcess(tmpCall, newServerDataRate);
        }
        else if (tmpCall->procCall->state == ProcBufferFifo::WAITING_IN_BUFFER)
        {
            //qDebug("Waking up the process, giving %d AS (%d -> %d / %d)", deltaServerDataRate, tmpCall->serverDataRate, newServerDataRate, tmpCall->reqAS);
            server->changeCallsResourcess(tmpCall, newServerDataRate);
#ifndef DO_NOT_USE_SECUTIRY_CHECKS
            if ((int)(tmpCall->serverDataRate) != -tmpCall->bufferDataRate)
                qFatal("Something is wrong");
#endif
            tmpCall->procCall->state = ProcBufferFifo::FLUSHING_BUFFER;
            tmpCall->procCall->execute = ProcBufferFifo::callTransmisionEnded;
            double timeOfEndSourceData = (tmpCall->bufferDU) / (-tmpCall->bufferDataRate);
            tmpCall->procCall->time = timeOfEndSourceData;
            addProcess(tmpCall->procCall);
        }
        else if (tmpCall->procCall->state == ProcBufferFifo::FLUSHING_BUFFER)
        {
            //qDebug("Increasing spead of flushing buffer by %d AS (%d -> %d / %d)", deltaServerDataRate, tmpCall->serverDataRate, newServerDataRate, tmpCall->reqAS);
            server->changeCallsResourcess(tmpCall, newServerDataRate);
            double timeOfEndSourceData = (tmpCall->bufferDU) / (-tmpCall->bufferDataRate);
            this->changeProcessWakeUpTime(tmpCall->procCall, timeOfEndSourceData);
        }
        numberOfAvailableAS = server->getNoOfMaxFreeAS();
    }
}

void SimulatorBufferFifo::System::clearTheStats()
{
    bzero(avgNumberOfCalls, systemData->m() * sizeof(double));
    server->clearTheStats();
    buffer->clearTheStats();
}

void SimulatorBufferFifo::System::collectTheStat(double time)
{
    _simulationTime +=time;

    foreach(Call *tmpCall, callsInSystem)
    {
        tmpCall->collectTheStats(time);
        avgNumberOfCalls[tmpCall->classIdx]+=time;
    }
    server->collectTheStats(time);
    buffer->collectTheStats(time);
}

void SimulatorBufferFifo::Server::clearTheStats()
{
    for (int st=0; st<=this->V; st++)
    {
        occupancyTimes[st] = 0;
    }
}

void SimulatorBufferFifo::Server::collectTheStats(double time)
{
    occupancyTimes[n] += time;
}

void SimulatorBufferFifo::Server::addCall(Call *call)
{
    n+=call->serverDataRate;
    //fqDebug("n_s = %d (-%d)", n, call->serverDataRate);
}

void SimulatorBufferFifo::Server::changeCallsResourcess(Call *call, int newServerDataRate)
{
#ifndef DO_NOT_USE_SECUTIRY_CHECKS
    if (newServerDataRate > call->reqAS)
        qFatal("Wrong new server datarate");
#endif
    n-=call->serverDataRate;
    call->bufferDataRate -= (newServerDataRate - call->serverDataRate);
    call->serverDataRate = newServerDataRate;

    n+=call->serverDataRate;
    //qDebug("n_s = %d nes call data rate %d", n, newServerDataRate);
}


void SimulatorBufferFifo::Server::release(int NoOfAS)
{
    n -= NoOfAS;
    //qDebug("n_s = %d (-%d)", n, NoOfAS);
}

SimulatorBufferFifo::Server::Server(int noOfAS, System *system)
{
    this->system = system;
    V = noOfAS;
    occupancyTimes = new double[V+1];
    bzero(occupancyTimes, (V+1)*sizeof(double));
    n = 0;
}

void ProcBufferFifo::initialize(
          SimulatorBufferFifo::System *system
        , const ModelTrClass *trClass
        , int classIdx
        , double a
        , int sumPropAt
        , int V)
{
    ProcBufferFifo *newEvent;
//    int sourceNo;

    switch(trClass->srcType())
    {
    case ModelTrClass::SourceType::Independent:
        newEvent = system->getNewProcess();
        newEvent->initializeErlang(system, trClass, classIdx, a, sumPropAt, V);
        break;

/*    case modelTrClass::Engset:
        for (sourceNo=0; sourceNo<trClass->s(); sourceNo++)
        {
            newEvent = system->agenda->getNewProcess();
            newEvent->initializeEngset(system, trClass, classIdx, a, sumPropAt, V);
        }
        break;

    case modelTrClass::Pascal:
        for (sourceNo=0; sourceNo<trClass->s(); sourceNo++)
        {
            newEvent = system->agenda->getNewProcess();
            newEvent->initializePascal(system, trClass, classIdx, a, sumPropAt, V);
        }
        break;
        */
    default:
        qDebug("Not supported clas type");
        break;
    }
}

void ProcBufferFifo::initializeErlang(
          SimulatorBufferFifo::System *system
        , const ModelTrClass *trClass
        , int classIdx
        , double a
        , int sumPropAt
        , int V
        )
{
    this->callData = system->getNewCall(trClass, classIdx, trClass->intensityNewCallTotal(a, V, sumPropAt), trClass->getMu());
    this->callData->procCall = this;

    this->state = ProcBufferFifo::WAITING_FOR_NEW;
    this->execute = ProcBufferFifo::newCall;
    this->time = distrLambda(this->callData->sourceIntensity);
    system->addProcess(this);
}


void ProcBufferFifo::newCall(ProcBufferFifo *proc, SimulatorBufferFifo::System *system)
{
#ifndef DO_NOT_USE_SECUTIRY_CHECKS
    if (proc->callData->classIdx > system->systemData->m())
        qFatal("Wrong class idx");
#endif
    //Adding new process with state Waiting for new call
    ProcBufferFifo *newProc = system->getNewProcess();
    newProc->callData = system->getNewCall(proc->callData);
    newProc->callData->procCall = newProc;

    newProc->state = ProcBufferFifo::WAITING_FOR_NEW;
    newProc->execute = ProcBufferFifo::newCall;
    newProc->time = distrLambda(newProc->callData->sourceIntensity);
    system->addProcess(newProc);

    //Serciving this call
    SimulatorBufferFifo::Call *callData = proc->callData;
    callData->plannedServiceTime = distrLambda(callData->serviceIntensity);
    callData->noOfDataUnitInMessage = callData->plannedServiceTime * callData->reqAS;

    proc->state = ProcBufferFifo::REC_DATA;
    proc->execute = sourceTransmissionEnded;
    proc->time = callData->plannedServiceTime;

    int serverAvailAS = system->getServerNumberOfFreeAS();

    if(serverAvailAS >= callData->reqAS)
    {
        callData->serverDataRate = callData->reqAS;
        callData->bufferDataRate = 0;
    }
    else
    {
        int bufferReqAS = callData->reqAS - serverAvailAS;
#ifndef DO_NOT_USE_SECUTIRY_CHECKS
        if (bufferReqAS < 0)
            qFatal("Resourcess needed are negative");
#endif
        if (system->getBufferNumberOfFreeAS() < bufferReqAS)
        {
            system->FinishCall(&proc->callData, false, false);
            system->reuseProcess(&proc);
            return;
        }
        callData->serverDataRate = qMin(serverAvailAS, callData->reqAS);
        callData->bufferDataRate = callData->reqAS - serverAvailAS;
#ifndef DO_NOT_USE_SECUTIRY_CHECKS
        if (callData->bufferDataRate < 0)
            qFatal("Negative buffer datarate");
#endif
    }
    callData->bufferAS = callData->bufferDataRate;
    system->addCall(callData);
    system->addProcess(proc);
}

void ProcBufferFifo::sourceTransmissionEnded(ProcBufferFifo *proc, SimulatorBufferFifo::System *system)
{
#ifndef DO_NOT_USE_SECUTIRY_CHECKS
    if (proc->callData->classIdx > system->systemData->m())
        qFatal("Wrong class idx");
#endif
    if (proc->callData->bufferDU == 0)
    {   //End of call service
        system->FinishCall(&proc->callData, true, true);
        system->reuseProcess(&proc);
        return;
    }

#ifndef DO_NOT_USE_SECUTIRY_CHECKS
    if (proc->callData->bufferDU < -0.001)
        qFatal("Negative value of Data Units in buffer");
#endif

    if (proc->callData->serverDataRate > 0)
    {
        if (proc->callData->bufferDU < 0)
        {
            qDebug()<<"something is wrong: negatice number of data in buffer";
        }
        proc->callData->bufferDataRate = -proc->callData->serverDataRate;
        proc->time = proc->callData->bufferDU/proc->callData->serverDataRate;

        proc->state = ProcBufferFifo::FLUSHING_BUFFER;
        proc->execute = callTransmisionEnded;
        system->addProcess(proc);
    }
    else
    {
        proc->callData->bufferDataRate = 0;
        proc->callData->serverDataRate = 0;
        proc->state = ProcBufferFifo::WAITING_IN_BUFFER;
    }
}

void ProcBufferFifo::callTransmisionEnded(ProcBufferFifo *proc, SimulatorBufferFifo::System *system)
{
#ifndef DO_NOT_USE_SECUTIRY_CHECKS
    if (proc->callData->classIdx > system->systemData->m())
        qFatal("Wrong class idx");
#endif
    system->FinishCall(&proc->callData, true, true);
    system->reuseProcess(&proc);
}

void ProcBufferFifo::configure(ProcBufferFifo::PROC_STATE state, void (*funPtr)(ProcBufferFifo *, SimulatorBufferFifo::System *), SimulatorBufferFifo::Call *callData, double time)
{
    this->state = state;
    execute = funPtr;
    this->time = time;
    this->callData = callData;
}

void SimulatorBufferFifo::Buffer::clearTheStats()
{
    for (int st=0; st<=this->V; st++)
    {
        occupancyTimes[st] = 0;
    }

    for (int i=0; i<system->systemData->m(); i++)
    {
        avgNumberOfCalls[i]=0;
    }
}

void SimulatorBufferFifo::Buffer::collectTheStats(double time)
{
    occupancyTimes[n] += time;

    for (int i=0; i<system->systemData->m(); i++)
    {
        avgNumberOfCalls[i]+=time*numberOfCalls[i];
    }
}

double SimulatorBufferFifo::Buffer::getOccupancyTimeOfState(int n)
{
    return occupancyTimes[n];
}

int SimulatorBufferFifo::Buffer::getV()
{
    return V;
}

void SimulatorBufferFifo::Buffer::addCall(SimulatorBufferFifo::Call *newCall)
{
    n+= newCall->bufferAS;
    //qDebug("n_b = %d (+%d)", n, newCall->bufferAS);

    numberOfCalls[newCall->classIdx]++;

    if (newCall->serverDataRate)
        callsInServerAndBuffer.append(newCall);
    else
        callsWaitingInBuffer.push(newCall);
}

void SimulatorBufferFifo::Buffer::removeCallAndReleaseRes(SimulatorBufferFifo::Call *call)
{
    if (call->timeOnBuffer == 0)
        return;
#ifndef DO_NOT_USE_SECUTIRY_CHECKS
    int numberOfRemovedCalls = callsInServerAndBuffer.removeAll(call);
    if (numberOfRemovedCalls != 1)
        qFatal("It should be only one call, removed %d calls", numberOfRemovedCalls);
#else
    callsInServerAndBuffer.removeAll(call);
#endif
    n -=call->bufferAS;
    //qDebug("n_b = %d (-%d)", n, call->bufferAS);

    numberOfCalls[call->classIdx]--;
}

SimulatorBufferFifo::Call *SimulatorBufferFifo::Buffer::getNextCall()
{
    foreach (Call *tmpCall, callsInServerAndBuffer)
    {
        if (tmpCall->serverDataRate < tmpCall->reqAS)
            return tmpCall;
    }
    if (callsWaitingInBuffer.size()==0)
        return NULL;

    Call *tmpCall = callsWaitingInBuffer.pop();
    callsInServerAndBuffer.append(tmpCall);
    return tmpCall;
}

SimulatorBufferFifo::Buffer::Buffer(int V, double V_du, System *system)
{
    this->V = V;
    this->V_du = V_du;
    this->n = 0;
    this->n_du = 0;

    this->system = system;


    this->occupancyTimes = new double[V+1];
    this->avgNumberOfCalls = new double[system->systemData->m()];
    this->numberOfCalls = new int[system->systemData->m()];
    bzero(numberOfCalls, sizeof(int)*system->systemData->m());
    bzero(avgNumberOfCalls, sizeof(double)*system->systemData->m());
}

void SimulatorBufferFifo::Call::fillData(SimulatorBufferFifo::Call *src)
{
    reqAS = src->reqAS;
    trClass = src->trClass;
    classIdx = src->classIdx;
    sourceIntensity = src->sourceIntensity;
    serviceIntensity= src->serviceIntensity;
}

void SimulatorBufferFifo::Call::collectTheStats(double time)
{
    timeOnSystem +=time;
    if (this->bufferDataRate > 0 || this->bufferDU > 0)
        this->timeOnBuffer += time;

    this->timeAndDU += (time * (this->bufferDU + this->bufferDataRate * time/2));

    this->bufferDU += (time*bufferDataRate);
#ifndef DO_NOT_USE_SECUTIRY_CHECKS
    if (bufferDU < -0.001)
        qFatal("Negative number of data units in buffer");
#endif

    this->noOfTransmittedDU += (serverDataRate * time);

    double tmp = noOfReceivedDU + time*reqAS;
    if (tmp <= noOfDataUnitInMessage)
        noOfReceivedDU = tmp;
    else
        noOfReceivedDU = noOfDataUnitInMessage;
}

} //namespace Algorithms
