#include "conv.h"
#include "algorithms/trclvector2.h"

/*
convolutionAlgorithm1::convolutionAlgorithm1() : algorithm(queueServDiscipline::cFIFO)
{

}

bool convolutionAlgorithm1::possible(modelSyst *system)
{
    if (system->V_q() == 0)
        return false;
    return algorithm::possible(system);
}

void convolutionAlgorithm1::calculateSystem(modelSyst *system, double a, algorithmResults *algRes, simulationParameters *simParameters)
{
    (void) simParameters;
    prepareTemporaryData(system, a);

    p_single = new trClVector[m];
    P_without_i = new trClVector[m];

    for (int i=0; i<m; i++)
        p_single[i] = system->getClass(i)->trDistribution(i, A[i], Vs, Vb);

    trClVector P(VsVb);

    for (int i=0; i<m; i++)
    {
        for (int j=0; j<m; j++)
        {
            if (i == j)
                continue;
            P_without_i[i] = trClVector::convQUEUE(P_without_i[i], p_single[j], Vs, true, VsVb);
        }
        trClVector tmp = trClVector::convQUEUE(P, p_single[i], Vs, true, VsVb);
        P = tmp;
    }

    for (int i=0; i<m; i++)
    {
        p_single[i].normalize();
        P_without_i[i].normalize();

        int t = system->getClass(i)->t();
        for (int n=0; n<=VsVb; n++)
        {
            double nominator = 0;
            double denumerator = 0;
            for (int l=0; l<=n; l+= t)
            {
                state pP;
                pP.p = 0;// p_single[i][l] * trClVectorDetail::QEUE2FAG(p_single[i][l], l, Vs) * P_without_i[i][n-l] * trClVectorDetail::QEUE2FAG(P_without_i[i][n-l], n-l, Vs);
                nominator += ((double)(l)/(double)(t) * pP.p);
                denumerator += pP.p;
            }
            ySYSTEM_VsVb[i][n]=nominator/denumerator;
        }
    }

    double AvgLen = 0;
    for (int i=0; i<system->m(); i++)
    {
        //Prawdopodobieństwo blokady
        double E = 0;
        for (int n=VsVb+1-t[i]; n<=VsVb; n++)
        {
            E+=P[n];
        }
        algRes->set_Block(system->getClass(i), a, E);

        //Prawdopodobieństwo strat
        double B_n = 0;
        double B_d = 0;
        for (int n=0; n<=VsVb; n++)
        {
            double x = system->getClass(i)->intensityNewCallForState(1, (int)(ySYSTEM_VsVb[i][n]));
            if (n > VsVb-t[i])
                B_n+=(x*P[n]);
            B_d +=(x*P[n]);
        }
        algRes->set_Lost(system->getClass(i), a, B_n/B_d);

        //Średnia liczba zgłoszeń w kolejce
        double yQ = 0;
        for (int n=Vs+1; n<=VsVb; n++)
        {
            yQ+=(P[n] * (ySYSTEM_VsVb[i][n] - ySYSTEM_VsVb[i][Vs]));
        }
        algRes->set_lQ(system->getClass(i), a, yQ);

        //Średnia długość kolejki
        AvgLen += (system->getClass(i)->t()*yQ);

        //Średnia liczba zajętych zasobów w kolejce
        double ytQ = 0;
        for (int n=Vs+1; n<=VsVb; n++)
        {
            ytQ+=(P[n] * t[i] * (ySYSTEM_VsVb[i][n] - ySYSTEM_VsVb[i][Vs]));
        }
        algRes->set_ltQ(system->getClass(i), a, ytQ);

        //Średnia liczba obsługiwanych zgłoszeń
        double y = 0;
        for (int n=0; n<=VsVb; n++)
        {
            y+=(P[n] * (ySYSTEM_VsVb[i][n]));
        }
        algRes->set_lSys(system->getClass(i), a, y);
    }
    algRes->set_Qlen(a, AvgLen);

    deleteTemporaryData();

    delete []P_without_i;
    delete []p_single;
}


convolutionAlgorithm2::convolutionAlgorithm2(): algorithm(queueServDiscipline::cFIFO)
{
}


void convolutionAlgorithm2::calculateSystem(modelSyst *system, double a, algorithmResults *algRes, simulationParameters *simParameters)
{
    (void) simParameters;
    prepareTemporaryData(system, a);
    p_single = new trClVector[m];
    P_without_i = new trClVector[m];

    for (int i=0; i<m; i++)
        p_single[i] = system->getClass(i)->trDistribution(i, A[i], Vs, Vb);

    trClVector P(VsVb);

    for (int i=0; i<m; i++)
    {
        for (int j=0; j<m; j++)
        {
            if (i == j)
                continue;
            P_without_i[i] = trClVector::convQUEUE(P_without_i[i], p_single[j], Vs, true, VsVb);
        }
        trClVector tmp = trClVector::convQUEUE(P, p_single[i], Vs, true, VsVb);
        P = tmp;
    }

    for (int i=0; i<m; i++)
    {
        p_single[i].normalize();
        P_without_i[i].normalize();

        int t = system->getClass(i)->t();
        for (int n=0; n<=VsVb; n++)
        {
            double nominator = 0;
            double denumerator = 0;

            for (int l=0; l<=n; l+= t)
            {
                double pP = 0;//p_single[i][l] * trClVector::QEUE2FAG(p_single[i][l], l, Vs) *
                        //P_without_i[i][n-l] * trClVector::QEUE2FAG(P_without_i[i][n-l], n-l, Vs);
                nominator += ((double)(l)/(double)(t) * pP);
                denumerator += pP;
            }
            ySYSTEM_VsVb[i][n]=nominator/denumerator;
        }


        for (int n=Vs+1; n<=VsVb; n++)
        {
            double numerator = 0;
            double denumerator = 0;
            for (int l=0; l<=n; l+= t)
            {
                double pP = 0; //p_single[i][l] * trClVector::QEUE2FAG(p_single[i][l], l, Vs) *
                        //P_without_i[i][n-l] * trClVector::QEUE2FAG(P_without_i[i][n-l], n-l, Vs);
                numerator += (pP  * (double)(l)/(double)(t) * (double)(n-Vs)/double(Vs));
                denumerator += pP;
            }
            yQEUE_Vb[i][n-Vs] = numerator/denumerator;
        }
    }

    double AvgLen = 0;
    for (int i=0; i<system->m(); i++)
    {
        //Prawdopodobieństwo blokady
        double E = 0;
        for (int n=VsVb+1-t[i]; n<=VsVb; n++)
        {
            E+=P[n];
        }
        algRes->set_Block(system->getClass(i), a, E);

        //Prawdopodobieństwo strat
        double B_n = 0;
        double B_d = 0;
        for (int n=0; n<=VsVb; n++)
        {
            double x = system->getClass(i)->intensityNewCallForState(1, (int)(ySYSTEM_VsVb[i][n]));
            if (n > VsVb-t[i])
                B_n+=(x*P[n]);
            B_d +=(x*P[n]);
        }
        algRes->set_Lost(system->getClass(i), a, B_n/B_d);

        //Średnia liczba zgłoszeń w kolejce
        double yQ = 0;
        for (int n=Vs+1; n<=VsVb; n++)
        {
            yQ+=(P[n] * (yQEUE_Vb[i][n-Vs]));
        }
        algRes->set_lQ(system->getClass(i), a, yQ);

        //Średnia długość kolejki
        AvgLen += (system->getClass(i)->t()*yQ);

        //Średnia liczba zajętych zasobów w kolejce
        double ytQ = 0;
        for (int n=Vs+1; n<=VsVb; n++)
        {
            ytQ+=(P[n] * t[i] * yQEUE_Vb[i][n-Vs]);
        }
        algRes->set_ltQ(system->getClass(i), a, ytQ);

        //Średnia liczba obsługiwanych zgłoszeń
        double y = 0;
        for (int n=0; n<=VsVb; n++)
        {
            y+=(P[n] * (ySYSTEM_VsVb[i][n]));
        }
        algRes->set_lSys(system->getClass(i), a, y);
    }
    algRes->set_Qlen(a, AvgLen);

    deleteTemporaryData();
}

void convolutionAlgorithm2::deleteTemporaryData()
{
    algorithm::deleteTemporaryData();
    delete []P_without_i;
    delete []p_single;
}
*/
