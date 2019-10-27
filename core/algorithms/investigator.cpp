#include "algorithms/investigator.h"

Investigator::Investigator():
    system(nullptr)
{
    calculationDone = false;
    _hasConfIntervall = false;
    isSelected = false;
}

bool Investigator::possible(const ModelSystem &system) const
{
    return possibleAlternative(system);
}

bool Investigator::possibleAlternative(const ModelSystem &system) const
{
    if (system.m() == 0)
        return false;
    if (system.getServer().V() == 0)
        return false;

    return true;
}

QSet<Type> Investigator::getQoS_Set() const
{
    QSet<Type> result;
    foreach (Type type, myQoS_Set)
    {
        //TODO add this method if (system->hasQoS(type))
            result+= type;
    }
    return result;
}



bool Investigator::correctSystemParameters(const ModelSystem &system, double a)
{
    size_t V = static_cast<size_t>(system.getServer().V());
    qDebug("a = %lf", a);
    for (int i=0; i<system.m(); i++)
    {
        double lambda = system.getTrClass(i).intensityNewCallTotal(a, V, system.getTotalAt());
        double A = lambda/system.getTrClass(i).getMu();

        if (system.getTrClass(i).srcType()==ModelTrClass::SourceType::DependentMinus)
            if (A>=system.getTrClass(i).s())
            {
                qDebug("Dep-: A >= s");
                return false;
            }
        double A_cor = A;
        switch (system.getTrClass(i).srcType())
        {
        case ModelTrClass::SourceType::DependentMinus:
            A_cor = A*system.getTrClass(i).s()/(system.getTrClass(i).s() - A);
            break;
        case ModelTrClass::SourceType::DependentPlus:
            A_cor = A*system.getTrClass(i).s()/(system.getTrClass(i).s() + A);
            break;
        case ModelTrClass::SourceType::Independent:
            A_cor = A;
            break;
        }
        qDebug("\tA_%d = %lf -> Aof_%d = %lf", i, A, i, A_cor);

    }
    return true;
}

void Investigator::prepareTemporaryData(const ModelSystem &system, double a)
{
    this->system = new ModelSystem(system);
    classes.resize(system.m());

    for (int i=0; i<system.m(); i++)
    {
        classes[i].A = system.getTrClass(i).intensityNewCallTotal(a, static_cast<size_t>(system.getServer().V()), system.getTotalAt()) /system.getTrClass(i).getMu();
        classes[i].mu = system.getTrClass(i).getMu();
        classes[i].lambda = classes[i].A*classes[i].mu;
        classes[i].t = system.t(i);
    }

    yQEUE_Vb.resize(system.m());
    yQEUE_VsVb.resize(system.m());
    ySERVER_Vs.resize(system.m());
    ySERVER_V.resize(system.m());
    ySYSTEM_V.resize(system.m());

    for (int i=0; i<system.m(); i++)
    {
        yQEUE_Vb[i].resize(system.getBuffer().V()+1);       yQEUE_Vb[i].fill(0);
        yQEUE_VsVb[i].resize(system.V()  + 1);              yQEUE_VsVb[i].fill(0);
        ySERVER_Vs[i].resize(system.getServer().V() + 1);   ySERVER_Vs[i].fill(0);
        ySERVER_V[i].resize(system.V() + 1);                ySERVER_V[i].fill(0);
        ySYSTEM_V[i].resize(system.V() + 1);                ySYSTEM_V[i].fill(0);
    }
}

void Investigator::deleteTemporaryData()
{
}
