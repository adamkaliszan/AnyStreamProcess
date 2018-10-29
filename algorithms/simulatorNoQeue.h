#ifndef SIMULATORNOQEUE_H
#define SIMULATORNOQEUE_H


#include <qglobal.h>
#include <QStack>
#include <QList>
#include <math.h>
#include "simulator.h"
#include "simulatordatacollection.h"

namespace Algorithms
{

class ProcNoQeue;

class simulatorNoQeue: public simulator
{
public:
    simulatorNoQeue(BufferResourcessScheduler disc = BufferResourcessScheduler::Disabled);

    QString shortName() const;
    int complexity()    const;
    virtual void calculateSystem(const ModelSyst *system
            , double a
            , Results::RInvestigator *results, SimulationParameters *simParameters
            );
    bool possible(const ModelSyst *system) const;

public:
    class System;
    class Server;
    struct Call;

    class System
    {
    private:
        int m;
        int n;
        int old_n;
        int classIdx;
        Server *server;
        QStack<Call *> uselessCalls;
        QList<Call *> callsInSystem;
        simulatorDataCollection<ProcNoQeue> *agenda;
        simulationResults results;

        Call *_getNewCall();
        inline void reuseCall(Call *callToReuse);

        void removeCallFromServer(Call *call);
        inline void removeCallFromQeue(Call *call);

        double     *occupancyTimes;
        double     **occupancyTimesDtl;

        long int   *outNew;
        long int   *outEnd;
        long int   **inNewSC;
        long int   **inEndSC;
        long int   **outNewSC;
        long int   **outEndSC;

        const BufferResourcessScheduler disc;

    public:
        int       totalNumberOfServicedCalls;
        int       totalNumberOfLostCalls;
        const ModelSyst  *systemData;
        double     *yTime_ClassI;
        double     *servTr_ClassI;

        double     **AStime_ofOccupiedAS_byClassI_inStateN;
        double     **qeueAStime_ofOccupiedAS_byClassI_inStateN;
        double     **serverAStime_ofOccupiedAS_byClassI_inStateN;

        System(const ModelSyst *system, int noOfSeries, BufferResourcessScheduler disc);
        ~System();

        void initialize(double a, int sumPropAt, int V);
        void doSimExperiment(int numberOfLostCall, int seed, int numberOfServicedCalls=0);

        void collectTheStatPre(double time);
        void collectTheStatPost(double time);

        inline void enableStatisticscollection(int serNo);
        inline void disableStatisticCollection();

        void writesResultsOfSingleExperiment(RSingle &resultsSingle);

        int getServerNumberOfFreeAS();

        Call *getNewCall(Call *parent);
        Call *getNewCall(const ModelTrClass *trClass, int classIdx, double IncE);

        Call *getNewCallUniformIncomServ(ModelTrClass *trClass, int classIdx, double incommingTMin, double incommingTMax, double serviceTMin, double serviceTMax);

        inline double getOccupancyTimeOfState(int n) { return occupancyTimes[n]; }
        inline double getOccupancyTimeOfState(int n_s, int n_b) { return occupancyTimesDtl[n_s][n_b]; }

        inline long int getOutNew(int n)             { return outNew[n]; }
        inline long int getOutEnd(int n)             { return outEnd[n]; }
        inline long int getInNewSC(int n, int i)    { return inNewSC[i][n]; }
        inline long int getInEndSC(int n, int i)    { return inEndSC[i][n]; }
        inline long int getOutNewSC(int n, int i)   { return outNewSC[i][n]; }
        inline long int getOutEndSC(int n, int i)   { return outEndSC[i][n]; }




        //inline double getTimeInNew(int n) { return timeInNew[n]; }
        //inline double getTimeInEnd(int n) { return timeInEnd[n]; }


        bool serveNewCall(Call *newCall);
        void endTransmission(Call *call);
        void FinishCall(Call *call, bool acceptedToService);

        void cancellScheduledCall(Call *call);

        inline ProcNoQeue* getNewProcess() { return agenda->getNewProcess(); }
        inline void addProcess(ProcNoQeue *proc);
        inline void removeProcess(ProcNoQeue *proc);
        inline ProcNoQeue *takeFirstProcess() { return agenda->takeFirstProcess(); }
        void reuseProcess(ProcNoQeue *proc);

        void serveCallsInEque();
    };
    class Server
    {
        friend void simulatorNoQeue::System::writesResultsOfSingleExperiment(Results::RSingle&);
        friend void simulatorNoQeue::System::collectTheStatPre(double time);
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
        double timeOnSystem;


        double DUmessageSize;
        double DUtransfered;

        const ModelTrClass *trClass;   /// Traffic Class data (parameters like call service intensity, number of BBUs, ...)
        int classIdx;                 /// Class number
        int reqAS;                    /// Required number of allocation slots.
        int allocatedAS;              /// Number of allocated AS

        ProcNoQeue *proc;
        Call *complementaryCall;       /// Only for Pascal tr classes

        void fillData(struct Call *src);
        void IncrTimeInServer(double time);
        void collectTheStats(double time);
        void (*trEndedFun)(ProcNoQeue *proc, simulatorNoQeue::System *system);
    };

private:
    bool isItTheSameSystem(const ModelSyst *system);
};

class ProcNoQeue
{
    friend bool simulatorNoQeue::System::serveNewCall(simulatorNoQeue::Call *newCallErlang);

public:
    enum PROC_STATE
    {
        USELESS,
        WAITING_FOR_NEW_CALL,
        SENDING_DATA
    };

private:
    PROC_STATE state;


    void initializeIndependent(
              simulatorNoQeue::System *system
            , const ModelTrClass *trClass
            , int classIdx
            , double a
            , int sumPropAt
            , int V
            , double (*funTimeNewCall)(double, double)
            , void (*funNewCall)(ProcNoQeue *proc, simulatorNoQeue::System *system));

#define INDEPENDENT_NO_QEUE(X,Y,NEW_CALL_DISTR,CALL_SERV_DISTRIB) \
    void initializeIndep##X##Y(simulatorNoQeue::System *system, const ModelTrClass *trClass, int idx, double a, int sumPropAt, int V)\
    { \
      initializeIndependent(system, trClass, idx, a, sumPropAt, V, simulator::distr##NEW_CALL_DISTR, newCallIndep##X##Y);\
    } \
      \
    inline static void newCallIndep##X##Y(ProcNoQeue *proc, simulatorNoQeue::System *system)\
    {\
      newCallIndep(proc, system, simulator::distr##NEW_CALL_DISTR, simulator::distr##CALL_SERV_DISTRIB,  newCallIndep##X##Y);\
    }\

    INDEPENDENT_NO_QEUE(M,M,LambdaED,LambdaED)
    INDEPENDENT_NO_QEUE(M,U,LambdaED,UniformED)
    INDEPENDENT_NO_QEUE(M,N,LambdaED,NormalED)
    INDEPENDENT_NO_QEUE(M,G,LambdaED,GammaED)
    INDEPENDENT_NO_QEUE(M,P,LambdaED,ParetoED)

    INDEPENDENT_NO_QEUE(U,M,UniformED,LambdaED)
    INDEPENDENT_NO_QEUE(U,U,UniformED,UniformED)
    INDEPENDENT_NO_QEUE(U,N,UniformED,NormalED)
    INDEPENDENT_NO_QEUE(U,G,UniformED,GammaED)
    INDEPENDENT_NO_QEUE(U,P,UniformED,ParetoED)

    INDEPENDENT_NO_QEUE(N,M,NormalED,LambdaED)
    INDEPENDENT_NO_QEUE(N,U,NormalED,UniformED)
    INDEPENDENT_NO_QEUE(N,N,NormalED,NormalED)
    INDEPENDENT_NO_QEUE(N,G,NormalED,GammaED)
    INDEPENDENT_NO_QEUE(N,P,NormalED,ParetoED)

    INDEPENDENT_NO_QEUE(G,M,GammaED,LambdaED)
    INDEPENDENT_NO_QEUE(G,U,GammaED,UniformED)
    INDEPENDENT_NO_QEUE(G,N,GammaED,NormalED)
    INDEPENDENT_NO_QEUE(G,G,GammaED,GammaED)
    INDEPENDENT_NO_QEUE(G,P,GammaED,ParetoED)

    INDEPENDENT_NO_QEUE(P,M,ParetoED,LambdaED)
    INDEPENDENT_NO_QEUE(P,U,ParetoED,UniformED)
    INDEPENDENT_NO_QEUE(P,N,ParetoED,NormalED)
    INDEPENDENT_NO_QEUE(P,G,ParetoED,GammaED)
    INDEPENDENT_NO_QEUE(P,P,ParetoED,ParetoED)

#undef INDEPENDENT_NO_QEUE

    void initializeDependent(
              simulatorNoQeue::System *system
            , const ModelTrClass *trClass
            , int classIdx
            , double a
            , int sumPropAt
            , int V
            , double (*funTimeNewCall)(double, double)
            , void (*funNewCall)(ProcNoQeue *proc, simulatorNoQeue::System *system)
            , void (*funEndCall)(ProcNoQeue *proc, simulatorNoQeue::System *system)
            );
#define DEP_MINUS_NO_QEUE(X,Y,NEW_CALL_DISTR,CALL_SERV_DISTRIB) \
    void initializeDepMinus##X##Y(simulatorNoQeue::System *system, const ModelTrClass *trClass, int idx, double a, int sumPropAt, int V) \
    {\
        initializeDependent(system, trClass, idx, a, sumPropAt, V, simulator::distr##NEW_CALL_DISTR, newCallDepMinus##X##Y, transmisionEndedDepMinus##X##Y);\
    }\
     \
    inline static void newCallDepMinus##X##Y(ProcNoQeue *proc, simulatorNoQeue::System *system)\
    {\
      newCallDepMinus(proc, system, simulator::distr##NEW_CALL_DISTR, simulator::distr##CALL_SERV_DISTRIB, newCallDepMinus##X##Y);\
    }\
     \
    inline static void transmisionEndedDepMinus##X##Y(ProcNoQeue *proc, simulatorNoQeue::System *system)\
    {\
      transmisionEndedDependentMinus(proc, system, simulator::distr##NEW_CALL_DISTR, newCallDepMinus##X##Y);\
    }\

    DEP_MINUS_NO_QEUE(M,M,LambdaED,LambdaED)
    DEP_MINUS_NO_QEUE(M,U,LambdaED,UniformED)
    DEP_MINUS_NO_QEUE(M,N,LambdaED,NormalED)
    DEP_MINUS_NO_QEUE(M,G,LambdaED,GammaED)
    DEP_MINUS_NO_QEUE(M,P,LambdaED,ParetoED)

    DEP_MINUS_NO_QEUE(U,M,UniformED,LambdaED)
    DEP_MINUS_NO_QEUE(U,U,UniformED,UniformED)
    DEP_MINUS_NO_QEUE(U,N,UniformED,NormalED)
    DEP_MINUS_NO_QEUE(U,G,UniformED,GammaED)
    DEP_MINUS_NO_QEUE(U,P,UniformED,ParetoED)

    DEP_MINUS_NO_QEUE(N,M,NormalED,LambdaED)
    DEP_MINUS_NO_QEUE(N,U,NormalED,UniformED)
    DEP_MINUS_NO_QEUE(N,N,NormalED,NormalED)
    DEP_MINUS_NO_QEUE(N,G,NormalED,GammaED)
    DEP_MINUS_NO_QEUE(N,P,NormalED,ParetoED)

    DEP_MINUS_NO_QEUE(G,M,GammaED,LambdaED)
    DEP_MINUS_NO_QEUE(G,U,GammaED,UniformED)
    DEP_MINUS_NO_QEUE(G,N,GammaED,NormalED)
    DEP_MINUS_NO_QEUE(G,G,GammaED,GammaED)
    DEP_MINUS_NO_QEUE(G,P,GammaED,ParetoED)

    DEP_MINUS_NO_QEUE(P,M,ParetoED,LambdaED)
    DEP_MINUS_NO_QEUE(P,U,ParetoED,UniformED)
    DEP_MINUS_NO_QEUE(P,N,ParetoED,NormalED)
    DEP_MINUS_NO_QEUE(P,G,ParetoED,GammaED)
    DEP_MINUS_NO_QEUE(P,P,ParetoED,ParetoED)

#undef DEP_MINUS_NO_QEUE


#define DEP_Plus_NO_QEUE(X,Y,NEW_CALL_DISTR,CALL_SERV_DISTRIB) \
    void initializeDepPlus##X##Y(simulatorNoQeue::System *system, const ModelTrClass *trClass, int idx, double a, int sumPropAt, int V)\
    {\
        initializeDependent(system, trClass, idx, a, sumPropAt, V, simulator::distr##NEW_CALL_DISTR, newCallDepPlus##X##Y, transmisionEndedDependentPlus);\
    }\
     \
    static void newCallDepPlus##X##Y(ProcNoQeue *proc, simulatorNoQeue::System *system)\
    {\
      newCallDepPlus(proc, system, simulator::distr##NEW_CALL_DISTR, simulator::distr##CALL_SERV_DISTRIB, newCallDepPlus##X##Y);\
    }

    DEP_Plus_NO_QEUE(M,M,LambdaED,LambdaED)
    DEP_Plus_NO_QEUE(M,U,LambdaED,UniformED)
    DEP_Plus_NO_QEUE(M,N,LambdaED,NormalED)
    DEP_Plus_NO_QEUE(M,G,LambdaED,GammaED)
    DEP_Plus_NO_QEUE(M,P,LambdaED,ParetoED)

    DEP_Plus_NO_QEUE(U,M,UniformED,LambdaED)
    DEP_Plus_NO_QEUE(U,U,UniformED,UniformED)
    DEP_Plus_NO_QEUE(U,N,UniformED,NormalED)
    DEP_Plus_NO_QEUE(U,G,UniformED,GammaED)
    DEP_Plus_NO_QEUE(U,P,UniformED,ParetoED)

    DEP_Plus_NO_QEUE(N,M,NormalED,LambdaED)
    DEP_Plus_NO_QEUE(N,U,NormalED,UniformED)
    DEP_Plus_NO_QEUE(N,N,NormalED,NormalED)
    DEP_Plus_NO_QEUE(N,G,NormalED,GammaED)
    DEP_Plus_NO_QEUE(N,P,NormalED,ParetoED)

    DEP_Plus_NO_QEUE(G,M,GammaED,LambdaED)
    DEP_Plus_NO_QEUE(G,U,GammaED,UniformED)
    DEP_Plus_NO_QEUE(G,N,GammaED,NormalED)
    DEP_Plus_NO_QEUE(G,G,GammaED,GammaED)
    DEP_Plus_NO_QEUE(G,P,GammaED,ParetoED)

    DEP_Plus_NO_QEUE(P,M,ParetoED,LambdaED)
    DEP_Plus_NO_QEUE(P,U,ParetoED,UniformED)
    DEP_Plus_NO_QEUE(P,N,ParetoED,NormalED)
    DEP_Plus_NO_QEUE(P,G,ParetoED,GammaED)
    DEP_Plus_NO_QEUE(P,P,ParetoED,ParetoED)

#undef DEP_Plus_NO_QEUE


    static void newCallIndep(
              ProcNoQeue *proc
            , simulatorNoQeue::System *system
            , double (*funTimeNewCall)(double, double)
            , double (*funTimeOfService)(double, double)
            , void (*funNewCall)(ProcNoQeue *proc, simulatorNoQeue::System *system)
            );

    static void newCallDepMinus(
              ProcNoQeue *proc
            , simulatorNoQeue::System *system
            , double (*funTimeNewCall)(double, double)
            , double (*funTimeOfService)(double, double)
            , void (*funNewCall)(ProcNoQeue *proc, simulatorNoQeue::System *system)
            );

    static void newCallDepPlus(
              ProcNoQeue *proc
            , simulatorNoQeue::System *system
            , double (*funTimeNewCall)(double, double)
            , double (*funTimeOfService)(double, double)
            , void (*funNewCall)(ProcNoQeue *, simulatorNoQeue::System *)
            );

    static void transmisionEndedIndependent(
              ProcNoQeue *proc
            , simulatorNoQeue::System *system
            );

    static void transmisionEndedDependentMinus(
              ProcNoQeue *proc
            , simulatorNoQeue::System *system
            , double (*funTimeNewCall)(double, double)
            , void (*funNewCall)(ProcNoQeue *proc, simulatorNoQeue::System *system)
            );

    static void transmisionEndedDependentPlus(
              ProcNoQeue *proc
            , simulatorNoQeue::System *system
            );

public:
    double time;             /// Those value can be out of date. Use them only on object taken from agenda
    uint   idx;              /// Index on the binary heap
    simulatorNoQeue::Call *callData;

    inline void setUseless()                                       { state = USELESS; }
    inline void removeCallData()                                   { callData = NULL; }
    inline void setCallData(simulatorNoQeue::Call *newCallData)    { callData = newCallData; }
    inline bool hasCallData()                                      { return (bool)(callData != NULL); }

    static void initialize(
              simulatorNoQeue::System *system
            , const ModelTrClass *trClass
            , int classIdx
            , double a
            , int sumPropAt
            , int V
            );
    void (*execute)(ProcNoQeue *simClass, simulatorNoQeue::System *system);
};

} // namespace Algorithms

#endif // SIMULATORNOQEUE_H
