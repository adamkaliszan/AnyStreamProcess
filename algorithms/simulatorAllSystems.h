
#ifndef SIMULATOR_ALL_SYSTEMS_H
#define SIMULATOR_ALL_SYSTEMS_H

#include <math.h>
#include <qglobal.h>
#include <QStack>
#include <QList>

#include "simulator.h"
#include "simulatordatacollection.h"
#include "simulatorStatistics.h"

namespace Algorithms
{



class ProcAll;

class SimulatorAll: public Simulator
{
public:
    struct CallRscData
    {
        int allocatedAU;          ///< Number of allocated AS
        int groupIndex;           ///< Index of the group, where the call was allocated
    };

    SimulatorAll();

    QString shortName()  const { return "Simulation"; }
    int complexity()     const { return 100; }
    void calculateSystem(
        const ModelSystem &system
      , double a
      , Results::RInvestigator *results, SimulationParameters *simParameters
    );
    bool possible(const ModelSystem &system) const;


    class System;                     ///< Whole system (Serwer, bufffer(optional) and Calls)
    class Server;                     ///< Consists of one or more groups
    class Buffer;                     ///< Consists of one or more groups
    struct Call;

    enum class EventType
    {
        newCallAccepted,
        newCallEnqued,
        newCallRejected,
        callServiceEnded,
        callServiceEndedAndBufferDequeued
    };

    static StatisticEventType SimEvent2statEvent(EventType simEvent);

    class Engine
    {

    private:
        int       totalNumberOfServicedCalls;  ///< Simulation experiment ending rules
        int       totalNumberOfLostCalls;      ///< Simulation experiment ending rules

        QStack<Call *> uselessCalls;
        SimulatorDataCollection<ProcAll> *agenda;

        Call *_getNewCall();

    public:
        System * const system;

        Engine(System *system);
        ~Engine();

        inline void reuseCall(Call *callToReuse)  { uselessCalls.push(callToReuse); }
        inline void notifyLostCall()              { totalNumberOfLostCalls++; }
        inline void notifyServicedCall()          { totalNumberOfServicedCalls++; }

        void initialize(double a, int sumPropAt, int vk_s);
        double doSimExperiment(int numberOfLostCall, unsigned int seed, int numberOfServicedCalls=0);

        inline void prepareCallToService(Call *callThatIsInService);
        inline void updateServiceTime(Call *uCall);


        inline ProcAll* getNewProcess()           { return agenda->getNewProcess(); }
        inline ProcAll *takeFirstProcess()        { return agenda->takeFirstProcess(); }
        inline void addProcess(ProcAll *proc);
        void removeProcess(ProcAll *proc);
        void reuseProcess(ProcAll *proc);

        Call *getNewCall(Call *parent);
        Call *getNewCall(const ModelTrClass *trClass, int classIdx, double IncE);
    };

    class System
    {
      public:
        Engine *engine;

        const ModelSystem &par;

      private:
        /// ****************************** System components
        Server *server;               ///< Server details
        Buffer *buffer;               ///< Buffer details
        QList<Call *> calls;          ///< Calls in system

        /// ****************************** System state
        struct State
        {
            int n;                    ///< Number of occupied resourcess by all the classes
            int old_n;                ///< Previous number of occupied resourcess by all the classes
            QVector<int> n_i;         ///< Number of occupied resourcess by given class. Vector length is m
            State(const ModelSystem &system): n(0), old_n(0)
            {
                n_i.resize(system.m());
            }
        } state;                      ///< System state

        SystemStatistics *statistics; ///< Statistics that are colected during simulation experiment


      public:
        System(const ModelSystem &system);
        ~System();

#define FOLDINGSTART { //Statistics
        void writesResultsOfSingleExperiment(RSingle &singleResults, double simulationTime);

        void statsCollectPre(double time);
        void statsCollectPost(int classIdx, EventType eventSim);
        void statsClear();
        void statsDisable();
        void statsEnable();
#define FOLDINGEND }

        bool serveNewCall(Call *newCall);                                    ///< this call may be or not accepted to the service
        void endCallService(Call *call);                                     ///< this call was accepted

        inline void finishCall(Call *call, bool acceptedToService)           ///< this call was accepted or rejected
        {   /// Update simulation end conditions
            if (!acceptedToService)
                engine->notifyLostCall();
            else
                engine->notifyServicedCall();
            /// Reuse object
            engine->reuseCall(call);
        }
        inline void cancellScheduledCall(Call *call)                         ///< For state dependent plus surcess
        {
            engine->removeProcess(call->proc);
            engine->reuseCall(call);
        }

    private:
        void addCall(Call *nCall);
        void removeCall(Call *rCall);

        void serveCallsInEque();

        inline void serveCallsInEqueSpDisabled() const { return; }
        void serveCallsInEqueSpSdFifo();
        void serveCallsInEqueSpDFifo();
        void serveCallsInEqueSpQFifo();
        void serveCallsInEqueSpCFifo();
    };

    class CommonRes
    {
        friend void SimulatorAll::System::writesResultsOfSingleExperiment(Results::RSingle&, double simulationTime);
    public:
        const System  * const system;
        const ResourcessScheduler scheduler;          ///< Algorithm that is responsible for choosing the group if more then 1
        const int vTotal;                             ///< Server capacity
        const int vMax;                               ///< Max number of AU in a single group
        const int k;                                  ///< Number of groups
        const int m;                                  ///< Number of traffic classes

    protected:
        struct
        {
            int n;                                    ///< Total number of occupied resourcess
            QVector<int> n_i;                         ///< Occupied resourcess vs class
            QVector<int> n_k;                         ///< Occupied resourcess of given group
            mutable QVector<int> subgroupSequence;    ///< Sequence of checking group for new call service
            mutable QVector<int> subgroupFreeAUs;     ///< Number of AS that is now available in given group

            void addCall(const Call *nCall, int noOfAS, int groupNo)
            {
                n += noOfAS;
                n_i[nCall->classIdx] += noOfAS;
                n_k[groupNo] += noOfAS;
                subgroupFreeAUs[groupNo] -=noOfAS;
            }
            void removeCall(const Call *rCall, const CallRscData &rscData)
            {
                subgroupFreeAUs[rscData.groupIndex]+= rscData.allocatedAU;
                n-= rscData.allocatedAU;
                n_i[rCall->classIdx]-= rscData.allocatedAU;
                n_k[rscData.groupIndex]-= rscData.allocatedAU;
#ifdef QT_DEBUG
                assert(n >=0);
                assert(n_i[rCall->classIdx]>=0);
                assert(n_k[rscData.groupIndex]>=0);
#endif
            }
        } state;
        QList<Call *> calls;

    public:
        CommonRes(SimulatorAll::System *system, const ModelResourcess &res);
        ~CommonRes();

        inline int getV()                                    const { return vTotal; }
        inline int getNoOfFreeAS()                           const { return vTotal - state.n;}
        inline int get_n()                                   const { return state.n; }
        inline const QVector<int> &getMicroStates()          const { return state.n_i; }
        inline const QVector<int> &getOccupancyOfTheGroups() const { return state.n_k; }

        int getMaxNumberOfAsInSingleGroup()                  const;

        bool findAS(int noOfAUs, int &groupNo)               const;
    };

    class Server: public CommonRes
    {
        friend void SimulatorAll::System::writesResultsOfSingleExperiment(RSingle&, double simulationTime);
        friend void SimulatorAll::System::statsCollectPre(double time);

    private:
        ServerStatistics *statistics;

    public:
        Server(System *system);
        ~Server();

        bool addCall(Call *call);
        bool addCall(Call *call, int groupNumber);
        bool addCallPartially(Call *call, int noOfAs);
        bool addCallPartially(Call *call, int noOfAs, int groupNumber);

        void removeCall(Call *call);
#define FOLDINGSTART { //Statistics
        inline void statsClear() { statistics->clear(); }
        void statsEnable();
        void statsDisable();

        void statsColectPre(const ModelSystem &system, double time);
        void statsCollectPost(int classIdx, int old_n, int n, EventType simEvent);

        inline double statsGetWorkoutPerClassAndState(int i, int n) const;
        inline double statsGetOccupancyTimeOfState(int state) const;

        void writesResultsOfSingleExperiment(RSingle& singleResults, double simulationTime);
#define FOLDINGEND }
    };

    class Buffer: public CommonRes
    {
        friend void SimulatorAll::System::writesResultsOfSingleExperiment(Results::RSingle&, double simulationTime);

      private:
        BufferStatistics *statistics;

      public:
        Buffer(System *system);
        ~Buffer();

        void   addCall(SimulatorAll::Call *nCall, int groupNo);
        void   removeCall(Call *first);
        Call*   popCall();

        inline Call *showFirstCall() {   return (calls.isEmpty()) ? nullptr : calls.first();}
        Call   *showCall(int callIdx);

#define FOLDINGSTART { //Statistics
        inline void statsClear()                          {statistics->clear();}
        void statsEnable();
        void statsDisable();

        void statsColectPre(double time);
        void statsCollectPost(int classIdx, int old_n, int n);

        inline double statsGetWorkoutPerClassAndState(int i, int n) const;
        inline double statsGetOccupancyTimeOfState(int state) const;

        void writesResultsOfSingleExperiment(RSingle& singleResults, double simulationTime);
#define FOLDINGEND }
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

        double plannedServiceTime;     ///< Expected time of service ending
        double timeOnServer;           ///< Total time of being in server
        double timeOnSystem;


        double DUmessageSize;
        double DUtransfered;

        const ModelTrClass *trClass;  ///< Traffic Class data (parameters like call service intensity, number of BBUs, ...)
        int classIdx;                 ///< Class number
        int reqAS;                    ///< Required number of allocation slots.

        CallRscData server;           ///< Information about call allocation on server
        CallRscData buffer;           ///< Information about call allocation on buffer

        ProcAll *proc;
        Call *complementaryCall;      ///< Only tr. classes where no os sourcess is increasing with the number of serviced calls

        void fillData(struct Call *src);
        void IncrTimeInServer(double time);
        void collectTheStats(double time);
        SimulatorAll::EventType (*trEndedFun)(ProcAll *proc, SimulatorAll::System *system);
    };


private:
    bool isItTheSameSystem(ModelCreator &system);

};


class ProcAll
{
    friend class SimulatorAll::Engine;
    friend bool SimulatorAll::System::serveNewCall(SimulatorAll::Call *newCallErlang);

public:
    enum class ProcState
    {
        USELESS,
        WAITING_FOR_NEW_CALL,
        SENDING_DATA
    };
#ifdef QT_DEBUG
    ProcState state;
#endif
private:



    void initializeIndependent(SimulatorAll::Engine *system
            , const ModelTrClass *trClass , int classIdx
            , double a, int sumPropAt, int V
            , double (*funTimeNewCall)(double, double)
            , SimulatorAll::EventType (*funNewCall)(ProcAll *, SimulatorAll::System *));



#define INDEPENDENT_ALL(X,Y,NEW_CALL_DISTR,CALL_SERV_DISTRIB) \
    void initializeIndep##X##Y(SimulatorAll::Engine *system, const ModelTrClass *trClass, int idx, double a, int sumPropAt, int V)\
    { \
      initializeIndependent(system, trClass, idx, a, sumPropAt, V, Simulator::distr##NEW_CALL_DISTR, newCallIndep##X##Y);\
    } \
      \
    inline static SimulatorAll::EventType newCallIndep##X##Y(ProcAll *proc, SimulatorAll::System *system)\
    {\
      return newCallIndep(proc, system, Simulator::distr##NEW_CALL_DISTR, Simulator::distr##CALL_SERV_DISTRIB,  newCallIndep##X##Y);\
    }\

    INDEPENDENT_ALL(M,M,LambdaED,LambdaED)
    INDEPENDENT_ALL(M,U,LambdaED,UniformED)
    INDEPENDENT_ALL(M,N,LambdaED,NormalED)
    INDEPENDENT_ALL(M,G,LambdaED,GammaED)
    INDEPENDENT_ALL(M,P,LambdaED,ParetoED)

    INDEPENDENT_ALL(U,M,UniformED,LambdaED)
    INDEPENDENT_ALL(U,U,UniformED,UniformED)
    INDEPENDENT_ALL(U,N,UniformED,NormalED)
    INDEPENDENT_ALL(U,G,UniformED,GammaED)
    INDEPENDENT_ALL(U,P,UniformED,ParetoED)

    INDEPENDENT_ALL(N,M,NormalED,LambdaED)
    INDEPENDENT_ALL(N,U,NormalED,UniformED)
    INDEPENDENT_ALL(N,N,NormalED,NormalED)
    INDEPENDENT_ALL(N,G,NormalED,GammaED)
    INDEPENDENT_ALL(N,P,NormalED,ParetoED)

    INDEPENDENT_ALL(G,M,GammaED,LambdaED)
    INDEPENDENT_ALL(G,U,GammaED,UniformED)
    INDEPENDENT_ALL(G,N,GammaED,NormalED)
    INDEPENDENT_ALL(G,G,GammaED,GammaED)
    INDEPENDENT_ALL(G,P,GammaED,ParetoED)

    INDEPENDENT_ALL(P,M,ParetoED,LambdaED)
    INDEPENDENT_ALL(P,U,ParetoED,UniformED)
    INDEPENDENT_ALL(P,N,ParetoED,NormalED)
    INDEPENDENT_ALL(P,G,ParetoED,GammaED)
    INDEPENDENT_ALL(P,P,ParetoED,ParetoED)

#undef INDEPENDENT_ALL

    void initializeDependent(SimulatorAll::Engine *engine
            , const ModelTrClass *trClass
            , int classIdx
            , double a
            , int sumPropAt
            , int V
            , double (*funTimeNewCall)(double, double)
            , SimulatorAll::EventType(*funNewCall)(ProcAll *, SimulatorAll::System *)
            , SimulatorAll::EventType(*funEndCall)(ProcAll *, SimulatorAll::System *)
            );
#define DEP_MINUS_ALL(X,Y,NEW_CALL_DISTR,CALL_SERV_DISTRIB) \
    void initializeDepMinus##X##Y(SimulatorAll::Engine *engine, const ModelTrClass *trClass, int idx, double a, int sumPropAt, int V) \
    {\
        initializeDependent(engine, trClass, idx, a, sumPropAt, V, Simulator::distr##NEW_CALL_DISTR, newCallDepMinus##X##Y, transmisionEndedDepMinus##X##Y);\
    }\
     \
    inline static SimulatorAll::EventType newCallDepMinus##X##Y(ProcAll *proc, SimulatorAll::System *system)\
    {\
        return newCallDepMinus(proc, system, Simulator::distr##NEW_CALL_DISTR, Simulator::distr##CALL_SERV_DISTRIB, newCallDepMinus##X##Y);\
    }\
     \
    inline static SimulatorAll::EventType transmisionEndedDepMinus##X##Y(ProcAll *proc, SimulatorAll::System *system)\
    {\
        return callServiceEndedDependentMinus(proc, system, Simulator::distr##NEW_CALL_DISTR, newCallDepMinus##X##Y);\
    }\

    DEP_MINUS_ALL(M,M,LambdaED,LambdaED)
    DEP_MINUS_ALL(M,U,LambdaED,UniformED)
    DEP_MINUS_ALL(M,N,LambdaED,NormalED)
    DEP_MINUS_ALL(M,G,LambdaED,GammaED)
    DEP_MINUS_ALL(M,P,LambdaED,ParetoED)

    DEP_MINUS_ALL(U,M,UniformED,LambdaED)
    DEP_MINUS_ALL(U,U,UniformED,UniformED)
    DEP_MINUS_ALL(U,N,UniformED,NormalED)
    DEP_MINUS_ALL(U,G,UniformED,GammaED)
    DEP_MINUS_ALL(U,P,UniformED,ParetoED)

    DEP_MINUS_ALL(N,M,NormalED,LambdaED)
    DEP_MINUS_ALL(N,U,NormalED,UniformED)
    DEP_MINUS_ALL(N,N,NormalED,NormalED)
    DEP_MINUS_ALL(N,G,NormalED,GammaED)
    DEP_MINUS_ALL(N,P,NormalED,ParetoED)

    DEP_MINUS_ALL(G,M,GammaED,LambdaED)
    DEP_MINUS_ALL(G,U,GammaED,UniformED)
    DEP_MINUS_ALL(G,N,GammaED,NormalED)
    DEP_MINUS_ALL(G,G,GammaED,GammaED)
    DEP_MINUS_ALL(G,P,GammaED,ParetoED)

    DEP_MINUS_ALL(P,M,ParetoED,LambdaED)
    DEP_MINUS_ALL(P,U,ParetoED,UniformED)
    DEP_MINUS_ALL(P,N,ParetoED,NormalED)
    DEP_MINUS_ALL(P,G,ParetoED,GammaED)
    DEP_MINUS_ALL(P,P,ParetoED,ParetoED)

#undef DEP_MINUS_ALL


#define DEP_Plus_ALL(X,Y,NEW_CALL_DISTR,CALL_SERV_DISTRIB) \
    void initializeDepPlus##X##Y(SimulatorAll::Engine *engine, const ModelTrClass *trClass, int idx, double a, int sumPropAt, int V)\
    {\
        initializeDependent(engine, trClass, idx, a, sumPropAt, V, Simulator::distr##NEW_CALL_DISTR, newCallDepPlus##X##Y, callServiceEndedDependentPlus);\
    }\
     \
    static SimulatorAll::EventType newCallDepPlus##X##Y(ProcAll *proc, SimulatorAll::System *system)\
    {\
        return newCallDepPlus(proc, system, Simulator::distr##NEW_CALL_DISTR, Simulator::distr##CALL_SERV_DISTRIB, newCallDepPlus##X##Y);\
    }

    DEP_Plus_ALL(M,M,LambdaED,LambdaED)
    DEP_Plus_ALL(M,U,LambdaED,UniformED)
    DEP_Plus_ALL(M,N,LambdaED,NormalED)
    DEP_Plus_ALL(M,G,LambdaED,GammaED)
    DEP_Plus_ALL(M,P,LambdaED,ParetoED)

    DEP_Plus_ALL(U,M,UniformED,LambdaED)
    DEP_Plus_ALL(U,U,UniformED,UniformED)
    DEP_Plus_ALL(U,N,UniformED,NormalED)
    DEP_Plus_ALL(U,G,UniformED,GammaED)
    DEP_Plus_ALL(U,P,UniformED,ParetoED)

    DEP_Plus_ALL(N,M,NormalED,LambdaED)
    DEP_Plus_ALL(N,U,NormalED,UniformED)
    DEP_Plus_ALL(N,N,NormalED,NormalED)
    DEP_Plus_ALL(N,G,NormalED,GammaED)
    DEP_Plus_ALL(N,P,NormalED,ParetoED)

    DEP_Plus_ALL(G,M,GammaED,LambdaED)
    DEP_Plus_ALL(G,U,GammaED,UniformED)
    DEP_Plus_ALL(G,N,GammaED,NormalED)
    DEP_Plus_ALL(G,G,GammaED,GammaED)
    DEP_Plus_ALL(G,P,GammaED,ParetoED)

    DEP_Plus_ALL(P,M,ParetoED,LambdaED)
    DEP_Plus_ALL(P,U,ParetoED,UniformED)
    DEP_Plus_ALL(P,N,ParetoED,NormalED)
    DEP_Plus_ALL(P,G,ParetoED,GammaED)
    DEP_Plus_ALL(P,P,ParetoED,ParetoED)

#undef DEP_Plus_ALL

    static SimulatorAll::EventType newCallIndep(ProcAll *proc, SimulatorAll::System *system
      , double (*funTimeNewCall)(double, double)
      , double (*funTimeOfService)(double, double)
      , SimulatorAll::EventType (*funNewCall)(ProcAll *, SimulatorAll::System *)
    );

    static SimulatorAll::EventType callServiceEndedIndependent(ProcAll *proc, SimulatorAll::System *system);

    static SimulatorAll::EventType newCallDepMinus(ProcAll *proc
      , SimulatorAll::System *system
      , double (*funTimeNewCall)(double, double)
      , double (*funTimeOfService)(double, double)
      , SimulatorAll::EventType(*funNewCall)(ProcAll *, SimulatorAll::System *)
    );

    static SimulatorAll::EventType callServiceEndedDependentMinus(
        ProcAll *proc
      , SimulatorAll::System *system
      , double (*funTimeNewCall)(double, double)
      , SimulatorAll::EventType (*funNewCall)(ProcAll *proc, SimulatorAll::System *system)
    );

    static SimulatorAll::EventType newCallDepPlus(
        ProcAll *proc
      , SimulatorAll::System *system
      , double (*funTimeNewCall)(double, double)
      , double (*funTimeOfService)(double, double)
      , SimulatorAll::EventType (*funNewCall)(ProcAll *, SimulatorAll::System *)
    );

    static SimulatorAll::EventType callServiceEndedDependentPlus(ProcAll *proc, SimulatorAll::System *system);

public:
    double time;             /// Those value can be out of date. Use them only on object taken from agenda
    uint   idx;              /// Index on the binary heap
    SimulatorAll::Call *callData;

#ifdef QT_DEBUG
    inline void setUseless()                                       { state = ProcState::USELESS; }
#endif
    inline void removeCallData()                                   { callData = nullptr; }
    inline void setCallData(SimulatorAll::Call *newCallData)       { callData = newCallData; }
    inline bool hasCallData()                                      { return (callData != nullptr); }

    static void initialize(
        SimulatorAll::Engine *system
      , const ModelTrClass *trClass
      , int classIdx
      , double a
      , int sumPropAt
      , int V
    );
    SimulatorAll::EventType (*execute)(ProcAll *simClass, SimulatorAll::System *system);
};

} // namespace Algorithms

#endif // SIMULATORNOQEUE_LAG_H
