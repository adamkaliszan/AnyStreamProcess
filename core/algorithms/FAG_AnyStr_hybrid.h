#ifndef ALGORITHMANYSTREAMFAG_H
#define ALGORITHMANYSTREAMFAG_H

#include <qglobal.h>
#include <QMetaType>
#include "algorithms/investigator.h"
#include "algorithms/conv.h"
#include "algorithms/trclvector2.h"

namespace Algorithms
{

class FAG_AnyStry_hybrid : public Investigator
{
protected:
    TrClVector *p_single;

public:
    FAG_AnyStry_hybrid();

    QString shortName() const { return QString("AnyStream"); }
    int complexity()    const { return 1; }
    void calculateSystem(const ModelSystem &system
          , double a
          , Results::RInvestigator *results, SimulationParameters *simParameters
          );
    bool possible(const ModelSystem &system) const;
};

} //namespace Algorithms

Q_DECLARE_METATYPE(Algorithms::FAG_AnyStry_hybrid*)

#endif // ALGORITHMANYSTREAMFAG_H

