#ifndef ALG_REK_LAG_H
#define ALG_REK_LAG_H

#include "algorithms/algRekGeneral.h"

namespace Algorithms
{

class algRekLAG: public AlgRekGeneral
{
public:
    algRekLAG();

    virtual QString shortName() const {return QString("LAG clasic"); }

    virtual double getSigma(int classNumber, int state);

    bool possible(const ModelSystem &system) const;
};

} //namespace Algorithms

Q_DECLARE_METATYPE(Algorithms::algRekLAG*)

#endif // ALG_REK_LAG_H
