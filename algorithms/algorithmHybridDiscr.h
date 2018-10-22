#ifndef ALGORITHMHYBRIDDISCR_H
#define ALGORITHMHYBRIDDISCR_H

#include <qglobal.h>
#include <QMetaType>

#include "algorithms/investigator.h"
#include "algorithms/algorithmConv.h"
#include "algorithms/trclvector2.h"

namespace Algorithms
{

class AlgorithmHybridDiscr : public Investigator
{
protected:
    TrClVector *P_without_i;
    TrClVector *p_single;
    TrClVector *p_singleQ;

public:
    AlgorithmHybridDiscr();

    //void deleteTemporaryData();

    QString shortName() {return "hybr discr ?"; }
    virtual int complexity() {return 1; }

    void calculateSystem(const ModelSyst *system
            , double a
            , Results::RInvestigator *results, SimulationParameters *simParameters);
    bool possible(ModelSyst *system);
};

} // namespace Algorithms

Q_DECLARE_METATYPE(Algorithms::AlgorithmHybridDiscr*)

#endif // ALGORITHMHYBRIDDISCR_H
