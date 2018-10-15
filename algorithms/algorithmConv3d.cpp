#include <QtDebug>
#include "algorithmConv3d.h"
#include "algorithms/trclvector2.h"


/*
convolutionAlgorithm3d_v1::convolutionAlgorithm3d_v1() :algorithm()
{
}

void convolutionAlgorithm3d_v1::calculateSystem(modelSyst *system, double a, algorithmResults *algRes, simulationParameters *simParameters)
{
    (void) simParameters;

    prepareTemporaryData(system, a);

    trClVector2d **p_single    = new trClVector2d*[m];
    trClVector2d **P_without_i = new trClVector2d*[m];

    for (int i=0; i<m; i++)
    {
        p_single[i] = new trClVector2d(Vs, Vb);
        for (int n =1; n<= Vs; n++)
        {
            trClVector distib = system->getClass(i)->trDistribution(A[i], n, Vb);
            p_single[i]->addQeueDistribution(distib, n);
        }
        P_without_i[i] = new trClVector2d(Vs, Vb);

        qDebug()<<"Class "<<i+1;
        qDebug()<<(*p_single[i]);
    }

    for (int i=0; i<m; i++)
    {
        for (int j=0; j<m; j++)
        {
            if (i == j)
                continue;
            trClVector2d *oldVal = P_without_i[i];
            P_without_i[i] = trClVector2d::convQUEUE(*p_single[j], *oldVal);
            delete oldVal;
        }
    }

    trClVector2d *P = trClVector2d::convQUEUE(*p_single[0], *P_without_i[0]);
    qDebug()<<"FinalDistribution:\n"<<*P;
    trClVector finalDistr = P->getNormializedDistribution(Vs, Vb);

    double avg = 0;

    for (int i=0; i<m; i++)
    {
        double E = 0;
        for (int n=Vs+Vb+1-t[i]; n<=Vb+Vs; n++)
            E+=finalDistr[n];
        algRes->set_Block(system->getClass(i), a, E);
    }

    for (int n=1; n<=Vb; n++)
        avg+=(n*finalDistr[n+Vs]);
    algRes->set_Qlen(a, avg);

    for (int i=0; i<m; i++)
    {
        for (int n=0; n<=VsVb; n++)
        {
            //int n_s = qMin(Vs, n);
            //int n_b = n - n_s;

            for (int l=0; l<=n; l++)
            {
                ;//l_s = n_s * ();
            }
        }
    }

    for (int i=0; i<m; i++)
    {
        delete p_single[i];
        delete P_without_i[i];
    }
    delete P;
    delete []P_without_i;
    delete []p_single;


    qDebug()<<finalDistr;

    deleteTemporaryData();
}

*/
