#include "algorithmConvGamma2d.h"


/*
convolutionAlgorithmGamma2d::convolutionAlgorithmGamma2d()
{
}

void convolutionAlgorithmGamma2d::calculateSystem(modelSyst *system, double a, algorithmResults *algRes, simulationParameters *simParameters)
{
    (void) simParameters;

    prepareTemporaryData(system, a);

    for (int i=0; i<m; i++)
        p_single[i] = new VectQEUE(Vs, Vb, 1, i, system->getClass(i), A[i]);

    VectQEUE *FD = new VectQEUE();

    for (int i=0; i<m; i++)
    {
        VectQEUE *FD2 = new VectQEUE();
        FD2->agregate(FD, p_single[i]);
        delete FD;
        FD = FD2;
    }

    for (int i=0; i<system->m(); i++)
    {
        //Prawdopodobieństwo blokady
        double E = 0;

        for (int y=Vb+1-t[i]; y<=Vb; y++)
        {
            for (int n=0; n<=Vs; n++)
            {
                E+=FD->getState(n, y);
            }
        }
        algRes->set_Block(system->getClass(i), a, E);

        //Ruch obsługiwany
        double Y = A[i]*(1-E)*t[i];
        algRes->set_ServTraffic(system->getClass(i), a, Y);

        //Prawdopodobieństwo strat
        double B_n = 0;
        double B_d = 0;

        for (int y=Vb+1-t[i]; y<=Vb; y++)
        {
            for (int x=0; x<=Vs; x++)
            {
                int n = x + y;
                double intensityx = system->getClass(i)->intensityNewCallForState(1, (int)(FD->get_y(i, n)));

                if (y > Vb-t[i])
                    B_n+=(intensityx*FD->getState(x, y));
                B_d +=(intensityx*FD->getState(x, y));

            }
        }
        algRes->set_Lost(system->getClass(i), a, B_n/B_d);

        //Średnia liczba zgłoszeń w kolejce
        //double yQ = 0;
        //for (int n=Vs+1; n<=VsVb; n++)
        //{
        //    yQ+=(FD->getState(n) * ());
        //}
        //algRes->set_yQ(system->getClass(i), a, yQ);

        //Średnia liczba zajętych zasobów w kolejce
        //double ytQ = 0;
        //for (int n=Vs+1; n<=VsVb; n++)
        //{
        //    ytQ+=(P[n] * t[i] * (ySYSTEM_VsVb[i][n] - ySYSTEM_VsVb[i][Vs]));
        //}
        //algRes->set_ytQ(system->getClass(i), a, ytQ);

        //Średnia liczba obsługiwanych zgłoszeń
        double y = 0;
        for (int x=0; x<=Vs; x++)
        {
            for (int y=0; y<=Vb; y++)
            {
                y+=(FD->getState(x, y) * FD->get_y(i, x+y));
            }
        }
        algRes->set_lSys(system->getClass(i), a, y);
    }
    //Średnia długość kolejki
    double AvgLen = 0;
    for (int y=1; y<=Vb; y++)
        for (int x=0; x<=Vs; x++)
            AvgLen += ((y)*FD->getState(x, y));
    algRes->set_Qlen(a, AvgLen);

    for (int i=0; i<m; i++)
    {
        modelTrClass *tmpClass = system->getClass(i);
        for (int n=0; n<=VsVb; n++)
        {
            algRes->resultsAS->setVal(resultsType::systemYt_vs_sys_n, a, tmpClass, n, FD->get_y(i, n) * tmpClass->t(), 0);
        }
    }

    for (int x=0; x<=Vs; x++)
        for (int y=0; y<=Vb; y++)
            algRes->resultsAS->setVal(resultsType::trDistribSystem, a, x+y, FD->getState(x, y), 0);

    deleteTemporaryData();
}

void convolutionAlgorithmGamma2d::prepareTemporaryData(modelSyst *system, double a)
{
    algorithm::prepareTemporaryData(system, a);
    this->p_single = new VectQEUE*[m];
}

void convolutionAlgorithmGamma2d::deleteTemporaryData()
{
    algorithm::deleteTemporaryData();

    for (int i=0; i<m; i++)
        delete p_single[i];
    delete []p_single;
}


convolutionAlgorithmGamma2d::VectQEUE::VectQEUE(int Vs, int Vb, int m, int i, modelTrClass *trClass, double A): Vs(Vs), Vb(Vb), VsVb(Vs+Vb), m(m), loc2globIdx(NULL), ySYSTEM(NULL)
{
    loc2globIdx = new int[1];
    loc2globIdx[0] = i;
    trClasses = new modelTrClass*[1];
    trClasses[0] = trClass;

    states = new state*[Vs+1];
    for (int q=0; q<=Vb; q++)
    {
        states[q] = new state[Vb+1];
        bzero(states[q], sizeof(state)*(Vb+1));
    }


    QVector<state>singleStates = trClass->trDistributionSimEna(A, Vs, Vb);
    int x = 0;
    int y = 0;
    while (y <= Vb)
    {
        states[x][y] = singleStates[x+y];
        x += trClass->t();
        if (x > Vs)
        {
            y += trClass->t();
            x -= trClass->t();
        }
    }


    ySYSTEM = new double*[1];

    ySYSTEM[0] = new double[VsVb+1];
    bzero(ySYSTEM[0], (VsVb+1) * sizeof(double));
    for (int n=0; n<=VsVb; n+=trClass->t())
        ySYSTEM[0][n] = (double)(n)/(double)(trClass->t());
}

convolutionAlgorithmGamma2d::VectQEUE::~VectQEUE()
{
    if (ySYSTEM != NULL)
    {
        for (int i=0; i<m; i++)
            delete []ySYSTEM[i];
        delete []ySYSTEM;
        ySYSTEM = NULL;
    }

    if (loc2globIdx != NULL)
    {
        delete []loc2globIdx;
        loc2globIdx = NULL;
    }

    if (states != NULL)
    {
        delete []states;
        states = NULL;
    }

    if (trClasses != NULL)
    {
        delete []trClasses;
        trClasses = NULL;
    }
}

double convolutionAlgorithmGamma2d::VectQEUE::get_y(int globIdx, int n) const
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

double convolutionAlgorithmGamma2d::VectQEUE::getState(int n_s, int n_q)
{
    return states[n_s][n_q];
}

void convolutionAlgorithmGamma2d::VectQEUE::fillGamma(double **gamma, double **gammaOther, int rowNo) const
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
        qDebug()<<"\tClass"<<loc2globIdx[i]<<"t ="<<trClasses[i]->t()<<"\t"<<yStr;
    }

    QString msgTmp = QString("Gamma (%1) = 1").arg(rowNo);

    gamma[rowNo][0] = 1;
    for (int l=1; l<=rowNo; l++)
    {
        gamma[rowNo][l] = 0;
        double sumYt = 0;

        for (int i=0; i<m; i++)
        {
            modelTrClass *tmpClass = trClasses[i];
            int t = tmpClass->t();

            if (t > l)
                continue;

            int prevLc = l-t;
            int prevLd = rowNo-l;
            int prev_n = rowNo - t;

            double prevGamma =  (double)(prevLc * gamma[prev_n][prevLc] + prevLd * gammaOther[prev_n][prevLd])/(double)prev_n;

            double sigmaN = qMax((double)1, (double)(rowNo)/Vs);
            double sigmaD = qMax((double)1, (double)(l)/Vs);

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

void convolutionAlgorithmGamma2d::VectQEUE::agregate(VectQEUE *A, const convolutionAlgorithmGamma2d::VectQEUE *B)
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
    loc2globIdx = new int[m];
    trClasses = new modelTrClass*[m];

    for (int i=0; i<A->m; i++)
    {
        trClasses[i] = A->trClasses[i];
        loc2globIdx[i] = A->loc2globIdx[i];
    }
    for (int i=0; i<B->m; i++)
    {
        trClasses[i+A->m] = B->trClasses[i];
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

    if (states != NULL)
    {
        delete []states;
        states = NULL;
    }
    states = new double*[Vs+1];
    for (int x=0; x<=Vs; x++)
        states[x] = new double[Vb+1];

    double sum = 0;
    for (int x=0; x<=Vs; x++)
    {
        for (int y=0; y<=Vb; y++)
        {
            states[x][y] = 0;
            for (int Lc_x=0; Lc_x<=x; Lc_x++)
            {
                for (int Lc_y=0; Lc_y<=y; Lc_y++)
                {
                    double gA = gammaA[x+y][Lc_x+Lc_y];
                    double gB = gammaB[x+y][x+y-Lc_x - Lc_y];
                    double gamma = (x+y == 0) ? 1 : ((Lc_x+Lc_y)*gA + (x+y-Lc_x - Lc_y)*gB)/(x+y);
                    if (std::isnan(gamma))
                        qFatal("Wrong gamma");
                    states[x][y] += gamma * A->states[Lc_x][Lc_y] * B->states[x-Lc_x][y-Lc_y];
                    sum += states[x][y];
                }
            }
        }
    }

    for (int x=0; x<=Vs; x++)
        for (int y=0; y<=Vb; y++)
            states[x][y] /= sum;
    sum=1;

    //Calculate Y
    if (ySYSTEM != NULL)
    {
        for (int i=0; i<m; i++)
            delete []ySYSTEM[i];
        delete []ySYSTEM;
        ySYSTEM = NULL;
    }

    ySYSTEM = new double*[m];
    for (int i=0; i<m; i++)
    {
        ySYSTEM[i] = new double[VsVb+1];
    }

    for (int i=0; i<m; i++)
    {
        //int classGlobIdx = loc2globIdx[i];

        ySYSTEM[i][0] = 0;

        for (int n=1; n<=VsVb; n++)
        {
            ySYSTEM[i][n] = 0;
            double sum = 0;
            for (int l=0; l<=n; l++)
            {
                //double y = A->get_y(classGlobIdx, l) + B->get_y(classGlobIdx, n-l);

                //double gamma = (double)(l*gammaA[n][l] + (n-l)*gammaB[n][n-l])/(n);

                //ySYSTEM[i][n] += gamma * y * A->states[l] * B->states[n-l];
                //sum += gamma * A->states[l] * B->states[n-l];

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
                    double gamma = (double)(l*gammaA[n][l] + (n-l)*gammaB[n][n-l])/(n);
                    strGamma.append(QString(" %L1").arg(gamma, 3, 'g', 2));
                }
                qDebug()<<strGamma<<"\n";
                for (int j=0; j<m; j++)
                    qDebug()<<strY[j];

                sum = 0;
                for (int l=0; l<=n; l++)
                {
                    //double y = A->get_y(classGlobIdx, l) + B->get_y(classGlobIdx, n-l);

                    //double gamma = (double)(l*gammaA[n][l] + (n-l)*gammaB[n][n-l])/(n);
                    //sum += gamma * y * A->states[l] * B->states[n-l];

                    //QString iterData = QString("y = %1 gamma = %2, A = %3 B=%4 sum = %5").arg(y).arg(gamma).arg(A->states[l]).arg(B->states[n-l]).arg(sum);
                    //qDebug()<<iterData;
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

void convolutionAlgorithmGamma2d::VectQEUE::clone(const convolutionAlgorithmGamma2d::VectQEUE *A)
{
    Vs = A->Vs;
    Vb = A->Vb;
    VsVb = A->VsVb;

    m = A->m;
    loc2globIdx = new int[m];
    memcpy(loc2globIdx, A->loc2globIdx, m * sizeof(int));

    trClasses = new modelTrClass*[m];
    memcpy(trClasses, A->trClasses, m * sizeof(modelTrClass*));
    //states = new double[VsVb+1];
    memcpy(states, A->states, (VsVb+1)*sizeof(double));

    ySYSTEM = new double*[m];
    for (int i=0; i<m; i++)
    {
        ySYSTEM[i] = new double[VsVb+1];
        memcpy(ySYSTEM[i], A->ySYSTEM[i], (VsVb+1)*sizeof(double*));
    }
}
*/
