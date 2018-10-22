#include "algorithmHybridDiscr.h"
#include "results/resultsInvestigator.h"

namespace Algorithms
{

AlgorithmHybridDiscr::AlgorithmHybridDiscr(): Investigator(QueueServDiscipline::dFIFO)
{
    myQoS_Set
       <<Results::Type::BlockingProbability
       <<Results::Type::OccupancyDistribution;
}

bool AlgorithmHybridDiscr::possible(ModelSyst *system)
{
    if (system->vk_b() == 0)
        return false;
    return Investigator::possible(system);
}

void AlgorithmHybridDiscr::calculateSystem(const ModelSyst *system
        , double a
        , RInvestigator *results
        , SimulationParameters *simParameters
        )
{
    (void) simParameters;
    int m = system->m();
    int Vs = system->vk_s();
    int Vb = system->vk_b();
    int VsVb = Vs + Vb;
    prepareTemporaryData(system, a);

    p_single = new TrClVector[m];
    P_without_i = new TrClVector[m];

    state **unusedProb;
    unusedProb = new state*[VsVb+1];


    double *delta = new double[VsVb+1];
    double **deltaDetail = new double*[VsVb+1];

    state *Q = new state[VsVb+1];
    state **Qdetail = new state*[VsVb+1];

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
         unusedProb[n] = new state[t_max];
         Qdetail[n] = new state[t_max];
         bzero(unusedProb[n], t_max*sizeof(state));
         bzero(Qdetail[n], t_max*sizeof(state));
         if (n<=Vs)
             unusedProb[n][0].p = 1;

         deltaDetail[n] = new double[t_max];
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
            unusedProb[n][x].p=0;

            if (x > n-Vs)
                continue;

            for (int i=0; i<m; i++)
            {
                if (x>=t[i])
                    continue;
                unusedProb[n][x].p+=ySYSTEM_V[i][n];

            }
            sum +=unusedProb[n][x].p;
        }

        if (sum == 0)
        {
            unusedProb[n][0].p = 1;
            for (int x=1; x<t_max; x++)
                unusedProb[n][x].p = 0;
        }
        else
            for (int x=0; x<t_max; x++)
                unusedProb[n][x].p /=sum;
    }

    double sumQ=0;
    for (int n=0; n<=Vs; n++)
    {
        delta[n] = 1;
        Q[n] = P.getState(n);
        Qdetail[n][0] = P.getState(n);
        sumQ += Q[n].p;

        for (int x=0; x<t_max; x++)
            deltaDetail[n][x] = 1;

        Qdetail[n][0] = Q[n];
    }

    for (int n=Vs+1; n<=VsVb; n++)
    {
        delta[n] = 0;
        for (int i=0; i<m; i++)
        {
            delta[n] += (delta[n-t[i]]*ySYSTEM_V[i][n] * t[i]);
        }
        delta[n] /=Vs;
        Q[n].p = 0;//delta[n] * P[n];
        //sumQ += Q[n];

        for (int x=0; x<t_max; x++)
        {
            deltaDetail[n][x] = 0;
            for (int i=0; i<m; i++)
                deltaDetail[n][x] += (deltaDetail[n-t[i]][x]*ySYSTEM_V[i][n] * t[i]);

            deltaDetail[n][x] /=(Vs-x);
            if (n+x<=VsVb)
                Qdetail[n][x].p = deltaDetail[n][x] * P[n] * unusedProb[n][x].p;
            else
                Qdetail[n][x].p = 0;

            sumQ += Qdetail[n][x].p;
            Q[n].p+=Qdetail[n][x].p;
        }
    }

    for (int n=0; n<= VsVb; n++)
    {
        Q[n].p /= sumQ;
        for (int x=0; x<t_max; x++)
            Qdetail[n][x].p /= sumQ;
    }

    for (int i=0; i<system->m(); i++)
    {
        //Prawdopodobieństwo blokady
        double E = 0;
        for (int n=VsVb-(t[i]-1)-(t_max-1); n<=VsVb; n++)
        {
            for (int x=0; x<t_max; x++)
            {
                if (x+n+t[i]>VsVb)
                    E+=Qdetail[n][x].p;
            }
        }
        (*results)->write(TypeForClass::BlockingProbability, E, i);

        //Prawdopodobieństwo strat
        double B_n = 0;
        double B_d = 0;
        for (int n=0; n<=VsVb; n++)
        {
            double intensity = system->getClass(i)->intensityNewCallForState(1, (int)(ySYSTEM_V[i][n]));
            for (int x=0; x<t_max; x++)
            {
                if (n + x + t[i] > VsVb)
                    B_n+=(intensity*Qdetail[n][x].p);
                B_d +=(intensity*Qdetail[n][x].p);
            }
        }
        (*results)->write(TypeForClass::LossProbability, B_n/B_d, i);

        //Średnia liczba zgłoszeń w kolejce
        double yQ = 0;
        for (int n=Vs+1; n<=VsVb; n++)
        {
            yQ+=(Q[n].p * (ySYSTEM_V[i][n] - ySYSTEM_V[i][Vs]));
        }
        (*results)->write(TypeForClass::AvarageNumbersOfCallsInBuffer, yQ, i);


        //Średnia liczba obsługiwanych zgłoszeń
        double y = 0;
        for (int n=0; n<=VsVb; n++)
        {
            y+=(Q[n].p * (ySYSTEM_V[i][n]));
        }
        (*results)->write(TypeForClass::AvarageNumbersOfCallsInSystem, y, i);

        //TODO algRes->set_lSys(system->getClass(i), a, y);
    }

    //Średnia długość kolejki
    double AvgLen = 0;
    for (int n=Vs+1; n<=VsVb; n++)
    {
        AvgLen += (n-Vs)*Q[n].p;
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

    //TODO for (int n=0; n<=VsVb; n++)
    //    algRes->resultsAS->setVal(resultsType::trDistribSystem, a, n, Q[n].p, 0);

    for (int n_s=0; n_s<=Vs; n_s++)
    {
        for (int n_b=0; n_b<=Vb; n_b++)
        {
            double val = 0;

            if (n_s <=Vs && n_b == 0)
                val = Qdetail[n_s + n_b][0].p;
            else
            {
                int unused = n_s - Vs;
                if (n_b >= unused)
                    val = Qdetail[n_s + n_b][unused].p;
            }
            (*results)->write(TypeForServerAngBufferState::StateProbability, val, n_s, n_b);
        }
    }

    deleteTemporaryData();

    delete []P_without_i;
    delete []p_single;
    delete []delta;
    //emit this->sigCalculationDone();
}

} // namespace Algorithms
