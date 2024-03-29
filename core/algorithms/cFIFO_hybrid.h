#ifndef ALGORITHMHYBRID_H
#define ALGORITHMHYBRID_H

#include <qglobal.h>
#include <QMetaType>
#include "algorithms/investigator.h"
#include "algorithms/trclvector2.h"

namespace Algorithms
{

class cFIFO_convHybrid : public Investigator
{
public:
    enum class AlgVariant;
    
    cFIFO_convHybrid(AlgVariant variant = AlgVariant::yFAG);

    QString shortName()      const;
    virtual int complexity() const {return 1; }
    void calculateSystem(const ModelSystem &system
               , double a
               , Results::RInvestigator *results, SimulationParameters *simParameters
               );
    bool possible(const ModelSystem &system) const;

    QVector<QVector<double> > ySystemFAG;


public:
    enum class AlgVariant
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


    AlgVariant variant;

public:

    void calculateYSystem(QVector<QVector<double> > &ySYSTEM_V, QVector<QVector<double> > yFAG, const ModelSystem &system, TrClVector *PwithoutI, TrClVector *pI);
    void calculateYQeue(QVector<QVector<double> >   &yQeueVsVb, QVector<QVector<double> > ySystemFAG, double *Q);
    void calculateYServer(QVector<QVector<double> > &ySeverVsVb, QVector<QVector<double> > ySystemFAG, double *Q);



private:
    void calculateYSystemFAG(QVector<QVector<double> > &ySystem, QVector<QVector<double> > yFAG);
    void calculateYSystemLambdaT(QVector<QVector<double> > &ySystem, QVector<QVector<double> > ySystemFAG);
    void calculateYSystemApprox(QVector<QVector<double> > &ySystem, QVector<QVector<double> > yFAG, int infinityFtr);

    void calculateYQeueFAG(QVector<QVector<double> > &yQeueVsVb, QVector<QVector<double> > ySystemFAG);
    void calculateYQeuePropLambdaT(QVector<QVector<double> > &yQeueVsVb, QVector<QVector<double> > yFAG, double *Q, bool lambdaIsDependent = false);

    void calculateYServerFAG(QVector<QVector<double> > &yServerVsVb, QVector<QVector<double> > ySystemFAG);
    void calculateYServerPropLambdaT(QVector<QVector<double> > &yServerVsVb, QVector<QVector<double> > ySystemFAG, double *Q);
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

    void calculateSystem(const ModelSystem &system, double a, algorithmResults *algRes, simulationParameters *simParameters);
};
*/

} // namespace Algorithms

Q_DECLARE_METATYPE(Algorithms::cFIFO_convHybrid*)
//Q_DECLARE_METATYPE(Algorithm2Pass*)

#endif // ALGORITHMHYBRID_H
