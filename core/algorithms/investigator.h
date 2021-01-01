#ifndef INVESTIGATOR_321_H
#define INVESTIGATOR_321_H

//#include <QMetaType>
#include <QSet>

#include <QString>

#include <iostream>
#include <math.h>

#include "algorithms/simulationParameters.h"

#include "results/resultsApiTypes.h"
#include "results/resultsInvestigator.h"
#include "model.h"

extern char *argv0;

using namespace Results;

//class AlgorithmResults;

namespace Results
{
    class RInvestigator;

}

class Investigator
{
protected:
    class ClassParameters;

    QSet<Results::Type> myQoS_Set;

public:
    Investigator();
    virtual ~Investigator() {}


    virtual QString shortName() const = 0;
    virtual int complexity()    const = 0;
    virtual void  calculateSystem(
              const ModelSystem &system
            , double a
            , RInvestigator *results
            , SimulationParameters *simParameters
            ) = 0;
    virtual bool possible(const ModelSystem &system) const;
    virtual bool possibleAlternative(const ModelSystem &system) const;

    QSet<Results::Type> getQoS_Set() const;

    bool hasConfIntervall() const { return _hasConfIntervall; }

    bool correctSystemParameters(const ModelSystem &system, double a);


    bool isSelected;
    bool calculationDone;
    bool show;

protected:
    virtual void prepareTemporaryData(const ModelSystem &system, double a);
    virtual void deleteTemporaryData();

    const ModelSystem *system;
    QVector<ClassParameters> classes;



    bool _hasConfIntervall;

    QVector<QVector<double> > ySERVER_Vs;    ///< Avarage number of calls of class i in server vs number of occupied AS in server
    QVector<QVector<double> > ySERVER_V;     ///< Avarage number of calls of class i in server vs number of occupied AS in system
    QVector<QVector<double> > yQEUE_Vb;      ///< Avarage number of calls of class i in qeue   vs number of occupied AS in qeue
    QVector<QVector<double> > yQEUE_VsVb;    ///< Avarage number of calls of class i in qeue   vs number of occupied AS in system
    QVector<QVector<double> > ySYSTEM_V;     ///< Avarage number of calls of class i in system vs number of occupied AS in system

    class ClassParameters
    {
    public:
        double  A;
        double  mu;
        double  lambda;
        int    t;
    };
};

Q_DECLARE_METATYPE(Investigator*)
Q_DECLARE_METATYPE(const Investigator*)


#endif // INVESTIGATOR2_H




