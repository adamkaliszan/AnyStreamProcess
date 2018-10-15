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

class ProcNoQeueLag;

class simulatorNoQeueLag: public simulator
{
public:
    simulatorNoQeueLag(QueueServDiscipline disc = QueueServDiscipline::NoQeue);

    QString shortName()  const { return "Sim. noQueue LAG"; }
    int complexity()     const { return 100; }
    void calculateSystem(const ModelSyst *system
            , double a
            , Results::RInvestigator *results, SimulationParameters *simParameters
            );
    bool possible(const ModelSyst *system) const;

public:
    class System;   ///Whole system (Serwer, bufffer(optional) and Calls)
    class Server;   ///Consists of one or more groups)
    //Optional buffer, now not implemented
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
        const QueueServDiscipline disc;

        int n;
        QVector<int> n_i;

        int old_n;
        int classIdx;

        Server *server;
        QStack<Call *> uselessCalls;
        QList<Call *> callsInSystem;
        simulatorDataCollection<ProcNoQeueLag> *agenda;
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

        System(const ModelSyst *system, int noOfSeries, QueueServDiscipline disc);
        ~System();

        void initialize(double a, int sumPropAt, int V);
        void doSimExperiment(int numberOfLostCall, int seed, int numberOfServicedCalls=0);

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

        inline ProcNoQeueLag* getNewProcess()           { return agenda->getNewProcess(); }
        inline void addProcess(ProcNoQeueLag *proc);
        inline void removeProcess(ProcNoQeueLag *proc);
        inline ProcNoQeueLag *takeFirstProcess()        { return agenda->takeFirstProcess(); }
        void reuseProcess(ProcNoQeueLag *proc);

        void serveCallsInEque();
        int getMaxNumberOfAsInSingleGroup();
    };
    class Server
    {

        friend void simulatorNoQeueLag::System::writesResultsOfSingleExperiment(RSingle&);
        friend void simulatorNoQeueLag::System::statsCollectPre(double time);
    private:
        System const *system;

        const ModelResourcessScheduler subgroupScheduler;

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

        ProcNoQeueLag *proc;
        Call *complementaryCall;       /// Only for Pascal tr classes

        void fillData(struct Call *src);
        void IncrTimeInServer(double time);
        void collectTheStats(double time);
        void (*trEndedFun)(ProcNoQeueLag *proc, simulatorNoQeueLag::System *system);
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

class ProcNoQeueLag
{
    friend bool simulatorNoQeueLag::System::serveNewCall(simulatorNoQeueLag::Call *newCallErlang);

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
              simulatorNoQeueLag::System *system
            , const ModelTrClass *trClass
            , int classIdx
            , double a
            , int sumPropAt
            , int V
            , double (*funTimeNewCall)(double, double)
            , void (*funNewCall)(ProcNoQeueLag *proc, simulatorNoQeueLag::System *system)
            );

#define INDEPENDENT_NO_QEUE(X,Y,NEW_CALL_DISTR,CALL_SERV_DISTRIB) \
    void initializeIndep##X##Y(simulatorNoQeueLag::System *system, const ModelTrClass *trClass, int idx, double a, int sumPropAt, int V)\
    { \
      initializeIndependent(system, trClass, idx, a, sumPropAt, V, simulator::distr##NEW_CALL_DISTR, newCallIndep##X##Y);\
    } \
      \
    inline static void newCallIndep##X##Y(ProcNoQeueLag *proc, simulatorNoQeueLag::System *system)\
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
              simulatorNoQeueLag::System *system
            , const ModelTrClass *trClass
            , int classIdx
            , double a
            , int sumPropAt
            , int V
            , double (*funTimeNewCall)(double, double)
            , void (*funNewCall)(ProcNoQeueLag *proc, simulatorNoQeueLag::System *system)
            , void (*funEndCall)(ProcNoQeueLag *proc, simulatorNoQeueLag::System *system)
            );
#define DEP_MINUS_NO_QEUE(X,Y,NEW_CALL_DISTR,CALL_SERV_DISTRIB) \
    void initializeDepMinus##X##Y(simulatorNoQeueLag::System *system, const ModelTrClass *trClass, int idx, double a, int sumPropAt, int V) \
    {\
        initializeDependent(system, trClass, idx, a, sumPropAt, V, simulator::distr##NEW_CALL_DISTR, newCallDepMinus##X##Y, transmisionEndedDepMinus##X##Y);\
    }\
     \
    inline static void newCallDepMinus##X##Y(ProcNoQeueLag *proc, simulatorNoQeueLag::System *system)\
    {\
      newCallDepMinus(proc, system, simulator::distr##NEW_CALL_DISTR, simulator::distr##CALL_SERV_DISTRIB, newCallDepMinus##X##Y);\
    }\
     \
    inline static void transmisionEndedDepMinus##X##Y(ProcNoQeueLag *proc, simulatorNoQeueLag::System *system)\
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
    void initializeDepPlus##X##Y(simulatorNoQeueLag::System *system, const ModelTrClass *trClass, int idx, double a, int sumPropAt, int V)\
    {\
        initializeDependent(system, trClass, idx, a, sumPropAt, V, simulator::distr##NEW_CALL_DISTR, newCallDepPlus##X##Y, transmisionEndedDependentPlus);\
    }\
     \
    static void newCallDepPlus##X##Y(ProcNoQeueLag *proc, simulatorNoQeueLag::System *system)\
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
              ProcNoQeueLag *proc, simulatorNoQeueLag::System *system
            , double (*funTimeNewCall)(double, double)
            , double (*funTimeOfService)(double, double)
            , void (*funNewCall)(ProcNoQeueLag *proc, simulatorNoQeueLag::System *system)
            );

    static void newCallDepMinus(
              ProcNoQeueLag *proc
            , simulatorNoQeueLag::System *system
            , double (*funTimeNewCall)(double, double)
            , double (*funTimeOfService)(double, double)
            , void (*funNewCall)(ProcNoQeueLag *proc, simulatorNoQeueLag::System *system)
            );

    static void newCallDepPlus(
              ProcNoQeueLag *proc
            , simulatorNoQeueLag::System *system
            , double (*funTimeNewCall)(double, double)
            , double (*funTimeOfService)(double, double)
            , void (*funNewCall)(ProcNoQeueLag *, simulatorNoQeueLag::System *)
            );

    static void transmisionEndedIndependent(
              ProcNoQeueLag *proc
            , simulatorNoQeueLag::System *system
            );

    static void transmisionEndedDependentMinus(
              ProcNoQeueLag *proc
            , simulatorNoQeueLag::System *system
            , double (*funTimeNewCall)(double, double)
            , void (*funNewCall)(ProcNoQeueLag *proc, simulatorNoQeueLag::System *system)
            );

    static void transmisionEndedDependentPlus(
              ProcNoQeueLag *proc
            , simulatorNoQeueLag::System *system
            );

public:
    double time;             /// Those value can be out of date. Use them only on object taken from agenda
    uint   idx;              /// Index on the binary heap
    simulatorNoQeueLag::Call *callData;

    inline void setUseless()                                       { state = USELESS; }
    inline void removeCallData()                                   { callData = nullptr; }
    inline void setCallData(simulatorNoQeueLag::Call *newCallData) { callData = newCallData; }
    inline bool hasCallData()                                      { return (bool)(callData != nullptr); }

    static void initialize(
              simulatorNoQeueLag::System *system
            , const ModelTrClass *trClass
            , int classIdx
            , double a
            , int sumPropAt
            , int V
            );
    void (*execute)(ProcNoQeueLag *simClass, simulatorNoQeueLag::System *system);
};

} // namespace Algorithms

#endif // SIMULATORNOQEUE_LAG_H
