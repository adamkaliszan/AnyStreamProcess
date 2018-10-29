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


using namespace Results;

//class AlgorithmResults;

namespace Results
{
    class RInvestigator;

}

class Investigator//: public QObject
{
//    Q_OBJECT

protected:
    class ClassParameters;

    QSet<Results::Type> myQoS_Set;

public:
    Investigator(BufferResourcessScheduler queueDisc);
    virtual ~Investigator() {}


    virtual QString shortName() const = 0;
    virtual int complexity()    const = 0;
    virtual void  calculateSystem(
              const ModelSyst *system
            , double a
            , RInvestigator *results
            , SimulationParameters *simParameters
            ) = 0;
    virtual bool possible(const ModelSyst *system) const;
    virtual bool possibleAlternative(const ModelSyst *system) const;

    QSet<Results::Type> getQoS_Set() const {return myQoS_Set; }

    QString shortQueueDiscipline() const;
    bool hasConfIntervall() const { return _hasConfIntervall; }

    bool correctSystemParameters(ModelSyst *system, double a);


    bool isSelected;
    bool calculationDone;
    bool show;

protected:
    virtual void prepareTemporaryData(const ModelSyst *system, double a);
    virtual void deleteTemporaryData();

    const ModelSyst* system;

    bool _hasConfIntervall;

    QVector<ClassParameters> classes;

    double **ySERVER_Vs;    ///Avarage number of calls of class i in server vs number of occupied AS in server
    double **ySERVER_V;     ///Avarage number of calls of class i in server vs number of occupied AS in system
    double **yQEUE_Vb;      ///Avarage number of calls of class i in qeue   vs number of occupied AS in qeue
    double **yQEUE_VsVb;    ///Avarage number of calls of class i in qeue   vs number of occupied AS in system
    double **ySYSTEM_V;     ///Avarage number of calls of class i in system vs number of occupied AS in system

    const BufferResourcessScheduler disc;

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




