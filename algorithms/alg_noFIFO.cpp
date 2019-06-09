#include "alg_noFIFO.h"
#include "results/resultsInvestigator.h"

namespace Algorithms
{

AlgorithmHybridNoFifo::AlgorithmHybridNoFifo() : Investigator()
{
    myQoS_Set
     << Results::Type::BlockingProbability
     << Results::Type::LossProbability
     << Results::Type::OccupancyDistribution
     << Results::Type::NumberOfCallsInSystemVsSystemState
     << Results::Type::NewCallOfSingleClassIntensityOut_inSystemVsSystemState<< Results::Type::NewCallOfSingleClassIntensityIn_inSystemVsSystemState<< Results::Type::EndCallOfSingleClassIntensityOut_inSystemVsSystemState<< Results::Type::EndCallOfSingleClassIntensityIn_inSystemVsSystemState
        ;
}


void AlgorithmHybridNoFifo::calculateSystem(const ModelCreator *system
        , double a
        , RInvestigator *results
        , SimulationParameters *simParameters
        )
{
    (void) simParameters;

    prepareTemporaryData(system, a);
    p_single = new TrClVector[system->m()];

    for (int i=0; i<system->m(); i++)
    {
        p_single[i] = system->getClass(i)->trDistribution(i, classes[i].A, system->V(), 0);
    }
    TrClVector P(system->V());

    P = TrClVector(system->V());
    for (int i=0; i < system->m(); i++)
    {
        P = TrClVector::convFAG(P, p_single[i], true, system->V());
    }

    for (int i=0; i < system->m(); i++)
    {
        //Prawdopodobieństwo blokady i strat
        double E = 0;
        double B_n = 0;
        double B_d = 0;
        for (int n = system->V() + 1 - classes[i].t; n <= system->V(); n++)
        {
            E+=P[n];
            B_n+=(P[n] * P.getIntOutNew(n, i));
        }
        for (int n=0; n <= system->V(); n++)
        {
            B_d+=(P[n] * P.getIntOutNew(n, i));
        }

        (*results)->write(TypeForClass::BlockingProbability, E, i);
        (*results)->write(TypeForClass::LossProbability, B_n/B_d, i);

        //Średnia liczba obsługiwanych zgłoszeń
        //TODO for (int n=0; n <= system->V(); n++)
        //{
        //    algResults->set_lSys(system->getClass(i), a, P.getY(n, i));
        //}
        //Obsługiwany ruch
        double yS = 0;

        for (int n=0; n <= system->V(); n++)
        {
            yS+=(P[n] * P.getY(n, i));
        }
        //TODO algResults->set_ServTraffic(system->getClass(i), a, yS * classes[i].t);


        //Średni czas obsługi
        //algRes->set_tService(system->getClass(i), a, lQeue / A[i] * system->getClass(i)->getMu());
        double avgToS = 0;
        for (int n=0; n<=system->vk_s(); n++)
        {
            avgToS += P[n] / P.getIntOutEnd(n, i);
        }
        //TODO algResults->set_tService(system->getClass(i), a, avgToS);


        for (int n=0; n<=system->vk_s(); n++)
        {
            (*results)->write(TypeForClassAndServerState::Usage, P.getY(n, i)*classes[i].t, i, n);
        }

        for (int n=0; n <= system->V(); n++)
        {
            (*results)->write(TypeForClassAndSystemState::UsageForSystem, P.getY(n, i)*classes[i].t, i, n);

        //Intensywności przejść klas
            (*results)->write(TypeForClassAndSystemState::NewCallIntensityInForSystem        , P.getIntInNew (n, i), i, n);
            (*results)->write(TypeForClassAndSystemState::EndCallIntensityInForSystem        , P.getIntInEnd (n, i), i, n);
            (*results)->write(TypeForClassAndSystemState::OfferedNewCallIntensityOutForSystem, P.getIntOutNew(n, i), i, n);
            (*results)->write(TypeForClassAndSystemState::EndCallIntensityOutForSystem       , P.getIntOutEnd(n, i), i, n);
        }
    }

    for (int n=0; n <= system->V(); n++)
    {
        //Rozkład zajętości
        (*results)->write(TypeForSystemState::StateProbability, P[n], n);

        //Intensywności przejść (wejście do stanu)
        (*results)->write(TypeForSystemState::IntensityNewCallIn, P.getState(n).tIntInNew, n);
        (*results)->write(TypeForSystemState::IntensityEndCallIn, P.getState(n).tIntInEnd, n);

        //Intensywności przejść (wyjście ze stanu)
        (*results)->write(TypeForSystemState::IntensityNewCallOutOffered, P.getState(n).tIntOutNew, n);
        (*results)->write(TypeForSystemState::IntensityEndCallOut, P.getState(n).tIntOutEnd, n);
    }

    deleteTemporaryData();
    delete []p_single;

    //emit this->sigCalculationDone();
}

void AlgorithmHybridNoFifo::calculateYSystem(
    QVector<QVector<double> > ySystem
  , const TrClVector &P
)
{
    for (int i=0; i < system->m(); i++)
        for (int n=0; n<=system->V(); n++)
            ySystem[i][n] = P.getY(n, i);
}

void AlgorithmHybridNoFifo::calculateYServer(
    QVector<QVector<double> > yServerVsVb
  , const TrClVector &P
)
{
    for (int i=0; i < system->m(); i++)
        for (int n=0; n <= system->V(); n++)
            yServerVsVb[i][n] = P.getY(n, i);
}

bool AlgorithmHybridNoFifo::possible(const ModelCreator *system) const
{
    if (system->vk_b() > 0)
        return false;

    if (system->k_s() > 1)
        return false;

    return Investigator::possible(system);
}

} // namespace Algorithms
