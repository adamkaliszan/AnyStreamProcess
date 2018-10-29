#include "simulatorNoQeue.h"
#include <qstring.h>
#include <qglobal.h>

namespace Algorithms
{

simulatorNoQeue::simulatorNoQeue(BufferResourcessScheduler disc) : simulator(disc)
{
    system = NULL;
}

QString simulatorNoQeue::shortName() const
{
    return "Sim. noQueue";
}

int simulatorNoQeue::complexity() const {return 100;}

bool simulatorNoQeue::possible(const ModelSyst *system) const
{
    if (system->vk_b() > 0)
        return false;

    if (system->k_s() > 0)
        return false;

    return simulator::possible(system);
}

void simulatorNoQeue::calculateSystem(const ModelSyst *system
        , double a
        , RInvestigator *results
        , SimulationParameters *simParameters)
{
    this->system = system;
    System *simData = new System(system, simParameters->noOfSeries, disc);
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
    //emit this->sigCalculationDone();
}

simulatorNoQeue::System::System(const ModelSyst *system, int noOfSeries, BufferResourcessScheduler disc): m(system->m()), n(0), old_n(0), results(system->m(), system->vk_s(), system->vk_b(), noOfSeries), disc(disc)
{
    systemData = system;

    agenda = new simulatorDataCollection<ProcNoQeue>();
    server = new Server(system->vk_s(), this);

    yTime_ClassI                                = new double[system->m()];
    servTr_ClassI                               = new double[system->m()];
    AStime_ofOccupiedAS_byClassI_inStateN       = new double*[system->m()];
    qeueAStime_ofOccupiedAS_byClassI_inStateN   = new double*[system->m()];
    serverAStime_ofOccupiedAS_byClassI_inStateN = new double*[system->m()];
    occupancyTimes                              = new double[system->vk_s()+system->vk_b()+1];

    outNew                                      = new long int[system->vk_s()+system->vk_b()+1];
    outEnd                                      = new long int[system->vk_s()+system->vk_b()+1];

    inNewSC                                     = new long int*[system->m()];
    inEndSC                                     = new long int*[system->m()];
    outNewSC                                    = new long int*[system->m()];
    outEndSC                                    = new long int*[system->m()];


    occupancyTimesDtl                           = new double*[system->vk_s()+1];
    for (int n=0; n<=system->vk_s(); n++)
        occupancyTimesDtl[n] = new double[system->vk_b()+1];

    for (int i=0; i<m; i++)
    {
        AStime_ofOccupiedAS_byClassI_inStateN[i]       = new double[system->vk_s()+system->vk_b()+1];
        qeueAStime_ofOccupiedAS_byClassI_inStateN[i]   = new double[system->vk_s()+system->vk_b()+1];
        serverAStime_ofOccupiedAS_byClassI_inStateN[i] = new double[system->vk_s()+system->vk_b()+1];
        inNewSC[i]                                     = new long int[system->vk_s()+system->vk_b()+1];
        inEndSC[i]                                     = new long int[system->vk_s()+system->vk_b()+1];
        outNewSC[i]                                    = new long int[system->vk_s()+system->vk_b()+1];
        outEndSC[i]                                    = new long int[system->vk_s()+system->vk_b()+1];
    }
}

simulatorNoQeue::System::~System()
{
    for (int n=0; n<=server->getV(); n++)
        delete []occupancyTimesDtl[n];

    delete agenda;
    delete server;

    for (int i=0; i<m; i++)
    {
        delete []AStime_ofOccupiedAS_byClassI_inStateN[i];
        delete []qeueAStime_ofOccupiedAS_byClassI_inStateN[i];
        delete []serverAStime_ofOccupiedAS_byClassI_inStateN[i];

        delete []inNewSC[i];
        delete []inEndSC[i];
        delete []outNewSC[i];
        delete []outEndSC[i];
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

    delete []inNewSC;
    delete []inEndSC;
    delete []outNewSC;
    delete []outEndSC;

    foreach(Call *tmp, callsInSystem)
        delete tmp;

    while (uselessCalls.length())
    {
        Call *tmp = uselessCalls.pop();
        delete tmp;
    }
}

void simulatorNoQeue::System::initialize(double a, int sumPropAt, int V)
{
    for(int i=0; i<systemData->m(); i++)
    {
        const ModelTrClass *tmpClass = systemData->getClass(i);
        ProcNoQeue::initialize(this, tmpClass, i, a, sumPropAt, V);
    }
}

void simulatorNoQeue::System::doSimExperiment(int numberOfLostCall, int seed, int numberOfServicedCalls)
{
    qsrand(seed);
    this->totalNumberOfLostCalls = 0;
    this->totalNumberOfServicedCalls = 0;
    if (numberOfServicedCalls == 0)
    {
        while(totalNumberOfLostCalls < numberOfLostCall)
        {
            ProcNoQeue *proc = takeFirstProcess();
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
                ProcNoQeue *proc = takeFirstProcess();
                collectTheStatPre(proc->time);
                proc->execute(proc, this);
                collectTheStatPost(proc->time);
            }
        }
        else
        {
            while(totalNumberOfServicedCalls < numberOfServicedCalls)
            {
                ProcNoQeue *proc = takeFirstProcess();
                collectTheStatPre(proc->time);
                proc->execute(proc, this);
                collectTheStatPost(proc->time);
            }
        }
    }
}

void simulatorNoQeue::System::writesResultsOfSingleExperiment(Results::RSingle& resultsSingle)
{
    int Vs = server->getV();
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

        double E = 0;
        for (int n_server = qMax(0, (int)Vs-t+1); n_server<=Vs; n_server++)
            E+= getOccupancyTimeOfState(n_server, 0);
        E/= results._simulationTime;
        results.act_E[i] = E;
        resultsSingle.write(TypeForClass::BlockingProbability, E, i);

        results.act_Y[i] =servTr_ClassI[i]/results._simulationTime;
        results.act_y[i] = yTime_ClassI[i]/results._simulationTime;

        results.act_t[i] /= results.act_noOfServicedCalls[i];
        results.act_tS[i] /= results.act_noOfServicedCalls[i];

        results.act_tServer[i] /=results.act_noOfServicedCalls[i];
        results.act_tPlanedServer[i] /=results.act_noOfServicedCalls[i];


        for (int n=0; n<=Vs; n++)
        {
            double x = server->AStime_ofOccupiedAS_byClassI_inStateN[i][n]/server->occupancyTimes[n];
            results.act_LOC_server_yt[i][n] = x;
        }

        for (int n=0; n<=Vs; n++)
        {
            double x = AStime_ofOccupiedAS_byClassI_inStateN[i][n]/getOccupancyTimeOfState(n);
            results.act_SYS_yt[i][n] = x;
            x = qeueAStime_ofOccupiedAS_byClassI_inStateN[i][n]/getOccupancyTimeOfState(n);
            results.act_SYS_qeue_yt[i][n] = x;
            x = serverAStime_ofOccupiedAS_byClassI_inStateN[i][n]/getOccupancyTimeOfState(n);
            results.act_SYS_server_yt[i][n] = x;
        }
    }

    for (int n=0; n<=server->getV(); n++)
    {
        double tmpstateDurationTime = getOccupancyTimeOfState(n);

        results.act_trDistribSys[n] = getOccupancyTimeOfState(n) / results._simulationTime;

        results.act_intOutNew[n] = (double)(getOutNew(n)) / tmpstateDurationTime;
        results.act_intOutEnd[n] = (double)(getOutEnd(n)) / tmpstateDurationTime;

        results.act_noOutNew[n] = getOutNew(n);
        results.act_noOutEnd[n] = getOutEnd(n);


        for (int i=0; i<m; i++)
        {
            int t = this->systemData->getClass(i)->t();

            tmpstateDurationTime = ((int)n-t >= 0) ? getOccupancyTimeOfState(n-t) : 0;
            results.act_intInNewSC[i][n] = (tmpstateDurationTime > 0) ? ((double) (getInNewSC(n, i)) / tmpstateDurationTime) : 0;

            tmpstateDurationTime = (n+t <= server->getV()) ? getOccupancyTimeOfState(n+t) : 0;
            results.act_intInEndSC[i][n] = (tmpstateDurationTime > 0) ? ((double) (getInEndSC(n, i)) / tmpstateDurationTime) : 0;

            tmpstateDurationTime = getOccupancyTimeOfState(n);
            results.act_intOutNewSC[i][n] = (double) (getOutNewSC(n, i)) / tmpstateDurationTime;
            results.act_intOutEndSC[i][n] = (double) (getOutEndSC(n, i)) / tmpstateDurationTime;

            results.act_noInNewSC[i][n]   = getInNewSC(n, i);
            results.act_noInEndSC[i][n]   = getInEndSC(n, i);
            results.act_noOutNewSC[i][n]  = getOutNewSC(n, i);
            results.act_noOutEndSC[i][n]  = getOutEndSC(n, i);
        }
    }

    for (int n=0; n<=server->getV(); n++)
    {
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
    }

    for (int n=0; n<=server->getV(); n++)
    {
        results.act_trDistribServ[n] = server->getOccupancyTimeOfState(n) / results._simulationTime;

        results.act_trDistrib[n][0] = getOccupancyTimeOfState(n, 0) / results._simulationTime;
    }
}

int simulatorNoQeue::System::getServerNumberOfFreeAS()
{
    return server->getNoOfFreeAS();
}

bool simulatorNoQeue::System::serveNewCall(simulatorNoQeue::Call *newCall)
{
#ifndef DO_NOT_USE_SECUTIRY_CHECKS

#endif
    classIdx = newCall->classIdx;

    int serverFreeAS = server->getNoOfFreeAS();
    if (serverFreeAS >= newCall->reqAS)
    {
        callsInSystem.append(newCall);

        newCall->allocatedAS    = newCall->reqAS;
        server->addCall(newCall, newCall->reqAS, true);
        newCall->proc           = agenda->getNewProcess();
        newCall->proc->state    = ProcNoQeue::SENDING_DATA;
        newCall->proc->execute  = newCall->trEndedFun;
        newCall->proc->callData = newCall;
        newCall->proc->time     = newCall->plannedServiceTime;
        agenda->addProcess(newCall->proc);
        n += newCall->reqAS;

        return true;
    }
    else
    {
#ifndef DO_NOT_USE_SECUTIRY_CHECKS
        if(getServerNumberOfFreeAS() >= newCall->reqAS)
            qFatal("Qeue statistics are incorrect, because there is no place in the server but there is a place in whole system");
#endif

        FinishCall(newCall, false);
        return false;
    }
}

void simulatorNoQeue::System::endTransmission(simulatorNoQeue::Call *call)
{
    classIdx = call->classIdx;


    removeCallFromServer(call);
    callsInSystem.removeAll(call);


    n-=call->reqAS;
    FinishCall(call, true);
}

void simulatorNoQeue::System::addProcess(ProcNoQeue *proc)
{
#ifndef DO_NOT_USE_SECUTIRY_CHECKS
    if (proc->time < 0)
        qFatal("Negative time value");
    if (proc->callData->classIdx > this->systemData->m())
        qFatal("Wrong class idx");
#endif
    agenda->addProcess(proc);
}

void simulatorNoQeue::System::removeProcess(ProcNoQeue *proc)
{
#ifndef DO_NOT_USE_SECUTIRY_CHECKS
    if (proc->time < 0)
        qFatal("Negative time value");
#endif
    agenda->removeProcess(&proc);
}

simulatorNoQeue::Call *simulatorNoQeue::System::getNewCall(simulatorNoQeue::Call *parent)
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

simulatorNoQeue::Call *simulatorNoQeue::System::getNewCall(
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

simulatorNoQeue::Call *simulatorNoQeue::System::_getNewCall()
{
    static int noOfCreatedCalls = 0;
    if(uselessCalls.length())
        return uselessCalls.pop();
    noOfCreatedCalls++;
    return new Call;
}

void simulatorNoQeue::System::reuseCall(simulatorNoQeue::Call *callToReuse)
{
    uselessCalls.push(callToReuse);
}

void simulatorNoQeue::System::reuseProcess(ProcNoQeue *proc)
{
    proc->idx = 0;
    proc->setUseless();

    agenda->reuseProcess(proc);
}

void simulatorNoQeue::System::removeCallFromServer(simulatorNoQeue::Call *call)
{
    server->release(call);
}

void simulatorNoQeue::System::FinishCall(simulatorNoQeue::Call *call, bool acceptedToService)
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

void simulatorNoQeue::System::cancellScheduledCall(simulatorNoQeue::Call *call)
{
    reuseCall(call);
}

void simulatorNoQeue::System::collectTheStatPre(double time)
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

    for (int i=0; i<m; i++)
    {
        AStime_ofOccupiedAS_byClassI_inStateN[i][n] += time*(server->numberOfAS[i]);
        serverAStime_ofOccupiedAS_byClassI_inStateN[i][n] += time*(server->numberOfAS[i]);
    }
    occupancyTimes[n] +=time;
    occupancyTimesDtl[server->n][0] += time;
}

void simulatorNoQeue::System::collectTheStatPost(double time)
{
    (void)time;
    if (n == old_n)
    {
        outNew  [old_n]++;
        outNewSC[classIdx][old_n]++;
        return;
    }
    if (n < old_n) //zakończenie obsługi zgłoszenia
    {
        outEnd  [old_n]++;

        inEndSC [classIdx][n]++;
        outEndSC[classIdx][old_n]++;
        return;
    }

    if (n > old_n) //przyjęcie do obsługi zgłoszenia
    {
        outNew  [old_n]++;

        inNewSC [classIdx][n]++;
        outNewSC[classIdx][old_n]++;
        return;
    }
}


void simulatorNoQeue::System::enableStatisticscollection(int serNo)
{
    int VsVb = systemData->vk_s() + systemData->vk_b();
    bzero(yTime_ClassI, systemData->m() * sizeof(double));
    bzero(servTr_ClassI, systemData->m() * sizeof(double));
    bzero(occupancyTimes, (VsVb+1)*sizeof(double));

    bzero(outNew, (VsVb+1)*sizeof(long int));
    bzero(outEnd, (VsVb+1)*sizeof(long int));

    for (int n=0; n<=systemData->vk_s(); n++)
        bzero(occupancyTimesDtl[n], (systemData->vk_b()+1)*sizeof(double));

    for (int i=0; i<m; i++)
    {
        bzero(AStime_ofOccupiedAS_byClassI_inStateN[i], (VsVb+1)*sizeof(double));
        bzero(qeueAStime_ofOccupiedAS_byClassI_inStateN[i], (VsVb+1)*sizeof(double));
        bzero(serverAStime_ofOccupiedAS_byClassI_inStateN[i], (VsVb+1)*sizeof(double));

        bzero(inNewSC[i],  (VsVb+1)*sizeof(long int));
        bzero(inEndSC[i],  (VsVb+1)*sizeof(long int));
        bzero(outNewSC[i],  (VsVb+1)*sizeof(long int));
        bzero(outEndSC[i],  (VsVb+1)*sizeof(long int));
    }

    results.enableStatisticscollection(serNo);
    server->clearTheStats();
}

void simulatorNoQeue::System::disableStatisticCollection()
{
    results.disableStatisticCollection();
}

void simulatorNoQeue::Server::clearTheStats()
{
    bzero(occupancyTimes, (V+1)* sizeof(double));

    for (int i=0; i<m; i++)
        bzero(AStime_ofOccupiedAS_byClassI_inStateN[i], (V+1)*sizeof(double));
}

void simulatorNoQeue::Server::collectTheStats(double time)
{
    for (int i=0; i<m; i++)
    {
        AStime_ofOccupiedAS_byClassI_inStateN[i][n]+=time * numberOfAS[i];
    }
    occupancyTimes[n] += time;

    foreach(Call *tmpCall, this->calls)
        tmpCall->IncrTimeInServer(time);
}

void simulatorNoQeue::Server::addCall(Call *call, int noOfAS, bool newCall)
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

void simulatorNoQeue::Server::release(simulatorNoQeue::Call *call)
{
    numberOfAS[call->classIdx] -=call->allocatedAS;
    this->n -=call->allocatedAS;
    calls.removeAll(call);
}

simulatorNoQeue::Server::Server(int noOfAS, System *system)
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

simulatorNoQeue::Server::~Server()
{
    delete []numberOfAS;

    for (int i=0; i<m; i++)
    {
        delete []AStime_ofOccupiedAS_byClassI_inStateN[i];
    }
    delete []AStime_ofOccupiedAS_byClassI_inStateN;

    delete []occupancyTimes;

    n = 0;
}

void ProcNoQeue::initialize(
          simulatorNoQeue::System *system
        , const ModelTrClass *trClass
        , int classIdx
        , double a
        , int sumPropAt
        , int V
        )
{
    ProcNoQeue *newEvent;
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

void ProcNoQeue::initializeIndependent(
          simulatorNoQeue::System *system
        , const ModelTrClass *trClass
        , int classIdx
        , double a
        , int sumPropAt
        , int V
        , double (*funTimeNewCall)(double, double)
        , void (*funNewCall)(ProcNoQeue *, simulatorNoQeue::System *)
        )
{
    double IncE = 1.0 / trClass->intensityNewCallTotal(a, V, sumPropAt);
    callData = system->getNewCall(trClass, classIdx, IncE);
    callData->trEndedFun = ProcNoQeue::transmisionEndedIndependent;

    this->state = ProcNoQeue::WAITING_FOR_NEW_CALL;
    this->execute = funNewCall;
    this->time = funTimeNewCall(this->callData->sourceE, this->callData->sourceD);
    system->addProcess(this);
}

void ProcNoQeue::ProcNoQeue::initializeDependent(
          simulatorNoQeue::System *system
        , const ModelTrClass *trClass
        , int classIdx
        , double a
        , int sumPropAt
        , int V
        , double (*funTimeNewCall)(double, double)
        , void (*funNewCall)(ProcNoQeue *, simulatorNoQeue::System *)
        , void (*funEndCall)(ProcNoQeue *, simulatorNoQeue::System *)
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

    double IncE = (double)(trClass->s() + sign * A) / trClass->intensityNewCallTotal(a, V, sumPropAt);
    callData = system->getNewCall(trClass, classIdx, IncE);
    callData->trEndedFun = funEndCall;

    this->state = ProcNoQeue::WAITING_FOR_NEW_CALL;
    this->execute = funNewCall;
    this->time = funTimeNewCall(this->callData->sourceE, this->callData->sourceD);
    system->addProcess(this);
}

void ProcNoQeue::newCallIndep(ProcNoQeue *proc, simulatorNoQeue::System *system, double (*funTimeNewCall)(double, double), double (*funTimeOfService)(double, double),
                                 void (*funNewCall)(ProcNoQeue *proc, simulatorNoQeue::System *system))
{
#ifndef DO_NOT_USE_SECUTIRY_CHECKS
    if (proc->callData->classIdx > system->systemData->m())
        qFatal("Wrong class idx");
#endif
    simulatorNoQeue::Call *callData = proc->callData;
    callData->plannedServiceTime = funTimeOfService(callData->serviceE, callData->serviceD);
    callData->reqAS              = proc->callData->reqAS;
    callData->DUmessageSize      = callData->reqAS * callData->plannedServiceTime;
    callData->proc               = nullptr;

    system->serveNewCall(callData);

    //Adding new process with state Waiting for new call
    ProcNoQeue *newProc = proc;
    newProc->callData = system->getNewCall(proc->callData);

    newProc->state = ProcNoQeue::WAITING_FOR_NEW_CALL;
    newProc->execute = funNewCall;
    newProc->time = funTimeNewCall(newProc->callData->sourceE, newProc->callData->sourceD);
    system->addProcess(newProc);
}

void ProcNoQeue::newCallDepMinus(ProcNoQeue *proc, simulatorNoQeue::System *system,
                                   double (*funTimeNewCall)(double, double), double (*funTimeOfService)(double, double),
                                   void (*funNewCall)(ProcNoQeue *, simulatorNoQeue::System *))
{
#ifndef DO_NOT_USE_SECUTIRY_CHECKS
    if (proc->callData->classIdx > system->systemData->m())
        qFatal("Wrong class idx");
#endif
    simulatorNoQeue::Call *callData = proc->callData;
    callData->plannedServiceTime = funTimeOfService(callData->serviceE, callData->serviceD);
    callData->reqAS              = proc->callData->reqAS;
    callData->DUmessageSize      = callData->reqAS * callData->plannedServiceTime;
    callData->proc               = nullptr;


    if (system->serveNewCall(callData) == false)
    {
        ProcNoQeue *newProc = system->getNewProcess();
        newProc->callData = system->getNewCall(callData);

    #ifndef DO_NOT_USE_SECUTIRY_CHECKS
        if (newProc->callData->classIdx > system->systemData->m())
            qFatal("Wrong class idx");
    #endif
        newProc->state = ProcNoQeue::WAITING_FOR_NEW_CALL;
        newProc->execute = funNewCall;
        newProc->time = funTimeNewCall(newProc->callData->sourceE, newProc->callData->sourceD);
        system->addProcess(newProc);
    }

    system->reuseProcess(proc);
}

void ProcNoQeue::newCallDepPlus(ProcNoQeue *proc, simulatorNoQeue::System *system,
                                  double (*funTimeNewCall)(double, double), double (*funTimeOfService)(double, double),
                                  void (*funNewCall)(ProcNoQeue *, simulatorNoQeue::System *))
{
#ifndef DO_NOT_USE_SECUTIRY_CHECKS
    if (proc->callData->classIdx > system->systemData->m())
        qFatal("Wrong class idx");
#endif
    simulatorNoQeue::Call *callData           = proc->callData;
    simulatorNoQeue::Call *parentServicedCall = callData->complementaryCall;

    callData->plannedServiceTime = funTimeOfService(callData->serviceE, callData->serviceD);
    callData->reqAS              = proc->callData->reqAS;
    callData->DUmessageSize      = callData->reqAS * callData->plannedServiceTime;
    callData->proc               = nullptr;

    ProcNoQeue *procNewCall = system->getNewProcess();
    procNewCall->callData = system->getNewCall(callData);
    procNewCall->callData->proc = procNewCall;

    if (parentServicedCall)
    {
        procNewCall->callData->complementaryCall = parentServicedCall;
        parentServicedCall->complementaryCall = procNewCall->callData;
        callData->complementaryCall = nullptr;
    }

    procNewCall->state = ProcNoQeue::WAITING_FOR_NEW_CALL;
    procNewCall->execute = funNewCall;
    procNewCall->time = funTimeNewCall(procNewCall->callData->sourceE, procNewCall->callData->sourceD);
    system->addProcess(procNewCall);

    if (system->serveNewCall(callData) == true)
    {
        ProcNoQeue *newProc = system->getNewProcess();
        newProc->callData = system->getNewCall(callData);
        newProc->callData->proc = newProc;
        callData->complementaryCall = newProc->callData;
        newProc->callData->complementaryCall = callData;

        newProc->state = ProcNoQeue::WAITING_FOR_NEW_CALL;
        newProc->execute = funNewCall;
        newProc->time = funTimeNewCall(newProc->callData->sourceE, newProc->callData->sourceD);
        system->addProcess(newProc);
    }
    system->reuseProcess(proc);
}



void ProcNoQeue::transmisionEndedIndependent(ProcNoQeue *proc, simulatorNoQeue::System *system)
{
#ifndef DO_NOT_USE_SECUTIRY_CHECKS
    if (proc->callData->classIdx > system->systemData->m())
        qFatal("Wrong class idx");
#endif
    system->endTransmission(proc->callData);

    proc->callData = NULL;
    system->reuseProcess(proc);
}

void ProcNoQeue::transmisionEndedDependentMinus(ProcNoQeue *proc, simulatorNoQeue::System *system,
                                                  double (*funTimeNewCall)(double, double),
                                                  void (*funNewCall)(ProcNoQeue *, simulatorNoQeue::System *))
{
    ProcNoQeue *newProc = system->getNewProcess();
    newProc->callData = system->getNewCall(proc->callData);

#ifndef DO_NOT_USE_SECUTIRY_CHECKS
    if (proc->callData->classIdx > system->systemData->m())
        qFatal("Wrong class idx");
#endif
    system->endTransmission(proc->callData);

    proc->callData = NULL;
    system->reuseProcess(proc);

    //Adding new process with state Waiting for new call
    newProc->state = ProcNoQeue::WAITING_FOR_NEW_CALL;
    newProc->execute = funNewCall;
    newProc->time = funTimeNewCall(newProc->callData->sourceE, newProc->callData->sourceD);
    system->addProcess(newProc);
}

void ProcNoQeue::transmisionEndedDependentPlus(ProcNoQeue *proc, simulatorNoQeue::System *system)
{
    simulatorNoQeue::Call *scheduledCall = proc->callData->complementaryCall;

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


void simulatorNoQeue::Call::fillData(simulatorNoQeue::Call *src)
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

void simulatorNoQeue::Call::IncrTimeInServer(double time)
{
    timeOnServer +=time;
}

void simulatorNoQeue::Call::collectTheStats(double time)
{
    timeOnSystem +=time;
    DUtransfered += (time * allocatedAS);
}

} // namespace Algorithms
