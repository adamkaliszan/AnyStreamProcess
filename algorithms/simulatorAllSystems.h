
#ifndef SIMULATOR_ALL_SYSTEMS_H
#define SIMULATOR_ALL_SYSTEMS_H

#include <QExplicitlySharedDataPointer>

#include <qglobal.h>
#include <QStack>
#include <QList>
#include <math.h>
#include "simulator.h"
#include "simulatordatacollection.h"

#include "simulatorStatistics.h"

namespace Algorithms
{

class ProcAll;

class SimulatorAll: public Simulator
{
  public:
    SimulatorAll();

    QString shortName()  const { return "Simulation"; }
    int complexity()     const { return 100; }
    void calculateSystem(
        const ModelSystem &system
      , double a
      , Results::RInvestigator *results, SimulationParameters *simParameters
    );
    bool possible(const ModelCreator *system) const;


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

        const struct Parameters
        {
            const int m;              ///< Number of offered traffic classes
            const int vk_sb;          ///< Total system capacity
            const int vk_s;           ///< Total server capacity
            const int vk_b;           ///< Total buffer capacity
            const QVector<int> t_i;   ///< Numbed od AUs required by single call of coresponding class

            const ModelCreator  *data;   ///< Description of investigated system

            Parameters(const ModelCreator *data);
        } par;                        ///< System Parameters

      private:
      // System components
        Server *server;               ///< Server details
        Buffer *buffer;               ///< Buffer details
        QList<Call *> calls;          ///< Calls in system

      // System state
        struct State
        {
            int n;                    ///< Number of occupied resourcess by all the classes
            int old_n;                ///< Previous number of occupied resourcess by all the classes
            QVector<int> n_i;         ///< Number of occupied resourcess by given class. Vector length is m
            State(const ModelCreator *system): n(0), old_n(0)
            {
                n_i.resize(system->m());
            }
        } state;                      ///< System state

      // System statistics
        SystemStatistics *statistics; ///< Statistics that are colected during simulation experiment


      public:
        System(const ModelCreator *system);
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
        void finishCall(Call *call, bool acceptedToService);                 ///< this call was accepted or rejected
        void cancellScheduledCall(Call *call) {  engine->removeProcess(call->proc); engine->reuseCall(call); }

    private:
        void removeCallFromServer(Call *call);
        void removeCallFromBuffer(Call *call);
        void serveCallsInEque();
    };

    class Server
    {
        friend void SimulatorAll::System::writesResultsOfSingleExperiment(RSingle&, double simulationTime);
        friend void SimulatorAll::System::statsCollectPre(double time);

    public:
        const System  * const system;
        const ResourcessScheduler scheduler;    /// Algorithm that is responsible for choosing the group if more then 1
        const int vTotal;                             /// Server capacity
        const int vMax;                               /// Max number of AU in a single group
        const int k;                                  /// Number of groups
        const int m;                                  /// Number of traffic classes

    private:
        struct
        {
            int n;                                    ///< Total number of occupied resourcess
            QVector<int> n_i;                         ///< Occupied resourcess vs class
            QVector<int> n_k;                         ///< Occupied resourcess of given group
            mutable QVector<int> subgroupSequence;    /// Sequence of checking group for new call service
            mutable QVector<int> subgroupFreeAUs;     /// Number of AS that is now available in given group
        } state;                                      ///< Server state


        ServerStatistics *statistics;

        QList<Call *> calls;

        double resourceUtilization(int ClassNumber, int stateNo) const;
        double getTimeOfState(int stateNo) const;


        void statsColectPreGroupsAvailability(double time);

    public:
        Server(System *system);
        ~Server();

        inline int get_n() const {return state.n; }
        inline const QVector<int> &getMicroStates() const { return state.n_i; }
        inline const QVector<int> &getOccupancyOfTheGroups() const { return state.n_k; }

        void writesResultsOfSingleExperiment(RSingle& singleResults, double simulationTime);

        inline int getNoOfFreeAS() const { return vTotal - state.n; }
        int getMaxNumberOfAsInSingleGroup();

        inline int getV() const {return vTotal;}

        void addCall(Call *call, int noOfAS, int groupNo, bool newCall);
        void removeCall(Call *call);

#define FOLDINGSTART { //Statistics
        void statsEnable();
        void statsDisable();
        void statsClear();

        void statsColectPre(const ModelCreator *mSystem, double time);


        void statsCollectPost(int classIdx, int old_n, int n, EventType simEvent);
        inline double statsGetWorkoutPerClassAndState(int i, int n) const;
        inline double statsGetOccupancyTimeOfState(int state) const;
#define FOLDINGEND }


        /**
         * @brief findAS
         * @param firstAU
         * @param groupNo
         * @param noOfAUs
         * @param allocationStrategy
         * @return True if there is a place for
         */
        bool findAS(int noOfAUs, int &groupNo) const;
    };

    class Buffer
    {
        friend void SimulatorAll::System::writesResultsOfSingleExperiment(Results::RSingle&, double simulationTime);



      public:

        const struct Parameters
        {
            System *system;
            int V;                                                            ///< Number of Allocated Slots, that the buffer is able to handle
            int m;

            Parameters(System *system): system(system), V(system->par.vk_b), m(system->par.m)
            {
            }
        } par;

        struct State
        {
            int n;                                                            ///< Number of AS that is used by the calls
            Call *firstCall;                                                  ///< Call that is partialy serviced

            QVector<int> n_i;                                                 ///< Detailed number of AS that is used by given classes
            QStack<Call *> calls;                                             ///< FiFo Qeue with calls

            State(const System *system): n(0), firstCall(nullptr)
            {
                n_i.resize(system->par.m);
            }
        } state;

      private:
        BufferStatistics *statistics;

      public:
        Buffer(System *system);
        ~Buffer();

        inline int   getV()                                      { return par.V; }
        inline int   getNoOfFreeAS()                             { return par.V - state.n;}

#define FOLDINGSTART { //Statistics
        void statsEnable();
        void statsDisable();
        void statsClear();
        void statsColectPre(double time);
        void statsCollectPost(int classIdx, int old_n, int n);
        inline double statsGetWorkoutPerClassAndState(int i, int n) const;
        inline double statsGetOccupancyTimeOfState(int state) const;
#define FOLDINGEND }

        void   addCall(SimulatorAll::Call *newCall);
        void   takeCall(SimulatorAll::Call *call, int noOfAS);
        inline void   removeCall(Call *first);

        inline int get_n() const {return state.n; }
        inline const QVector<int> &getMicroStates() const { return state.n_i; }
        inline const QVector<int> &getOccupancyOfTheGroups() const { return state.n_i; }


#ifndef DO_NOT_USE_SECUTIRY_CHECKS
        void consistencyCheck()
        {
            int tmp = 0;
            if (state.firstCall != nullptr)
                tmp = state.firstCall->reqAS - state.firstCall->allocatedAS;
            foreach (SimulatorAll::Call *tmpCall, state.calls)
            {
                tmp += (tmpCall->reqAS - tmpCall->allocatedAS);
            }
            if (tmp > state.n)
                qFatal("Qeue error: n=%d, sum ad AS = %d", state.n, tmp);
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

        double plannedServiceTime;     ///< Expected time of service ending
        double timeOnServer;           ///< Total time of being in server
        double timeOnSystem;


        double DUmessageSize;
        double DUtransfered;

        const ModelTrClass *trClass;  ///< Traffic Class data (parameters like call service intensity, number of BBUs, ...)
        int classIdx;                 ///< Class number
        int reqAS;                    ///< Required number of allocation slots.
        int allocatedAS;              ///< Number of allocated AS

        int firstAuIndexInAGroup;     ///< Index of a first AU in a group, next AUs are allocated in a sequence
        int groupIndex;               ///< Index of the group, where the call was allocated

        ProcAll *proc;
        Call *complementaryCall;      ///< Only tr. classes where no os sourcess is increasing with the number of serviced calls

        void fillData(struct Call *src);
        void IncrTimeInServer(double time);
        void collectTheStats(double time);
        SimulatorAll::EventType (*trEndedFun)(ProcAll *proc, SimulatorAll::System *system);
    };


private:
    bool isItTheSameSystem(ModelCreator *system);

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

private:
    ProcState state;


    void initializeIndependent(SimulatorAll::Engine *system
            , const ModelTrClass *trClass
            , int classIdx
            , double a
            , int sumPropAt
            , int V
            , double (*funTimeNewCall)(double, double)
            , SimulatorAll::EventType (*funNewCall)(ProcAll *, SimulatorAll::System *)
            );

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

    inline void setUseless()                                       { state = ProcState::USELESS; }
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
