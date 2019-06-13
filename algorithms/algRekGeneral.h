#ifndef ALG_REK_LAG_GENERAL_H
#define ALG_REK_LAG_GENERAL_H

#include <QSharedDataPointer>

#include "algorithms/investigator.h"

//class algRekLagGSData;

namespace Algorithms
{

class AlgRekGeneral: public Investigator
{
public:
    AlgRekGeneral();

    virtual QString shortName() const {return QString("Kaufman-Roberts"); }
    virtual int complexity()    const {return 1; }
    virtual void calculateSystem(const ModelSystem &system
            , double a
            , RInvestigator *results
            , SimulationParameters *simParameters
            );
    virtual bool possible(const ModelSystem &system) const;



    virtual void prepareTemporaryData(const ModelSystem &system, double a);
    virtual void deleteTemporaryData();

    ~AlgRekGeneral();

    virtual double getSigma(int classNumber, int state);
protected:
    QVector<double> states;

    //QSharedDataPointer<algRekLagGSData> data;
};

} //namespace Algorithms

#endif // ALG_REK_LAG_GENERAL_H
