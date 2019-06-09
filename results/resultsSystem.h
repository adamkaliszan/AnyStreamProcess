#ifndef RESULTS_SYSTEM_H
#define RESULTS_SYSTEM_H


#include "model.h"
#include "utils/decimal.h"

#include "algorithms/investigator.h"
#include "results/resultsInvestigator.h"

namespace Results
{

class RSystem
{
public:
    RSystem(const ModelCreator& model);

    RInvestigator& createNewInvestigation(Investigator *algorithm, decimal aPerAU, unsigned noOfSeries = 1);
    RInvestigator *getInvestigationResults(Investigator *algorithm, decimal aPerAU);

    const QList<decimal> &     getAvailableAperAU()     const { return _aPerAU;     }
    const QList<Investigator*> getAvailableAlgorithms() const { return _algorithms; }

    int getNumberOrCalculatedAlgorithms() const;

    const ModelCreator& getModel() const {return model;}

    double getMinAperAU() const;
    double getMaxAperAU() const;
    int getNoOfGroupsCombinations() const;

    QVector<int> getGroupCombination(int combinationNo);
    QString getGroupCombinationStr(int combinationNo);

private:
    const ModelCreator& model;
    QList<decimal> _aPerAU;
    QList<Investigator*> _algorithms;

    mutable QVector<QPair<QVector<int>, QVector<int> > > groupCombinations;

    //Dodać identyfikator algorytmu do mapy
    QMap<Investigator*, QMap<decimal, RInvestigator > > resultsForInvestigatorsAndOfferedTraffic;
};

} // namespace Results


#endif // RESULTS_SYSTEM_H
