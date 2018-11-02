#include "algorithmConvGamma3.h"
#include "results/resultsInvestigator.h"

namespace Algorithms
{

convolutionAlgorithmGamma3::convolutionAlgorithmGamma3(): Investigator()
{
    myQoS_Set
       <<Results::Type::BlockingProbability
       <<Results::Type::OccupancyDistribution;
}

bool convolutionAlgorithmGamma3::possible(ModelSyst *system)
{
    if (system->vk_b() == 0)
        return false;
    return Investigator::possible(system);
}

void convolutionAlgorithmGamma3::calculateSystem(const ModelSyst& system
        , double a
        , RInvestigator *results
        , SimulationParameters *simParameters
        )
{
    prepareTemporaryData(&system, a);
    (void) simParameters;
    int m = system.m();
    int Vs = system.vk_s();
    int Vb = system.vk_b();
    int VsVb = Vs + Vb;



    for (int i=0; i<m; i++)
        p_single[i] = new VectQEUE(Vs, Vb, 1, i, system.getClass(i), classes[i].A);

    VectQEUE *FD = new VectQEUE();

    for (int i=0; i<m; i++)
    {
        VectQEUE *FD2 = new VectQEUE();
        FD2->agregate(FD, p_single[i]);
        delete FD;
        FD = FD2;
    }

    QString msgTmp = "Final distribution: ";
    for (int n=0; n<=VsVb; n++)
        msgTmp.append(QString(" %1").arg(FD->getState(n)));

    qDebug()<<msgTmp;

    for (int i=0; i<system.m(); i++)
    {
        //Prawdopodobieństwo blokady
        double E = 0;
        for (int n=VsVb+1-classes[i].t; n<=VsVb; n++)
        {
            E+=FD->getState(n);
        }
        (*results)->write(TypeForClass::BlockingProbability, E, i);

        //Prawdopodobieństwo strat
        double B_n = 0;
        double B_d = 0;
        for (int n=0; n<=VsVb; n++)
        {
            double x = system.getClass(i)->intensityNewCallForState(1, (int)(FD->get_y(i, n)));
            if (n > VsVb-classes[i].t)
                B_n+=(x*FD->getState(n));
            B_d +=(x*FD->getState(n));
        }
        (*results)->write(TypeForClass::LossProbability, B_n/B_d, i);

        //Średnia liczba zgłoszeń w kolejce
        double yQ = 0;
        for (int n=Vs+1; n<=VsVb; n++)
        {
            yQ+=(FD->getState(n) * yQEUE_VsVb[i][n]);
        }
        (*results)->write(TypeForClass::AvarageNumbersOfCallsInBuffer, yQ, i);

        //Średnia liczba zajętych zasobów w kolejce
        //double ytQ = 0;
        //for (int n=Vs+1; n<=VsVb; n++)
        //{
        //    ytQ+=(P[n] * t[i] * (ySYSTEM_VsVb[i][n] - ySYSTEM_VsVb[i][Vs]));
        //}
        //algRes->set_ytQ(system->getClass(i), a, ytQ);

        //Średnia liczba obsługiwanych zgłoszeń
        double y = 0;
        for (int n=0; n<=VsVb; n++)
        {
            y+=(FD->getState(n) * FD->get_y(i, n));
        }
        //TODO algRes->set_lSys(system.getClass(i), a, y);
    }

    double avgLen;

    //Średnie wykorzystanie systemu
    avgLen = 0;
    for (int n=0; n<=VsVb; n++)
        avgLen += n*FD->getState(n);
    (*results)->write(TypeGeneral::SystemUtilization, avgLen);


    //Średnie wykorzystanie serwera
    avgLen = 0;
    for (int n=0; n<=VsVb; n++)
        avgLen += FD->getState(n) * (n <= Vs ? n : Vs);
    (*results)->write(TypeGeneral::SystemUtilization, avgLen);


    //Średnia długość kolejki
    avgLen = 0;
    for (int n=Vs+1; n<=VsVb; n++)
        avgLen += ((n-Vs)*FD->getState(n));
    (*results)->write(TypeGeneral::BufferUtilization, avgLen);

    for (int i=0; i<m; i++)
    {
        for (int n=0; n<=VsVb; n++)
        {
            (*results)->write(TypeForClassAndSystemState::UsageForSystem, FD->get_y(i, n), i, n);
        }
    }
    deleteTemporaryData();
}

void convolutionAlgorithmGamma3::prepareTemporaryData(const ModelSyst *system, double a)
{
    Investigator::prepareTemporaryData(system, a);
    this->p_single = new VectQEUE*[system->m()];
}

void convolutionAlgorithmGamma3::deleteTemporaryData()
{
    Investigator::deleteTemporaryData();

    for (int i=0; i<system->m(); i++)
        delete p_single[i];
    delete []p_single;
}


convolutionAlgorithmGamma3::VectQEUE::VectQEUE(int Vs, int Vb, int m, int i, const ModelTrClass *trClass, double A): Vs(Vs), Vb(Vb), VsVb(Vs+Vb), m(m), loc2globIdx(NULL), ySYSTEM(NULL)
{
    (void) A;
    loc2globIdx = new int[1];
    loc2globIdx[0] = i;
    trClasses = new const ModelTrClass*[1];
    trClasses[0] = trClass;

    states = new double[Vs+Vb+1];
    qFatal("Update to nwe libraries");
    //memcpy(states, trClass->trDistributionSimEna(A, Vs, Vb).constData(), sizeof(double)*(Vs+Vb+1));
    ySYSTEM = new double*[1];

    ySYSTEM[0] = new double[VsVb+1];
    bzero(ySYSTEM[0], (VsVb+1) * sizeof(double));
    for (int n=0; n<=VsVb; n+=trClass->t())
        ySYSTEM[0][n] = (double)(n)/(double)(trClass->t());
}

convolutionAlgorithmGamma3::VectQEUE::~VectQEUE()
{
    if (ySYSTEM != nullptr)
    {
        for (int i=0; i<m; i++)
            delete []ySYSTEM[i];
        delete []ySYSTEM;
        ySYSTEM = nullptr;
    }

    if (loc2globIdx != nullptr)
    {
        delete []loc2globIdx;
        loc2globIdx = nullptr;
    }

    if (states != nullptr)
    {
        delete []states;
        states = nullptr;
    }

    if (trClasses != nullptr)
    {
        delete []trClasses;
        trClasses = nullptr;
    }
}

double convolutionAlgorithmGamma3::VectQEUE::get_y(int globIdx, int n) const
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

double convolutionAlgorithmGamma3::VectQEUE::getState(int n)
{
    return states[n];
}

void convolutionAlgorithmGamma3::VectQEUE::fillGamma(double **gamma) const
{
    for (int n=0; n<=Vs; n++)
    {
        for (int l = 0; l<=n; l++)
            gamma[n][l] = 1;
    }

    qDebug("Determining Gamma for distribution with classes:");

    for (int i=0; i<m; i++)
    {
        QString yStr;
        for (int n=0; n<=VsVb; n++)
            yStr.append(QString(" %1").arg(ySYSTEM[i][n], 3, 'g', 2));
        qDebug()<<"\tClass"<<loc2globIdx[i]<<"t ="<<trClasses[i]->t()<<"\t"<<yStr;
    }
    for (int n=Vs+1; n<=VsVb; n++)
    {
        QString msgTmp = "Gamma (%1) = 1";
        msgTmp = msgTmp.arg(n);


        gamma[n][0] = 1;
        for (int l=1; l<=n; l++)
        {
            gamma[n][l] = 0;
            double sumYt = 0;

            for (int i=0; i<m; i++)
            {
                const ModelTrClass *tmpClass = trClasses[i];
                int t = tmpClass->t();

                if (t > l)
                    continue;
                double prevGamma = gamma[n-t][l-t];

                double sigmaN = qMax((double)1, (double)(n)/Vs);
                double sigmaD = qMax((double)1, (double)(l)/Vs);

                double y = ySYSTEM[i][l];

                sumYt += (y*t);

                gamma[n][l]+= (prevGamma * y * t * sigmaN / sigmaD);


                if (std::isnan(gamma[n][l]))
                    qFatal("Wrong gamma");
            }
            if (sumYt>0)
            {
                gamma[n][l] /=l;
                if ((sumYt < l-0.01) || (sumYt > l+0.01))
                    qFatal("Wrong sum yt");
            }
            else
                gamma[n][l] = 0;

/*            {
                for (int i=0; i<m; i++)
                {
                    modelTrClass *tmpClass = trClasses[i];
                    int t = tmpClass->t();

                    double prevGamma = (t > l) ? 1 : gamma[n-t][l-t];

                    double sigmaN = qMax((double)1, (double)(n)/Vs);
                    double sigmaD = qMax((double)1, (double)(l)/Vs);

                    double y = ySYSTEM[i][l];

                    qDebug()<<"n ="<<n<<"l ="<<l<<"Class t="<<t<<"prevGama ="<<prevGamma<<"N"<<sigmaN<<"D"<<sigmaD<<"y="<<y;
                }

                qFatal("Wrong gamma = 0");
            }
*/
            msgTmp.append(QString(" %1").arg(gamma[n][l]));
            if (std::isnan(gamma[n][l]))
                qFatal("Wrong gamma");
        }
        qDebug()<<msgTmp;
    }
}

void convolutionAlgorithmGamma3::VectQEUE::agregate(VectQEUE *A, const convolutionAlgorithmGamma3::VectQEUE *B)
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
    trClasses = new const ModelTrClass*[m];

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

    A->fillGamma(gammaA);
    B->fillGamma(gammaB);
    if (states != NULL)
    {
        delete []states;
        states = NULL;
    }
    states = new double[VsVb+1];

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
        states[n] /= sum;
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

            ySYSTEM[i][n] = ySYSTEM[i][n]/ sum;
            if (std::isnan(ySYSTEM[i][n]))
            {
                QString strGamma = "Gamma %1: ";
                strGamma = strGamma.arg(n);

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
                    double y = A->get_y(classGlobIdx, l) + B->get_y(classGlobIdx, n-l);

                    double gamma = (double)(l*gammaA[n][l] + (n-l)*gammaB[n][n-l])/(n);
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

void convolutionAlgorithmGamma3::VectQEUE::clone(const convolutionAlgorithmGamma3::VectQEUE *A)
{
    Vs = A->Vs;
    Vb = A->Vb;
    VsVb = A->VsVb;

    m = A->m;
    loc2globIdx = new int[m];
    memcpy(loc2globIdx, A->loc2globIdx, m * sizeof(int));

    trClasses = new const ModelTrClass*[m];
    memcpy(trClasses, A->trClasses, m * sizeof(ModelTrClass*));
    states = new double[VsVb+1];
    memcpy(states, A->states, (VsVb+1)*sizeof(double));

    ySYSTEM = new double*[m];
    for (int i=0; i<m; i++)
    {
        ySYSTEM[i] = new double[VsVb+1];
        memcpy(ySYSTEM[i], A->ySYSTEM[i], (VsVb+1)*sizeof(double*));
    }
}

} // namespace Algorithms
