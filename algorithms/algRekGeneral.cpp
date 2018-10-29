#include "algRekGeneral.h"

#include <utils/vectorUtils.h>

namespace Algorithms
{

AlgRekGeneral::AlgRekGeneral(): Investigator(BufferResourcessScheduler::Disabled)
    //: data(new algRekLagGSData)
{
    myQoS_Set
       <<Results::Type::BlockingProbability
       <<Results::Type::OccupancyDistribution;
}

bool AlgRekGeneral::possible(const ModelSyst *system) const
{
    if (
            !Investigator::possible(system) //Sprawdzanie czy w systemie jest odpowiednia kolejka
            || (system->m() < 1)            //W systemie nie ma wystarczająco dużo klas ruchu
            || (system->k_sType() > 1)      //W systemie są podgrupy o różnych pojemnościach
            || (system->k_s(0) > 1)         //W systemie jest więcej niż 1 podgrupa o różnych pojemnościach
            )
        return false;
    return true;
}

void AlgRekGeneral::calculateSystem(
          const ModelSyst *system
        , double a
        , Results::RInvestigator *results
        , SimulationParameters *simParameters)
{
    (void) simParameters;

    prepareTemporaryData(system, a);


    states[0] = 1;
    for (int n=1; n<=system->V(); n++)
    {
        states[n] = 0;
        for (int i=0; i<system->m(); i++)
        {
            int t = classes[i].t;
            if (t <= n)
            {
                double sigma = getSigma(i, n-t);
                states[n] += states[n - t] * classes[i].A * t * sigma;
            }
        }
        states[n] /= n;
    }
    vectorUtils::normalize(states);

    // Zapis rozkładu zajętości
    for (int n=0; n<=system->V(); n++)
    {
        (*results)->write(TypeForSystemState::StateProbability, states[n], n);
    }

    for (int n=0; n<=system->vk_s(); n++)
    {
        (*results)->write(TypeForServerState::StateProbability, states[n], n);
    }


    for (int i=0; i < system->m(); i++)
    {
        int t = system->getClass(i)->t();
        double E = 0;
        for (int n=0; n<=system->V(); n++)
        {
            double sigma = getSigma(i, n);
            if (sigma > 0 && n+t > system->V())
                qFatal("Sigma calculation error");

            E += ((1 - sigma) * states[n]);
            (*results)->write(TypeForClassAndServerState::CAC_Probability, sigma, i, n);
        }
        (*results)->write(TypeForClass::BlockingProbability, E, i);
    }
    //emit this->sigCalculationDone();
}

double AlgRekGeneral::getSigma(int classNumber, int state)
{
    (void) classNumber;
    (void) state;
    return (state + system->getClass(classNumber)->t() <= system->V()) ? 1: 0;
}


void AlgRekGeneral::prepareTemporaryData(const ModelSyst *system, double a)
{
    Investigator::prepareTemporaryData(system, a);
    states.resize(system->V()+1);
}

void AlgRekGeneral::deleteTemporaryData()
{

}

AlgRekGeneral::~AlgRekGeneral()
{

}

} //namespace Algorithms
