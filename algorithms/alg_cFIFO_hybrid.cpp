#include "alg_cFIFO_hybrid.h"

namespace Algorithms
{

/*
 * Anomalie
 * W stanie n w obszarze serwera zmieniają się y po dodaniu kolejki
 * Zatem zmieniają się prawdopodobieństwa mikrostanów
 * Suma prawdopodobieńśtw mikrostanów zachowuje proporcje podobnei jak prawdipodobieństwa makrostanóœ w obszarze serwera
 * Jest to możliwe gdy zostqnie zachowany lokalny balans. Róœnania rekurencyjne pomijają yki, brany jest pod uwagę suma yt
 * Co się dzieje, gdy różne są intensywności obsługi ?
 * Sumy prawdopodobieństw kombinacji l, n-l jest poprawna, w odniesieniu do symulacji. Podejrzenie p`[l] = xp[l] i P`[n-l] = 1/xP[n-l]. Dla FAG x = 1
 *
 *
 */
AlgorithmHybrid::AlgorithmHybrid() : Investigator(QueueServDiscipline::cFIFO), variant(algVariant::yFAG)
{
    myQoS_Set
       <<Results::Type::BlockingProbability
       <<Results::Type::OccupancyDistribution;
}

AlgorithmHybrid::AlgorithmHybrid(AlgorithmHybrid::algVariant var) : Investigator(QueueServDiscipline::cFIFO), variant(var)
{
    myQoS_Set
       <<Results::Type::BlockingProbability
       <<Results::Type::OccupancyDistribution;
}

QString AlgorithmHybrid::shortName() const
{
    QString result;
    switch (variant)
    {
    case algVariant::yFAG:
        result = "hybr cFIFO FAG";
        break;

    case algVariant::yProp:
        result = "hybr cFIFO prop";
        break;

    case algVariant::yPropPlus:
        result = "hybr cFIFO prop+";
        break;

    case algVariant::yAprox:
        result = "hybr cFIFO lin aprox ";

        result += "At alg";
        break;

    default:
        result = "hybr cont XXX";
        break;
    }
    return result;
}


void AlgorithmHybrid::calculateSystem(const ModelSyst *system
        , double a
        , RInvestigator *results
        , SimulationParameters *simParameters
        )
{
    (void) simParameters;

    prepareTemporaryData(system, a);

    int m = system->m();
    int Vs = system->vk_s();
    int Vb = system->V_b();
    int VsVb = Vs + Vb;

    p_single = new TrClVector[m];
    //p_singleQ = new trClVector[m];
    P_without_i = new TrClVector[m];

    double *delta = new double[VsVb+1];
    double *Q = new double[VsVb+1];

    for (int i=0; i<m; i++)
    {
        p_single[i] = system->getClass(i)->trDistribution(i, classes[i].A, VsVb, 0);
        //p_singleQ[i] = system->getClass(i)->trDistribution(A[i], Vs, Vb);
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

    ySystemFAG = new double*[m];
    for (int i=0; i<m; i++)
    {
        ySystemFAG[i] = new double[VsVb+1];
        p_single[i].normalize();
        P_without_i[i].normalize();

        for (int n=0; n<=VsVb; n++)
        {
            double numerator = 0;
            double denumerator = 0;
            for (int l=0; l<=n; l+= classes[i].t)
            {
                if (n-l > P_without_i[i].V())
                    continue;
                double pP = p_single[i][l] * P_without_i[i][n-l];

                numerator += ((double)(l)/(double)(classes[i].t) * pP);
                denumerator += pP;
            }
            ySystemFAG[i][n]=numerator/denumerator;
        }
    }

    double sumQ=0;
    for (int n=0; n<=Vs; n++)
    {
        delta[n] = 1;
        Q[n] = P[n];
        sumQ += Q[n];
    }

    for (int n=Vs+1; n<=VsVb; n++)
    {
        delta[n] = 0;
        for (int i=0; i<m; i++)
        {
            delta[n] += (delta[n-classes[i].t]*ySystemFAG[i][n] * classes[i].t);
        }
        delta[n] /=Vs;
        Q[n] = delta[n] * P[n];
        sumQ += Q[n];
    }

    for (int n=0; n<= VsVb; n++)
    {
        Q[n] /= sumQ;
    }

    calculateYSystem(ySYSTEM_V, ySystemFAG, system, P_without_i, p_single);
    calculateYServer(ySERVER_V, ySystemFAG, Q);
    calculateYQeue(yQEUE_VsVb, ySystemFAG, Q);


    for (int i=0; i<m; i++)
        delete []ySystemFAG[i];
    delete []ySystemFAG;

    for (int i=0; i<system->m(); i++)
    {
        //Prawdopodobieństwo blokady
        double E = 0;
        for (int n=VsVb+1-classes[i].t; n<=VsVb; n++)
        {
            E+=Q[n];
        }
        (*results)->write(TypeForClass::BlockingProbability, E, i);

        //Prawdopodobieństwo strat


        double B_n = 0;
        double B_d = 0;
        for (int n=0; n<=VsVb; n++)
        {
            double x = system->getClass(i)->intensityNewCallForState(1, (int)(ySYSTEM_V[i][n]));
            if (n > VsVb-classes[i].t)
                B_n+=(x*Q[n]);
            B_d +=(x*Q[n]);
        }
        (*results)->write(TypeForClass::LossProbability, B_n/B_d, i);

        //Średnia liczba zgłoszeń w kolejce
        double lQeue = 0;
        for (int n=Vs+1; n<=VsVb; n++)
            lQeue+=(Q[n] * yQEUE_VsVb[i][n]);
        //TODO algRes->set_lQ(system->getClass(i), a, lQeue);

        //Średnia liczba zajętych zasobów w kolejce
        double ltQeue = 0;
        for (int n=Vs+1; n<=VsVb; n++)
            ltQeue+=(Q[n] * yQEUE_VsVb[i][n] * classes[i].t);
        //TODO algRes->set_ltQ(system->getClass(i), a, ltQeue);

        //Średnia liczba obsługiwanych zgłoszeń
        double lSys = 0;
        for (int n=0; n<=VsVb; n++)
        {
            lSys+=(Q[n] * (ySYSTEM_V[i][n]));
        }
        //TODO algRes->set_lSys(system->getClass(i), a, lSys);

        //Obsługiwany ruch
        double yS = 0;

        for (int n=0; n<=Vs; n++)
        {
            yS+=(Q[n] * (ySYSTEM_V[i][n]));
        }
        for (int n=Vs+1; n<=VsVb; n++)
        {
            double tmp = Q[n] * (ySYSTEM_V[i][n]);
            yS+=((tmp *Vs)/n);
        }
        //TODO algRes->set_ServTraffic(system->getClass(i), a, yS * classes[i].t);


        //Średni czas obsługi
        //algRes->set_tService(system->getClass(i), a, lQeue / A[i] * system->getClass(i)->getMu());
        double avgToS = 0;
        for (int n=0; n<=Vs; n++)
        {
            avgToS += Q[n] * ySYSTEM_V[i][n] / system->getClass(i)->getMu();
        }
        for (int n=Vs+1; n<=VsVb; n++)
        {
            avgToS += Q[n] * ySYSTEM_V[i][n] / system->getClass(i)->getMu() * (n - (double) (classes[i].t) / 2.0) / Vs;
        }
        avgToS /= lSys;
        //TODO algRes->set_tService(system->getClass(i), a, avgToS);

        //Bezawzględny czas oczekiwania w kolejce
        double medY = 0;
        for (int n=Vs+1; n<=VsVb; n++)
        {
            medY += (yQEUE_VsVb[i][n] * Q[n]);
        }
        //double totInt = 0;
        //for (int i2=0; i2<system->m(); i2++)
        //{
        //    totInt += A[i2] * system->getClass(i2)->getMu();
        //}
        double lambda = classes[i].lambda;
        //algRes->set_tQeue(system->getClass(i), a, medY / totInt);
        //TODO algRes->set_tQeue(system->getClass(i), a, medY/lambda);


        //Efektywny czas oczekiwania w kolejce
        //TODO algRes->set_t(system->getClass(i), a, avgToS - 1.0/system->getClass(i)->getMu());


        //Średni czas obsługi zgłoszenia w serwerze
        //algRes->set_tServer(system->getClass(i), a, 1.0/system->getClass(i)->getMu());

        for (int n=0; n<=Vb; n++)
            (*results)->write(TypeForClassAndQueueState::Usage, yQEUE_VsVb[i][n+Vs] * classes[i].t, i, n);
            //algRes->resultsAS->setVal(resultsType::qeueYt_vs_q_n, a, system->getClass(i), n, yQEUE_VsVb[i][n+Vs] * classes[i].t, 0);

        for (int n=0; n<=Vs; n++)
            (*results)->write(TypeForClassAndServerState::Usage, ySERVER_V[i][n]*classes[i].t, i, n);

        for (int n=0; n<=VsVb; n++)
        {
            (*results)->write(TypeForClassAndSystemState::UsageForServer, ySERVER_V[i][n]  * classes[i].t, i, n);
            (*results)->write(TypeForClassAndSystemState::UsageForQueue,  yQEUE_VsVb[i][n] * classes[i].t, i, n);
            (*results)->write(TypeForClassAndSystemState::UsageForSystem, ySYSTEM_V[i][n]  * classes[i].t, i, n);

        //Intensywności przejść klas
            (*results)->write(TypeForClassAndSystemState::NewCallIntensityInForSystem,  P.getIntInNew(n, i), i, n);
            (*results)->write(TypeForClassAndSystemState::EndCallIntensityInForSystem,  P.getIntInEnd(n, i), i, n);

            (*results)->write(TypeForClassAndSystemState::OfferedNewCallIntensityOutForSystem, P.getIntOutNew(n, i), i, n);
            (*results)->write(TypeForClassAndSystemState::EndCallIntensityOutForSystem, P.getIntOutEnd(n, i), i, n);
        }
    }

    //Średnia długość kolejki
    double AvgLen = 0;
    for (int n=Vs+1; n<=VsVb; n++)
    {
        AvgLen += (n-Vs)*Q[n];
    }
    //TODOalgRes->set_Qlen(a, AvgLen);


    for (int n=0; n<=VsVb; n++)
    {
        //Rozkład zajętości
        (*results)->write(TypeForSystemState::StateProbability, Q[n], n);


    //Intensywności przejść (wejście do stanu)
        (*results)->write(TypeForSystemState::IntensityNewCallIn, P.getState(n).tIntInNew, n);
        (*results)->write(TypeForSystemState::IntensityEndCallIn, P.getState(n).tIntInEnd, n);

    //Intensywności przejść (wyjście ze stanu)
        (*results)->write(TypeForSystemState::IntensityNewCallOut, P.getState(n).tIntOutNew, n);
        (*results)->write(TypeForSystemState::IntensityEndCallOut, P.getState(n).tIntOutEnd, n);
    }

    deleteTemporaryData();

    delete []P_without_i;
    delete []p_single;
    //delete []p_singleQ;
    delete []delta;

    //emit this->sigCalculationDone();
}

void AlgorithmHybrid::calculateYSystem(
          double **ySYSTEM_VsVb
        , double **yFAG
        , const ModelSyst *system
        , TrClVector *PwithoutI
        , TrClVector *pI)
{
    (void) system;
    (void) PwithoutI;
    (void) pI;

    switch (variant)
    {
    case algVariant::yFAG:
        calculateYSystemFAG(ySYSTEM_VsVb, yFAG);
        break;
    case algVariant::yAprox:
        calculateYSystemApprox(ySYSTEM_VsVb, yFAG, 5);
        break;
    case algVariant::yProp:
    case algVariant::yPropPlus:
        calculateYSystemLambdaT(ySYSTEM_VsVb, yFAG);
        break;
    }
}

void AlgorithmHybrid::calculateYSystemFAG(double **ySystem, double **yFAG)
{
    for (int i=0; i<system->m(); i++)
    {
        for (int n=0; n<=system->V(); n++)
            ySystem[i][n] = yFAG[i][n];
    }
}

void AlgorithmHybrid::calculateYSystemApprox(double **ySystem, double **yFAG, int infinityFtr)
{
    double *yNfty = new double[system->m()];
    double sumAt = 0;
    for (int i=0; i<system->m(); i++)
    {
        sumAt+=(classes[i].A*classes[i].t);
    }
    double infinityVal = infinityFtr * sumAt;
    for (int i=0; i<system->m(); i++)
    {
        yNfty[i] = (double)(infinityVal) * classes[i].A/sumAt;
        for (int n=0; n<=system->vk_s(); n++)
            ySystem[i][n] = yFAG[i][n];

        for (int n=system->vk_s() + 1; n<= system->V(); n++)
            ySystem[i][n] = yFAG[i][system->vk_s()] + (yNfty[i] - yFAG[i][system->vk_s()]) * (double)(n-system->vk_s()) / (double)(infinityVal - system->vk_s());
    }
}

void AlgorithmHybrid::calculateYSystemLambdaT(double **ySystem, double **ySystemFAG)
{
    for (int i=0; i<system->m(); i++)
    {
        for (int n=0; n<=system->V(); n++)
            ySystem[i][n] = ySystemFAG[i][n];
    }
}

void AlgorithmHybrid::calculateYServer(double **ySeverVsVb, double **ySystemFAG, double *Q)
{
    switch (variant)
    {
    case algVariant::yFAG:
        calculateYServerFAG(ySeverVsVb, ySystemFAG);
        break;
    case algVariant::yAprox:
        calculateYServerFAG(ySeverVsVb, ySystemFAG);
        break;
    case algVariant::yProp:
    case algVariant::yPropPlus:
        calculateYServerPropLambdaT(ySeverVsVb, ySystemFAG, Q);
        break;
    }
}

bool AlgorithmHybrid::possible(const ModelSyst *system) const
{
    if (system->V_b() == 0)
        return false;
    return Investigator::possible(system);
}

void AlgorithmHybrid::calculateYServerFAG(double **yServerVsVb, double **ySystemFAG)
{
    for (int i=0; i < system->m(); i++)
    {
        for (int n=0; n <= system->vk_s(); n++)
        {
            yServerVsVb[i][n] = 0;
        }
        for (int n = system->vk_s() + 1; n<=system->V(); n++)
        {
            double tmp = system->vk_s();
            tmp /=n;

            yServerVsVb[i][n] = tmp * ySystemFAG[i][n];
        }
    }
}

void AlgorithmHybrid::calculateYServerPropLambdaT(double **yServerVsVb, double **ySystemFAG, double *Q)
{
    double **yQ = new double*[system->m()];
    for (int i=0; i<system->m(); i++)
        yQ[i] = new double [system->V() + 1];

    calculateYQeue(yQ, ySystemFAG, Q);

    for (int i=0; i<system->m(); i++)
    {
        for (int n=0; n < system->V(); n++)
            yServerVsVb[i][n] = ySystemFAG[i][n] - yQ[i][n];
        delete []yQ[i];
    }
    delete []yQ;
}


void AlgorithmHybrid::calculateYQeue(double **yQeueVsVb, double **ySystemFAG, double *Q)
{
    switch (variant)
    {
    case algVariant::yFAG:
        calculateYQeueFAG(yQeueVsVb, ySystemFAG);
        break;
    case algVariant::yAprox:
        calculateYQeueFAG(yQeueVsVb, ySystemFAG);
        break;
    case algVariant::yProp:
        calculateYQeuePropLambdaT(yQeueVsVb, ySystemFAG, Q, false);
        break;
    case algVariant::yPropPlus:
        calculateYQeuePropLambdaT(yQeueVsVb, ySystemFAG, Q, true);
        break;
    }
}

void AlgorithmHybrid::calculateYQeueFAG(double **yQeueVsVb, double **ySystemFAG)
{
    for (int i=0; i<system->m(); i++)
    {
        for (int n=0; n<=system->vk_s(); n++)
        {
            yQeueVsVb[i][n] = 0;
        }
        for (int n = system->vk_s() + 1; n<=system->V(); n++)
        {
            double tmp = n - system->vk_s();
            tmp /=n;

            yQeueVsVb[i][n] = tmp * ySystemFAG[i][n];
        }
    }
}

void AlgorithmHybrid::calculateYQeuePropLambdaT(double **yQeueVsVb, double **yFAG, double *Q, bool lambdaIsDependent)
{
    for (int i=0; i < system->m(); i++)
        for (int n=0; n <= system->vk_s(); n++)
            yQeueVsVb[i][n] = 0;

    for (int n=system->vk_s() + 1; n <= system->V(); n++)
    {
        double denumerator = 0;
        for (int i=0; i < system->m(); i++)
        {
            int x = qMin(classes[i].t, n-system->vk_s());
            double lambda_i;
            if (lambdaIsDependent)
            {
                double y = yFAG[i][n-classes[i].t];
                lambda_i = system->getClass(i)->intensityNewCallForY(classes[i].lambda, y);
            }
            else
                lambda_i = classes[i].lambda;

            denumerator += (x * Q[n-classes[i].t] * lambda_i);
        }

        for (int i=0; i < system->m(); i++)
        {
            int x = qMin(classes[i].t, n - system->vk_s());

            double lambda_i;
            if (lambdaIsDependent)
            {
                double y = yFAG[i][n-classes[i].t];
                lambda_i = system->getClass(i)->intensityNewCallForY(classes[i].lambda, y);
            }
            else
                lambda_i = classes[i].lambda;

            double delta = x * Q[n-classes[i].t] * lambda_i/denumerator;

            yQeueVsVb[i][n] = yQeueVsVb[i][n-1] + delta;
        }
    }

    for (int n = system->vk_s() + 1; n <= system->V(); n++)
    {
        for (int i=0; i < system->m(); i++)
            yQeueVsVb[i][n] /= classes[i].t;
    }
}
/*
Algorithm2Pass::Algorithm2Pass()
{
}

void Algorithm2Pass::calculateSystem(modelSyst *system, double a, algorithmResults *algRes, simulationParameters *simParameters)
{
    (void) simParameters;
    prepareTemporaryData(system, a);

    p_single = new trClVector[m];
    p_singleQ = new trClVector[m];
    P_without_i = new trClVector[m];

    double *delta = new double[VsVb+1];

    int *tresholds = new int[m];

    trClVector Q(VsVb);

    for (int i=0; i<m; i++)
    {
        p_single[i] = system->getClass(i)->trDistribution(A[i], VsVb, 0);
    }


    for (int i=0; i<m; i++)
    {
        for (int j=0; j<m; j++)
        {
            if (i == j)
                continue;
            P_without_i[i] = trClVector::convFAG(P_without_i[i], p_single[j], true, VsVb);
        }
    }

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
                nominator+= ((double)(l)/(double)(t[i]) * pP);
                denumerator += pP;
            }
            ySYSTEM_VsVb[i][n]=nominator/denumerator;
        }
    }

    for (int i=0; i<m; i++)
    {
        tresholds[i] = Vs;//(int)(y_VsVb[i][Vs] * t[i]);
        p_singleQ[i] = system->getClass(i)->trDistribution(A[i], tresholds[i], VsVb - tresholds[i]);
        trClVector tmp = trClVector::convFAG(p_singleQ[i], Q, true, VsVb);
        Q = tmp;
    }

    for (int i=0; i<system->m(); i++)
    {
        //Prawdopodobieństwo blokady
        double E = 0;
        for (int n=VsVb+1-t[i]; n<=VsVb; n++)
        {
            E+=Q[n];
        }
        algRes->set_Block(system->getClass(i), a, E);

        //Prawdopodobieństwo strat
        double B_n = 0;
        double B_d = 0;
        for (int n=0; n<=VsVb; n++)
        {
            double x = system->getClass(i)->intensityNewCallForState(1, (int)(ySYSTEM_VsVb[i][n]));
            if (n > VsVb-t[i])
                B_n+=(x*Q[n]);
            B_d +=(x*Q[n]);
        }
        algRes->set_Lost(system->getClass(i), a, B_n/B_d);

        //Średnia liczba zgłoszeń w kolejce
        double yQ = 0;
        for (int n=Vs+1; n<=VsVb; n++)
        {
            yQ+=(Q[n] * (ySYSTEM_VsVb[i][n] - ySYSTEM_VsVb[i][Vs]));
        }
        algRes->set_lQ(system->getClass(i), a, yQ);

        //Średnia liczba zajętych zasobów w kolejce
        double ytQ = 0;
        for (int n=Vs+1; n<=VsVb; n++)
        {
            ytQ+=(Q[n] * t[i] * (ySYSTEM_VsVb[i][n] - ySYSTEM_VsVb[i][Vs]));
        }
        algRes->set_ltQ(system->getClass(i), a, ytQ);

        //Średnia liczba obsługiwanych zgłoszeń
        double y = 0;
        for (int n=0; n<=VsVb; n++)
        {
            y+=(Q[n] * (ySYSTEM_VsVb[i][n]));
        }
        algRes->set_lSys(system->getClass(i), a, y);
    }

    //Średnia długość kolejki
    double AvgLen = 0;
    for (int n=Vs+1; n<=VsVb; n++)
    {
        AvgLen += (n-Vs)*Q[n];
    }
    algRes->set_Qlen(a, AvgLen);


    for (int i=0; i<m; i++)
    {
        modelTrClass *tmpClass = system->getClass(i);
        for (int n=0; n<=VsVb; n++)
        {
            algRes->resultsAS->setVal(resultsType::systemYt_vs_sys_n, a, tmpClass, n, ySYSTEM_VsVb[i][n], 0);
        }
    }

    deleteTemporaryData();

    delete []P_without_i;
    delete []p_single;
    delete []p_singleQ;
    delete []delta;
    delete []tresholds;
}
*/

} // namespace Algorithms
