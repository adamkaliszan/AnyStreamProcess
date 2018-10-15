#ifndef ALG_REK_LAG_GS_H
#define ALG_REK_LAG_GS_H

#include "algorithms/algRekGeneral.h"

namespace Algorithms
{

class algRekLagGS: public AlgRekGeneral
{
public:
    algRekLagGS();

    virtual QString shortName() const {return QString("LAG GS 1"); }

    virtual void calculateSystem(const ModelSyst *system
            , double a
            , Results::RInvestigator *results, SimulationParameters *simParameters
            );

    virtual bool possible(const ModelSyst *system) const;
    virtual double getSigma(int classNumber, int state);
};


class algRekLagGS2: public AlgRekGeneral
{
public:
    algRekLagGS2();

    virtual QString shortName() const {return QString("LAG GS 2"); }

    virtual void calculateSystem(const ModelSyst *system
            , double a
            , Results::RInvestigator *results
            , SimulationParameters *simParameters
            );

    virtual bool possible(const ModelSyst *system) const;
    virtual double getSigma(int classNumber, int state);
};

} // namespace Algorithms

#endif // ALG_REK_LAG_GS_H
