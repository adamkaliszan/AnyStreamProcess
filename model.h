#ifndef MODEL_H
#define MODEL_H

#include <QMetaType>
#include <QTextStream>
#include <QDebug>
#include <qglobal.h>
#include <math.h>

#include "utils/probDistributions.h"
#include "algorithms/trclvector2.h"


/*
 * Server
 */
enum class ServerResourcessScheduler
{
    Random,
    Sequencial
};

QString serverResourcessSchedulerToString(ServerResourcessScheduler value);

enum class BufferResourcessScheduler
{
    Continuos,     //Call can be splited between server and buffers resourcess
    SD_FIFO,       //All calls are being served. If there is no room in server, the service time is increased
    dFIFO_Seq,     //Dyskretna fifo, zgłoszenie trafia do bufora jeśli bufor jest zajęty
    qFIFO_Seq,     //Zgłoszenie może trafić do serwera, jeśli jest tam miejsce, a w buforze są inne zgłoszenia
    Disabled   //Brak kolejki
};

QString bufferResourcessSchedulerToString(BufferResourcessScheduler value);


class ModelResourcess
{
private:
    int _k;  ///Number of the grous
    int _v;  ///single group capacity

public:
    ModelResourcess(int k, int v);
    ModelResourcess(): _k(0), _v(0) {}

    void set_v(int v);
    void set_k(int k);
    int v();
    int k();
    int V();
};

class ModelTrClass
{
    friend QTextStream& operator<<(QTextStream &stream, TrClVector &ModelTrClass);
    friend QDebug&      operator<<(QDebug &stream, ModelTrClass &trClass);

public:
    enum class SourceType
    {
        Independent,
        DependentMinus,
        DependentPlus
    };

    enum class StreamType
    {
        Poisson,
        Uniform,
        Normal,
        Gamma,
        Pareto
    };

public:
    static QString streamTypeToString(StreamType str);
    static QString streamTypeToShortString(StreamType str);

private:
    StreamType _newCallStr;
    StreamType _callServStr;
    SourceType _srcType;

    int _noOfSourcess;
    int _propAt;
    int _t;
    double _mu;
    double _ServiceExPerDx;
    double _IncommingExPerDx;

public:
    ModelTrClass();
    ModelTrClass(const ModelTrClass& rho);

    QString shortName() const;

    void setNewCallStrType(StreamType strType, SourceType srcType);
    void setCallServStrType(StreamType strType);

    void setNoOfSourcess(int noOfSourcess);
    void setPropAt(int propAt);

    inline void setServiceExPerDx(double value) {_ServiceExPerDx = value;}
    inline void setIncommingExPerDx(double value) {_IncommingExPerDx = value;}

    inline double getServiceExPerDx()   const { return _ServiceExPerDx; }
    inline double getIncommingExPerDx() const { return _IncommingExPerDx;}

    void setT(int t);
    void setMu(double getMu);

    StreamType newCallStr(void)  const { return _newCallStr; }
    StreamType callServStr(void) const { return _callServStr; }
    SourceType srcType(void)     const { return _srcType; }

    int   t(void)        const {return _t;}
    int   propAt(void)   const {return _propAt;}
    double getMu(void)    const {return _mu;}
    int    s (void)       const;


    double intensityNewCallTotal(double a, size_t V, int sumPropAT) const;
    double intensityNewCallForState(double intensityNewCallTotal, int stateN) const;
    double intensityNewCallForY(double lambdaZero, double y) const;

    TrClVector trDistribution(int classIdx, double A, int Vs, int Vb) const;

    bool operator ==(const ModelTrClass& rho) const;
    bool operator !=(const ModelTrClass& rho) const;
    bool operator <(const ModelTrClass& rho) const;
    bool operator >(const ModelTrClass& rho) const;
    bool operator <=(const ModelTrClass& rho) const;
    bool operator >=(const ModelTrClass& rho) const;


    void doSimExpUnlimitedSoNo(
              TrClVector &states
            , int Vs
            , int Vb
            , double Aoffered
            , double IncommingEx2perDxDNewCall
            , double EsingleCallServ
            , double DsingleCallServ
            ) const;

    void doSimExpLimitedSoNo(
              TrClVector &states
            , int Vs
            , int Vb
            , double Aoffered
            , double IncommingEx2perDxDNewCall
            , double EsingleCallServ
            , double DsingleCallServ
            ) const;

    class SimulatorProcess;
    class SimulatorSingleServiceSystem
    {
    private:
        //classType trClassType;

        std::random_device rd;
        std::mt19937 gen;

        std::normal_distribution<> distribNormalNewCall;
        std::normal_distribution<> distribNormalService;

        std::gamma_distribution<double> distribGammaNewCall;
        std::gamma_distribution<double> distribGammaService;

        paretoDistrib distribParetoNewCall;
        paretoDistrib distribParetoService;


        double time;
        int n_inServer;
        int n_total;
        int t;

        double E_newCall;
        double D_newCall;
        double t_newCallMin;
        double t_newCallMax;

        double E_service;
        double D_service;
        double t_serviceMin;
        double t_serviceMax;
        QList<SimulatorProcess *> agenda;
        QList<SimulatorProcess *> qeue;


        SimulatorProcess* callPartialyServiced;
        int resPartOccupied;

        double distrLambda(double Ex);
        double distrUniform(double tMin, double tMax);

    public:
        TrClVector *states;
        int Vs, Vb;

        SimulatorSingleServiceSystem(int noOfSourcess, SourceType srcType, int Vs, int Vb, int t, TrClVector &states, double E_newCall, double D_newCall, double E_service, double D_service);
        ~SimulatorSingleServiceSystem();

        inline double timeNewCallExp() { return distrLambda(E_newCall); }
        inline double timeNewCallUni() { return distrUniform(t_newCallMin, t_newCallMax); }
        double timeNewCallNormal();
        double timeNewCallGamma();
        double timeNewCallPareto();

        inline double timeServEndExp() {return distrLambda(E_service); }
        inline double timeServEndUni() {return distrUniform(t_serviceMin, t_serviceMax); }
        double timeServEndNormal();
        double timeServEndGamma();
        double timeServEndPareto();


        void addProcess(SimulatorProcess *newProc, double relativeTime);
        void removeProcess(SimulatorProcess *proc);

        void stabilize(int noOfEvents);
        void doSimExperiment(int noOfEvents, TrClVector &states);

        bool addCall(SimulatorProcess *call, double timeOfService);
        void endCallService(SimulatorProcess *call);

        static bool simProcComparer(const ModelTrClass::SimulatorProcess *a, const ModelTrClass::SimulatorProcess *b);
    };

    class SimulatorProcess
    {
    public:
        SimulatorSingleServiceSystem *system;
        double time;

        SimulatorProcess(SimulatorSingleServiceSystem *system);
        virtual ~SimulatorProcess();

        inline bool execute(SimulatorSingleServiceSystem *system);
        virtual void initialize() = 0;
        bool operator<(const SimulatorProcess &rho) const;
        bool (*procFun)(SimulatorSingleServiceSystem *system, SimulatorProcess *proc);
    };

    // ++++++++++++++++++++
    class SimulatorProcess_Indep: public SimulatorProcess
    {
    protected:
        SimulatorProcess_Indep(SimulatorSingleServiceSystem *system);

        template <class P> void initializeT(double timeOfNewCall);
        template <class P> static bool newCall(SimulatorSingleServiceSystem *system, SimulatorProcess *proc, double EOS_TIME);
        static bool endOfCallService(ModelTrClass::SimulatorSingleServiceSystem *system, SimulatorProcess *proc);

    public:
        virtual void initialize() = 0;
    };

    class SimulatorProcess_DepMinus: public SimulatorProcess
    {
    protected:
        SimulatorProcess_DepMinus(SimulatorSingleServiceSystem *system);

        template <class P> void initializeT(double timeOfNewCall);
        template <class P> static bool newCall(SimulatorSingleServiceSystem *system, SimulatorProcess *proc, double EOS_TIME);
        template <class P> static bool endOfCallService(ModelTrClass::SimulatorSingleServiceSystem *system, SimulatorProcess *proc);

    public:
        virtual void initialize() = 0;
    };

    class SimulatorProcess_DepPlus: public SimulatorProcess
    {
    protected:
        template <class P> static bool newCall(SimulatorSingleServiceSystem *system, SimulatorProcess *proc, double endOfServiceTime);
        template <class P> static bool endOfCallService(SimulatorSingleServiceSystem *system, SimulatorProcess *proc);

    public:
        SimulatorProcess_DepPlus(SimulatorSingleServiceSystem *system);

        SimulatorProcess_DepPlus *child;
        SimulatorProcess_DepPlus *parent;

        virtual void initialize() = 0;
    };
    // #######################

#define CLASS_SIMULATOR_INDEP(X,Y,FUN_TIME_NEW,FUN_TIME_END) \
    class SimulatorProcess_Indep##X##Y: public SimulatorProcess_Indep \
    {\
    public:\
        SimulatorProcess_Indep##X##Y(SimulatorSingleServiceSystem *system): SimulatorProcess_Indep(system) {}\
        ~SimulatorProcess_Indep##X##Y() {}\
\
        void initialize();\
        static bool newCall(SimulatorSingleServiceSystem *system, SimulatorProcess *proc);\
    };

#define CLASS_SIMULATOR_DEP_MINUS(X,Y,FUN_TIME_NEW,FUN_TIME_END) \
    class SimulatorProcess_DepMinus##X##Y: public SimulatorProcess_DepMinus \
    {\
    public:\
        SimulatorProcess_DepMinus##X##Y(SimulatorSingleServiceSystem *system) : SimulatorProcess_DepMinus(system) {}\
        ~SimulatorProcess_DepMinus##X##Y() {}\
    \
        void initialize();\
    \
        static bool newCall(SimulatorSingleServiceSystem *system, SimulatorProcess *proc);\
        static bool endOfCallService(SimulatorSingleServiceSystem *system, SimulatorProcess *proc);\
    };


#define CLASS_SIMULATOR_DEP_PLUS(X,Y,FUN_TIME_NEW,FUN_TIME_END) \
    class SimulatorProcess_DepPlus##X##Y: public SimulatorProcess_DepPlus\
    {\
    public:\
        SimulatorProcess_DepPlus##X##Y(SimulatorSingleServiceSystem *system): SimulatorProcess_DepPlus(system) {}\
        ~SimulatorProcess_DepPlus##X##Y() {}\
    \
        void initialize();\
    \
        static bool newCall(SimulatorSingleServiceSystem *system, SimulatorProcess *proc);\
        static bool endOfCallService(SimulatorSingleServiceSystem *system, SimulatorProcess *proc);\
    };

    CLASS_SIMULATOR_INDEP(M, M, NewCallExp, ServEndExp)
    CLASS_SIMULATOR_INDEP(M, U, NewCallExp, ServEndUni)
    CLASS_SIMULATOR_INDEP(M, N, NewCallExp, ServEndNormal)
    CLASS_SIMULATOR_INDEP(M, G, NewCallExp, ServEndGamma)
    CLASS_SIMULATOR_INDEP(M, P, NewCallExp, ServEndPareto)

    CLASS_SIMULATOR_INDEP(U, M, NewCallUni, ServEndExp)
    CLASS_SIMULATOR_INDEP(U, U, NewCallUni, ServEndUni)
    CLASS_SIMULATOR_INDEP(U, N, NewCallUni, ServEndNormal)
    CLASS_SIMULATOR_INDEP(U, G, NewCallUni, ServEndGamma)
    CLASS_SIMULATOR_INDEP(U, P, NewCallUni, ServEndPareto)

    CLASS_SIMULATOR_INDEP(N, M, NewCallNormal, ServEndExp)
    CLASS_SIMULATOR_INDEP(N, U, NewCallNormal, ServEndUni)
    CLASS_SIMULATOR_INDEP(N, N, NewCallNormal, ServEndNormal)
    CLASS_SIMULATOR_INDEP(N, G, NewCallNormal, ServEndGamma)
    CLASS_SIMULATOR_INDEP(N, P, NewCallNormal, ServEndPareto)

    CLASS_SIMULATOR_INDEP(G, M, NewCallGamma, ServEndExp)
    CLASS_SIMULATOR_INDEP(G, U, NewCallGamma, ServEndUni)
    CLASS_SIMULATOR_INDEP(G, N, NewCallGamma, ServEndNormal)
    CLASS_SIMULATOR_INDEP(G, G, NewCallGamma, ServEndGamma)
    CLASS_SIMULATOR_INDEP(G, P, NewCallGamma, ServEndPareto)

    CLASS_SIMULATOR_INDEP(P, M, NewCallPareto, ServEndExp)
    CLASS_SIMULATOR_INDEP(P, U, NewCallPareto, ServEndUni)
    CLASS_SIMULATOR_INDEP(P, N, NewCallPareto, ServEndNormal)
    CLASS_SIMULATOR_INDEP(P, G, NewCallPareto, ServEndGamma)
    CLASS_SIMULATOR_INDEP(P, P, NewCallPareto, ServEndPareto)

    CLASS_SIMULATOR_DEP_MINUS(M, M, NewCallExp, ServEndExp)
    CLASS_SIMULATOR_DEP_MINUS(M, U, NewCallExp, ServEndUni)
    CLASS_SIMULATOR_DEP_MINUS(M, N, NewCallExp, ServEndNormal)
    CLASS_SIMULATOR_DEP_MINUS(M, G, NewCallExp, ServEndGamma)
    CLASS_SIMULATOR_DEP_MINUS(M, P, NewCallExp, ServEndPareto)

    CLASS_SIMULATOR_DEP_MINUS(U, M, NewCallUni, ServEndExp)
    CLASS_SIMULATOR_DEP_MINUS(U, U, NewCallUni, ServEndUni)
    CLASS_SIMULATOR_DEP_MINUS(U, N, NewCallUni, ServEndNormal)
    CLASS_SIMULATOR_DEP_MINUS(U, G, NewCallUni, ServEndGamma)
    CLASS_SIMULATOR_DEP_MINUS(U, P, NewCallUni, ServEndPareto)

    CLASS_SIMULATOR_DEP_MINUS(N, M, NewCallNormal, ServEndExp)
    CLASS_SIMULATOR_DEP_MINUS(N, U, NewCallNormal, ServEndUni)
    CLASS_SIMULATOR_DEP_MINUS(N, N, NewCallNormal, ServEndNormal)
    CLASS_SIMULATOR_DEP_MINUS(N, G, NewCallNormal, ServEndGamma)
    CLASS_SIMULATOR_DEP_MINUS(N, P, NewCallNormal, ServEndPareto)

    CLASS_SIMULATOR_DEP_MINUS(G, M, NewCallGamma, ServEndExp)
    CLASS_SIMULATOR_DEP_MINUS(G, U, NewCallGamma, ServEndUni)
    CLASS_SIMULATOR_DEP_MINUS(G, N, NewCallGamma, ServEndNormal)
    CLASS_SIMULATOR_DEP_MINUS(G, G, NewCallGamma, ServEndGamma)
    CLASS_SIMULATOR_DEP_MINUS(G, P, NewCallGamma, ServEndPareto)

    CLASS_SIMULATOR_DEP_MINUS(P, M, NewCallPareto, ServEndExp)
    CLASS_SIMULATOR_DEP_MINUS(P, U, NewCallPareto, ServEndUni)
    CLASS_SIMULATOR_DEP_MINUS(P, N, NewCallPareto, ServEndNormal)
    CLASS_SIMULATOR_DEP_MINUS(P, G, NewCallPareto, ServEndGamma)
    CLASS_SIMULATOR_DEP_MINUS(P, P, NewCallPareto, ServEndPareto)

    CLASS_SIMULATOR_DEP_PLUS(M, M, NewCallExp, ServEndExp)
    CLASS_SIMULATOR_DEP_PLUS(M, U, NewCallExp, ServEndUni)
    CLASS_SIMULATOR_DEP_PLUS(M, N, NewCallExp, ServEndNormal)
    CLASS_SIMULATOR_DEP_PLUS(M, G, NewCallExp, ServEndGamma)
    CLASS_SIMULATOR_DEP_PLUS(M, P, NewCallExp, ServEndPareto)

    CLASS_SIMULATOR_DEP_PLUS(U, M, NewCallUni, ServEndExp)
    CLASS_SIMULATOR_DEP_PLUS(U, U, NewCallUni, ServEndUni)
    CLASS_SIMULATOR_DEP_PLUS(U, N, NewCallUni, ServEndNormal)
    CLASS_SIMULATOR_DEP_PLUS(U, G, NewCallUni, ServEndGamma)
    CLASS_SIMULATOR_DEP_PLUS(U, P, NewCallUni, ServEndPareto)

    CLASS_SIMULATOR_DEP_PLUS(N, M, NewCallNormal, ServEndExp)
    CLASS_SIMULATOR_DEP_PLUS(N, U, NewCallNormal, ServEndUni)
    CLASS_SIMULATOR_DEP_PLUS(N, N, NewCallNormal, ServEndNormal)
    CLASS_SIMULATOR_DEP_PLUS(N, G, NewCallNormal, ServEndGamma)
    CLASS_SIMULATOR_DEP_PLUS(N, P, NewCallNormal, ServEndPareto)

    CLASS_SIMULATOR_DEP_PLUS(G, M, NewCallGamma, ServEndExp)
    CLASS_SIMULATOR_DEP_PLUS(G, U, NewCallGamma, ServEndUni)
    CLASS_SIMULATOR_DEP_PLUS(G, N, NewCallGamma, ServEndNormal)
    CLASS_SIMULATOR_DEP_PLUS(G, G, NewCallGamma, ServEndGamma)
    CLASS_SIMULATOR_DEP_PLUS(G, P, NewCallGamma, ServEndPareto)

    CLASS_SIMULATOR_DEP_PLUS(P, M, NewCallPareto, ServEndExp)
    CLASS_SIMULATOR_DEP_PLUS(P, U, NewCallPareto, ServEndUni)
    CLASS_SIMULATOR_DEP_PLUS(P, N, NewCallPareto, ServEndNormal)
    CLASS_SIMULATOR_DEP_PLUS(P, G, NewCallPareto, ServEndGamma)
    CLASS_SIMULATOR_DEP_PLUS(P, P, NewCallPareto, ServEndPareto)
};

class ModelSyst
{
    friend QTextStream& operator<<(QTextStream &stream, const ModelSyst &model);
    friend QDebug&      operator<<(QDebug &stream, const ModelSyst &model);

private:
    int _noOfTrClasses;
    ModelTrClass **_trClasses;
    int _capacityTrClasses;

    ServerResourcessScheduler _serverSchedulerAlgorithm;
    ModelResourcess *_servers;
    int _noOfTypesOfGroups;    // 1 for LAG of FAG, >1 for GLAG
    int _totalGroupsCapacity;  // Pojemność wszystkich wiązek
    int _totalNumberOfGroups;  // Liczba wszystkich wiązek
    int _capacityTypeOfGroups; // Array length TODO use QVector<>

    BufferResourcessScheduler _bufferSchedulerAlgorithm;
    ModelResourcess *_bufers;
    int _noOfTypesOfBuffers;
    int _totalBufferCapacity;  // Pojemność wszystkich kolejek
    int _totalNumberOfBuffers; // Liczba wszystkich kolejek
    int _capacityTypeOfQeues;  // Array length TODO use QVector<>

    int _totalAt;              // suma wszystkich proporcji


public:
    ModelSyst();
    ~ModelSyst();

    void getLinkParameters(int32_t **k
            , int32_t **v
            , int32_t *numberOfTypes
            ) const;
    void getBufferParameters(int32_t **k
            , int32_t **v
            , int32_t *numberOfTypes
            ) const;
    const ModelTrClass *getClass(int idx) const;

    ServerResourcessScheduler getGroupsSchedulerAlgorithm() const;

    bool operator ==(const ModelSyst& rho) const;
    bool operator !=(const ModelSyst& rho) const;
    bool operator >(const ModelSyst& rho) const;
    bool operator <(const ModelSyst& rho) const;


    int totalAt(void)  const {return _totalAt;}
    int m(void)        const {return _noOfTrClasses;}
    int V(void)              const {return _totalGroupsCapacity + _totalBufferCapacity;}

    int v_s(int groupClNo)   const;                                        /// Single server group capacity
    int vk_s(void)           const {return _totalGroupsCapacity; }         /// Server capacity
    int vk_s(int groupClNo)  const;                                        /// Capacity of givens class of server's groups
    int v_sMax(void)         const;                                        /// Capacity of the biggest group in the server
    int k_s(void)            const {return _totalNumberOfGroups; }         /// Total number of server groups
    int k_s(int groupClNo)   const;                                        /// Number of servers group of given class (GLAG model)
    int k_sType(void)        const {return _noOfTypesOfGroups; }           /// Total number of servers group classes (1 for FAG and LAG, >1 for GLAG)

    int v_b(int bufferClNo)  const;                                        /// Single buffer group capacity
    int vk_b(void)           const {return _totalBufferCapacity; }         /// Buffer capacity
    int vk_b(int bufferClNo) const;                                        /// Capacity of givens class of buffers's groups
    int v_bMax(void)         const;                                        /// Capacity of the biggest group in the buffer
    int k_b(void)            const {return _totalNumberOfBuffers; }        /// Total number of buffer groups
    int k_b(int bufferClNo)  const;                                        /// Number of buffers group of given class
    int k_bType(void)        const {return _noOfTypesOfBuffers; }          /// Total number of buffers group classes

    int id;

    void addClass(ModelTrClass *newClass);                            //Creates a copy. The copy is added to the system
    void addGroups(ModelResourcess newGroup, bool optimize = true);   //Add groups to the system. Previous types of group are checked before
    void addQeues(ModelResourcess qeue, bool optimize = true);        //Add qeues to the system

    void setServerSchedulerAlgorithm(ServerResourcessScheduler algorithm);
    void setBufferSchedulerAlgorithm(BufferResourcessScheduler algorithm);

    inline BufferResourcessScheduler getBufferScheduler() const {return _bufferSchedulerAlgorithm; }
    inline ServerResourcessScheduler getServerScheduler() const {return _serverSchedulerAlgorithm; }

    void clearAll();
};

QTextStream &operator<<(QTextStream &stream, const ModelSyst &model);
QTextStream &operator<<(QTextStream &stream, const ModelTrClass &trClass);

QDebug &operator<<(QDebug &stream, const ModelSyst &model);
QDebug &operator<<(QDebug &stream, const ModelTrClass &trClass);



Q_DECLARE_METATYPE(ModelResourcess*)
Q_DECLARE_METATYPE(ModelTrClass*)
Q_DECLARE_METATYPE(const ModelTrClass*)
Q_DECLARE_METATYPE(ModelSyst*)
Q_DECLARE_METATYPE(ModelTrClass::SourceType)
Q_DECLARE_METATYPE(ModelTrClass::StreamType)
Q_DECLARE_METATYPE(ServerResourcessScheduler)
Q_DECLARE_METATYPE(BufferResourcessScheduler)


#endif // MODEL_H

