#include "algorithms/investigator.h"

Investigator::Investigator():
    classes(0), ySERVER_Vs(nullptr), ySERVER_V(nullptr), yQEUE_Vb(nullptr), yQEUE_VsVb(nullptr), ySYSTEM_V(nullptr)
{
    calculationDone = false;
    _hasConfIntervall = false;
    isSelected = false;
}

bool Investigator::possible(const ModelSyst *system) const
{
    return possibleAlternative(system);
}

bool Investigator::possibleAlternative(const ModelSyst *system) const
{
    if (system->m() == 0)
        return false;
    if (system->vk_s() == 0)
        return false;

    return true;
}



bool Investigator::correctSystemParameters(ModelSyst *system, double a)
{
    qDebug("a = %lf", a);
    for (int i=0; i<system->m(); i++)
    {
        double A = system->getClass(i)->intensityNewCallTotal(a, static_cast<size_t>(system->vk_s()), system->totalAt()) /system->getClass(i)->getMu();
        if (system->getClass(i)->srcType()==ModelTrClass::SourceType::DependentMinus)
            if (A>=system->getClass(i)->s())
            {
                qDebug("Dep-: A >= s");
                return false;
            }
        double A_cor = A;
        switch (system->getClass(i)->srcType())
        {
        case ModelTrClass::SourceType::DependentMinus:
            A_cor = A*system->getClass(i)->s()/(system->getClass(i)->s() - A);
            break;
        case ModelTrClass::SourceType::DependentPlus:
            A_cor = A*system->getClass(i)->s()/(system->getClass(i)->s() + A);
            break;
        case ModelTrClass::SourceType::Independent:
            A_cor = A;
            break;
        }
        qDebug("\tA_%d = %lf -> Aof_%d = %lf", i, A, i, A_cor);

    }
    return true;
}

void Investigator::prepareTemporaryData(const ModelSyst *system, double a)
{
    this->system = system;
    classes.resize(system->m());

    for (int i=0; i<system->m(); i++)
    {
        classes[i].A = system->getClass(i)->intensityNewCallTotal(a, static_cast<size_t>(system->vk_s()), system->totalAt()) /system->getClass(i)->getMu();
        classes[i].mu = system->getClass(i)->getMu();
        classes[i].lambda = classes[i].A*classes[i].mu;
        classes[i].t = system->getClass(i)->t();
    }

    if (yQEUE_Vb != nullptr)
    {
        for (int i=0; i<system->m(); i++)
            if (yQEUE_Vb[i]!=nullptr)
                delete []yQEUE_Vb[i];
        delete []yQEUE_Vb;
    }

    if (yQEUE_VsVb != nullptr)
    {
        for (int i=0; i<system->m(); i++)
            if (yQEUE_VsVb[i]!=nullptr)
                delete []yQEUE_VsVb[i];
        delete []yQEUE_VsVb;
    }

    if (ySERVER_Vs != nullptr)
    {
        for (int i=0; i<system->m(); i++)
            if (ySERVER_Vs[i]!=nullptr)
                delete []ySERVER_Vs[i];
        delete []ySERVER_Vs;
    }

    if (ySERVER_V != nullptr)
    {
        for (int i=0; i<system->m(); i++)
            if (ySERVER_V[i]!=nullptr)
                delete []ySERVER_V[i];
        delete []ySERVER_V;
    }

    if (ySYSTEM_V != nullptr)
    {
        for (int i=0; i<system->m(); i++)
            if (ySYSTEM_V[i]!=nullptr)
                delete []ySYSTEM_V[i];
        delete []ySYSTEM_V;
    }

    yQEUE_Vb      = new double*[system->m()];
    yQEUE_VsVb    = new double*[system->m()];
    ySERVER_Vs    = new double*[system->m()];
    ySERVER_V     = new double*[system->m()];
    ySYSTEM_V     = new double*[system->m()];

    for (int i=0; i<system->m(); i++)
    {
        yQEUE_Vb[i]   = new double[system->vk_b()+1];   bzero(yQEUE_Vb[i],   static_cast<size_t>(system->vk_b() + 1) * sizeof(double));
        yQEUE_VsVb[i] = new double[system->V()  + 1];  bzero(yQEUE_VsVb[i],  static_cast<size_t>(system->V() + 1)    * sizeof(double));
        ySERVER_Vs[i] = new double[system->vk_s() + 1]; bzero(ySERVER_Vs[i], static_cast<size_t>(system->vk_s() + 1) * sizeof(double));
        ySERVER_V[i]  = new double[system->V() + 1];   bzero(ySERVER_V[i],   static_cast<size_t>(system->V() + 1)    * sizeof(double));
        ySYSTEM_V[i]  = new double[system->V() + 1];   bzero(ySYSTEM_V[i],   static_cast<size_t>(system->V() + 1)    * sizeof(double));
    }
}

void Investigator::deleteTemporaryData()
{
    for (int i=0; i<system->m(); i++)
    {
        delete []yQEUE_Vb[i];
        delete []yQEUE_VsVb[i];
        delete []ySERVER_Vs[i];
        delete []ySERVER_V[i];
        delete []ySYSTEM_V[i];
        yQEUE_Vb[i]     = nullptr;
        yQEUE_VsVb[i]   = nullptr;
        ySERVER_Vs[i]   = nullptr;
        ySERVER_V[i]    = nullptr;
        ySYSTEM_V[i]    = nullptr;
    }

    delete []yQEUE_Vb;
    delete []yQEUE_VsVb;
    delete []ySERVER_Vs;
    delete []ySERVER_V;
    delete []ySYSTEM_V;
    yQEUE_Vb = yQEUE_VsVb = ySERVER_Vs = ySERVER_V = ySYSTEM_V = nullptr;
}
