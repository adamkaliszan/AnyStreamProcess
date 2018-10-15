#ifndef SIMULATOR_QEUE_FIFO_H
#define SIMULATOR_QEUE_FIFO_H

#include <qglobal.h>
#include <QStack>
#include <QList>
#include <math.h>
#include "simulator.h"
#include "simulatordatacollection.h"

namespace Algorithms
{

class ProcQeueFifo;

class SimulatorQeueFifo: public simulator
{
public:    SimulatorQeueFifo(QueueServDiscipline disc = QueueServDiscipline::cFIFO);

    QString shortName()   const;
    int complexity()      const;

    void calculateSystem(const ModelSyst *system
                , double a
                , Results::RInvestigator *results, SimulationParameters *simParameters
                );
    bool possible(const ModelSyst *system) const;

public:
    class System;
    class Server;
    class Qeue;
    struct Call;



    class System
    {
    private:
        int m;
        int n;
        int old_n;
        int classIdx;
        Server *server;
        Qeue *qeue;
        QStack<Call *> uselessCalls;
        QList<Call *> callsInSystem;
        simulatorDataCollection<ProcQeueFifo> *agenda;
        simulationResults results;

        Call *_getNewCall();
        inline void reuseCall(Call *callToReuse);

        void removeCallFromServer(Call *call);
        inline void removeCallFromQeue(Call *call);

        double     *occupancyTimes;
        double     **occupancyTimesDtl;

        int        *outNew;
        int        *outEnd;
        int        **outNewSCof;
        int        **outEndSCof;
        int        **outNewSCserv;
        int        **outEndSCserv;

        const QueueServDiscipline disc;

    public:
        int       totalNumberOfServicedCalls;
        int       totalNumberOfLostCalls;
        const ModelSyst  *systemData;
        double     *yTime_ClassI;
        double     *servTr_ClassI;

        double     **AStime_ofOccupiedAS_byClassI_inStateN;
        double     **qeueAStime_ofOccupiedAS_byClassI_inStateN;
        double     **serverAStime_ofOccupiedAS_byClassI_inStateN;

        System(const ModelSyst *system, int noOfSeries, QueueServDiscipline disc);
        ~System();

        void initialize(double a, int sumPropAt, int V);
        void doSimExperiment(int numberOfLostCall, int seed, int numberOfServicedCalls=0);

        void collectTheStatPre(double time);
        void collectTheStatPost(double time);

        inline void enableStatisticscollection(int serNo);
        inline void disableStatisticCollection();

        void writesResultsOfSingleExperiment(RSingle &singleResults);

        int getServerNumberOfFreeAS();

        Call *getNewCall(Call *parent);
        Call *getNewCall(const ModelTrClass *trClass, int classIdx, double IncE);

        Call *getNewCallUniformIncomServ(ModelTrClass *trClass, int classIdx, double incommingTMin, double incommingTMax, double serviceTMin, double serviceTMax);

        inline double getOccupancyTimeOfState(int n) { return occupancyTimes[n]; }
        inline double getOccupancyTimeOfState(int n_s, int n_b) { return occupancyTimesDtl[n_s][n_b]; }

        inline int getNoOutNew(int n) { return outNew[n]; }
        inline int getNoOutEnd(int n) { return outEnd[n]; }
        inline int getNoOutNewSCof(int n, int i) { return outNewSCof[i][n]; }
        inline int getNoOutEndSCof(int n, int i) { return outEndSCof[i][n]; }

        inline int getNoOutNewSCserv(int n, int i) { return outNewSCserv[i][n]; }
        inline int getNoOutEndSCserv(int n, int i) { return outEndSCserv[i][n]; }


        //inline double getTimeInNew(int n) { return timeInNew[n]; }
        //inline double getTimeInEnd(int n) { return timeInEnd[n]; }


        bool serveNewCall(Call *newCall);
        void endTransmission(Call *call);
        void FinishCall(Call *call, bool acceptedToService);

        void cancellScheduledCall(Call *call);

        inline ProcQeueFifo* getNewProcess() { return agenda->getNewProcess(); }
        inline void addProcess(ProcQeueFifo *proc);
        inline void removeProcess(ProcQeueFifo *proc);
        inline ProcQeueFifo *takeFirstProcess() { return agenda->takeFirstProcess(); }
        void reuseProcess(ProcQeueFifo *proc);

        void serveCallsInEque();
    };
    class Server
    {
        friend void SimulatorQeueFifo::System::writesResultsOfSingleExperiment(Results::RSingle&);
        friend void SimulatorQeueFifo::System::collectTheStatPre(double time);
    private:
        System *system;
        int V;
        int n;
        int m;
        int    *numberOfAS;                             /// Actual number of AS occupied by call in qeue
        double  *occupancyTimes;
        double **AStime_ofOccupiedAS_byClassI_inStateN;  /// Total time*AS for class i in server state n

        QList<Call *> calls;

    public:
        Server(int noOfAS, System *system);
        ~Server();

        inline int getNoOfFreeAS() { return V - n; }
        inline int getAvgNoOfASinStateN(int i, int n) { return AStime_ofOccupiedAS_byClassI_inStateN[i][n]; }
        inline int getV() {return V;}

        void addCall(Call *call, int noOfAS, bool newCall);
        inline void release(Call *call);

        void clearTheStats();
        void collectTheStats(double time);
        inline double getOccupancyTimeOfState(int state) { return (state <= V) ? occupancyTimes[state] : 0;}
    };
    class Qeue
    {
        friend void SimulatorQeueFifo::System::writesResultsOfSingleExperiment(Results::RSingle&);
        friend void SimulatorQeueFifo::System::collectTheStatPre(double time);
    private:
        System *system;
        int V;                          /// Number of Allocated Slots, that the buffer is able to handle
        int m;
        double *occupancyTimes;

        QStack<Call *> calls;            /// FiFo Qeue with calls
        Call *firstCall;                 /// Call that is partialy serviced

        int   *numberOfCalls;           /// Actual number of calls that are awainting in qeue
        int   *numberOfAS;              /// Actual number of AS occupied by call in qeue
        double *avgNumberOfCalls;        /// Number of calls multiplied by the time in qeue
        double *AStime_ofOccupiedAS_byClassI;            /// Number of calls multiplied by the time in qeue
        double **AStime_ofOccupiedAS_byClassI_inStateN;  /// Avarage number of resuorcess occupied by class i in state n

    public:
        int n;                          /// Number of AS that is used by the calls
        Qeue(int V, System *system);
        ~Qeue();

        inline int   getV()                                      { return V; }
        inline int   getNoOfFreeAS()                             { return V - n;}
        inline double getOccupancyTimeOfState(int n)             { return occupancyTimes[n]; }
        inline double getAvarageNumberOfCalls(int classIdx)      { return avgNumberOfCalls[classIdx]; }
        inline double getAvarageNumberOfAS(int classIdx)         { return AStime_ofOccupiedAS_byClassI[classIdx]; }
        inline double getAvgNoOfASinStateN(int classIdx, int n) { return AStime_ofOccupiedAS_byClassI_inStateN[classIdx][n]; }

        void   clearTheStats();
        void   collectTheStats(double time);

        void   addCall(SimulatorQeueFifo::Call *newCall);
        void   takeCall(SimulatorQeueFifo::Call *call, int noOfAS);
        inline void   removeFirstCall(Call *first);

#ifndef DO_NOT_USE_SECUTIRY_CHECKS
        void consistencyCheck()
        {
            int tmp = 0;
            if (firstCall != NULL)
                tmp = firstCall->reqAS - firstCall->allocatedAS;
            foreach (Call *tmpCall, calls)
            {
                tmp += (tmpCall->reqAS - tmpCall->allocatedAS);
            }
            if (tmp > n)
                qFatal("Qeue error: n=%d, sum ad AS = %d", n, tmp);
        }
#endif
        Call   *getNextCall();
    };
    struct Call
    {
        double sourceE;
        double sourceD;

        double serviceE;
        double serviceD;
        double serviceTmin;
        double serviceTmax;
        double incommingTmin;
        double incommingTmax;

        double plannedServiceTime;     /// Expected time of service ending
        double timeOnServer;           /// Total time of being in server
        double timeOnBuffer;
        double effectiveTimeOnBuffer;
        double timeOnBufferAS;
        double timeOnSystem;


        double DUmessageSize;
        double DUtransfered;

        const ModelTrClass *trClass;   /// Traffic Class data (parameters like call service intensity, number of BBUs, ...)
        int classIdx;                 /// Class number
        int reqAS;                    /// Required number of allocation slots.
        int allocatedAS;              /// Number of allocated AS

        ProcQeueFifo *proc;
        Call *complementaryCall;       /// Only for Pascal tr classes

        void fillData(struct Call *src);
        void IncrTimeOfWaitingInBuffer(double time);
        void IncrTimeInServer(double time);
        void collectTheStats(double time);
        void (*trEndedFun)(ProcQeueFifo *proc, SimulatorQeueFifo::System *system);
    };

private:
    System *systemSimulator;

    bool isItTheSameSystem(ModelSyst *system);
};

class ProcQeueFifo
{
    friend void SimulatorQeueFifo::System::serveCallsInEque();
    friend bool SimulatorQeueFifo::System::serveNewCall(SimulatorQeueFifo::Call *newCallErlang);

public:
    enum PROC_STATE
    {
        USELESS,
        WAITING_FOR_NEW_CALL,
        WAITING_IN_QEUE,              /// call is waiting in buffer
        SENDING_DATA
    };

private:
    PROC_STATE state;

    void initializeIndependent(SimulatorQeueFifo::System *system, const ModelTrClass *trClass, int classIdx, double a, int sumPropAt, int V,
                               double (*funTimeNewCall)(double, double), void (*funNewCall)(ProcQeueFifo *proc, SimulatorQeueFifo::System *system));

#define INDEPENDENT_FIFO(X,Y,NEW_CALL_DISTR,CALL_SERV_DISTRIB) \
    void initializeIndep##X##Y(SimulatorQeueFifo::System *system, const ModelTrClass *trClass, int idx, double a, int sumPropAt, int V)\
    {\
      initializeIndependent(system, trClass, idx, a, sumPropAt, V, simulator::distr##NEW_CALL_DISTR, newCallIndep##X##Y);\
    }\
     \
    inline static void newCallIndep##X##Y(ProcQeueFifo *proc, SimulatorQeueFifo::System *system)\
    {\
      newCallIndep(proc, system, simulator::distr##NEW_CALL_DISTR, simulator::distr##CALL_SERV_DISTRIB,  newCallIndep##X##Y);\
    }\

    INDEPENDENT_FIFO(M,M,LambdaED,LambdaED)
    INDEPENDENT_FIFO(M,U,LambdaED,UniformED)
    INDEPENDENT_FIFO(M,N,LambdaED,NormalED)
    INDEPENDENT_FIFO(M,G,LambdaED,GammaED)
    INDEPENDENT_FIFO(M,P,LambdaED,ParetoED)

    INDEPENDENT_FIFO(U,M,UniformED,LambdaED)
    INDEPENDENT_FIFO(U,U,UniformED,UniformED)
    INDEPENDENT_FIFO(U,N,UniformED,NormalED)
    INDEPENDENT_FIFO(U,G,UniformED,GammaED)
    INDEPENDENT_FIFO(U,P,UniformED,ParetoED)

    INDEPENDENT_FIFO(N,M,NormalED,LambdaED)
    INDEPENDENT_FIFO(N,U,NormalED,UniformED)
    INDEPENDENT_FIFO(N,N,NormalED,NormalED)
    INDEPENDENT_FIFO(N,G,NormalED,GammaED)
    INDEPENDENT_FIFO(N,P,NormalED,ParetoED)

    INDEPENDENT_FIFO(G,M,GammaED,LambdaED)
    INDEPENDENT_FIFO(G,U,GammaED,UniformED)
    INDEPENDENT_FIFO(G,N,GammaED,NormalED)
    INDEPENDENT_FIFO(G,G,GammaED,GammaED)
    INDEPENDENT_FIFO(G,P,GammaED,ParetoED)

    INDEPENDENT_FIFO(P,M,ParetoED,LambdaED)
    INDEPENDENT_FIFO(P,U,ParetoED,UniformED)
    INDEPENDENT_FIFO(P,N,ParetoED,NormalED)
    INDEPENDENT_FIFO(P,G,ParetoED,GammaED)
    INDEPENDENT_FIFO(P,P,ParetoED,ParetoED)

#undef INDEPENDENT_FIFO

    void initializeDependent(
              SimulatorQeueFifo::System *system
            , const ModelTrClass *trClass
            , int classIdx
            , double a
            , int sumPropAt
            , int V
            , double (*funTimeNewCall)(double, double)
            , void (*funNewCall)(ProcQeueFifo *proc, SimulatorQeueFifo::System *system)
            , void (*funEndCall)(ProcQeueFifo *proc, SimulatorQeueFifo::System *system)
            );

#define DEP_MINUS_FIFO(X,Y,NEW_CALL_DISTR,CALL_SERV_DISTRIB) \
    void initializeDepMinus##X##Y(SimulatorQeueFifo::System *system, const ModelTrClass *trClass, int idx, double a, int sumPropAt, int V) \
    {\
        initializeDependent(system, trClass, idx, a, sumPropAt, V, simulator::distr##NEW_CALL_DISTR, newCallDepMinus##X##Y, transmisionEndedDepMinus##X##Y);\
    }\
     \
    inline static void newCallDepMinus##X##Y(ProcQeueFifo *proc, SimulatorQeueFifo::System *system)\
    {\
      newCallDepMinus(proc, system, simulator::distr##NEW_CALL_DISTR, simulator::distr##CALL_SERV_DISTRIB, newCallDepMinus##X##Y);\
    }\
    inline static void transmisionEndedDepMinus##X##Y(ProcQeueFifo *proc, SimulatorQeueFifo::System *system)\
    {\
      transmisionEndedDependentMinus(proc, system, simulator::distr##NEW_CALL_DISTR, newCallDepMinus##X##Y);\
    }\

    DEP_MINUS_FIFO(M,M,LambdaED,LambdaED)
    DEP_MINUS_FIFO(M,U,LambdaED,UniformED)
    DEP_MINUS_FIFO(M,N,LambdaED,NormalED)
    DEP_MINUS_FIFO(M,G,LambdaED,GammaED)
    DEP_MINUS_FIFO(M,P,LambdaED,ParetoED)

    DEP_MINUS_FIFO(U,M,UniformED,LambdaED)
    DEP_MINUS_FIFO(U,U,UniformED,UniformED)
    DEP_MINUS_FIFO(U,N,UniformED,NormalED)
    DEP_MINUS_FIFO(U,G,UniformED,GammaED)
    DEP_MINUS_FIFO(U,P,UniformED,ParetoED)

    DEP_MINUS_FIFO(N,M,NormalED,LambdaED)
    DEP_MINUS_FIFO(N,U,NormalED,UniformED)
    DEP_MINUS_FIFO(N,N,NormalED,NormalED)
    DEP_MINUS_FIFO(N,G,NormalED,GammaED)
    DEP_MINUS_FIFO(N,P,NormalED,ParetoED)

    DEP_MINUS_FIFO(G,M,GammaED,LambdaED)
    DEP_MINUS_FIFO(G,U,GammaED,UniformED)
    DEP_MINUS_FIFO(G,N,GammaED,NormalED)
    DEP_MINUS_FIFO(G,G,GammaED,GammaED)
    DEP_MINUS_FIFO(G,P,GammaED,ParetoED)

    DEP_MINUS_FIFO(P,M,ParetoED,LambdaED)
    DEP_MINUS_FIFO(P,U,ParetoED,UniformED)
    DEP_MINUS_FIFO(P,N,ParetoED,NormalED)
    DEP_MINUS_FIFO(P,G,ParetoED,GammaED)
    DEP_MINUS_FIFO(P,P,ParetoED,ParetoED)

#undef DEP_MINUS_FIFO


#define DEP_PLUS_FIFO(X,Y,NEW_CALL_DISTR,CALL_SERV_DISTRIB) \
    void initializeDepPlus##X##Y(SimulatorQeueFifo::System *system, const ModelTrClass *trClass, int idx, double a, int sumPropAt, int V)\
    {\
        initializeDependent(system, trClass, idx, a, sumPropAt, V, simulator::distr##NEW_CALL_DISTR, newCallDepPlus##X##Y, transmisionEndedDependentPlus);\
    }\
    static void newCallDepPlus##X##Y(ProcQeueFifo *proc, SimulatorQeueFifo::System *system)\
    {\
      newCallDepPlus(proc, system, simulator::distr##NEW_CALL_DISTR, simulator::distr##CALL_SERV_DISTRIB, newCallDepPlus##X##Y);\
    }

    DEP_PLUS_FIFO(M,M,LambdaED,LambdaED)
    DEP_PLUS_FIFO(M,U,LambdaED,UniformED)
    DEP_PLUS_FIFO(M,N,LambdaED,NormalED)
    DEP_PLUS_FIFO(M,G,LambdaED,GammaED)
    DEP_PLUS_FIFO(M,P,LambdaED,ParetoED)

    DEP_PLUS_FIFO(U,M,UniformED,LambdaED)
    DEP_PLUS_FIFO(U,U,UniformED,UniformED)
    DEP_PLUS_FIFO(U,N,UniformED,NormalED)
    DEP_PLUS_FIFO(U,G,UniformED,GammaED)
    DEP_PLUS_FIFO(U,P,UniformED,ParetoED)

    DEP_PLUS_FIFO(N,M,NormalED,LambdaED)
    DEP_PLUS_FIFO(N,U,NormalED,UniformED)
    DEP_PLUS_FIFO(N,N,NormalED,NormalED)
    DEP_PLUS_FIFO(N,G,NormalED,GammaED)
    DEP_PLUS_FIFO(N,P,NormalED,ParetoED)

    DEP_PLUS_FIFO(G,M,GammaED,LambdaED)
    DEP_PLUS_FIFO(G,U,GammaED,UniformED)
    DEP_PLUS_FIFO(G,N,GammaED,NormalED)
    DEP_PLUS_FIFO(G,G,GammaED,GammaED)
    DEP_PLUS_FIFO(G,P,GammaED,ParetoED)

    DEP_PLUS_FIFO(P,M,ParetoED,LambdaED)
    DEP_PLUS_FIFO(P,U,ParetoED,UniformED)
    DEP_PLUS_FIFO(P,N,ParetoED,NormalED)
    DEP_PLUS_FIFO(P,G,ParetoED,GammaED)
    DEP_PLUS_FIFO(P,P,ParetoED,ParetoED)

#undef DEP_PLUS_FIFO

    static void newCallIndep(
              ProcQeueFifo *proc
            , SimulatorQeueFifo::System *system
            , double (*funTimeNewCall)(double, double)
            , double (*funTimeOfService)(double, double)
            , void (*funNewCall)(ProcQeueFifo *proc, SimulatorQeueFifo::System *system)
            );

    static void newCallDepMinus(
              ProcQeueFifo *proc
            , SimulatorQeueFifo::System *system
            , double (*funTimeNewCall)(double, double)
            , double (*funTimeOfService)(double, double)
            , void (*funNewCall)(ProcQeueFifo *proc, SimulatorQeueFifo::System *system)
            );

    static void newCallDepPlus(
              ProcQeueFifo *proc
            , SimulatorQeueFifo::System *system
            , double (*funTimeNewCall)(double, double)
            , double (*funTimeOfService)(double, double)
            , void (*funNewCall)(ProcQeueFifo *, SimulatorQeueFifo::System *)
            );


    static void transmisionEndedIndependent(
              ProcQeueFifo *proc
            , SimulatorQeueFifo::System *system
            );

    static void transmisionEndedDependentMinus(
              ProcQeueFifo *proc
            , SimulatorQeueFifo::System *system
            , double (*funTimeNewCall)(double, double)
            , void (*funNewCall)(ProcQeueFifo *proc, SimulatorQeueFifo::System *system)
            );

    static void transmisionEndedDependentPlus(
              ProcQeueFifo *proc
            , SimulatorQeueFifo::System *system
            );

public:
    double time;             /// Those value can be out of date. Use them only on object taken from agenda
    uint   idx;              /// Index on the binary heap
    SimulatorQeueFifo::Call *callData;

    inline void setUseless()                                       { state = USELESS; }
    inline void removeCallData()                                   { callData = NULL; }
    inline void setCallData(SimulatorQeueFifo::Call *newCallData)  { callData = newCallData; }
    inline bool hasCallData()                                      { return (bool)(callData != NULL); }

    static void initialize(
              SimulatorQeueFifo::System *system
            , const ModelTrClass *trClass
            , int classIdx
            , double a
            , int sumPropAt
            , int V
            );
    void (*execute)(ProcQeueFifo *simClass, SimulatorQeueFifo::System *system);
};

} // namespace Algorithms
#endif // SIMULATOR_QEUE_FIFO_H
