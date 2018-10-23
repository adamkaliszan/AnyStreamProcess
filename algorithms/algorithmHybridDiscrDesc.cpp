#include "algorithmHybridDiscrDesc.h"
#include "results/resultsInvestigator.h"

namespace Algorithms
{

AlgorithmHybridDiscrDesc::AlgorithmHybridDiscrDesc(): Investigator(QueueServDiscipline::dFIFO)
{
    myQoS_Set
       <<Results::Type::BlockingProbability
       <<Results::Type::OccupancyDistribution;
}

bool AlgorithmHybridDiscrDesc::possible(const ModelSyst *system) const
{
    if (system->vk_b() == 0)
        return false;
    return Investigator::possible(system);
}

void AlgorithmHybridDiscrDesc::calculateSystem(const ModelSyst *system
        , double a
        , RInvestigator *results
        , SimulationParameters *simParameters
        )
{
    (void) simParameters;
    prepareTemporaryData(system, a);
    int m = system->m();
    int Vs = system->vk_s();
    int Vb = system->vk_b();
    int VsVb = Vs + Vb;

    p_single = new TrClVector[m];             /// FAG traffic distribution
    P_without_i = new TrClVector[m];          /// FAG traffic distribution
    TrClVector P(VsVb);                       /// FAG traffic distribution

    double **X;                               /// 2d distribution - X[n, x] is a probability, that x AS in state n are unused
    double **delta_X;                         /// Q_2d[s][q] = delta_X[s][q]

    X = new double*[VsVb+1];
    delta_X = new double*[VsVb+1];            ///
    double **Q_X = new double*[VsVb+1];       /// 2d distribution

    int t_max = 0;
    int *t = new int[m];
    for (int i=0; i<m; i++)
    {
        p_single[i] = system->getClass(i)->trDistribution(i, classes[i].A, VsVb, 0);
        t[i] = system->getClass(i)->t();
        t_max = qMax(t_max, t[i]);
    }

    for (int n=0; n<=VsVb; n++)
    {
         X[n] = new double[t_max];
         Q_X[n] = new double[t_max];
         bzero(X[n], t_max*sizeof(double));
         bzero(Q_X[n], t_max*sizeof(double));
         if (n<=Vs)
             X[n][0] = 1;

         delta_X[n] = new double[t_max];
    }

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
            for (int l=0; l<=n; l+= t[i])
            {
                double pP = p_single[i][l] * P_without_i[i][n-l];
                nominator += ((double)(l)/(double)(t[i]) * pP);
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

            if (x > n-Vs)
                continue;

            for (int i=0; i<m; i++)
            {
                if (x>=t[i])
                    continue;
                X[n][x]+=ySYSTEM_V[i][n];

            }
            sum +=X[n][x];
        }

        if (qFuzzyCompare(sum, 0))
        {
            X[n][0] = 1;
            for (int x=1; x<t_max; x++)
                X[n][x] = 0;
        }
        else
            for (int x=0; x<t_max; x++)
                X[n][x] /=sum;
    }

    for (int n=0; n<=VsVb; n++)
    {
        for (int x=0; x<t_max; x++)
        {
            if (n+x <= Vs)
                delta_X[n][x] = 1;
            else
            {
                delta_X[n][x] = 0;
                for (int i=0; i<m; i++)
                    delta_X[n][x] += (delta_X[n-t[i]][x]*ySYSTEM_V[i][n] * t[i]);

                delta_X[n][x] /=(Vs-x);
            }
        }
    }

    double sumQ=0;
    for (int n=0; n<=VsVb; n++)
    {
        for (int x=0; x<t_max; x++)
        {
            if (n+x<=VsVb)
                Q_X[n][x] = delta_X[n][x] * P[n] * X[n][x];
            else
                Q_X[n][x] = 0;

            sumQ += Q_X[n][x];
        }
    }

    for (int n=0; n<= VsVb; n++)
    {
        for (int x=0; x<t_max; x++)
            Q_X[n][x] /= sumQ;
    }

    for (int i=0; i<system->m(); i++)
    {
        //Prawdopodobieństwo blokady
        double E = 0;
        for (int n=VsVb-(t[i]-1)-(t_max-1); n<=VsVb; n++)
        {
            for (int x=0; x<qMin(t_max, t[i]); x++)
            {
                if (x+n+t[i]>VsVb)
                    E+=Q_X[n][x];
            }
        }
        (*results)->write(TypeForClass::BlockingProbability, E, i);

        //Prawdopodobieństwo strat
        double B_n = 0;
        double B_d = 0;
        for (int n=0; n<=VsVb; n++)
        {
            double intensity = system->getClass(i)->intensityNewCallForState(1, (int)(ySYSTEM_V[i][n]));
            for (int x=0; x<qMin(t_max, t[i]); x++)
            {
                if (x+n+t[i]>VsVb)
                    B_n+=(intensity*Q_X[n][x]);
                B_d +=(intensity*Q_X[n][x]);
            }
        }
        (*results)->write(TypeForClass::LossProbability, B_n/B_d, i);

        //Średnia liczba zgłoszeń w kolejce
        double yQ = 0;
        for (int n=Vs+1; n<=VsVb; n++)
        {
            for (int x=0; x<t_max; x++)
                yQ+=(Q_X[n][x] * (ySYSTEM_V[i][n] - ySYSTEM_V[i][Vs]));
        }
        //TODO algRes->set_lQ(system->getClass(i), a, yQ);

        //Średnia liczba zajętych zasobów w kolejce
        double ytQ = 0;
        for (int n=Vs+1; n<=VsVb; n++)
        {
            for (int x=0; x<t_max; x++)
                ytQ+=(Q_X[n][x] * t[i] * (ySYSTEM_V[i][n] - ySYSTEM_V[i][Vs]));
        }
        //TODO algRes->set_ltQ(system->getClass(i), a, ytQ);

        //Średnia liczba obsługiwanych zgłoszeń
        double y = 0;
        for (int n=0; n<=VsVb; n++)
        {
            for (int x=0; x<t_max; x++)
                y+=(Q_X[n][x] * (ySYSTEM_V[i][n]));
        }
        //TODO algRes->set_lSys(system->getClass(i), a, y);

        for (int n=0; n<=Vb; n++)
            (*results)->write(TypeForClassAndBufferState::Usage, (ySYSTEM_V[i][Vs+n] - ySYSTEM_V[i][Vs]) * t[i] / n, i, n);

        for (int n=0; n<=Vs; n++)
            (*results)->write(TypeForClassAndServerState::Usage, ySYSTEM_V[i][n] *t[i] / n, i, n);

        for (int n=0; n<=VsVb; n++)
        {
            if (n > Vs)
                (*results)->write(TypeForClassAndSystemState::UsageForBuffer, (ySYSTEM_V[i][n] - ySYSTEM_V[i][Vs]) *t[i] / n, i, n);
            else
                (*results)->write(TypeForClassAndSystemState::UsageForBuffer, 0, i, n);


            (*results)->write(TypeForClassAndSystemState::UsageForSystem, ySYSTEM_V[i][n]*t[i]/n, i, n);

            if (n <= Vs)
            {
                (*results)->write(TypeForClassAndSystemState::UsageForServer, ySYSTEM_V[i][n]*t[i]/n, i, n);
            }
            else
            {
                (*results)->write(TypeForClassAndSystemState::UsageForServer, ySYSTEM_V[i][Vs]*t[i]/n, i, n);
            }
        }

    }

    //Średnia długość kolejki
    double AvgLen = 0;
    for (int n=Vs+1; n<=VsVb; n++)
    {
        for (int x=0; x<t_max; x++)
            AvgLen += (n-(Vs-x))*Q_X[n][x];
    }
    //TODO algRes->set_Qlen(a, AvgLen);

    for (int i=0; i<m; i++)
    {
        const ModelTrClass *tmpClass = system->getClass(i);
        for (int n=0; n<=VsVb; n++)
        {
            (*results)->write(TypeForClassAndSystemState::UsageForSystem, ySYSTEM_V[i][n] * tmpClass->t(), i, n);
        }
    }

    for (int n=0; n<=VsVb; n++)
    {
        double Qn = 0;
        for (int x=0; x<t_max; x++)
            Qn+=Q_X[n][x];
        //TODO algRes->resultsAS->setVal(resultsType::trDistribSystem, a, n, Qn, 0);
    }

    for (int n_s=0; n_s<=Vs; n_s++)
    {
        for (int n_b=0; n_b<=Vb; n_b++)
        {
            double val = 0;

            if (n_s <=Vs && n_b == 0)
                val = Q_X[n_s + n_b][0];
            else
            {
                int unused = n_s - Vs;
                if (n_b >= unused)
                    val = Q_X[n_s + n_b][unused];
            }
            (*results)->write(TypeForServerAngBufferState::StateProbability, val, n_s, n_b);
        }
    }

    deleteTemporaryData();

    delete []P_without_i;
    delete []p_single;

    //emit this->sigCalculationDone();
}

} // namespace Algorithms
