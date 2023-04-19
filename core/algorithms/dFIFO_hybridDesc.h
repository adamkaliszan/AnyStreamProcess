#ifndef ALGORITHMHYBRIDDISCR_DESC_H
#define ALGORITHMHYBRIDDISCR_DESC_H

#include <qglobal.h>
#include <QMetaType>
#include "algorithms/investigator.h"
#include "algorithms/trclvector2.h"

namespace Algorithms
{

class AlgorithmHybridDiscrDesc : public Investigator
{
public:
    AlgorithmHybridDiscrDesc();

    QString shortName()      const {return "hybr dFIFO"; }
    virtual int complexity() const {return 1; }
    virtual void calculateSystem(
            const ModelSystem &system
          , double a
          , Results::RInvestigator *results, SimulationParameters *simParameters
        );
    bool possible(const ModelSystem &system) const;

protected:
    TrClVector *P_without_i;
    TrClVector *p_single;
    TrClVector *p_singleQ;

};

} // namespace Algorithms

Q_DECLARE_METATYPE(Algorithms::AlgorithmHybridDiscrDesc*)

#endif // ALGORITHMHYBRIDDISCR_DESC_H
