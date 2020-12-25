#ifndef MODEL_H
#define MODEL_H

#include <QMetaType>
#include <QTextStream>
#include <QDebug>
#include <QVector>
#include <QList>
#include <QLinkedList>
#include <QThread>
#include <QRunnable>
#include <qglobal.h>
#include <math.h>

#include <iostream>

#include "utils/probDistributions.h"
#include "algorithms/trclvector2.h"


/*
 * Server
 */
enum class ResourcessScheduler
{
    Random,
    Sequencial
};

QString serverResourcessSchedulerToString(ResourcessScheduler value);

enum class SystemPolicy
{
    NoBuffer,      ///< Buffer is disabled
    cFifo,         ///< Call can be splited between server and buffers resourcess. Calls goes to the buffer first
    dFIFO,         ///< Calls are discrette ang goes to the buffer first. If the buffer is empty, the calls are accepted by the server
    qFIFO,         ///< Calls are discrette and goes to server and next to the buffer
    SD_FIFO        ///< All calls are being served. If there is no room in server, the service time is increased and calls occypy server and budder
};

QString bufferResourcessSchedulerToString(SystemPolicy value);


class ModelSubResourcess
{
private:
    int _k;  ///Number of the grous
    int _v;  ///single group capacity

public:
    ModelSubResourcess(int k, int v);
    ModelSubResourcess(): _k(0), _v(0) {}

    void set_v(int v);
    void set_k(int k);
    int v() const;
    int k() const;
    int V() const;


    bool operator== (const ModelSubResourcess &rho) const;
    bool operator!= (const ModelSubResourcess &rho) const;
    bool operator> (const ModelSubResourcess &rho) const;
    bool operator< (const ModelSubResourcess &rho) const;
};


class ModelResourcess
{
public:
    const ResourcessScheduler schedulerAlg;

private:
    const QList<ModelSubResourcess> _listSubRes;
    const int _k;  ///< Number of the grous
    int _V;        ///< Total group capacity
    int _vMax;     ///< Maximum subgroup capacity

    QVector<int> _subgrpCapacity;
public:
    ModelResourcess(QList<ModelSubResourcess> listSubRes, ResourcessScheduler schedulerAlg);

    bool operator==(const ModelResourcess &rho) const;
    bool operator!=(const ModelResourcess &rho) const;

    inline int V() const                    { return _V; }
    inline int V(int groupNo) const         { return ((groupNo < _subgrpCapacity.length()) && groupNo >=0) ? _subgrpCapacity[groupNo] : 0;}
    int V(int groupClassNo, int groupNo) const;

    inline int k() const                    { return _k; }
    inline int k(int groupClassNo) const    { return ((groupClassNo>=0) && (groupClassNo < _listSubRes.length())) ? _listSubRes[groupClassNo].k() : 0; }

    inline int kTypes() const               { return _listSubRes.length();}
    inline int vMax() const                 { return _vMax; }
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

    struct
    {
        QString name;
        StreamType value;
    } PairStrAndStrType;


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
        static std::random_device rd;
        std::mt19937_64 gen;

        std::normal_distribution<> distribNormalNewCall;
        std::normal_distribution<> distribNormalService;

        std::gamma_distribution<double> distribGammaNewCall;
        std::gamma_distribution<double> distribGammaService;

        paretoDistrib distribParetoNewCall;
        paretoDistrib distribParetoService;

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

        int getT() const { return t; }

        void stabilize(int noOfEvents);
        void doSimExperiment(long noOfEvents, TrClVector &states);

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

class ModelTrClassSimulationWork : public QRunnable
{
private:
    ModelTrClass::SimulatorSingleServiceSystem *system;
    ModelTrClass::SimulatorProcess *proc;

    TrClVector *states;
public:
    ModelTrClassSimulationWork(TrClVector *states, int Vs, int Vb, double Aoffered, int t
      , ModelTrClass::SourceType srcNewCallSrcType, ModelTrClass::StreamType newCallStreamType, double IncommingEx2perDxDNewCall
      , ModelTrClass::StreamType endCallStreamType, double EsingleCallServ, double DsingleCallServ);
    ~ModelTrClassSimulationWork();
    void run();
};


QDebug& operator<<(QDebug &stream, const ModelTrClass &trClass);
QDebug& operator<<(QDebug &stream, ModelTrClass &trClass);

class ModelSystem
{
private:
    const QVector<ModelTrClass> _trClasses;
    const ModelResourcess       _server;
    const ModelResourcess       _buffer;
    const SystemPolicy          _bufferPolicy;

    int _totalAt;
    int _V;

public:
    ModelSystem(const ModelSystem &system);
    ModelSystem(const QVector<ModelTrClass> &trClasses, const ModelResourcess &server, const ModelResourcess &buffer, SystemPolicy bufferPolicy);

    bool operator==(const ModelSystem &rho) const;

    inline const ModelTrClass& getTrClass(int index) const {return _trClasses[index];}
    inline const QVector<ModelTrClass>& getTrClasses() const {return _trClasses;}
    inline const ModelResourcess& getServer() const {return _server;}
    inline const ModelResourcess& getBuffer() const {return _buffer;}
    inline SystemPolicy getBufferPolicy() const {return _bufferPolicy;}

    bool isInBlockingState(int classNo, const QVector<int> &serverGroupsState, const QVector<int> bufferGroupsState) const;
    bool isServerAvailable(int classNo, const QVector<int> &serverGroupsState) const;
    bool isBufferAvailable(int classNo, const QVector<int> &bufferGroupsState) const;

    int getTotalAt() const {return _totalAt;}
    int m() const {return _trClasses.length();}
    int V() const {return _V;}
    int t(int classIdx) const {return _trClasses[classIdx].t();}
};

class MCRsc
{
public:
    QList<ModelSubResourcess> resourcess;
    ResourcessScheduler       scheduler;

    bool operator>(const MCRsc &rho) const;
    bool operator<(const MCRsc &rho) const;
};

class MCTrCl
{
public:
    QVector<ModelTrClass> trClasses;
    int totalAt() const
    {
        int result = 0;
        foreach(ModelTrClass tmp, trClasses)
            result+= tmp.propAt();
        return result;
    }                                             ///< Sum of all traffic proportions

    bool operator>(const MCTrCl &rho) const;
    bool operator<(const MCTrCl &rho) const;
    bool operator==(const MCTrCl &rho) const;
    bool operator!=(const MCTrCl &rho) const;
};

class ModelCreator
{


    friend QTextStream& operator<<(QTextStream &stream, const ModelCreator &model);
    friend QDebug&      operator<<(QDebug &stream, const ModelCreator &model);

private:
    MCTrCl _traffic;
    MCRsc _server;
    MCRsc _buffer;

    SystemPolicy _systemPolicy;

public:
    ModelCreator();
    ~ModelCreator();

    const ModelTrClass &getClass(int idx) const;
    ModelTrClass *getClassClone(int idx) const;
    const ModelSystem getConstSyst() const;

    ResourcessScheduler getGroupsSchedulerAlgorithm() const;

    bool operator ==(const ModelCreator& rho) const;
    bool operator !=(const ModelCreator& rho) const;
    bool operator >(const ModelCreator& rho) const;
    bool operator <(const ModelCreator& rho) const;


    int totalAt(void)  const {return _traffic.totalAt();}
    int m(void)        const {return _traffic.trClasses.length();}
    int V(void)              const;

    int v_s(int groupClNo)   const;                                        ///< Single server group capacity
    int vk_s(void)           const;                                        ///< Server capacity
    int vk_s(int groupClNo)  const;                                        ///< Capacity of givens class of server's groups
    int v_sMax(void)         const;                                        ///< Capacity of the biggest group in the server
    int k_s(void)            const;                                        ///< Total number of server groups
    int k_s(int groupClNo)   const;                                        ///< Number of servers group of given class (GLAG model)
    int k_sType(void)        const;                                        ///< Total number of servers group classes (1 for FAG and LAG, >1 for GLAG)

    int v_b(int bufferClNo)  const;                                        ///< Single buffer group capacity
    int vk_b(void)           const;                                        ///< Buffer capacity
    int vk_b(int bufferClNo) const;                                        ///< Capacity of givens class of buffers's groups
    int v_bMax(void)         const;                                        ///< Capacity of the biggest group in the buffer
    int k_b(void)            const;                                        ///< Total number of buffer groups
    int k_b(int bufferClNo)  const;                                        ///< Number of buffers group of given class
    int k_bType(void)        const;                                        ///< Total number of buffers group classes

    int id;

    void addClass(ModelTrClass *newClass);                                 ///< Creates a copy. The copy is added to the system
    void addServerGroups(ModelSubResourcess newGroup, bool optimize = false);     ///< Add groups to the system. Previous types of group are checked before
    void addBufferGroups(ModelSubResourcess qeue, bool optimize = false);          ///< Add qeues to the system

    void setServerSchedulerAlgorithm(ResourcessScheduler algorithm);
    void setBufferSchedulerAlgorithm(ResourcessScheduler algorithm);

    void setSystemSchedulerAlgorithm(SystemPolicy algorithm);

    inline ResourcessScheduler getBufferScheduler() const {return _buffer.scheduler; }
    inline ResourcessScheduler getServerScheduler() const {return _server.scheduler; }
    inline SystemPolicy getSystemScheduler() const {return _systemPolicy; }

    void clearAll();

    QString getGnuplotDescription() const;
};

QTextStream &operator<<(QTextStream &stream, enum ModelTrClass::StreamType strType);
QDebug &operator<<(QDebug &stream, enum ModelTrClass::StreamType strType);


QTextStream &operator<<(QTextStream &stream, const ModelCreator &model);
QTextStream &operator<<(QTextStream &stream, const ModelSystem &model);
QTextStream &operator<<(QTextStream &stream, const ModelTrClass &trClass);

QDebug &operator<<(QDebug &stream, const ModelCreator &model);
QDebug &operator<<(QDebug &stream, const ModelSystem &model);
QDebug &operator<<(QDebug &stream, const ModelTrClass &trClass);



Q_DECLARE_METATYPE(ModelSubResourcess*)
Q_DECLARE_METATYPE(ModelResourcess*)
Q_DECLARE_METATYPE(ModelTrClass*)
Q_DECLARE_METATYPE(const ModelTrClass*)
Q_DECLARE_METATYPE(ModelCreator*)
Q_DECLARE_METATYPE(ModelTrClass::SourceType)
Q_DECLARE_METATYPE(ModelTrClass::StreamType)
Q_DECLARE_METATYPE(ResourcessScheduler)
Q_DECLARE_METATYPE(SystemPolicy)


#endif // MODEL_H

