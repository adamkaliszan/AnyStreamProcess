#include "hybridDiscr.h"
#include "results/resultsInvestigator.h"
#include "model.h"

namespace Algorithms
{

AlgorithmHybridDiscr::AlgorithmHybridDiscr(): Investigator()
{
    myQoS_Set
       <<Results::Type::BlockingProbability
       <<Results::Type::OccupancyDistribution
       <<Results::Type::OccupancyDistributionServerAndBuffer;
}

bool AlgorithmHybridDiscr::possible(ModelSystem &system)
{
    if (system.getBuffer().V() == 0)
        return false;
    return Investigator::possible(system);
}

void AlgorithmHybridDiscr::calculateSystem(
        const ModelSystem &system
      , double a
      , RInvestigator *results
      , SimulationParameters *simParameters
    )
{
    (void) simParameters;
    int m = system.m();
    int Vs = system.getServer().V();
    int Vb = system.getBuffer().V();
    int VsVb = Vs + Vb;
    prepareTemporaryData(system, a);

    p_single = new TrClVector[m];
    P_without_i = new TrClVector[m];

    QVector<QVector<double>> X(VsVb+1);


    double *delta = new double[VsVb+1];
    double **delta2d = new double*[VsVb+1];

    State *Q = new State[VsVb+1];
    State **Q2d = new State*[VsVb+1];

    int t_max = 0;
    for (int i=0; i<m; i++)
    {
        p_single[i] = system.getTrClass(i).trDistribution(i, classes[i].A, VsVb, 0);
        t_max = qMax(t_max, classes[i].t);
    }

    for (int n=0; n<=VsVb; n++)
    {
         X[n].resize(t_max);
         Q2d[n] = new State[t_max];
         bzero(Q2d[n], static_cast<size_t>(t_max)*sizeof(State));
         if (n<=Vs)
             X[n][0] = 1;

         delta2d[n] = new double[t_max];
    }

    TrClVector P(VsVb);

    for (int i=0; i<m; i++)
    {
        for (int j=0; j<m; j++)
        {
            if (i == j)
                continue;
            P_without_i[i] = TrClVector::convFAG(P_without_i[i], p_single[j], true, VsVb);
        }
        TrClVector tmp = TrClVector::convFAG(P, p_single[i], true, VsVb);
        P = tmp;
    }

    // Liczenie y
    for (int i=0; i<m; i++)
    {
        p_single[i].normalize();
        P_without_i[i].normalize();

        for (int n=0; n<=VsVb; n++)
        {
            double nominator = 0;
            double denumerator = 0;
            for (int l=0; l<=n; l+= classes[i].t)
            {
                double pP = p_single[i][l] * P_without_i[i][n-l];
                nominator += (static_cast<double>(l)/static_cast<double>(classes[i].t) * pP);
                denumerator += pP;
            }
            ySYSTEM_V[i][n]=nominator/denumerator;
        }
    }

    //Liczenie rozkładu niedostępnych PJP w serwerze
    for (int n=Vs+1; n<=VsVb; n++)
    {
        double sum = 0;
        for (int x=0; x<t_max; x++)
        {
            X[n][x]=0;

            if (x > Vs)
                continue;

            for (int i=0; i<m; i++)
            {
                if (x>=classes[i].t)
                    continue;
                X[n][x]+=ySYSTEM_V[i][n];

            }
            sum +=X[n][x];
        }

        if (qFuzzyIsNull(sum)) //sum == 0
        {
            X[n][0] = 1;
            for (int x=1; x<t_max; x++)
                X[n][x] = 0;
        }
        else
            for (int x=0; x<t_max; x++)
                X[n][x] /=sum;
    }

    double sumQ=0;
    for (int n=0; n<=Vs; n++)
    {
        delta[n] = 1;
        Q[n] = P.getState(n);
        Q2d[n][0] = P.getState(n);
        sumQ += Q[n].p;

        for (int x=0; x<t_max; x++)
            delta2d[n][x] = 1;

        Q2d[n][0] = Q[n];
    }

    for (int n=Vs+1; n<=VsVb; n++)
    {
        delta[n] = 0;
        for (int i=0; i<m; i++)
        {
            delta[n] += (delta[n-classes[i].t]*ySYSTEM_V[i][n] * classes[i].t);
        }
        delta[n] /=Vs;
        Q[n].p = 0;//delta[n] * P[n];
        //sumQ += Q[n];

        for (int x=0; x<t_max; x++)
        {
            delta2d[n][x] = 0;
            for (int i=0; i<m; i++)
                delta2d[n][x] += (delta2d[n-classes[i].t][x]*ySYSTEM_V[i][n] * classes[i].t);

            delta2d[n][x] /=(Vs-x);
            if (n+x<=VsVb)
                Q2d[n][x].p = delta2d[n][x] * P[n] * X[n][x];
            else
                Q2d[n][x].p = 0;

            sumQ += Q2d[n][x].p;
            Q[n].p+=Q2d[n][x].p;
        }
    }

    for (int n=0; n<= VsVb; n++)
    {
        Q[n].p /= sumQ;
        for (int x=0; x<t_max; x++)
            Q2d[n][x].p /= sumQ;
    }

    for (int i=0; i<system.m(); i++)
    {
        //Prawdopodobieństwo blokady
        double E = 0;
        for (int n=VsVb-(classes[i].t-1)-(t_max-1); n<=VsVb; n++)
        {
            for (int x=0; x<t_max; x++)
            {
                if (x+n+classes[i].t>VsVb)
                    E+=Q2d[n][x].p;
            }
        }
        (*results)->write(TypeForClass::BlockingProbability, E, i);

        //Prawdopodobieństwo strat
        double B_n = 0;
        double B_d = 0;
        for (int n=0; n<=VsVb; n++)
        {
            double intensity = system.getTrClass(i).intensityNewCallForState(1, static_cast<int>(ySYSTEM_V[i][n]));
            for (int x=0; x<t_max; x++)
            {
                if (n + x + classes[i].t > VsVb)
                    B_n+=(intensity*Q2d[n][x].p);
                B_d +=(intensity*Q2d[n][x].p);
            }
        }
        (*results)->write(TypeForClass::LossProbability, B_n/B_d, i);

        //Średnia liczba obsługiwanych zgłoszeń w całym systemie
        double y = 0;
        for (int n=0; n<=VsVb; n++)
        {
            y+=(Q[n].p * (ySYSTEM_V[i][n]));
        }
        (*results)->write(TypeForClass::AvarageNumbersOfCallsInSystem, y, i);

        //Średnia liczba zgłoszeń w serwerze
        y = 0;
        for (int n=0; n<=VsVb; n++)
        {
            y+=(Q[n].p * (n < Vs ?  ySYSTEM_V[i][n] : ySYSTEM_V[i][Vs]));
        }
        (*results)->write(TypeForClass::AvarageNumbersOfCallsInServer, y, i);

        //Średnia liczba zgłoszeń w kolejce
        double yQ = 0;
        for (int n=Vs+1; n<=VsVb; n++)
        {
            yQ+=(Q[n].p * (ySYSTEM_V[i][n] - ySYSTEM_V[i][Vs]));
        }
        (*results)->write(TypeForClass::AvarageNumbersOfCallsInBuffer, yQ, i);
    }

    double avgLen;

    //Średnia liczba zajętych zasobów systemu
    avgLen= 0;
    for (int n=0; n<=VsVb; n++)
        avgLen += Q[n].p * n;
    (*results)->write(TypeGeneral::SystemUtilization, avgLen);


    //Średnia liczba zajętych zasobów serwera
    avgLen= 0;
    for (int n=0; n<=VsVb; n++)
        avgLen += Q[n].p * (n <= Vs ? n : Vs);
    (*results)->write(TypeGeneral::ServerUtilization, avgLen);

    //Średnia długość kolejki
    avgLen= 0;
    for (int n=Vs+1; n<=VsVb; n++)
        avgLen += (n-Vs)*Q[n].p;
    (*results)->write(TypeGeneral::BufferUtilization, avgLen);

    for (int i=0; i<m; i++)
    {
        for (int n=0; n<=VsVb; n++)
        {
            (*results)->write(TypeForClassAndSystemState::UsageForSystem, ySYSTEM_V[i][n] * classes[i].t, i, n);
        }
    }

    for (int n=0; n<=VsVb; n++)
        (*results)->write(TypeForSystemState::StateProbability, Q[n].p, n);

    for (int n_s=0; n_s<=Vs; n_s++)
    {
        for (int n_b=0; n_b<=Vb; n_b++)
        {
            double val = 0;

            if (n_s <=Vs && n_b == 0)
                val = Q2d[n_s + n_b][0].p;
            else
            {
                int unused = n_s - Vs;
                if (n_b >= unused)
                    val = Q2d[n_s + n_b][unused].p;
            }
            (*results)->write(TypeForServerAngBufferState::StateProbability, val, n_s, n_b);

        }
    }

    deleteTemporaryData();

    delete []P_without_i;
    delete []p_single;
    delete []delta;
}

} // namespace Algorithms
