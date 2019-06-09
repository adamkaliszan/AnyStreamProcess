#include "simulatorQeueFIFO.h"
#include <qstring.h>
#include <qglobal.h>

namespace Algorithms
{

SimulatorQeueFifo::SimulatorQeueFifo(): Simulator()
{
    myQoS_Set
       <<Results::Type::BlockingProbability
       <<Results::Type::OccupancyDistribution;
}

QString SimulatorQeueFifo::shortName() const
{
    return "Sim. buffer";
}

int SimulatorQeueFifo::complexity() const {return 100;}

bool SimulatorQeueFifo::possible(const ModelCreator *system) const
{
    if (system->vk_b() <= 0)
        return false;
    return Simulator::possible(system);
}

void SimulatorQeueFifo::calculateSystem(const ModelCreator *system
        , double a
        , RInvestigator *results
        , SimulationParameters *simParameters)
{
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
    delete simData;
    //emit sigCalculationDone();
}

SimulatorQeueFifo::System::System(const ModelCreator *system
        , int noOfSeries
        ):
    m(system->m())
  , n(0)
  , old_n(0)
  , results(system->m(), system->vk_s(), system->vk_b(), noOfSeries)
{
    systemData = system;

    agenda = new SimulatorDataCollection<ProcQeueFifo>();
    server = new Server(system->vk_s(), this);
    buffer   = new Buffer(system->vk_b(), this, system->getBufferScheduler());

    yTime_ClassI                                = new double[system->m()];
    servTr_ClassI                               = new double[system->m()];
    AStime_ofOccupiedAS_byClassI_inStateN       = new double*[system->m()];
    qeueAStime_ofOccupiedAS_byClassI_inStateN   = new double*[system->m()];
    serverAStime_ofOccupiedAS_byClassI_inStateN = new double*[system->m()];
    occupancyTimes                              = new double[system->vk_s()+system->vk_b()+1];

    outNew                                      = new int[system->vk_s()+system->vk_b()+1];
    outEnd                                      = new int[system->vk_s()+system->vk_b()+1];

    outNewSCof                                  = new int*[system->m()];
    outEndSCof                                  = new int*[system->m()];
    outNewSCserv                                = new int*[system->m()];
    outEndSCserv                                = new int*[system->m()];


    occupancyTimesDtl                           = new double*[system->vk_s()+1];
    for (int n=0; n<=system->vk_s(); n++)
        occupancyTimesDtl[n] = new double[system->vk_b()+1];

    for (int i=0; i<m; i++)
    {
        AStime_ofOccupiedAS_byClassI_inStateN[i]       = new double[system->vk_s()+system->vk_b()+1];
        qeueAStime_ofOccupiedAS_byClassI_inStateN[i]   = new double[system->vk_s()+system->vk_b()+1];
        serverAStime_ofOccupiedAS_byClassI_inStateN[i] = new double[system->vk_s()+system->vk_b()+1];
        outNewSCof[i]                                    = new int[system->vk_s()+system->vk_b()+1];
        outEndSCof[i]                                    = new int[system->vk_s()+system->vk_b()+1];
        outNewSCserv[i]                                  = new int[system->vk_s()+system->vk_b()+1];
        outEndSCserv[i]                                  = new int[system->vk_s()+system->vk_b()+1];    }
}

SimulatorQeueFifo::System::~System()
{
    for (int n=0; n<=server->getV(); n++)
        delete []occupancyTimesDtl[n];

    delete agenda;
    delete server;
    delete buffer;

    for (int i=0; i<m; i++)
    {
        delete []AStime_ofOccupiedAS_byClassI_inStateN[i];
        delete []qeueAStime_ofOccupiedAS_byClassI_inStateN[i];
        delete []serverAStime_ofOccupiedAS_byClassI_inStateN[i];

        delete []outNewSCof[i];
        delete []outEndSCof[i];
        delete []outNewSCserv[i];
        delete []outEndSCserv[i];
    }

    delete []yTime_ClassI;
    delete []servTr_ClassI;
    delete []AStime_ofOccupiedAS_byClassI_inStateN;
    delete []qeueAStime_ofOccupiedAS_byClassI_inStateN;
    delete []serverAStime_ofOccupiedAS_byClassI_inStateN;
    delete []occupancyTimes;
    delete []occupancyTimesDtl;

    delete []outNew;
    delete []outEnd;

    delete []outNewSCof;
    delete []outEndSCof;
    delete []outNewSCserv;
    delete []outEndSCserv;

    foreach(Call *tmp, callsInSystem)
        delete tmp;

    while (uselessCalls.length())
    {
        Call *tmp = uselessCalls.pop();
        delete tmp;
    }
}

void SimulatorQeueFifo::System::initialize(double a, int sumPropAt, int V)
{
    for(int i=0; i<systemData->m(); i++)
    {
        const ModelTrClass *tmpClass = systemData->getClass(i);
        ProcQeueFifo::initialize(this, tmpClass, i, a, sumPropAt, V);
    }
}

void SimulatorQeueFifo::System::doSimExperiment(int numberOfLostCall, unsigned int seed, int numberOfServicedCalls)
{
    qsrand(seed);
    this->totalNumberOfLostCalls = 0;
    this->totalNumberOfServicedCalls = 0;
    if (numberOfServicedCalls == 0)
    {
        while(totalNumberOfLostCalls < numberOfLostCall)
        {
            ProcQeueFifo *proc = takeFirstProcess();
            collectTheStatPre(proc->time);
            proc->execute(proc, this);
            collectTheStatPost(proc->time);
        }
    }
    else
    {
        if(totalNumberOfLostCalls > 0)
        {
            while(totalNumberOfLostCalls < numberOfLostCall && totalNumberOfServicedCalls < numberOfServicedCalls)
            {
                ProcQeueFifo *proc = takeFirstProcess();
                collectTheStatPre(proc->time);
                proc->execute(proc, this);
                collectTheStatPost(proc->time);
            }
        }
        else
        {
            while(totalNumberOfServicedCalls < numberOfServicedCalls)
            {
                ProcQeueFifo *proc = takeFirstProcess();
                collectTheStatPre(proc->time);
                proc->execute(proc, this);
                collectTheStatPost(proc->time);
            }
        }
    }
}

void SimulatorQeueFifo::System::writesResultsOfSingleExperiment(Results::RSingle &singleResults)
{
    int Vs = server->getV();
    int Vb = buffer->getV();
    int m = systemData->m();

    int max_t = 0;

    for (int i=0; i<m; i++)
    {
        int t = systemData->getClass(i)->t();
        max_t = qMax(t, max_t);
    }

    for (int i=0; i<m; i++)
    {
        int t = systemData->getClass(i)->t();

        double tmp = 0;
        double E = 0;
        for (int n_server = qMax(0, (int)Vs-t+1); n_server<=Vs; n_server++)
        {
            for (int n_Buffer = qMax(0, (int)Vb-t+1); n_Buffer<=Vb; n_Buffer++)
            {
                tmp=getOccupancyTimeOfState(n_server, n_Buffer);

                if (this->buffer->scheduler == BufferPolicy::dFIFO_Seq || (this->buffer->scheduler == BufferPolicy::qFIFO_Seq))
                    E += tmp;
                else if (n_server + n_Buffer + t > Vs + Vb)
                    E += tmp;
            }
        }
        E/= results._simulationTime;
        results.act_E[i] = E;
        singleResults.write(TypeForClass::BlockingProbability, E, i);

        results.act_Y[i] =servTr_ClassI[i]/results._simulationTime;
        results.act_yQ[i] = buffer->getAvarageNumberOfCalls(i)/results._simulationTime;
        results.act_y[i] = yTime_ClassI[i]/results._simulationTime;
        results.act_ytQ[i] = buffer->getAvarageNumberOfAS(i)/results._simulationTime;

        results.act_t[i] /= results.act_noOfServicedCalls[i];
        results.act_tQeue[i] /= results.act_noOfServicedCalls[i];
        results.act_tS[i] /= results.act_noOfServicedCalls[i];

        results.act_tServer[i] /=results.act_noOfServicedCalls[i];
        results.act_tPlanedServer[i] /=results.act_noOfServicedCalls[i];

        for (int n=0; n<=Vb; n++)
        {
            double x = buffer->AStime_ofOccupiedAS_byClassI_inStateN[i][n]/buffer->occupancyTimes[n];
            results.act_LOC_qeue_yt[i][n] = x;
        }

        for (int n=0; n<=Vs; n++)
        {
            double x = server->AStime_ofOccupiedAS_byClassI_inStateN[i][n]/server->occupancyTimes[n];
            results.act_LOC_server_yt[i][n] = x;
        }

        for (int n=0; n<=Vs+Vb; n++)
        {
            double x = AStime_ofOccupiedAS_byClassI_inStateN[i][n]/getOccupancyTimeOfState(n);
            //results.act_SYS_yt[i][n] = x;
            x = qeueAStime_ofOccupiedAS_byClassI_inStateN[i][n]/getOccupancyTimeOfState(n);
            results.act_SYS_qeue_yt[i][n] = x;
            x = serverAStime_ofOccupiedAS_byClassI_inStateN[i][n]/getOccupancyTimeOfState(n);
            results.act_SYS_server_yt[i][n] = x;
        }
    }

    double ex = 0;
    double probSum = buffer->getOccupancyTimeOfState(0);
    for (int n=1; n<=buffer->getV(); n++)
    {
        ex += n*buffer->getOccupancyTimeOfState(n);
        probSum +=buffer->getOccupancyTimeOfState(n);
    }
    *results.act_Qlen = ex/probSum;

    for (int n=0; n<=server->getV()+buffer->getV(); n++)
    {
        double tmpstateDurationTime = getOccupancyTimeOfState(n);

        results.act_trDistribSys[n] = getOccupancyTimeOfState(n) / results._simulationTime;

        results.act_intOutNew[n] = (double)(getNoOutNew(n)) / tmpstateDurationTime;

        results.act_intOutEnd[n] = (double)(getNoOutEnd(n)) / tmpstateDurationTime;

        for (int i=0; i<m; i++)
        {
            results.act_intOutNewSC[i][n] = (double) (getNoOutNewSCof(n, i)) / tmpstateDurationTime;
            results.act_intOutEndSC[i][n] = (double) (getNoOutEndSCof(n, i)) / tmpstateDurationTime;

            results.act_intInNewSC[i][n] = (double) (getNoOutNewSCserv(n, i)) / tmpstateDurationTime;
            results.act_intInEndSC[i][n] = (double) (getNoOutEndSCserv(n, i)) / tmpstateDurationTime;
        }
    }

    for (int n=0; n<=server->getV()+buffer->getV(); n++)
    {
        results.act_intInNew[n]  = 0;
        results.act_intInEnd[n]  = 0;
        for (int i=0; i<m; i++)
        {
            int t = systemData->getClass(i)->t();

            if (n>=(int)t)
                results.act_intInNew[n]  += results.act_intInNewSC[i][n-t];

            if (n+t  <= server->getV()+buffer->getV())
                results.act_intInEnd[n]  += results.act_intInEndSC[i][n+t];
        }
    }

    for (int n=0; n<=server->getV(); n++)
    {
        results.act_trDistribServ[n] = server->getOccupancyTimeOfState(n) / results._simulationTime;

        for (int q=0; q<=buffer->getV(); q++)
            results.act_trDistrib[n][q] = getOccupancyTimeOfState(n, q) / results._simulationTime;
    }
    for (int n=0; n<=buffer->getV(); n++)
        results.act_trDistribQeue[n] = buffer->getOccupancyTimeOfState(n) / results._simulationTime;
}

int SimulatorQeueFifo::System::getServerNumberOfFreeAS()
{
    return server->getNoOfFreeAS();
}

bool SimulatorQeueFifo::System::serveNewCall(SimulatorQeueFifo::Call *newCall)
{
#ifndef DO_NOT_USE_SECUTIRY_CHECKS

#endif
    classIdx = newCall->classIdx;

    int serverFreeAS = server->getNoOfFreeAS();
    int bufferFreeAS = buffer->getNoOfFreeAS();
    if (((this->buffer->scheduler==BufferPolicy::Continuos || this->buffer->scheduler==BufferPolicy::qFIFO_Seq) && serverFreeAS >= newCall->reqAS)
            || (this->buffer->scheduler==BufferPolicy::dFIFO_Seq  && serverFreeAS >= newCall->reqAS && buffer->n == 0))
    {
        callsInSystem.append(newCall);

        newCall->allocatedAS    = newCall->reqAS;
        server->addCall(newCall, newCall->reqAS, true);
        newCall->proc           = agenda->getNewProcess();
        newCall->proc->state    = ProcQeueFifo::SENDING_DATA;
        newCall->proc->execute  = newCall->trEndedFun;
        newCall->proc->callData = newCall;
        newCall->proc->time     = newCall->plannedServiceTime;
        agenda->addProcess(newCall->proc);
        n += newCall->reqAS;

        return true;
    }


    if (bufferFreeAS >= newCall->reqAS)
    {
        callsInSystem.append(newCall);
        buffer->addCall(newCall);

        if (buffer->scheduler==BufferPolicy::Continuos && serverFreeAS > 0)
            serveCallsInEque();
        n += newCall->reqAS;
        return true;
    }
    else
    {
#ifndef DO_NOT_USE_SECUTIRY_CHECKS
        if(getServerNumberOfFreeAS() >= newCall->reqAS)
            qFatal("Qeue statistics are incorrect, because qeue is full (%d free AS), and server is empty (%d freeAS)",
                   buffer->getNoOfFreeAS(), server->getNoOfFreeAS());
#endif

        FinishCall(newCall, false);
        return false;
    }
}

void SimulatorQeueFifo::System::endTransmission(SimulatorQeueFifo::Call *call)
{
    classIdx = call->classIdx;

    if (call->allocatedAS != call->reqAS)
        removeCallFromQeue(call);

    removeCallFromServer(call);
    callsInSystem.removeAll(call);


    n-=call->reqAS;
    FinishCall(call, true);
}

void SimulatorQeueFifo::System::addProcess(ProcQeueFifo *proc)
{
#ifndef DO_NOT_USE_SECUTIRY_CHECKS
    if (proc->time < 0)
        qFatal("Negative time value");
    if (proc->callData->classIdx > this->systemData->m())
        qFatal("Wrong class idx");
#endif
    agenda->addProcess(proc);
}

void SimulatorQeueFifo::System::removeProcess(ProcQeueFifo *proc)
{
#ifndef DO_NOT_USE_SECUTIRY_CHECKS
    if (proc->time < 0)
        qFatal("Negative time value");
#endif
    agenda->removeProcess(&proc);
}

SimulatorQeueFifo::Call *SimulatorQeueFifo::System::getNewCall(SimulatorQeueFifo::Call *parent)
{
    Call *result = _getNewCall();

    result->fillData(parent);
    result->DUtransfered          = 0;
    result->timeOnBuffer          = 0;
    result->effectiveTimeOnBuffer = 0;
    result->timeOnBufferAS        = 0;
    result->timeOnSystem          = 0;
    result->timeOnServer          = 0;
    result->allocatedAS           = 0;

    result->complementaryCall = nullptr;

    return result;
}

SimulatorQeueFifo::Call *SimulatorQeueFifo::System::getNewCall(
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
    result->timeOnBuffer          = 0;
    result->effectiveTimeOnBuffer = 0;
    result->timeOnBufferAS        = 0;
    result->timeOnSystem          = 0;
    result->timeOnServer          = 0;
    result->allocatedAS           = 0;

    result->complementaryCall = nullptr;

    return result;
}

SimulatorQeueFifo::Call *SimulatorQeueFifo::System::_getNewCall()
{
    static int noOfCreatedCalls = 0;
    if(uselessCalls.length())
        return uselessCalls.pop();
    noOfCreatedCalls++;
    return new Call;
}

void SimulatorQeueFifo::System::reuseCall(SimulatorQeueFifo::Call *callToReuse)
{
    uselessCalls.push(callToReuse);
}

void SimulatorQeueFifo::System::reuseProcess(ProcQeueFifo *proc)
{
    proc->idx = 0;
    proc->setUseless();

    agenda->reuseProcess(proc);
}

void SimulatorQeueFifo::System::removeCallFromServer(SimulatorQeueFifo::Call *call)
{
    server->release(call);
    serveCallsInEque();
}

void SimulatorQeueFifo::System::removeCallFromQeue(SimulatorQeueFifo::Call *call) { buffer->removeFirstCall(call); }

void SimulatorQeueFifo::System::FinishCall(SimulatorQeueFifo::Call *call, bool acceptedToService)
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
        results.act_t[call->classIdx] += call->effectiveTimeOnBuffer;
        results.act_tQeue[call->classIdx] += call->timeOnBuffer;
        results.act_tS[call->classIdx] += call->timeOnSystem;
        results.act_tServer[call->classIdx] += call->timeOnServer;
        results.act_tPlanedServer[call->classIdx] += call->plannedServiceTime;
        results.act_noOfServicedCalls[call->classIdx] ++;
    }
    reuseCall(call);
}

void SimulatorQeueFifo::System::cancellScheduledCall(SimulatorQeueFifo::Call *call)
{
    reuseCall(call);
}

void SimulatorQeueFifo::System::serveCallsInEque()
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
        if (tmpCall == NULL)
            break;

        if (buffer->scheduler != BufferPolicy::Continuos && tmpCall->reqAS > numberOfAvailableAS)
            break;

        int maxResToAll = tmpCall->reqAS - tmpCall->allocatedAS;
#ifndef DO_NOT_USE_SECUTIRY_CHECKS
        if (buffer->n < (int)maxResToAll)
            qFatal("Wrong number of max ressourcess to allocate");
#endif
        bool newCall = (tmpCall->allocatedAS == 0);
        int noOfAS_toAllocate =qMin(numberOfAvailableAS, (int) maxResToAll);
        tmpCall->allocatedAS += noOfAS_toAllocate;

        double newTime = (tmpCall->DUmessageSize-tmpCall->DUtransfered)/tmpCall->allocatedAS;

        server->addCall(tmpCall, noOfAS_toAllocate, newCall);

        if (newCall)
        {
            tmpCall->proc = agenda->getNewProcess();
            tmpCall->proc->time = newTime;
            tmpCall->proc->callData = tmpCall;
            tmpCall->proc->state = ProcQeueFifo::SENDING_DATA;
            tmpCall->proc->execute = tmpCall->trEndedFun;
            agenda->addProcess(tmpCall->proc);
        }
        else
        {
            agenda->changeProcessWakeUpTime(tmpCall->proc, newTime);
        }
        buffer->takeCall(tmpCall, noOfAS_toAllocate);


        numberOfAvailableAS = server->getNoOfFreeAS();

#ifndef DO_NOT_USE_SECUTIRY_CHECKS
        buffer->consistencyCheck();
#endif
    }
}

void SimulatorQeueFifo::System::collectTheStatPre(double time)
{
    old_n = n;
    results._simulationTime +=time;

    foreach(Call *tmpCall, callsInSystem)
    {
        tmpCall->collectTheStats(time);
        yTime_ClassI[tmpCall->classIdx] += time;
        servTr_ClassI[tmpCall->classIdx] += (time*tmpCall->allocatedAS);
    }
    server->collectTheStats(time);
    buffer->collectTheStats(time);

    for (int i=0; i<m; i++)
    {
        AStime_ofOccupiedAS_byClassI_inStateN[i][n] += time*(server->numberOfAS[i]+ buffer->numberOfAS[i]);
        qeueAStime_ofOccupiedAS_byClassI_inStateN[i][n] += time*(buffer->numberOfAS[i]);
        serverAStime_ofOccupiedAS_byClassI_inStateN[i][n] += time*(server->numberOfAS[i]);
    }
    occupancyTimes[n] +=time;
    occupancyTimesDtl[server->n][buffer->n] += time;
}

void SimulatorQeueFifo::System::collectTheStatPost(double time)
{
    (void)time;
    if (n == old_n)
    {
        outNewSCof[classIdx][old_n]++;
    }
    if (n < old_n) //zakończenie obsługi zgłoszenia
    {
        outEnd[old_n]++;
        outEndSCof[classIdx][old_n]++;
        outEndSCserv[classIdx][old_n]++;
    }

    if (n > old_n) //przyjęcie do obsługi zgłoszenia
    {
        outNew[old_n]++;
        outNewSCof[classIdx][old_n]++;
        outNewSCserv[classIdx][old_n]++;
    }
}


void SimulatorQeueFifo::System::enableStatisticscollection(int serNo)
{
    int VsVb = systemData->vk_s() + systemData->vk_b();
    bzero(yTime_ClassI, systemData->m() * sizeof(double));
    bzero(servTr_ClassI, systemData->m() * sizeof(double));
    bzero(occupancyTimes, (VsVb+1)*sizeof(double));

    bzero(outNew, (VsVb+1)*sizeof(int));
    bzero(outEnd, (VsVb+1)*sizeof(int));

    for (int n=0; n<=systemData->vk_s(); n++)
        bzero(occupancyTimesDtl[n], (systemData->vk_b()+1)*sizeof(double));

    for (int i=0; i<m; i++)
    {
        bzero(AStime_ofOccupiedAS_byClassI_inStateN[i], (VsVb+1)*sizeof(double));
        bzero(qeueAStime_ofOccupiedAS_byClassI_inStateN[i], (VsVb+1)*sizeof(double));
        bzero(serverAStime_ofOccupiedAS_byClassI_inStateN[i], (VsVb+1)*sizeof(double));

        bzero(outNewSCof[i],  (VsVb+1)*sizeof(int));
        bzero(outEndSCof[i],  (VsVb+1)*sizeof(int));
        bzero(outNewSCserv[i],  (VsVb+1)*sizeof(int));
        bzero(outEndSCserv[i],  (VsVb+1)*sizeof(int));
    }

    results.enableStatisticscollection(serNo);
    server->clearTheStats();
    buffer->clearTheStats();
}

void SimulatorQeueFifo::System::disableStatisticCollection()
{
    results.disableStatisticCollection();
}

void SimulatorQeueFifo::Server::clearTheStats()
{
    bzero(occupancyTimes, (V+1)* sizeof(double));

    for (int i=0; i<m; i++)
        bzero(AStime_ofOccupiedAS_byClassI_inStateN[i], (V+1)*sizeof(double));
}

void SimulatorQeueFifo::Server::collectTheStats(double time)
{
    for (int i=0; i<m; i++)
    {
        AStime_ofOccupiedAS_byClassI_inStateN[i][n]+=time * numberOfAS[i];
    }
    occupancyTimes[n] += time;

    foreach(Call *tmpCall, this->calls)
        tmpCall->IncrTimeInServer(time);
}

void SimulatorQeueFifo::Server::addCall(Call *call, int noOfAS, bool newCall)
{
    numberOfAS[call->classIdx] += noOfAS;
#ifndef DO_NOT_USE_SECUTIRY_CHECKS
    if (call->reqAS < call->allocatedAS)
        qFatal("More AS is allocated then it is required");
    if (noOfAS > call->allocatedAS)
        qFatal("Wrong argument value");
#endif
    this->n += noOfAS;
    if (newCall)
    {
        calls.append(call);
#ifndef DO_NOT_USE_SECUTIRY_CHECKS
        if ((int)calls.length() > this->V)
            qFatal("To many calls on server's list");
#endif
    }
}

void SimulatorQeueFifo::Server::release(SimulatorQeueFifo::Call *call)
{
    numberOfAS[call->classIdx] -=call->allocatedAS;
    this->n -=call->allocatedAS;
    calls.removeAll(call);
}

SimulatorQeueFifo::Server::Server(int noOfAS, System *system)
{
    m = system->systemData->m();
    this->system = system;
    V = noOfAS;

    numberOfAS            = new int[m];
    bzero(numberOfAS, (m)*sizeof(int));

    AStime_ofOccupiedAS_byClassI_inStateN = new double*[m];
    for (int i=0; i<m; i++)
    {
        AStime_ofOccupiedAS_byClassI_inStateN[i] = new double[V+1];
        bzero(AStime_ofOccupiedAS_byClassI_inStateN[i], (V+1)*sizeof(double));
    }

    occupancyTimes = new double[V+1];
    bzero(occupancyTimes, (V+1)*sizeof(double));
    n = 0;
}

SimulatorQeueFifo::Server::~Server()
{
    delete []numberOfAS;

    for (int i=0; i<m; i++)
    {
        delete []AStime_ofOccupiedAS_byClassI_inStateN[i];
    }
    delete []AStime_ofOccupiedAS_byClassI_inStateN;

    occupancyTimes = new double[V+1];
    bzero(occupancyTimes, (V+1)*sizeof(double));
    n = 0;
}

void ProcQeueFifo::initialize(
          SimulatorQeueFifo::System *system
        , const ModelTrClass *trClass
        , int classIdx
        , double a
        , int sumPropAt
        , int V
        )
{
    ProcQeueFifo *newEvent;
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

void ProcQeueFifo::initializeIndependent(
          SimulatorQeueFifo::System *system
        , const ModelTrClass *trClass
        , int classIdx
        , double a
        , int sumPropAt
        , int V
        , double (*funTimeNewCall)(double, double)
        , void (*funNewCall)(ProcQeueFifo *proc, SimulatorQeueFifo::System *system)
        )
{
    double IncE = 1.0 / trClass->intensityNewCallTotal(a, V, sumPropAt);
    callData = system->getNewCall(trClass, classIdx, IncE);
    callData->trEndedFun = ProcQeueFifo::transmisionEndedIndependent;

    this->state = ProcQeueFifo::WAITING_FOR_NEW_CALL;
    this->execute = funNewCall;
    this->time = funTimeNewCall(this->callData->sourceE, this->callData->sourceD);
    system->addProcess(this);
}

void ProcQeueFifo::ProcQeueFifo::initializeDependent(
          SimulatorQeueFifo::System *system
        , const ModelTrClass *trClass
        , int classIdx
        , double a
        , int sumPropAt
        , int V
        , double (*funTimeNewCall)(double, double)
        , void (*funNewCall)(ProcQeueFifo *, SimulatorQeueFifo::System *)
        , void (*funEndCall)(ProcQeueFifo *, SimulatorQeueFifo::System *))
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

    this->state = ProcQeueFifo::WAITING_FOR_NEW_CALL;
    this->execute = funNewCall;
    this->time = funTimeNewCall(this->callData->sourceE, this->callData->sourceD);
    system->addProcess(this);
}

void ProcQeueFifo::newCallIndep(ProcQeueFifo *proc, SimulatorQeueFifo::System *system, double (*funTimeNewCall)(double, double), double (*funTimeOfService)(double, double),
                                 void (*funNewCall)(ProcQeueFifo *proc, SimulatorQeueFifo::System *system))
{
#ifndef DO_NOT_USE_SECUTIRY_CHECKS
    if (proc->callData->classIdx > system->systemData->m())
        qFatal("Wrong class idx");
#endif
    SimulatorQeueFifo::Call *callData = proc->callData;
    callData->plannedServiceTime = funTimeOfService(callData->serviceE, callData->serviceD);
    callData->reqAS              = proc->callData->reqAS;
    callData->DUmessageSize      = callData->reqAS * callData->plannedServiceTime;
    callData->proc               = nullptr;

    system->serveNewCall(callData);

    //Adding new process with state Waiting for new call
    ProcQeueFifo *newProc = proc;
    newProc->callData = system->getNewCall(proc->callData);

    newProc->state = ProcQeueFifo::WAITING_FOR_NEW_CALL;
    newProc->execute = funNewCall;
    newProc->time = funTimeNewCall(newProc->callData->sourceE, newProc->callData->sourceD);
    system->addProcess(newProc);
}

void ProcQeueFifo::newCallDepMinus(ProcQeueFifo *proc, SimulatorQeueFifo::System *system,
                                   double (*funTimeNewCall)(double, double), double (*funTimeOfService)(double, double),
                                   void (*funNewCall)(ProcQeueFifo *, SimulatorQeueFifo::System *))
{
#ifndef DO_NOT_USE_SECUTIRY_CHECKS
    if (proc->callData->classIdx > system->systemData->m())
        qFatal("Wrong class idx");
#endif
    SimulatorQeueFifo::Call *callData = proc->callData;
    callData->plannedServiceTime = funTimeOfService(callData->serviceE, callData->serviceD);
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
        newProc->execute = funNewCall;
        newProc->time = funTimeNewCall(newProc->callData->sourceE, newProc->callData->sourceD);
        system->addProcess(newProc);
    }

    system->reuseProcess(proc);
}

void ProcQeueFifo::newCallDepPlus(ProcQeueFifo *proc, SimulatorQeueFifo::System *system,
                                  double (*funTimeNewCall)(double, double), double (*funTimeOfService)(double, double),
                                  void (*funNewCall)(ProcQeueFifo *, SimulatorQeueFifo::System *))
{
#ifndef DO_NOT_USE_SECUTIRY_CHECKS
    if (proc->callData->classIdx > system->systemData->m())
        qFatal("Wrong class idx");
#endif
    SimulatorQeueFifo::Call *callData           = proc->callData;
    SimulatorQeueFifo::Call *parentServicedCall = callData->complementaryCall;

    callData->plannedServiceTime = funTimeOfService(callData->serviceE, callData->serviceD);
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
    procNewCall->execute = funNewCall;
    procNewCall->time = funTimeNewCall(procNewCall->callData->sourceE, procNewCall->callData->sourceD);
    system->addProcess(procNewCall);

    if (system->serveNewCall(callData) == true)
    {
        ProcQeueFifo *newProc = system->getNewProcess();
        newProc->callData = system->getNewCall(callData);
        newProc->callData->proc = newProc;
        callData->complementaryCall = newProc->callData;
        newProc->callData->complementaryCall = callData;

        newProc->state = ProcQeueFifo::WAITING_FOR_NEW_CALL;
        newProc->execute = funNewCall;
        newProc->time = funTimeNewCall(newProc->callData->sourceE, newProc->callData->sourceD);
        system->addProcess(newProc);
    }
    system->reuseProcess(proc);
}



void ProcQeueFifo::transmisionEndedIndependent(ProcQeueFifo *proc, SimulatorQeueFifo::System *system)
{
#ifndef DO_NOT_USE_SECUTIRY_CHECKS
    if (proc->callData->classIdx > system->systemData->m())
        qFatal("Wrong class idx");
#endif
    system->endTransmission(proc->callData);

    proc->callData = nullptr;
    system->reuseProcess(proc);
}

void ProcQeueFifo::transmisionEndedDependentMinus(ProcQeueFifo *proc, SimulatorQeueFifo::System *system,
                                                  double (*funTimeNewCall)(double, double),
                                                  void (*funNewCall)(ProcQeueFifo *proc, SimulatorQeueFifo::System *system))
{
    ProcQeueFifo *newProc = system->getNewProcess();
    newProc->callData = system->getNewCall(proc->callData);

#ifndef DO_NOT_USE_SECUTIRY_CHECKS
    if (proc->callData->classIdx > system->systemData->m())
        qFatal("Wrong class idx");
#endif
    system->endTransmission(proc->callData);

    proc->callData = nullptr;
    system->reuseProcess(proc);

    //Adding new process with state Waiting for new call
    newProc->state = ProcQeueFifo::WAITING_FOR_NEW_CALL;
    newProc->execute = funNewCall;
    newProc->time = funTimeNewCall(newProc->callData->sourceE, newProc->callData->sourceD);
    system->addProcess(newProc);
}

void ProcQeueFifo::transmisionEndedDependentPlus(ProcQeueFifo *proc, SimulatorQeueFifo::System *system)
{
    SimulatorQeueFifo::Call *scheduledCall = proc->callData->complementaryCall;

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

void SimulatorQeueFifo::Buffer::clearTheStats()
{
    for (int st=0; st<=this->V; st++)
        occupancyTimes[st] = 0;

    for (int i=0; i<system->systemData->m(); i++)
    {
        avgNumberOfCalls[i] = 0;
        AStime_ofOccupiedAS_byClassI[i]    = 0;

        for (int n=0; n<=V; n++)
            AStime_ofOccupiedAS_byClassI_inStateN[i][n] = 0;
    }
}

void SimulatorQeueFifo::Buffer::collectTheStats(double time)
{
    occupancyTimes[n] += time;

    for (int i=0; i<system->systemData->m(); i++)
    {
        avgNumberOfCalls[i]+=time*numberOfCalls[i];
        AStime_ofOccupiedAS_byClassI[i]+=time*numberOfAS[i];
        AStime_ofOccupiedAS_byClassI_inStateN[i][n] +=time*numberOfAS[i];
    }

    foreach (Call *tmp, calls)
        tmp->IncrTimeOfWaitingInBuffer(time);

    if (firstCall != nullptr)
        firstCall->IncrTimeOfWaitingInBuffer(time);
}

void SimulatorQeueFifo::Buffer::addCall(SimulatorQeueFifo::Call *newCall)
{
#ifndef DO_NOT_USE_SECUTIRY_CHECKS
    consistencyCheck();
#endif
    n+= (newCall->reqAS - newCall->allocatedAS);
#ifndef DO_NOT_USE_SECUTIRY_CHECKS
    if (n > V)
        qFatal("Qeue: n > V (%d > %d)", n, V);
#endif
    //qDebug("n_b = %d (+%d)", n, newCall->bufferAS);

    numberOfCalls[newCall->classIdx]++;
    numberOfAS[newCall->classIdx] += newCall->reqAS;
    calls.push(newCall);
#ifndef DO_NOT_USE_SECUTIRY_CHECKS
    if ((int)calls.length() > this->V)
        qFatal("To many calls on qeue's list");
    consistencyCheck();
#endif
}

void SimulatorQeueFifo::Buffer::takeCall(SimulatorQeueFifo::Call *call, int noOfAS)
{
    n-=noOfAS;
#ifndef DO_NOT_USE_SECUTIRY_CHECKS
    if (n > V)
        qFatal("Wrong number of allocated AS in qeue: %d", (int)n);
#endif
    if (call->allocatedAS == call->reqAS)
    {
#ifndef DO_NOT_USE_SECUTIRY_CHECKS
        if (call==firstCall)
        {
            numberOfCalls[call->classIdx]--;
            firstCall = nullptr;
        }
        else
            qFatal("Something is wrong, it should be first call");
        consistencyCheck();
#else
        numberOfCalls[call->classIdx]--;
        firstCall = NULL;
#endif
    }
    numberOfAS[call->classIdx] -= noOfAS;
}

void SimulatorQeueFifo::Buffer::removeFirstCall(SimulatorQeueFifo::Call *first)
{
#ifndef DO_NOT_USE_SECUTIRY_CHECKS
    if (first != firstCall)
        qFatal("Cal Qeue error");
#endif
    firstCall = nullptr;
    n -= (first->reqAS - first->allocatedAS);
    numberOfCalls[first->classIdx]--;
    numberOfAS[first->classIdx] -= (first->reqAS - first->allocatedAS);
#ifndef DO_NOT_USE_SECUTIRY_CHECKS
    consistencyCheck();
#endif
}

SimulatorQeueFifo::Call *SimulatorQeueFifo::Buffer::getNextCall()
{
#ifndef DO_NOT_USE_SECUTIRY_CHECKS
    consistencyCheck();
#endif
    if (firstCall == nullptr)
    {
        if (!calls.isEmpty())
            firstCall = calls.pop();
    }
    return firstCall;
}

SimulatorQeueFifo::Buffer::Buffer(int V, SimulatorQeueFifo::System *system, BufferPolicy bufferScheduler):
    system(system), scheduler(bufferScheduler)
{
    firstCall = nullptr;
    this->V = V;
    this->n = 0;
    this->m = system->systemData->m();

    this->occupancyTimes        = new double[V+1];
    this->avgNumberOfCalls      = new double[system->systemData->m()];
    this->AStime_ofOccupiedAS_byClassI         = new double[system->systemData->m()];
    this->numberOfCalls         = new int[system->systemData->m()];
    this->numberOfAS            = new int[system->systemData->m()];
    this->AStime_ofOccupiedAS_byClassI_inStateN = new double*[m];
    for (int i=0; i<m; i++)
    {
        AStime_ofOccupiedAS_byClassI_inStateN[i] = new double[V+1];
        bzero(AStime_ofOccupiedAS_byClassI_inStateN[i], static_cast<size_t>(V+1)*sizeof(double));
    }
    bzero(occupancyTimes,   sizeof(double)*(V+1));
    bzero(avgNumberOfCalls, sizeof(double)*system->systemData->m());
    bzero(AStime_ofOccupiedAS_byClassI,    sizeof(double)*system->systemData->m());
    bzero(numberOfCalls,    sizeof(int)*system->systemData->m());
    bzero(numberOfAS,       sizeof(int)*system->systemData->m());
}

SimulatorQeueFifo::Buffer::~Buffer()
{
    for (int i=0; i<m; i++)
        delete []AStime_ofOccupiedAS_byClassI_inStateN[i];

    delete []occupancyTimes;
    delete []avgNumberOfCalls;
    delete []AStime_ofOccupiedAS_byClassI;
    delete []numberOfCalls;
    delete []numberOfAS;
    delete []AStime_ofOccupiedAS_byClassI_inStateN;
}

void SimulatorQeueFifo::Call::fillData(SimulatorQeueFifo::Call *src)
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

void SimulatorQeueFifo::Call::IncrTimeOfWaitingInBuffer(double time)
{
    timeOnBuffer += time;
    effectiveTimeOnBuffer +=(time * (double)(this->reqAS - this->allocatedAS)/(double) this->reqAS);
}

void SimulatorQeueFifo::Call::IncrTimeInServer(double time)
{
    timeOnServer +=time;
}

void SimulatorQeueFifo::Call::collectTheStats(double time)
{
    timeOnSystem +=time;
    DUtransfered += (time * allocatedAS);
}

} // namespace Algorithms
