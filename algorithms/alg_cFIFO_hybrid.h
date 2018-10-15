#ifndef ALGORITHMHYBRID_H
#define ALGORITHMHYBRID_H

#include <qglobal.h>
#include <QMetaType>
#include "algorithms/investigator.h"
#include "algorithms/algorithmConv.h"
#include "algorithms/trclvector2.h"
#include "results/resultsOld.h"

namespace Algorithms
{

class AlgorithmHybrid : public Investigator
{
public:
    enum class algVariant;

    AlgorithmHybrid();
    AlgorithmHybrid(algVariant variant);

    QString shortName()      const;
    virtual int complexity() const {return 1; }
    void calculateSystem(const ModelSyst *system
               , double a
               , Results::RInvestigator *results, SimulationParameters *simParameters
               );
    bool possible(const ModelSyst *system) const;

    double **ySystemFAG;


public:
    enum class algVariant
    {
        yFAG,
        yAprox,
        yProp,
        yPropPlus
    };

protected:
    TrClVector *P_without_i;
    TrClVector *p_single;
    TrClVector *p_singleQ;


    algVariant variant;

public:

    void calculateYSystem(double **ySYSTEM_V, double **yFAG, const ModelSyst *system, TrClVector *PwithoutI, TrClVector *pI);
    void calculateYQeue(double **yQeueVsVb, double **ySystemFAG, double *Q);
    void calculateYServer(double **ySeverVsVb, double **ySystemFAG, double *Q);



private:
    void calculateYSystemFAG(double **ySystem, double **yFAG);
    void calculateYSystemLambdaT(double **ySystem, double **ySystemFAG);
    void calculateYSystemApprox(double **ySystem, double **yFAG, int infinityFtr);

    void calculateYQeueFAG(double **yQeueVsVb, double **ySystemFAG);
    void calculateYQeuePropLambdaT(double **yQeueVsVb, double **yFAG, double *Q, bool lambdaIsDependent = false);

    void calculateYServerFAG(double **yServerVsVb, double **ySystemFAG);
    void calculateYServerPropLambdaT(double **yServerVsVb, double **ySystemFAG, double *Q);
};
/*
class Algorithm2Pass : public algorithm
{
protected:
    trClVectorDetail *P_without_i;
    trClVectorDetail *p_single;
    trClVectorDetail *p_singleQ;

public:
    Algorithm2Pass();

    //void deleteTemporaryData();

    QString shortName()
    {
        return "2pass alg";
    }
    virtual int complexity() {return 1; }

    void calculateSystem(modelSyst *system, double a, algorithmResults *algRes, simulationParameters *simParameters);
};
*/

} // namespace Algorithms

Q_DECLARE_METATYPE(Algorithms::AlgorithmHybrid*)
//Q_DECLARE_METATYPE(Algorithm2Pass*)

#endif // ALGORITHMHYBRID_H
