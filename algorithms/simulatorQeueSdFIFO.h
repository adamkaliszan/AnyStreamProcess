#ifndef SIMULATOR_QEUE_SD_FIFO_H
#define SIMULATOR_QEUE_SD_FIFO_H

#include <qglobal.h>

#include <QStack>
#include <QList>
#include <math.h>
#include "simulator.h"
#include "simulatordatacollection.h"

namespace Algorithms
{

class ProcQeueSdFifo;

class SimulatorQeueSdFifo: public Simulator
{
public:
    SimulatorQeueSdFifo();

    QString shortName() const { return "Sim. sdFIFO"; }
    int complexity()    const { return 100; }
    virtual void calculateSystem(const ModelSystem &system
            , double a
            , Results::RInvestigator *results, SimulationParameters *simParameters
            );
    bool possible(const ModelSystem &system) const;

    QSet<Results::Type> getQoS_Set(const ModelSystem &system) const;

public:
    class System;
    class Server;
    struct Call;

    class System
    {
    private:
        QStack<Call *> uselessCalls;
        SimulatorDataCollection<ProcQeueSdFifo> *agenda;
        SimulationResults results;

        Call *_getNewCall();
        inline void reuseCall(Call *callToReuse);

    public:
        Server *server;
        int totalNumberOfLostCalls;
        int totalNumberOfServicedCalls;
        const ModelSystem *systemData;

        System(const ModelSystem &system, int noOfSeries);

        void initialize(double a, int sumPropAt, int V);
        void doSimExperiment(int numberOfLostCall, int seed, int numberOfServicedCalls);

        inline void enableStatisticscollection(int serNo);
        inline void disableStatisticCollection();
        void writesResultsOfSingleExperiment(RSingle &singleRes);
        void collectTheStat(double time);

        Call *getNewCall(Call *parent);
        Call *getNewCall(const ModelTrClass *trClass, int classIdx, double sourceIntensity, double incD, double servE, double servD);

        void serveNewCall(Call *newCall);
        void FinishCall(Call *call, bool acceptedToService);

        inline ProcQeueSdFifo *getNewProcess() { return agenda->getNewProcess(); }
        inline void addProcess(ProcQeueSdFifo *proc);
        inline ProcQeueSdFifo *takeFirstProcess() { return agenda->takeFirstProcess(); }
        inline void reuseProcess(ProcQeueSdFifo *proc);

        void changeServiceTimeOfAllTheCalls();
    };

    class Server
    {
    private:
        System *system;
        int V;                       /// Number of AS that can serve the calls with full spead
        int Vc;                      /// Number of AS that can be addiotionally compressed
        int n;                       /// Number of allocated AS in the system
        int *numberOfClassesInSystem;/// Number of resourcess occupied by classes in system

        double compression;           /// Call service compressio: max(1, n/V)

        double *occupancyTimes;
        double **AStime_ofOccupiedAS_byClassI_inStateN;
        double *avarageNumberOfCalls;
    public:
        QList<Call *> calls;

        Server(int numberOfAS, int compCapacity, System *system);
        inline int getV()  { return V; }
        inline int getVc() { return Vc;}
        inline int get_n() { return n; }
        inline int getOccupancyTimeOfState(int n)              { return occupancyTimes[n];}
        inline int getASSystemForClassInState(int i, int n)   { return this->AStime_ofOccupiedAS_byClassI_inStateN[i][n];}

        inline double getAvarageNumberOfCalls(int i) { return avarageNumberOfCalls[i];}
        inline double getCompression()                { return compression; }

        bool addCall(Call *call, bool &compressionChanged);
        void removeCall(Call *call, bool &compressionChanged);

        inline void clearTheStats()
        {
            bzero(occupancyTimes, (V+Vc+1)*sizeof(double));
            for (int i=0; i<system->systemData->m(); i++)
                bzero(AStime_ofOccupiedAS_byClassI_inStateN[i], (V+Vc+1) * sizeof(double));
        }
        void collectTheStats(double time);
    };

    struct Call
    {
        double sourceE;
        double sourceD;

        double serviceE;
        double serviceD;

        double plannedServiceTime;     /// Expected time of service ending

        double timeOnBuffer;
        double timeOnSystem;

        const ModelTrClass *trClass;   /// Traffic Class data (parameters like call service intensity, number of BBUs, ...)
        int classIdx;                 /// Class number
        int reqAS;                    /// Required number of allocation slots.

        double effectiveServiceTime;   /// sum integral of 1/compression(t)
        double compression;            /// max(1, n/V)

        ProcQeueSdFifo *proc;

        void fillData(struct Call *src)
        {
            reqAS = src->reqAS;
            trClass = src->trClass;
            classIdx = src->classIdx;

            sourceE = src->sourceE;
            sourceD = src->sourceD;

            serviceE = src->serviceE;
            serviceD = src->serviceD;
        }
        void IncrTimeOfWaitingInBuffer(double time);
        void collectTheStats(double time);
    };
};

class ProcQeueSdFifo
{
    friend void SimulatorQeueSdFifo::System::serveNewCall(SimulatorQeueSdFifo::Call *newCallErlangMM);
public:
    enum PROC_STATE
    {
#ifndef DO_NOT_USE_SECUTIRY_CHECKS
        USELESS,
#endif
        WAITING_FOR_NEW,
        SENDING_DATA
    };

private:
    PROC_STATE state;
public:
    double time;                                /// Those value can be out of date. Use them only on object taken from agenda
    uint   idx;                                 /// Index on the binary heap
    SimulatorQeueSdFifo::Call *callData;
    void (*execute)(ProcQeueSdFifo *simClass, SimulatorQeueSdFifo::System *system);
    static void initialize(
              SimulatorQeueSdFifo::System *system
            , const ModelTrClass *trClass
            , int classIdx
            , double a
            , int sumPropAt
            , int V);

private:
    void initializeErlang(
              SimulatorQeueSdFifo::System *system
            , const ModelTrClass *trClass
            , int idx
            , double a
            , int sumPropAt
            , int V
            , double (*funTimeNewCall)(double, double)
            , void (*funNewCall)(ProcQeueSdFifo *proc, SimulatorQeueSdFifo::System *system)
            );
    void initializeErlangMM(
              SimulatorQeueSdFifo::System *system
            , const ModelTrClass *trClass
            , int idx
            , double a
            , int sumPropAt
            , int V
            )
    {
        initializeErlang(system, trClass, idx, a, sumPropAt, V, Simulator::distrLambdaED, newCallErlangMM);
    }

    void initializeErlangMU(
              SimulatorQeueSdFifo::System *system
            , const ModelTrClass *trClass
            , int idx
            , double a
            , int sumPropAt
            , int V
            )
    {
        initializeErlang(system, trClass, idx, a, sumPropAt, V, Simulator::distrLambdaED, newCallErlangMU);
    }

    void initializeErlangMN(
              SimulatorQeueSdFifo::System *system
            , const ModelTrClass *trClass
            , int idx
            , double a
            , int sumPropAt
            , int V
            )
    {
        initializeErlang(system, trClass, idx, a, sumPropAt, V, Simulator::distrLambdaED, newCallErlangMN);
    }

    void initializeErlangUM(
              SimulatorQeueSdFifo::System *system
            , const ModelTrClass *trClass
            , int idx
            , double a
            , int sumPropAt
            , int V
            )
    {
        initializeErlang(system, trClass, idx, a, sumPropAt, V, Simulator::distrUniformED, newCallErlangUM);
    }

    void initializeErlangUU(
            SimulatorQeueSdFifo::System *system
            , const ModelTrClass *trClass
            , int idx
            , double a
            , int sumPropAt
            , int V
            )
    {
        initializeErlang(system, trClass, idx, a, sumPropAt, V, Simulator::distrUniformED, newCallErlangUU);
    }

    void initializeErlangUN(
              SimulatorQeueSdFifo::System *system
            , const ModelTrClass *trClass
            , int idx
            , double a
            , int sumPropAt
            , int V
            )
    {
        initializeErlang(system, trClass, idx, a, sumPropAt, V, Simulator::distrUniformED, newCallErlangUN);
    }

    void initializeErlangNM(
              SimulatorQeueSdFifo::System *system
            , const ModelTrClass *trClass
            , int idx
            , double a
            , int sumPropAt
            , int V
            )
    {
        initializeErlang(system, trClass, idx, a, sumPropAt, V, Simulator::distrNormalED, newCallErlangNM);
    }

    void initializeErlangNU(
              SimulatorQeueSdFifo::System *system
            , const ModelTrClass *trClass
            , int idx
            , double a
            , int sumPropAt
            , int V
            )
    {
        initializeErlang(system, trClass, idx, a, sumPropAt, V, Simulator::distrNormalED, newCallErlangNU);
    }

    void initializeErlangNN(
              SimulatorQeueSdFifo::System *system
            , const ModelTrClass *trClass
            , int idx
            , double a
            , int sumPropAt
            , int V
            )
    {
        initializeErlang(system, trClass, idx, a, sumPropAt, V, Simulator::distrNormalED, newCallErlangNN);
    }

    static void newCallErlang(
              ProcQeueSdFifo *proc
            , SimulatorQeueSdFifo::System *system
            , double (*funTimeNewCall)(double, double)
            , double (*funTimeOfService)(double, double)
            , void (*funNewCall)(ProcQeueSdFifo *proc, SimulatorQeueSdFifo::System *system)
            );

    static void newCallErlangMM(
              ProcQeueSdFifo *proc
            , SimulatorQeueSdFifo::System *system
            )
    {
        ProcQeueSdFifo::newCallErlang(proc, system, Simulator::distrLambdaED, Simulator::distrLambdaED, ProcQeueSdFifo::newCallErlangMM);
    }

    static void newCallErlangMU(
              ProcQeueSdFifo *proc
            , SimulatorQeueSdFifo::System *system
            )
    {
        ProcQeueSdFifo::newCallErlang(proc, system, Simulator::distrLambdaED, Simulator::distrUniformED, ProcQeueSdFifo::newCallErlangMU);
    }

    static void newCallErlangMN(
              ProcQeueSdFifo *proc
            , SimulatorQeueSdFifo::System *system
            )
    {
        ProcQeueSdFifo::newCallErlang(proc, system, Simulator::distrLambdaED, Simulator::distrNormalED, ProcQeueSdFifo::newCallErlangMN);
    }

    static void newCallErlangUM(
              ProcQeueSdFifo *proc
            , SimulatorQeueSdFifo::System *system
            )
    {
        ProcQeueSdFifo::newCallErlang(proc, system, Simulator::distrUniformED, Simulator::distrLambdaED, ProcQeueSdFifo::newCallErlangUM);
    }

    static void newCallErlangUU(
              ProcQeueSdFifo *proc
            , SimulatorQeueSdFifo::System *system
            )
    {
        ProcQeueSdFifo::newCallErlang(proc, system, Simulator::distrUniformED, Simulator::distrUniformED, ProcQeueSdFifo::newCallErlangUU);
    }

    static void newCallErlangUN(
              ProcQeueSdFifo *proc
            , SimulatorQeueSdFifo::System *system
            )
    {
        ProcQeueSdFifo::newCallErlang(proc, system, Simulator::distrUniformED, Simulator::distrNormalED, ProcQeueSdFifo::newCallErlangUN);
    }

    static void newCallErlangNM(
              ProcQeueSdFifo *proc
            , SimulatorQeueSdFifo::System *system
            )
    {
        ProcQeueSdFifo::newCallErlang(proc, system, Simulator::distrNormalED, Simulator::distrLambdaED, ProcQeueSdFifo::newCallErlangNM);
    }

    static void newCallErlangNU(
              ProcQeueSdFifo *proc
            , SimulatorQeueSdFifo::System *system
            )
    {
        ProcQeueSdFifo::newCallErlang(proc, system, Simulator::distrNormalED, Simulator::distrUniformED, ProcQeueSdFifo::newCallErlangNU);
    }

    static void newCallErlangNN(
              ProcQeueSdFifo *proc
            , SimulatorQeueSdFifo::System *system
            )
    {
        ProcQeueSdFifo::newCallErlang(proc, system, Simulator::distrNormalED, Simulator::distrNormalED, ProcQeueSdFifo::newCallErlangNN);
    }

    static void transmisionEnded(ProcQeueSdFifo *proc, SimulatorQeueSdFifo::System *system);

#ifndef DO_NOT_USE_SECUTIRY_CHECKS
public:
    inline void setUseless() { state = USELESS; }
    inline bool hasCallData() {return (bool) (callData != NULL); }
    inline void removeCallData() { callData = NULL; }
    inline void setCallData(SimulatorQeueSdFifo::Call *newCallData) { callData = newCallData; }
#endif

};

} // namespace Algorithms

#endif // SIMULATOR_QEUE_SD_FIFO_H
