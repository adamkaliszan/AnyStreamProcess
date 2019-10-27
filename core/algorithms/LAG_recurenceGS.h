#ifndef ALG_REK_LAG_GS_H
#define ALG_REK_LAG_GS_H

#include "algorithms/reccurenceGeneral.h"

namespace Algorithms
{

class algRekLagGS: public AlgRekGeneral
{
public:
    algRekLagGS();

    virtual QString shortName() const {return QString("LAG GS 1"); }

    virtual void calculateSystem(const ModelSystem &system
            , double a
            , Results::RInvestigator *results, SimulationParameters *simParameters
            );

    virtual bool possible(const ModelSystem &system) const;
    virtual double getSigma(int classNumber, int State);
};


class algRekLagGS2: public AlgRekGeneral
{
public:
    algRekLagGS2();

    virtual QString shortName() const {return QString("LAG GS 2"); }

    virtual void calculateSystem(const ModelSystem &system
            , double a
            , Results::RInvestigator *results
            , SimulationParameters *simParameters
            );

    virtual bool possible(const ModelSystem &system) const;
    virtual double getSigma(int classNumber, int State);
};

} // namespace Algorithms

#endif // ALG_REK_LAG_GS_H
