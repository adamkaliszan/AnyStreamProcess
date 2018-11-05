#ifndef SIMULATORNOQEUE_LAG_H
#define SIMULATORNOQEUE_LAG_H

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

class SimulatorAll: public simulator
{
public:
    SimulatorAll();

    QString shortName()  const { return "Simulation"; }
    int complexity()     const { return 100; }
    void calculateSystem(
        const ModelSyst *system
      , double a
      , Results::RInvestigator *results, SimulationParameters *simParameters
    );
    bool possible(const ModelSyst *system) const;


    class System;   ///Whole system (Serwer, bufffer(optional) and Calls)
    class Server;   ///Consists of one or more groups
    class Buffer;   ///Consists of one or more groups
    class Group;    ///Cinsists of V AUs
    class AU;       ///Single allocation unit. Calls can ocupy more then one AU
    struct Call;

    enum class GroupResourcessAllocationlgorithm
    {
        NonContinuous,
        FirstFit,
        BestFit,
        WorstFit,
        Random
    };

    class Engine
    {

    private:
        int       totalNumberOfServicedCalls;  /// Simulation experiment ending rules
        int       totalNumberOfLostCalls;      /// Simulation experiment ending rules

        QStack<Call *> uselessCalls;
        SimulatorDataCollection<ProcAll> *agenda;

        Call *_getNewCall();

    public:
        System * const system;

        Engine(System *system);
        ~Engine();

        inline void reuseCall(Call *callToReuse);
        inline void notifyLostCall() { totalNumberOfLostCalls++;}
        inline void notifyServicedCall() { totalNumberOfServicedCalls++;}

        void initialize(double a, int sumPropAt, int vk_s);
        void doSimExperiment(int numberOfLostCall, unsigned int seed, int numberOfServicedCalls=0);

        inline void prepareCallToService(Call *callThatIsInService);

        inline ProcAll* getNewProcess()           { return agenda->getNewProcess(); }
        inline ProcAll *takeFirstProcess()        { return agenda->takeFirstProcess(); }
        inline void addProcess(ProcAll *proc);
        inline void removeProcess(ProcAll *proc);
        void reuseProcess(ProcAll *proc);

        Call *getNewCall(Call *parent);
        Call *getNewCall(const ModelTrClass *trClass, int classIdx, double IncE);
    };

    class System
    {
    public:
        Engine *engine;

        const ModelSyst *systemData;  /// Description of investigated system
        const int m;                  /// Number of offered traffic classes
        const int vk_sb;              /// Total system capacity
        const int vk_s;               /// Total server capacity
        const int vk_b;               /// Total buffer capacity

    private:
        // System components
        Server *server;               /// Server details
        Buffer *buffer;               /// Buffer details

        QList<Call *> calls;

        // Statistics and results
        SystemStatistics *statistics; /// Statistics that are colected during simulation experiment
        simulationResults results;    //TODO deprecated, use newresult

        // System state
        int n;                        /// Number of occupied resourcess by all the classes
        int old_n;                    /// Previous number of occupied resourcess by all the classes
        QVector<int> n_i;             /// Number of occupied resourcess by given class. Vector length is m


        void removeCallFromServer(Call *call);
        void removeCallFromBuffer(Call *call);

    public:
        System(const ModelSyst *system, int noOfSeries);
        ~System();

#define FOLDINGSTART { //Statistics
        void writesResultsOfSingleExperiment(RSingle &singleResults);

        void statsCollectPre(double time);
        void statsCollectPost(int classIdx);
        void statsClear();
        void statsDisable();
        void statsEnable(int serNo);
#define FOLDINGEND }


        bool serveNewCall(Call *newCall);
        void endCallService(Call *call);
        void FinishCall(Call *call, bool acceptedToService);
        void cancellScheduledCall(Call *call) {  engine->removeProcess(call->proc); engine->reuseCall(call); }

        void serveCallsInEque();
    };
    class Server
    {

        friend void SimulatorAll::System::writesResultsOfSingleExperiment(RSingle&);
        friend void SimulatorAll::System::statsCollectPre(double time);
    public:
        const System  * const system;
        const ServerResourcessScheduler scheduler;    /// Algorithm that is responsible for choosing the group if more then 1
        const int V;                                  /// Server capacity
        const int vMax;                               /// Max number of AU in a single group
        const int k;                                  /// Number of groups
        const int m;                                  /// Number of traffic classes


        QVector<QExplicitlySharedDataPointer<Group> > groups;

    private:
        // Server state
        int n;                                        /// Server state
        QVector<int> n_i;                             /// Server microstate (numbers of occupied AS by all the offered classes

        mutable QVector<int> groupSequence;           /// Sequence of checking group for new call service
        mutable QVector<int> tmpAvailabilityInGroups; /// Number of AS that is now available in given group

        ServerStatistics *statistics;

        QList<Call *> calls;
        QVector<QExplicitlySharedDataPointer<AU> >serverResourcess;

        double resourceUtilization(int ClassNumber, int stateNo) const;
        double getTimeOfState(int stateNo) const;


        void statsColectPreGroupsAvailability(double time);

    public:
        Server(System *system);
        ~Server();

        inline int get_n() const {return n; }
        inline const QVector<int> &getMicroStates() const { return n_i; }

        void writesResultsOfSingleExperiment(RSingle& singleResults, double simulationTime);

        inline int getNoOfFreeAS() const { return V - n; }
        int getMaxNumberOfAsInSingleGroup();

        inline int getV() const {return V;}

        void addCall(Call *call, int noOfAS, int groupNo, const QList<int> &asIndexes, bool newCall);
        void removeCall(Call *call);

#define FOLDINGSTART { //Statistics
        void statsEnable(int serNo);
        void statsDisable();
        void statsClear();
        void statsColectPre(double time);
        void statsCollectPost(int classIdx, int old_n, int n);
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
        bool findAS(int noOfAUs, int &groupNo, QList<int> &auIndexes
                    , GroupResourcessAllocationlgorithm groupResourcessAllocationlgorithm = GroupResourcessAllocationlgorithm::NonContinuous
                ) const;
    };

    class Buffer
    {
        friend void SimulatorAll::System::writesResultsOfSingleExperiment(Results::RSingle&);
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
        QVector<int> n_i;
        Buffer(int V, System *system);
        ~Buffer();

        inline int   getV()                                      { return V; }
        inline int   getNoOfFreeAS()                             { return V - n;}
        inline double getOccupancyTimeOfState(int n)             { return occupancyTimes[n]; }
        inline double getAvarageNumberOfCalls(int classIdx)      { return avgNumberOfCalls[classIdx]; }
        inline double getAvarageNumberOfAS(int classIdx)         { return AStime_ofOccupiedAS_byClassI[classIdx]; }
        inline double getAvgNoOfASinStateN(int classIdx, int n)  { return AStime_ofOccupiedAS_byClassI_inStateN[classIdx][n]; }

#define FOLDINGSTART { //Statistics
        void statsEnable(int serNo);
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

        inline int get_n() const {return n; }
        inline const QVector<int> &getMicroStates() const { return n_i; }



#ifndef DO_NOT_USE_SECUTIRY_CHECKS
        void consistencyCheck()
        {
            int tmp = 0;
            if (firstCall != nullptr)
                tmp = firstCall->reqAS - firstCall->allocatedAS;
            foreach (SimulatorAll::Call *tmpCall, calls)
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
        double timeOnSystem;


        double DUmessageSize;
        double DUtransfered;

        const ModelTrClass *trClass;  /// Traffic Class data (parameters like call service intensity, number of BBUs, ...)
        int classIdx;                 /// Class number
        int reqAS;                    /// Required number of allocation slots.
        int allocatedAS;              /// Number of allocated AS

        int firstAuIndexInAGroup;     /// Index of a first AU in a group, next AUs are allocated in a sequence
        int groupIndex;               /// Index of the group, where the call was allocated

        ProcAll *proc;
        Call *complementaryCall;      /// Only for Pascal tr classes

        void fillData(struct Call *src);
        void IncrTimeInServer(double time);
        void collectTheStats(double time);
        void (*trEndedFun)(ProcAll *proc, SimulatorAll::System *system);
    };

    class Group: public QSharedData
    {
    private:
        const int m;            /// Number of traffic classes
        const int v;            /// Group capacity
        int n;                  /// Group state      (total number of occupied AS)
        QVector<int> n_i;       /// Group microstate (numbers of occupied AS by all the offered classes)

/// Statistics scope: system resourcess
        QVector<QExplicitlySharedDataPointer<AU> > allocationUnits;                          /// Scope: system resourcess

/// Statistics scope: group state
        QVector<double> timePerState;                       /// State duration time
        QVector<QVector<double> >workoutPerClassAndState;   /// State duration time*number os AS

    public:
        Group(int v, int m);
        Group(const Group& rho);

/// Simulation
        /**
         * @brief findAS
         * @param *firstAU
         * @param *noOfFreeAuInARow
         * @param noOfAUs
         * @param allocationStrategy
         * @return True if there is a place for
         */
        int findAS(
                int noOfAUs
              , QList<int>& ausToOccupy
              , GroupResourcessAllocationlgorithm groupResourcessAllocationlgorithm = GroupResourcessAllocationlgorithm::NonContinuous
             ) const;

        int findMaxAS(GroupResourcessAllocationlgorithm groupResourcessAllocationlgorithm = GroupResourcessAllocationlgorithm::NonContinuous) const;

        void removeCall(Call *endedCall);
        void addCall(Call *newCall, const QList<int> &asIndexes);

/// Statistics
        void statsClear();
        void statsColectPre(double time);
        void statsCollectPost(int classIdx);

        int getNoOfFreeAUs(bool considerAllocationAlgorithm=false);
    };

    /**
     * @brief The Allocation Unit
     *
     */
    class AU: public QSharedData
    {
    private:
        int            m;                                 /// Number of offered traffic classes
        Call            *servicedCall;                     /// Call of this class is curently in service
///Statistics
        QVector<double> occupancyTimesPerClass;            /// Times of the occupancy by specified class
        double          occupancyTime;                     /// Time of the ocupancy by any class

    public:
        AU(int m = 0);

        bool isFree() const;
        void increaseOccupancyTime(double time);
        void addCall(Call *newCall);
        void removeCall();
        bool isThisCallHere(Call *comparedCall);

#define FOLDINGSTART { //Statistics
        void statsClear();
        void statsColectPre(double time);
        void statsCollectPost(int classIdx);
#define FOLDINGEND }
    };



private:
    bool isItTheSameSystem(ModelSyst *system);

};

class ProcAll
{
    friend class SimulatorAll::Engine;
    friend bool SimulatorAll::System::serveNewCall(SimulatorAll::Call *newCallErlang);

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
              SimulatorAll::Engine *system
            , const ModelTrClass *trClass
            , int classIdx
            , double a
            , int sumPropAt
            , int V
            , double (*funTimeNewCall)(double, double)
            , void (*funNewCall)(ProcAll *proc, SimulatorAll::System *system)
            );

#define INDEPENDENT_ALL(X,Y,NEW_CALL_DISTR,CALL_SERV_DISTRIB) \
    void initializeIndep##X##Y(SimulatorAll::Engine *system, const ModelTrClass *trClass, int idx, double a, int sumPropAt, int V)\
    { \
      initializeIndependent(system, trClass, idx, a, sumPropAt, V, simulator::distr##NEW_CALL_DISTR, newCallIndep##X##Y);\
    } \
      \
    inline static void newCallIndep##X##Y(ProcAll *proc, SimulatorAll::System *system)\
    {\
      newCallIndep(proc, system, simulator::distr##NEW_CALL_DISTR, simulator::distr##CALL_SERV_DISTRIB,  newCallIndep##X##Y);\
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
            , void (*funNewCall)(ProcAll *proc, SimulatorAll::System *system)
            , void (*funEndCall)(ProcAll *proc, SimulatorAll::System *system)
            );
#define DEP_MINUS_ALL(X,Y,NEW_CALL_DISTR,CALL_SERV_DISTRIB) \
    void initializeDepMinus##X##Y(SimulatorAll::Engine *engine, const ModelTrClass *trClass, int idx, double a, int sumPropAt, int V) \
    {\
        initializeDependent(engine, trClass, idx, a, sumPropAt, V, simulator::distr##NEW_CALL_DISTR, newCallDepMinus##X##Y, transmisionEndedDepMinus##X##Y);\
    }\
     \
    inline static void newCallDepMinus##X##Y(ProcAll *proc, SimulatorAll::System *system)\
    {\
        newCallDepMinus(proc, system, simulator::distr##NEW_CALL_DISTR, simulator::distr##CALL_SERV_DISTRIB, newCallDepMinus##X##Y);\
    }\
     \
    inline static void transmisionEndedDepMinus##X##Y(ProcAll *proc, SimulatorAll::System *system)\
    {\
        callServiceEndedDependentMinus(proc, system, simulator::distr##NEW_CALL_DISTR, newCallDepMinus##X##Y);\
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
        initializeDependent(engine, trClass, idx, a, sumPropAt, V, simulator::distr##NEW_CALL_DISTR, newCallDepPlus##X##Y, callServiceEndedDependentPlus);\
    }\
     \
    static void newCallDepPlus##X##Y(ProcAll *proc, SimulatorAll::System *system)\
    {\
        newCallDepPlus(proc, system, simulator::distr##NEW_CALL_DISTR, simulator::distr##CALL_SERV_DISTRIB, newCallDepPlus##X##Y);\
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

    static void newCallIndep(
        ProcAll *proc, SimulatorAll::System *system
      , double (*funTimeNewCall)(double, double)
      , double (*funTimeOfService)(double, double)
      , void (*funNewCall)(ProcAll *proc, SimulatorAll::System *system)
    );

    static void callServiceEndedIndependent(ProcAll *proc, SimulatorAll::System *system);

    static void newCallDepMinus(
        ProcAll *proc
      , SimulatorAll::System *system
      , double (*funTimeNewCall)(double, double)
      , double (*funTimeOfService)(double, double)
      , void (*funNewCall)(ProcAll *proc, SimulatorAll::System *system)
    );

    static void callServiceEndedDependentMinus(
        ProcAll *proc
      , SimulatorAll::System *system
      , double (*funTimeNewCall)(double, double)
      , void (*funNewCall)(ProcAll *proc, SimulatorAll::System *system)
    );

    static void newCallDepPlus(
        ProcAll *proc
      , SimulatorAll::System *system
      , double (*funTimeNewCall)(double, double)
      , double (*funTimeOfService)(double, double)
      , void (*funNewCall)(ProcAll *, SimulatorAll::System *)
    );

    static void callServiceEndedDependentPlus(ProcAll *proc, SimulatorAll::System *system);

public:
    double time;             /// Those value can be out of date. Use them only on object taken from agenda
    uint   idx;              /// Index on the binary heap
    SimulatorAll::Call *callData;

    inline void setUseless()                                       { state = USELESS; }
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
    void (*execute)(ProcAll *simClass, SimulatorAll::System *system);
};

} // namespace Algorithms

#endif // SIMULATORNOQEUE_LAG_H
