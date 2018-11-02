#ifndef SIMULATORNOQEUE_LAG_H
#define SIMULATORNOQEUE_LAG_H

#include <QExplicitlySharedDataPointer>

#include <qglobal.h>
#include <QStack>
#include <QList>
#include <math.h>
#include "simulator.h"
#include "simulatordatacollection.h"

namespace Algorithms
{

class ProcAll;

class SimulatorAll: public simulator
{
public:
    SimulatorAll();

    QString shortName()  const { return "Simulation"; }
    int complexity()     const { return 100; }
    void calculateSystem(const ModelSyst *system
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


    class EvenStatistics
    {
    public:
        long unsigned int inNew;
        long unsigned int inEnd;

        long unsigned int outNew;
        long unsigned int outNewOffered;
        long unsigned int outNewLost;
        long unsigned int outEnd;

        EvenStatistics();

        void statsClear();
    };

    class TimeStatisticsState
    {
    public:
        double availabilityTime;  /// Duration time of "state n equivalent", where V-n AUs are available for a single call. FAG state equivalent
        double occupancyTime;     /// State duration time

        TimeStatisticsState();

        void statsClear();
    };

    class System
    {
    private:
        int m;
        int V;

        int n;
        QVector<int> n_i;

        int old_n;
        int classIdx;

        Server *server;
        QStack<Call *> uselessCalls;
        QList<Call *> callsInSystem;
        simulatorDataCollection<ProcAll> *agenda;
        simulationResults results;

        Call *_getNewCall();
        inline void reuseCall(Call *callToReuse);

        void removeCallFromServer(Call *call);
        inline void removeCallFromQeue(Call *call);

        QVector<TimeStatisticsState>            timesPerState;
        QVector<QVector<TimeStatisticsState> >  timesPerClassAndState;

        QVector<EvenStatistics>                 eventsPerClass;
        QVector<EvenStatistics>                 eventsPerState;
        QVector<QVector <EvenStatistics> >      eventsPerClassAndState;


    public:
        int       totalNumberOfServicedCalls;
        int       totalNumberOfLostCalls;
        const ModelSyst * systemData;

        System(const ModelSyst *system, int noOfSeries);
        ~System();

        void initialize(double a, int sumPropAt, int V);
        void doSimExperiment(int numberOfLostCall, unsigned int seed, int numberOfServicedCalls=0);

#define FOLDINGSTART { //Statistics
        void statsCollectPre(double time);
        void statsCollectPost(int classIdx);
        void statsClear();
        void statsDisable();
        void statsEnable(int serNo);
#define FOLDINGEND }

        void writesResultsOfSingleExperiment(RSingle &singleResults);

        int getServerNumberOfFreeAS();

        Call *getNewCall(Call *parent);
        Call *getNewCall(const ModelTrClass *trClass, int classIdx, double IncE);

        Call *getNewCallUniformIncomServ(ModelTrClass *trClass, int classIdx, double incommingTMin, double incommingTMax, double serviceTMin, double serviceTMax);

        inline double getOccupancyTimeOfState(int n) { return timesPerState[n].occupancyTime; }

        inline long unsigned int getOutNew(int n)             { return eventsPerState[n].outNew; }
        inline long unsigned int getOutEnd(int n)             { return eventsPerState[n].outEnd; }
        inline long unsigned int getInNewSC(int n, int i)    { return eventsPerClassAndState[i][n].inNew; }
        inline long unsigned int getInEndSC(int n, int i)    { return eventsPerClassAndState[i][n].inEnd; }
        inline long unsigned int getOutNewSC(int n, int i)   { return eventsPerClassAndState[i][n].outNew; }
        inline long unsigned int getOutEndSC(int n, int i)   { return eventsPerClassAndState[i][n].outEnd; }

        bool serveNewCall(Call *newCall);
        void endTransmission(Call *call);
        void FinishCall(Call *call, bool acceptedToService);

        void cancellScheduledCall(Call *call);

        inline ProcAll* getNewProcess()           { return agenda->getNewProcess(); }
        inline void addProcess(ProcAll *proc);
        inline void removeProcess(ProcAll *proc);
        inline ProcAll *takeFirstProcess()        { return agenda->takeFirstProcess(); }
        void reuseProcess(ProcAll *proc);

        void serveCallsInEque();
        int getMaxNumberOfAsInSingleGroup();
    };
    class Server
    {

        friend void SimulatorAll::System::writesResultsOfSingleExperiment(RSingle&);
        friend void SimulatorAll::System::statsCollectPre(double time);
    private:
        System const *system;

        const ServerResourcessScheduler subgroupScheduler;

        const int V;      ///Server capacity
        const int vMax;   ///Max number of AU in a single group
        const int k;      ///Number of groups
        const int m;      ///Number of traffic classes
        int n;            /// Server state
        QVector<int> n_i;  /// Server microstate (numbers of occupied AS by all the offered classes

        mutable QVector<int> groupSequence;
        mutable QVector<int> tmpAvailabilityInGroups;


/// Statistics scope: groups
        QVector<QExplicitlySharedDataPointer<Group> > groups;

/// Statistics scope: server state
        QVector<TimeStatisticsState> timePerState;              /// State time statistics (duration and FAG availability equivalent)
        //QVector<double> timePerAsAvailability;                /// Time of availability specified number of AS for single call

        QVector<QVector<double> >resourceUtilizationByClassInState;     /// State duration time*number os AS
        QVector<double>resourceUtilizationByClass;                      /// time*total number of AS ooccupied by calls of class i

        QList<Call *> calls;
        QVector<QExplicitlySharedDataPointer<AU> >serverResourcess;

        double resourceUtilization(int ClassNumber, int stateNo) const;
        double getTimeOfState(int stateNo) const;

        QVector<QPair<QVector<int>, QVector<int> > > combinationList;
        QVector<QVector<double> >freeAUsInWorstGroupInCombination;
        QVector<QVector<double> >freeAUsInBestGroupInCombination;
        QVector<QVector<double> >availabilityOnlyInAllGroupsInCombination;  /// Outside combination, the groups are not available
        QVector<QVector<double> >availabilityInAllGroupsInCombination;      /// Don't care about the groups outside the combination
        QVector<QVector<double> >inavailabilityInAllGroupsInCombination;    /// Don't care about the groups outside the combination

        QVector<QVector<double> >availabilityTimeInGroupSet;    /// Availability in given number of groups, other groups are not considered
        QVector<QVector<double> >availabilityTimeOnlyInExactNoOfGroups; /// Availability in EXACT number of groups, other groups are not available

        void statsColectPreGroupsAvailability(double time);

    public:
        Server(System *system);
        ~Server();

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
        void statsCollectPost(int classIdx);
        inline double statsGetWorkoutPerClassAndState(int i, int n) const;
        inline double statsGetOccupancyTimeOfState(int state) const;
        double statsGetFAGequivalentAvailabilityTimeOfState(int state) const;
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

        int firstAuIndexInAGroup;     /// Index of a first AU in a group, next AUs are allocated in a sequence
        int groupIndex;               /// Index of the group, where the call was allocated

        ProcAll *proc;
        Call *complementaryCall;       /// Only for Pascal tr classes

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
        QVector<int> n_i;        /// Group microstate (numbers of occupied AS by all the offered classes)

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
              SimulatorAll::System *system
            , const ModelTrClass *trClass
            , int classIdx
            , double a
            , int sumPropAt
            , int V
            , double (*funTimeNewCall)(double, double)
            , void (*funNewCall)(ProcAll *proc, SimulatorAll::System *system)
            );

#define INDEPENDENT_ALL(X,Y,NEW_CALL_DISTR,CALL_SERV_DISTRIB) \
    void initializeIndep##X##Y(SimulatorAll::System *system, const ModelTrClass *trClass, int idx, double a, int sumPropAt, int V)\
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

    void initializeDependent(
              SimulatorAll::System *system
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
    void initializeDepMinus##X##Y(SimulatorAll::System *system, const ModelTrClass *trClass, int idx, double a, int sumPropAt, int V) \
    {\
        initializeDependent(system, trClass, idx, a, sumPropAt, V, simulator::distr##NEW_CALL_DISTR, newCallDepMinus##X##Y, transmisionEndedDepMinus##X##Y);\
    }\
     \
    inline static void newCallDepMinus##X##Y(ProcAll *proc, SimulatorAll::System *system)\
    {\
      newCallDepMinus(proc, system, simulator::distr##NEW_CALL_DISTR, simulator::distr##CALL_SERV_DISTRIB, newCallDepMinus##X##Y);\
    }\
     \
    inline static void transmisionEndedDepMinus##X##Y(ProcAll *proc, SimulatorAll::System *system)\
    {\
      transmisionEndedDependentMinus(proc, system, simulator::distr##NEW_CALL_DISTR, newCallDepMinus##X##Y);\
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
    void initializeDepPlus##X##Y(SimulatorAll::System *system, const ModelTrClass *trClass, int idx, double a, int sumPropAt, int V)\
    {\
        initializeDependent(system, trClass, idx, a, sumPropAt, V, simulator::distr##NEW_CALL_DISTR, newCallDepPlus##X##Y, transmisionEndedDependentPlus);\
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

    static void newCallDepMinus(
              ProcAll *proc
            , SimulatorAll::System *system
            , double (*funTimeNewCall)(double, double)
            , double (*funTimeOfService)(double, double)
            , void (*funNewCall)(ProcAll *proc, SimulatorAll::System *system)
            );

    static void newCallDepPlus(
              ProcAll *proc
            , SimulatorAll::System *system
            , double (*funTimeNewCall)(double, double)
            , double (*funTimeOfService)(double, double)
            , void (*funNewCall)(ProcAll *, SimulatorAll::System *)
            );

    static void transmisionEndedIndependent(
              ProcAll *proc
            , SimulatorAll::System *system
            );

    static void transmisionEndedDependentMinus(
              ProcAll *proc
            , SimulatorAll::System *system
            , double (*funTimeNewCall)(double, double)
            , void (*funNewCall)(ProcAll *proc, SimulatorAll::System *system)
            );

    static void transmisionEndedDependentPlus(
              ProcAll *proc
            , SimulatorAll::System *system
            );

public:
    double time;             /// Those value can be out of date. Use them only on object taken from agenda
    uint   idx;              /// Index on the binary heap
    SimulatorAll::Call *callData;

    inline void setUseless()                                       { state = USELESS; }
    inline void removeCallData()                                   { callData = nullptr; }
    inline void setCallData(SimulatorAll::Call *newCallData)       { callData = newCallData; }
    inline bool hasCallData()                                      { return (bool)(callData != nullptr); }

    static void initialize(
              SimulatorAll::System *system
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