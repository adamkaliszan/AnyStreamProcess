#ifndef ALGORITHMANYSTREAMFAG_H
#define ALGORITHMANYSTREAMFAG_H

#include <qglobal.h>
#include <QMetaType>
#include "algorithms/investigator.h"
#include "algorithms/trclvector2.h"

namespace Algorithms
{

class FAG_conv_AnyStr_sim : public Investigator
{
protected:
    TrClVector *p_single;

public:
    FAG_conv_AnyStr_sim();

    QString shortName() const { return QString("FAG conv 2d sim"); }
    int complexity()    const { return 1; }
    void calculateSystem(const ModelSystem &system
          , double a
          , Results::RInvestigator *results, SimulationParameters *simParameters
          );
    bool possible(const ModelSystem &system) const;
};

} //namespace Algorithms

Q_DECLARE_METATYPE(Algorithms::FAG_conv_AnyStr_sim*)

#endif // ALGORITHMANYSTREAMFAG_H

