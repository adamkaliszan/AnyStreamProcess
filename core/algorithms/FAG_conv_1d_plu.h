#ifndef ALGORITHM_HYBRID_NOFIFO_H
#define ALGORITHM_HYBRID_NOFIFO_H

#include <qglobal.h>
#include <QMetaType>
#include "algorithms/investigator.h"
#include "algorithms/trclvector2.h"

namespace Algorithms
{

/*
 *  Legacy convalution algorithm with additional intensity of service and arrival calculation
 */
class FAG_conv_1d : public Investigator
{
protected:
    TrClVector *p_single;

public:
    FAG_conv_1d();

    QString shortName()  const { return QString("FAG conv legacy"); }
    int complexity()     const { return 1; }
    void calculateSystem(
        const ModelSystem &system
      , double a
      , Results::RInvestigator *results, SimulationParameters *simParameters
    );
    bool possible(const ModelSystem &system) const;

private:
    void calculateYSystem(QVector<QVector<double> > ySystem,  const TrClVector &P);
    void calculateYServer(QVector<QVector<double> > yServerVsVb, const TrClVector &P);
};

} // namespace Algorithms

Q_DECLARE_METATYPE(Algorithms::FAG_conv_1d*)

#endif // ALGORITHM_HYBRID_NOFIFO_H
