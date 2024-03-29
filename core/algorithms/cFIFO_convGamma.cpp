#include "cFIFO_convGamma.h"

#include "results/resultsInvestigator.h"

namespace Algorithms
{

cFIFO_convGamma::cFIFO_convGamma(): Investigator()
{
    myQoS_Set
       <<Results::Type::BlockingProbability
       <<Results::Type::OccupancyDistribution;
}

bool cFIFO_convGamma::possible(const ModelSystem &system) const
{
    if (system.getBuffer().V() == 0)
        return false;
    return Investigator::possible(system);
}

void cFIFO_convGamma::calculateSystem(const ModelSystem &system
      , double a
      , RInvestigator *results
      , SimulationParameters *simParameters
      )
{
    (void) simParameters;

    prepareTemporaryData(system, a);

    for (int i=0; i<system.m(); i++)
    {
        p_single[i] = new VectQEUE(system.getServer().V(), system.getBuffer().V(), 1, i, system.getTrClass(i));
        TrClVector tmp = system.getTrClass(i).trDistribution(i, classes[i].A, system.getServer().V(), system.getBuffer().V());

        p_single[i]->setStates(tmp);
    }
    VectQEUE *FD = new VectQEUE();

    for (int i=0; i<system.m(); i++)
    {
        VectQEUE *FD2 = new VectQEUE();
        FD2->agregate(FD, p_single[i]);
        delete FD;
        FD = FD2;
    }

    QString msgTmp = "Final distribution: ";
    for (int n=0; n <= system.V(); n++)
        msgTmp.append(QString(" %1").arg(FD->getState(n)));

    qDebug()<<msgTmp;


    for (int i=0; i < system.m(); i++)
    {
        //Prawdopodobieństwo blokady
        double E = 0;
        for (int n=system.V() + 1 - classes[i].t; n <= system.V(); n++)
        {
            E+=FD->getState(n);
        }
        (*results)->write(TypeForClass::BlockingProbability, E, i);


        //Prawdopodobieństwo strat
        double B_n = 0;
        double B_d = 0;
        for (int n=0; n <= system.V(); n++)
        {
            double x = system.getTrClass(i).intensityNewCallForState(1, static_cast<int>(FD->get_y(i, n)));
            if (n > system.V() - classes[i].t)
                B_n+=(x*FD->getState(n));
            B_d +=(x*FD->getState(n));
        }
        (*results)->write(TypeForClass::LossProbability, B_n/B_d, i);

        //Obsługiwany ruch
        double yS = 0;

        for (int n=0; n <= system.getServer().V(); n++)
        {
            yS+=(FD->getState(n) * FD->get_y(i, n));
        }
        for (int n=system.getServer().V() + 1; n <= system.V(); n++)
        {
            double tmp = FD->getState(n) * FD->get_y(i, n);
            yS+=(tmp * system.getServer().V()/n);
        }
        //TODO algResults->set_ServTraffic(system->getClass(i), a, yS * classes[i].t);

        //Średnia liczba zgłoszeń w kolejce
        double lQeue = 0;
        for (int n=system.getServer().V() + 1; n<=system.V(); n++)
        {
            double tmp = static_cast<double>(n - system.getServer().V()) / static_cast<double>(n);
            lQeue+=(FD->getState(n) * FD->get_y(i, n) * tmp);
        }
        //TODO algResults->set_lQ(system->getClass(i), a, lQeue);

        //Średnia liczba zajętych zasobów w kolejce
        double ltQeue = 0;
        for (int n = system.getServer().V() + 1; n <= system.V(); n++)
        {
            double tmp = (double)(n - system.getServer().V()) / static_cast<double>(n);
            ltQeue+=(FD->getState(n) * FD->get_y(i, n) * tmp * classes[i].t);
        }
        //TODO algResults->set_ltQ(system->getClass(i), a, ltQeue);


        //Średnia liczba obsługiwanych zgłoszeń
        double y = 0;
        for (int n=0; n <= system.V(); n++)
        {
            y+=(FD->getState(n) * FD->get_y(i, n));
        }
        //TODO algResults->set_lSys(system->getClass(i), a, y);

        //Średni czas obsługi
        double avgToS = 0;
        for (int n=0; n <= system.getServer().V(); n++)
        {
            avgToS += FD->getState(n) * FD->get_y(i, n) / classes[i].mu;
        }
        for (int n = system.getServer().V() + 1; n <= system.V(); n++)
        {
            avgToS += FD->getState(n) * FD->get_y(i, n) / classes[i].mu * (n - (double) (classes[i].t-1) / 2.0) / system.getServer().V();
        }
        avgToS /= y;
        //TODO algResults->set_tService(system->getClass(i), a, avgToS);

        //Efektywny czas oczekiwania w kolejce
        //TODO algResults->set_t(system->getClass(i), a, avgToS - 1.0/system->getClass(i)->getMu());


    }
    //Średnia długość kolejki
    double AvgLen = 0;
    for (int n = system.getServer().V() + 1; n <= system.V(); n++)
        AvgLen += ((n-system.getServer().V())*FD->getState(n));
    //TODO algResults->set_Qlen(a, AvgLen);

    for (int i=0; i < system.m(); i++)
    {
        for (int n=0; n<=system.V(); n++)
            (*results)->write(TypeForClassAndSystemState::UsageForSystem, FD->get_y(i, n) * classes[i].t, i, n);

//TODO        for (int n=0; n <= system->V_b(); n++)
//            (*results)->write(TypeForClassAndQueueState, FD->get_y(i, n) * tmpClass->t(), i, n);
//            algResults->resultsAS->setVal(resultsType::qeueYt_vs_q_n, a, system->getClass(i), n, classes[i].t * (FD->get_y(i, n) * n/(system->V_s() + n)), 0);

        for (int n=0; n <= system.getServer().V(); n++)
            (*results)->write(TypeForClassAndServerState::Usage, FD->get_y(i, n) * classes[i].t, i, n);

        for (int n=0; n <= system.V(); n++)
        {
            if (n > system.getServer().V())
                (*results)->write(TypeForClassAndSystemState::UsageForBuffer, classes[i].t * (FD->get_y(i, n) *(n-system.getServer().V())/n), i, n);
            else
                (*results)->write(TypeForClassAndSystemState::UsageForBuffer, 0, i, n);

            (*results)->write(TypeForClassAndSystemState::UsageForSystem, classes[i].t * (FD->get_y(i, n)), i, n);


            if (n <= system.getServer().V())
            {
                (*results)->write(TypeForClassAndSystemState::UsageForServer, FD->get_y(i, n) * classes[i].t, i, n);
            }
            else
            {
                (*results)->write(TypeForClassAndSystemState::UsageForServer, FD->get_y(i, n) * classes[i].t * system.getServer().V()/n, i, n);
            }
        }
    }

    for (int n=0; n <= system.V(); n++)
        (*results)->write(TypeForSystemState::StateProbability, FD->getState(n), n);

    deleteTemporaryData();
    //emit this->sigCalculationDone();
}

void cFIFO_convGamma::prepareTemporaryData(const ModelSystem &system, double a)
{
    Investigator::prepareTemporaryData(system, a);
    this->p_single.fill(nullptr, system.m());
}

void cFIFO_convGamma::deleteTemporaryData()
{
    Investigator::deleteTemporaryData();

    for (int i=0; i < system->m(); i++)
        delete p_single[i];
    p_single.clear();
}


cFIFO_convGamma::VectQEUE::VectQEUE(): Vs(0), Vb(0), VsVb(0), m(0)
{
    trClasses2.clear(); loc2globIdx.clear(); ySYSTEM.clear();
    states.clear();
    states.fill(1, 1);
}

cFIFO_convGamma::VectQEUE::VectQEUE(int Vs, int Vb, int m, int i, const ModelTrClass &trClass): Vs(Vs), Vb(Vb), VsVb(Vs+Vb), m(m)
{
    loc2globIdx.fill(0, 1);
    loc2globIdx[0] = i;
    trClasses2.clear();
    trClasses2.fill(trClass, 1);

    states.fill(0, Vs+Vb+1);
    states[0] = 1;

    ySYSTEM.clear();
    ySYSTEM.fill(QVector<double>(), 1);
    ySYSTEM[0].fill(0, VsVb+1);

    for (int n=0; n<=VsVb; n+=trClass.t())
        ySYSTEM[0][n] = static_cast<double>(n)/static_cast<double>(trClass.t());
}

cFIFO_convGamma::VectQEUE::~VectQEUE()
{
    ySYSTEM.clear();
    loc2globIdx.clear();
    states.clear();
    trClasses2.clear();
}

void cFIFO_convGamma::VectQEUE::setStates(TrClVector &src)
{
    if (VsVb != src.V())
        qFatal("Wrong distribution's length");

    for (int n=0; n<=VsVb; n++)
        states[n] = src[n];
}

double cFIFO_convGamma::VectQEUE::get_y(int globIdx, int n) const
{
    int classLocIdx = -1;
    int i;
    for (i=0; i<m; i++)
    {
        if (loc2globIdx[i] == globIdx)
        {
            classLocIdx = i;
            break;
        }
    }
    if (classLocIdx == -1)
        return 0;
    return ySYSTEM[classLocIdx][n];
}

double cFIFO_convGamma::VectQEUE::getState(int n)
{
    return states[n];
}

void cFIFO_convGamma::VectQEUE::fillGamma(double **gamma, double **gammaOther, int rowNo) const
{
    if (rowNo <=Vs)
    {
        for (int l = 0; l<=rowNo; l++)
            gamma[rowNo][l] = 1;

        return;
    }

    qDebug("Determining Gamma for distribution with classes:");

    for (int i=0; i<m; i++)
    {
        QString yStr = QString();
        for (int n=0; n<=VsVb; n++)
            yStr.append(QString(" %1").arg(ySYSTEM[i][n], 3, 'g', 2));
        qDebug()<<"\tClass"<<loc2globIdx[i]<<"t ="<<trClasses2[i].t()<<"\t"<<yStr;
    }

    QString msgTmp = QString("Gamma (%1) = 1").arg(rowNo);

    gamma[rowNo][0] = 1;
    for (int l=1; l<=rowNo; l++)
    {
        gamma[rowNo][l] = 0;
        double sumYt = 0;

        for (int i=0; i<m; i++)
        {
            int t = trClasses2[i].t();

            if (t > l)
                continue;

            int prevLc = l-t;
            int prevLd = rowNo-l;
            int prev_n = rowNo - t;

            double prevGamma =  static_cast<double>(prevLc * gamma[prev_n][prevLc] + prevLd * gammaOther[prev_n][prevLd])/static_cast<double>(prev_n);

            double sigmaN = qMax(1.0, static_cast<double>(rowNo)/Vs);
            double sigmaD = qMax(1.0, static_cast<double>(l)/Vs);

            double y = ySYSTEM[i][l];

            sumYt += (y*t);

            gamma[rowNo][l]+= (prevGamma * y * t * sigmaN / sigmaD);

            if (std::isnan(gamma[rowNo][l]))
                qFatal("Wrong gamma");
        }
        if (sumYt>0)
        {
            gamma[rowNo][l] /=l;
            if ((sumYt < l-0.01) || (sumYt > l+0.01))
                qFatal("Wrong sum yt");
        }
        else
            gamma[rowNo][l] = 0;

        msgTmp.append(QString(" %1").arg(gamma[rowNo][l]));
        if (std::isnan(gamma[rowNo][l]))
            qFatal("Wrong gamma");
    }
    qDebug()<<msgTmp;
}

void cFIFO_convGamma::VectQEUE::agregate(const VectQEUE *A, const cFIFO_convGamma::VectQEUE *B)
{
    if (A->m == 0)
    {
        clone(B);
        return;
    }

    if (B->m == 0)
    {
        clone(A);
        return;
    }

    m = A->m + B->m;
    Vs = A->Vs;
    Vb = A->Vb;
    VsVb = Vs+Vb;

    loc2globIdx.fill(0, m);

    for (int i=0; i<A->m; i++)
    {
        trClasses2[i] = A->trClasses2[i];
        loc2globIdx[i] = A->loc2globIdx[i];
    }
    for (int i=0; i<B->m; i++)
    {
        trClasses2[i+A->m] = B->trClasses2[i];
        loc2globIdx[i+A->m] = B->loc2globIdx[i];
    }

    double **gammaA = new double*[VsVb+1];
    double **gammaB = new double*[VsVb+1];

    for (int n=0; n<=VsVb; n++)
    {
        gammaA[n] = new double[n+1];
        gammaB[n] = new double[n+1];
    }

    for (int n=0; n<=VsVb; n++)
    {
        A->fillGamma(gammaA, gammaB, n);
        B->fillGamma(gammaB, gammaA, n);
    }

    states.clear();
    states.fill(0, VsVb+1);

    double sum = 0;
    for (int n=0; n<=VsVb; n++)
    {
        states[n] = 0;
        for (int Lc=0; Lc<=n; Lc++)
        {
            double gA = gammaA[n][Lc];
            double gB = gammaB[n][n-Lc];
            double gamma = (n == 0) ? 1 : (Lc*gA + (n-Lc)*gB)/n;
            if (std::isnan(gamma))
                qFatal("Wrong gamma");
            states[n] += gamma * A->states[Lc] * B->states[n-Lc];
        }
        sum += states[n];
    }
    for (int n=0; n<=VsVb; n++)
    {
        if (std::isnan(sum))
            qFatal("Wrong sum");

        if (sum == 0)
            qFatal("Sum = 0");
        states[n] /= sum;
    }
    sum=1;

    //Calculate Y
    ySYSTEM.clear();
    ySYSTEM.fill(QVector<double>(), m);
    for (int i=0; i<m; i++)
    {
        ySYSTEM[i].fill(0, VsVb+1);
    }

    for (int i=0; i<m; i++)
    {
        int classGlobIdx = loc2globIdx[i];

        ySYSTEM[i][0] = 0;

        for (int n=1; n<=VsVb; n++)
        {
            ySYSTEM[i][n] = 0;
            double sum = 0;
            for (int l=0; l<=n; l++)
            {
                double y = A->get_y(classGlobIdx, l) + B->get_y(classGlobIdx, n-l);

                double gamma = (double)(l*gammaA[n][l] + (n-l)*gammaB[n][n-l])/(n);

                ySYSTEM[i][n] += gamma * y * A->states[l] * B->states[n-l];
                sum += gamma * A->states[l] * B->states[n-l];

                if (std::isnan(ySYSTEM[i][n]))
                    qFatal("Wrong Y");
            }

            if (ySYSTEM[i][n] != 0)
                ySYSTEM[i][n] = ySYSTEM[i][n]/ sum;
            if (std::isnan(ySYSTEM[i][n]))
            {
                QString strGamma = QString("Wrong ySYSTEM[%1][%2]/%3").arg(i).arg(n).arg(sum);

                QString *strY;
                strY = new QString[A->m + B->m];
                for (int j=0; j<A->m; j++)
                    strY[j] = QString("yA %1 (%2):").arg(j).arg(A->loc2globIdx[j]);

                for (int j=0; j<B->m; j++)
                    strY[A->m + j] = QString("yB %1 (%2):").arg(j).arg(B->loc2globIdx[j]);

                for (int l=0; l<=VsVb; l++)
                {
                    for (int j=0; j<A->m; j++)
                        strY[j].append(QString(" %1").arg(A->ySYSTEM[j][l], 5, 'g', 2));

                    for (int j=0; j<B->m; j++)
                        strY[A->m+j].append(QString(" %1").arg(B->ySYSTEM[j][l], 5, 'g', 2));
                }

                for (int l=0; l<=n; l++)
                {
                    double gamma = static_cast<double>(l*gammaA[n][l] + (n-l)*gammaB[n][n-l])/(n);
                    strGamma.append(QString(" %L1").arg(gamma, 3, 'g', 2));
                }
                qDebug()<<strGamma<<"\n";
                for (int j=0; j<m; j++)
                    qDebug()<<strY[j];

                sum = 0;
                for (int l=0; l<=n; l++)
                {
                    double y = A->get_y(classGlobIdx, l) + B->get_y(classGlobIdx, n-l);

                    double gamma = static_cast<double>(l*gammaA[n][l] + (n-l)*gammaB[n][n-l])/(n);
                    sum += gamma * y * A->states[l] * B->states[n-l];

                    QString iterData = QString("y = %1 gamma = %2, A = %3 B=%4 sum = %5").arg(y).arg(gamma).arg(A->states[l]).arg(B->states[n-l]).arg(sum);
                    qDebug()<<iterData;
                }

                qFatal("Wrong Y");
            }
        }
    }
    for (int n=0; n<=VsVb; n++)
    {
        delete []gammaA[n];
        delete []gammaB[n];
    }
    delete []gammaA;
    delete []gammaB;
}

void cFIFO_convGamma::VectQEUE::clone(const cFIFO_convGamma::VectQEUE *A)
{
    Vs   = A->Vs;
    Vb   = A->Vb;
    VsVb = A->VsVb;
    m    = A->m;

    loc2globIdx = A->loc2globIdx;
    trClasses2  = A->trClasses2;
    states      = A->states;
    ySYSTEM     = A->ySYSTEM;
    /*ySYSTEM.clear();
    ySYSTEM.fill(QVector<double>(), m);
    for (int i=0; i<m; i++)
    {
        ySYSTEM[i] = A->ySYSTEM[i];
    } */
}

} //namespace Algorithms
