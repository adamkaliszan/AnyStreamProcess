#if 0
#ifndef RESULTSOLD_H
#define RESULTSOLD_H

#include <QList>
#include <QMap>
#include <QVector>
#include <QDebug>
#include <QFile>
#include <QMetaType>

#include <iostream>
#include <iomanip>
#include <boost/multiprecision/cpp_dec_float.hpp>

#include <math.h>
#include <iostream>

#include "algorithms/simulationParameters.h"
//#include "algorithms/investigator.h"
#include "model.h"
//#include "results/resultsGnuplot.h"
//#include "results/resultsSystem.h"

const double WspStFish95[] = {0,
    6.314, 2.920, 2.353, 2.132, 2.015, 1.943, 1.895, 1.860, 1.833, 1.812,
    1.796, 1.782, 1.771, 1.761, 1.753, 1.746, 1.740, 1.734, 1.729, 1.725,
    1.721, 1.717, 1.714, 1.711, 1.708, 1.706, 1.703, 1.701, 1.699, 1.697};

const double WspStFish99[] = {0,
    63.656, 9.925, 5.841, 4.4604, 4.032, 3.707, 3.499, 3.355, 3.250, 3.169,
    3.108, 3.055, 3.012, 2.977, 2.947, 2.921, 2.898, 2.878, 2.861, 2.845,
    2.831, 2.819, 2.807, 2.797, 2.787, 2.779, 2.771, 2.763, 2.756, 2.750};

enum class CiTrust
{
    trust95,
    trust99
};

enum class resultsType
{
    yQ,                // Avarage number of class' i calls in qeue
    y,                 // Avarage number of serviced calls in whole system
    ytQ,               // Avarage number of queues resourcess obbupied by class class i
    tWait,             // Effective waiting time
    tService,          // Avarage time on system
    tServer,           // Avarage time on server
    tQeue,             // Avarage absolute time on qeue
    tPlandedServer,    // Planed service time
    tServerMult,       // Time on server/planed time on server
    B,                 // Lost probability
    E,                 // Blocking probability
    Y,                 // Serviced traffic
    Qlen,              // Avarage qeue length
    qeueYt_vs_q_n,     // Avg AS occupied in qeue by class when n res of qeue is occupied
    serverYt_vs_s_n,   // Avg AS occupied in server by class when n res of server is occupied
    qeueYt_vs_sys_n,   // AVG AS occupied in qeue by class when n res of the system (qeue and server) is occupied
    serverYt_vs_sys_n, // AVG AS occupied in server by class when n res of the system (qeue and server) is occupied
    systemYt_vs_sys_n, // AVG AS occupied in system by class when n res of the system (qeue and server) is occupied
    serverQeueAS,      // AVG AS occupied in the state (n_server, n_buffer)
    trDistrib,         // State probabilities in [server][qeue]
    trDistribSystem,   // State probabilities in whole system
    trDistribServer,   // State probabilities in server
    trDistribQeue,     // State probabilities in qeue

    intInNew,          // intensity (served) of reaching new state because of new call of any class
    intInEnd,          // intensity (served) of reaching new state because of call service ending of any class
    intOutNew,         // intensity (offered) of leaving the state because of new call of any class
    intOutEnd,         // intensity (offered) of leaving the state because of call service ending of any class
    noInNew,           // number of passages from this state because of new call of any class
    noInEnd,           // number of passages from this state because of call service ending of any class
    noOutNew,          // number of passages to this state because of new call of any class
    noOutEnd,          // number of passages to this state because of call service ending of any class
//#warining "Not implemented"
//    intOutNewServ,   // Real intensity of leaving the state because of new call of any class
//    intOutEndServ,   // Real intensity of leaving the state because of call service ending of any class
//    intInNewServ,    // Real intensity of reaching new state because of new call of any class
//    intInEndServ,    // Real intensity of reaching new state because of call service ending of any class
    intInNewSC,        // intensity (served) of reaching state because of new call of specified class
    intInEndSC,        // intensity (served) of reaching state because of call service ending of specified class
    intOutNewSC,       // intensity (offered) of leaving the state because of new call of specified class
    intOutEndSC,       // intensity (offered) of leaving the state because of call service ending of specified class
    noInNewSC,        // number of passages from this state because of new call of specified class
    noInEndSC,        // number of passages from this state because of call service ending of specified class
    noOutNewSC,       // number of passages to this state because of new call of specified class
    noOutEndSC,       // number of passages to this state because of call service ending of specified class

//#warining "Not implemented"
//    intOutNewSCserv,   // Real intensity of leaving the state because of new call of specified class
//    intOutEndSCserv    // Real intensity of leaving the state because of call service ending of specified class
};

class Investigator;
class AlgorithmResults;
class GnuplotScript;

enum class resultFormat
{
    typeDouble,
    typeLongInt
};


class ResultKey
{

};

class ResultsTmp
{
    friend QTextStream &operator<<(QTextStream &stream, ResultsTmp &results);
    friend QDebug      &operator<<(QDebug &stream,      ResultsTmp &results);

private:

    QVector<double> a;
    QMap<const Investigator*, AlgorithmResults* > _algResultsDepracated;

public:
    ResultsTmp();

    const QVector<double>& get_a() const { return a;}

    static class _init
    {
      public:
        _init();
    } _initializer;

private:
    const ModelSyst *lastSystem;

public:
    class QoSdetails
    {
    public:
        QString description;
        QString shortDescr;
        bool requiresQeue;
        bool dependsOnClass;
        bool dependsOnState;
        bool dependsOnServerState;
        bool dependsOnQeueState;
        bool logScaleY;
        resultFormat result;


        QoSdetails(const char *description, const char *shortDescr, bool requiresQeue, bool dependsOnClass,  bool dependsOnState, bool dependsOnServerState, bool dependsOnQeueState, bool logScaleY, resultFormat result):
          requiresQeue(requiresQeue), dependsOnClass(dependsOnClass), dependsOnState(dependsOnState), dependsOnServerState(dependsOnServerState), dependsOnQeueState(dependsOnQeueState), logScaleY(logScaleY), result(result)
        {
            this->description = QString(description);
            this->shortDescr = QString(shortDescr);
        }
        QoSdetails(): description(QString("Error")) {}
    };

private:
    static QString _NA;
    static QString _logScale;
    static QString _linearScale;

    static QMap<resultsType, ResultsTmp::QoSdetails> _resType2details;

public:
    const ModelTrClass *getClass(int idx) const;
    int get_m() const;
    int get_V() const;
    int get_C() const;

    double get_sumPropAt() const;


    int numberOfXsamples() {return a.length(); }
    double getXsample(int idx)
    {
        return a[idx];
    }
    //RGnuplot gnuplot();

    QVector<resultsType> QoSPar;





    void clearAll()
    {
        a.clear();
        _algResultsDepracated.clear();
    }
    simulationParameters *simParameters;

    QList<const Investigator *> algorithms() const { return _algResultsDepracated.keys();}

    void addAlgorithmDepracated(Investigator *alg, AlgorithmResults *algResults);
    AlgorithmResults* getAlgResult(const Investigator *alg) const;

    QVector<double>* get_a();

    static QString &resType2string(resultsType type);
    static QString &resType2shortString(resultsType type);
    static QString &resType2yScale(resultsType type);

    static resultFormat resType2resFormat(resultsType type);

    static bool resTypeHasLogScale(resultsType type);
    static bool isTrClassDependent(resultsType type);

    double min_a();
    double max_a();

};

class AlgorithmResults
{
    friend QTextStream &operator<<(QTextStream &stream, ResultsTmp &results);
    friend QDebug      &operator<<(QDebug &stream,      ResultsTmp &results);

public:

    /**
     * @brief The AU_singleA_singleClass class
     * Holds information about Allocation Units used by single class in state n
     * Offered traffic a is fixed
     */
    class AU_singleA_singleClass
    {
    private:
        int          Vs;
        int          Vb;
        bool          hasCI;
        QVector<bool> hasQvsQ;
        QVector<bool> hasQvsSYS;
        QVector<bool> hasSERVvsSERV;
        QVector<bool> hasSERVvsSYS;
        QVector<bool> hasSYSvsSYS;

        QVector<bool> hasIntOutNew;
        QVector<bool> hasIntOutEnd;
        QVector<bool> hasIntInNew;
        QVector<bool> hasIntInEnd;

        QVector<bool> hasNoOutNew;
        QVector<bool> hasNoOutEnd;
        QVector<bool> hasNoInNew;
        QVector<bool> hasNoInEnd;

        QVector<double> qeueAS_vs_qeue;
        QVector<double> qeueAS_vs_qeue_CI;
        QVector<double> serverAS_vs_server;
        QVector<double> serverAS_vs_server_CI;
        QVector<double> qeueAS_vs_system;
        QVector<double> qeueAS_vs_system_CI;
        QVector<double> serverAS_vs_system;
        QVector<double> serverAS_vs_system_CI;
        QVector<double> systemAS_vs_system;
        QVector<double> systemAS_vs_system_CI;

        QVector<double> IntOutNew;
        QVector<double> IntOutEnd_CI;
        QVector<double> IntOutEnd;
        QVector<double> IntInEnd_CI;

        QVector<double> IntInNew;
        QVector<double> IntInNew_CI;
        QVector<double> IntInEnd;
        QVector<double> IntOutNew_CI;

        QVector<long int> NoOutNew;
        QVector<double>   NoOutEnd_CI;
        QVector<long int> NoOutEnd;
        QVector<double>   NoInEnd_CI;

        QVector<long int> NoInNew;
        QVector<double>   NoInNew_CI;
        QVector<long int> NoInEnd;
        QVector<double>   NoOutNew_CI;

        QVector<QVector<double>> serverQeueAS;
        QVector<QVector<double>> serverQeueAS_CI;
        QVector<QVector<bool>> hasServerQeueAS;

    public:
        AU_singleA_singleClass() {}
        AU_singleA_singleClass(int Vs, int Vb, bool hasCI);

        bool getVal       (resultsType type, int n, double   &value, double &CI) const;
        bool getValLongInt(resultsType type, int n, long int &value, double &CI) const;
        void setVal       (resultsType type, int n, double    value, double  CI);
        void setValLongInt(resultsType type, int n, long int  value, double  CI);

        bool getVal(resultsType type, int nServer, int nQeue, double &value, double &CI);
        void setVal(resultsType type, int nServer, int nQeue, double value, double CI);

    };

    /**
     * @brief The AU_singleA class.
     * Holds information about Allocation Units used by single class
     * in state n when offered traffic is equal a
     */
    class AU_singleA
    {
    private:
        int          Vs;
        int          Vb;
        bool          hasCI;

        QMap<const ModelTrClass*, AU_singleA_singleClass*> AS_SinglClass;

        QVector<QVector<double>> states;
        QVector<double>   statesSystem;
        QVector<double>   statesServer;
        QVector<double>   statesQeue;
        QVector<double>   intOutNew;
        QVector<double>   intOutEnd;
        QVector<double>   intInNew;
        QVector<double>   intInEnd;
        QVector<long int> noOutNew;
        QVector<long int> noOutEnd;
        QVector<long int> noInNew;
        QVector<long int> noInEnd;


        QVector<QVector<double>> states_CI;
        QVector<double>   statesSystem_CI;
        QVector<double>   statesServer_CI;
        QVector<double>   statesQeue_CI;
        QVector<double>   intOutNew_CI;
        QVector<double>   intOutEnd_CI;
        QVector<double>   intInNew_CI;
        QVector<double>   intInEnd_CI;
        QVector<long int> noOutNew_CI;
        QVector<long int> noOutEnd_CI;
        QVector<long int> noInNew_CI;
        QVector<long int> noInEnd_CI;
    public:
        //AS_singleA() {}
        AU_singleA(int Vs, int Vb, bool hasCI);

        bool getVal(
                  resultsType type
                , int        n
                , double&     value
                , double&     CI
                ) const;

        void setVal(
                  resultsType type
                , int        n
                , double      value
                , double      CI
                );

        bool getValLongInt(
                  resultsType type
                , int        n
                , long int&   value
                , double&     CI
                ) const ;

        void setValLongInt(
                  resultsType type
                , int        n
                , long int    value
                ,  double CI
                );

        bool getVal(
                  resultsType type
                , int        n
                , int        q
                , double&     value
                , double&     CI
                ) const;

        void setVal(
                  resultsType type
                , int        n
                , int        q
                , double      value
                , double CI
                );

        bool getVal(
                  resultsType        type
                , const ModelTrClass *trClass
                , int               n
                , double&            value
                , double&            CI
                ) const ;

        void setVal(
                  resultsType        type
                , const ModelTrClass *trClass
                , int               n
                , double             value
                , double             CI
                );

        bool getValLongInt(
                  resultsType        type
                , const ModelTrClass *trClass
                , int               n
                , long int&          value
                , double&            CI
                ) const;

        void setValLongInt(
                  resultsType        type
                , const ModelTrClass *trClass
                , int               n
                , long int           value
                , double             CI
                );
    };

    /**
     * @brief The AU class.
     * Holds information about Allocation Units used by all offered classes
     * in state n when offered traffic is equal a
     */
    class AU
    {
    private:
        int          Vs;
        int          Vb;

        //totalna wiocha QMap na double
        QMap<double, AU_singleA*> AS_A;

    public:
        bool          hasCI;

        AU(int Vs, int Vb):
            Vs(Vs)
          , Vb(Vb)
          , hasCI(false) {}

        bool getVal(
                  resultsType        type
                , double             a
                , int               n
                , double&            value
                , double&            CI
                ) const;

        bool setVal(
                  resultsType        type
                , double             a
                , int               n
                , double             value
                , double             CI
                );

        bool getValLongInt(
                  resultsType        type
                , double             a
                , int               n
                , long int&          value
                , double&            CI
                ) const;

        bool setValLongInt(
                  resultsType        type
                , double             a
                , int               n
                , long int           value
                , double             CI
                );

        bool getVal(
                  resultsType        type
                , double             a
                , int               n
                , int               q
                , double&            value
                , double&            CI
                ) const;

        bool setVal(
                  resultsType        type
                , double             a
                , int               n
                , int               q
                , double             value
                , double             CI
                );

        bool getVal(
                  resultsType        type
                , double             a
                , const ModelTrClass *trClass
                , int               n
                , double&            value
                , double&            CI
                ) const;

        bool setVal(
                  resultsType        type
                , double             a
                , const ModelTrClass *trClass
                , int               n
                , double             value
                , double             CI
                );

        bool getValLongInt(
                  resultsType        type
                , double             a
                , const ModelTrClass *trClass
                , int               n
                , long int&          value
                , double&            CI
                ) const;

        bool setValLongInt(
                  resultsType        type
                , double             a
                , const ModelTrClass *trClass
                , int               n
                , long int           value
                , double CI
                );
    };

    int noOfSer;

private:
    void set(
              resultsType        type
            , const ModelTrClass *trClass
            , double             a
            , double             value
            , double             valueCi
            );
public:
    const ModelSyst *system;

    AU *resultsAS;

    AlgorithmResults(
              const ModelSyst *system
            , QVector<double> *a
            , int noOfSer);

    void calculateExAndWariance(double *samples, int noOfSamples, double *medValue, double *variance);
    void calculateExAndWarianceLongInt(long int *samples, int noOfSamples, long int *medValue, double *variance);


    QVector<double> *a;                           // Offered traffic. Input parameter (X axis) common for all results
    QVector<double> qLen;                         // Avarage queue length. Common for all classes
    QVector<double> conf_qLen;

    QMap<const ModelTrClass*, QVector<double>* > yQ;    // Avarage number of calls in qeue
    QMap<const ModelTrClass*, QVector<double>* > conf_yQ;

    QMap<const ModelTrClass*, QVector<double>* > y;     // Avarage number of calls in whole system
    QMap<const ModelTrClass*, QVector<double>* > conf_y;

    QMap<const ModelTrClass*, QVector<double>* > ytQ;   // Avarage number of resourcess in qeue occupied by class i
    QMap<const ModelTrClass*, QVector<double>* > conf_ytQ;

    QMap<const ModelTrClass*, QVector<double>* > t;     // Avarage time of waiting for a service
    QMap<const ModelTrClass*, QVector<double>* > conf_t;

    QMap<const ModelTrClass*, QVector<double>* > tS;    // Avarage service time
    QMap<const ModelTrClass*, QVector<double>* > conf_tS;

    QMap<const ModelTrClass*, QVector<double>* > tServer;  // Avarage time in serwer
    QMap<const ModelTrClass*, QVector<double>* > conf_tServer;

    QMap<const ModelTrClass*, QVector<double>* > tQeue;  // Avarage time in qeue
    QMap<const ModelTrClass*, QVector<double>* > conf_tQeue;

    QMap<const ModelTrClass*, QVector<double>* > tPlanedServer; // Planed avarage time in server
    QMap<const ModelTrClass*, QVector<double>* > conf_tPlanedServer;

    QMap<const ModelTrClass*, QVector<double>* > tServerMult;    // Avg time in server / planed time in serwer
    QMap<const ModelTrClass*, QVector<double>* > conf_tServerMult;

    QMap<const ModelTrClass*, QVector<double>* > B;     // Lost probability
    QMap<const ModelTrClass*, QVector<double>* > conf_B;

    QMap<const ModelTrClass*, QVector<double>* > E;     // Blocking probability
    QMap<const ModelTrClass*, QVector<double>* > conf_E;

    QMap<const ModelTrClass*, QVector<double>* > Y;     // Serviced traffic
    QMap<const ModelTrClass*, QVector<double>* > conf_Y;

    QMap<const ModelTrClass*, QVector<double>* > Q;     // Number of resources in the qeue ocupied by class (yQ*t)
    QMap<const ModelTrClass*, QVector<double>* > conf_Q;

    void set_lQ(
              const ModelTrClass *trClass
            , const double&      a
            , double             value
            , double             ConfInt=0
            );
    void set_lSys(
              const ModelTrClass *trClass
            , const double&      a
            , double             value
            , double             ConfInt=0
            );
    void set_ltQ(
              const ModelTrClass *trClass
            , const double&      a
            , double             value
            , double             ConfInt=0
            );
    void set_t(
              const ModelTrClass *trClass
            , const double&      a
            , double             value
            , double ConfInt=0
            );
    void set_tService(
              const ModelTrClass *trClass
            , const double&      a
            , double             value
            , double             ConfInt=0
            );
    void set_tServer(
              const ModelTrClass *trClass
            , const double&      a
            , double             value
            , double             ConfInt=0
            );
    void set_tQeue(
              const ModelTrClass *trClass
            , const double&      a
            , double             value
            , double             ConfInt=0
            );
    void set_tPlannedServer(
              const ModelTrClass *trClass
            , const double&      a
            , double             value
            , double             ConfInt=0
            );
    void set_tServerMult(
              const ModelTrClass *trClass
            , const double&      a
            , double             value
            , double             ConfInt=0
            );
    void set_Qlen(
              const double&      a
            , double             value
            , double             ConfInt=0
            );
    void set_Lost(
              const ModelTrClass *trClass
            , const double&      a
            , double             value
            , double             ConfInt=0
            );
    void set_Block(
              const ModelTrClass *trClass
            , const double&      a
            , double             value
            , double             ConfInt=0
            );
    void set_ServTraffic(
              const ModelTrClass *trClass
            , const double&      a
            , double             value
            , double             ConfInt=0
            );

    bool getVal(
              double &value
            , double &ci
            , resultsType type
            , const ModelTrClass *trClass
            , const double &a
            , CiTrust trust=CiTrust::trust95) const;
};

QTextStream &operator<<(QTextStream &st, const ResultsTmp &results);
QDebug      &operator<<(QDebug &stream,  const ResultsTmp &results);


#endif // RESULTSOLD_H
#endif
