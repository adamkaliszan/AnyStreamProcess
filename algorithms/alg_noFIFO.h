#ifndef ALGORITHM_HYBRID_NOFIFO_H
#define ALGORITHM_HYBRID_NOFIFO_H

#include <qglobal.h>
#include <QMetaType>
#include "algorithms/algorithmConv.h"
#include "algorithms/investigator.h"
#include "algorithms/trclvector2.h"

namespace Algorithms
{

class AlgorithmHybridNoFifo : public Investigator
{
protected:
    TrClVector *p_single;

public:
    AlgorithmHybridNoFifo();

    QString shortName()  const { return QString("hybr NQ"); }
    int complexity()     const { return 1; }
    void calculateSystem(const ModelSyst *system
            , double a
            , Results::RInvestigator *results, SimulationParameters *simParameters
            );
    bool possible(const ModelSyst *system) const;

private:
    void calculateYSystem(QVector<QVector<double> > ySystem,  const TrClVector &P);
    void calculateYServer(QVector<QVector<double> > yServerVsVb, const TrClVector &P);
};

} // namespace Algorithms

Q_DECLARE_METATYPE(Algorithms::AlgorithmHybridNoFifo*)

#endif // ALGORITHM_HYBRID_NOFIFO_H
