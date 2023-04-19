#ifndef ALGSPLOTOWY_H
#define ALGSPLOTOWY_H

#include <QMetaType>
#include "algorithms/trclvector2.h"


/**
 * @brief The convolutionAlgorithm1: rozkłady w przybliżony sposób konwertorane z [Q] do [P] splatane i konwertowane z [P] do [Q].
 * Operacja konwercji jest przybliżona.
 */
/*
class convolutionAlgorithm1 : public algorithm
{
protected:
    trClVector *P_without_i;
    trClVector *p_single;
public:
    convolutionAlgorithm1();


    QString shortName() {return "conv alg 1"; }
    virtual int complexity() {return 1; }

    void calculateSystem(modelSyst *system, double a, algorithmResults *algRes, simulationParameters *simParameters);
    bool possible(modelSyst *system);
};


class convolutionAlgorithm2 : public algorithm
{
protected:
    trClVector *P_without_i;
    trClVector *p_single;

public:
    convolutionAlgorithm2();

    void deleteTemporaryData();

    QString shortName() {return "conv alg 2"; }
    virtual int complexity() {return 1; }

    void calculateSystem(modelSyst *system, double a, algorithmResults *algRes, simulationParameters *simParameters);
};




Q_DECLARE_METATYPE(convolutionAlgorithm1*)
Q_DECLARE_METATYPE(convolutionAlgorithm2*)
*/

#endif // ALGSPLOTOWY_H
