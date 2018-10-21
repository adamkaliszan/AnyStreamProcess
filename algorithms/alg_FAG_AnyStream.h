#ifndef ALGORITHMANYSTREAMFAG_H
#define ALGORITHMANYSTREAMFAG_H

#include <qglobal.h>
#include <QMetaType>
#include "algorithms/investigator.h"
#include "algorithms/algorithmConv.h"
#include "algorithms/trclvector2.h"

namespace Algorithms
{

class AlgorithmAnyStreamFAG : public Investigator
{
protected:
    TrClVector *p_single;

public:
    AlgorithmAnyStreamFAG();

    QString shortName() const { return QString("AnyStream"); }
    int complexity()    const { return 1; }
    void calculateSystem(const ModelSyst *system
          , double a
          , Results::RInvestigator *results, SimulationParameters *simParameters
          );
    bool possible(const ModelSyst *system) const;
};

} //namespace Algorithms

Q_DECLARE_METATYPE(Algorithms::AlgorithmAnyStreamFAG*)

#endif // ALGORITHMANYSTREAMFAG_H

