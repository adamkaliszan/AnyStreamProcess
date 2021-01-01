#ifndef FAG_ANYSTR_ML_H
#define FAG_ANYSTR_ML_H

#include <qglobal.h>
#include <QMetaType>
#include "algorithms/investigator.h"
#include "algorithms/conv.h"
#include "algorithms/trclvector2.h"

namespace Algorithms
{

class FAG_AnyStr_ML : public Investigator
{
protected:
    TrClVector *p_single;

public:
    FAG_AnyStr_ML();

    QString shortName() const { return QString("AnyStream ML"); }
    int complexity()    const { return 1; }
    void calculateSystem(const ModelSystem &system
          , double a
          , Results::RInvestigator *results, SimulationParameters *simParameters
          );
    bool possible(const ModelSystem &system) const;
};

}
#endif // FAG_ANYSTR_ML_H
