#include "simulatordatacollection.h"

#include "results/resultsInvestigator.h"

namespace Algorithms
{

simulationResults::simulationResults(int m, int Vs, int Vb, int noOfSer): m(m), Vs(Vs), Vb(Vb), noOfSeries(noOfSer), _simulationTime(0)
{
    __act_yQ                      = new double[m];
    __act_y                       = new double[m];
    __act_ytQ                     = new double[m];
    __act_t                       = new double[m];
    __act_tQeue                   = new double[m];
    __act_tS                      = new double[m];
    __act_tServer                 = new double[m];
    __act_tPlanedServer           = new double[m];
    __act_Qlen                    = new double;
    __act_E                       = new double[m];
    __act_Y                       = new double[m];
    __act_noOfLostCallsBeforeQeue = new unsigned int[m];
    __act_noOfLostCallsInQeue     = new unsigned int[m];
    __act_noOfCalls               = new unsigned int[m];
    __act_noOfServicedCalls       = new unsigned int[m];

    __act_LOC_qeue_yt             = new double*[m];
    __act_LOC_server_yt           = new double*[m];
    __act_SYS_qeue_yt             = new double*[m];
    __act_SYS_server_yt           = new double*[m];
    __act_SYS_yt                  = new double*[m];

    __act_trDistribServ           = new double[Vs+1];
    __act_trDistrib               = new double*[Vs+1];
    for (int n=0; n<=Vs; n++)
        __act_trDistrib[n]        = new double[Vb+1];
    __act_trDistribSys            = new double[Vs+Vb+1];
    __act_trDistribQeue           = new double[Vb+1];

    for (int i=0; i<m; i++)
    {
        __act_LOC_qeue_yt  [i]    = new double[Vb+1];
        __act_LOC_server_yt[i]    = new double[Vs+1];
        __act_SYS_qeue_yt  [i]    = new double[Vb+Vs+1];
        __act_SYS_server_yt[i]    = new double[Vb+Vs+1];
        __act_SYS_yt       [i]    = new double[Vb+Vs+1];
    }
    yQ                            = new double*[noOfSer];
    y                             = new double*[noOfSer];
    ytQ                           = new double*[noOfSer];
    t                             = new double*[noOfSer];
    tQeue                         = new double*[noOfSer];
    tS                            = new double*[noOfSer];
    tServer                       = new double*[noOfSer];
    tPlanedServer                 = new double*[noOfSer];
    Qlen                          = new double[noOfSer];
    E                             = new double*[noOfSer];
    Y                             = new double*[noOfSer];

    noOfLostCallsBeforeQeue       = new unsigned int*[noOfSer];
    noOfLostCallsInQeue           = new unsigned int*[noOfSer];
    noOfCalls                     = new unsigned int*[noOfSer];
    noOfServicedCalls             = new unsigned int*[noOfSer];

    LOC_qeue_yt                   = new double**[noOfSer];
    LOC_server_yt                 = new double**[noOfSer];
    SYS_qeue_yt                   = new double**[noOfSer];
    SYS_server_yt                 = new double**[noOfSer];
    SYS_yt                        = new double**[noOfSer];

    trDistrib                     = new double**[noOfSer];
    trDistribServ                 = new double*[noOfSer];
    trDistribSys                  = new double*[noOfSer];
    trDistribQeue                 = new double*[noOfSer];

    intInNew                      = new double*[noOfSer];
    intInEnd                      = new double*[noOfSer];
    intOutNew                     = new double*[noOfSer];
    intOutEnd                     = new double*[noOfSer];

    noInNew                       = new unsigned long int*[noOfSer];
    noInEnd                       = new unsigned long int*[noOfSer];
    noOutNew                      = new unsigned long int*[noOfSer];
    noOutEnd                      = new unsigned long int*[noOfSer];

    intInNewSC                    = new double**[noOfSer];
    intInEndSC                    = new double**[noOfSer];
    intOutNewSC                   = new double**[noOfSer];
    intOutEndSC                   = new double**[noOfSer];

    noInNewSC                     = new unsigned long int**[noOfSer];
    noInEndSC                     = new unsigned long int**[noOfSer];
    noOutNewSC                    = new unsigned long int**[noOfSer];
    noOutEndSC                    = new unsigned long int**[noOfSer];


    for (int serNo=0; serNo<noOfSer; serNo++)
    {
        yQ           [serNo] = new double[m];
        y            [serNo] = new double[m];
        ytQ          [serNo] = new double[m];
        t            [serNo] = new double[m];
        tQeue        [serNo] = new double[m];
        tS           [serNo] = new double[m];
        tServer      [serNo] = new double[m];
        tPlanedServer[serNo] = new double[m];
        E            [serNo] = new double[m];
        Y            [serNo] = new double[m];

        noOfLostCallsBeforeQeue[serNo] = new unsigned int[m];
        noOfLostCallsInQeue[serNo]     = new unsigned int[m];
        noOfCalls[serNo]               = new unsigned int[m];
        noOfServicedCalls[serNo]       = new unsigned int[m];

        LOC_qeue_yt    [serNo] = new double*[m];
        LOC_server_yt  [serNo] = new double*[m];
        SYS_qeue_yt    [serNo] = new double*[m];
        SYS_server_yt  [serNo] = new double*[m];
        SYS_yt         [serNo] = new double*[m];

        intInNewSC     [serNo] = new double*[m];
        intInEndSC     [serNo] = new double*[m];
        intOutNewSC    [serNo] = new double*[m];
        intOutEndSC    [serNo] = new double*[m];

        noInNewSC     [serNo] = new unsigned long int*[m];
        noInEndSC     [serNo] = new unsigned long int*[m];
        noOutNewSC    [serNo] = new unsigned long int*[m];
        noOutEndSC    [serNo] = new unsigned long int*[m];

        for (int i=0; i<m; i++)
        {
            LOC_qeue_yt  [serNo][i] = new double[Vb+1];
            LOC_server_yt[serNo][i] = new double[Vs+1];
            SYS_qeue_yt  [serNo][i] = new double[Vs+Vb+1];
            SYS_server_yt[serNo][i] = new double[Vs+Vb+1];
            SYS_yt       [serNo][i] = new double[Vs+Vb+1];

            intInNewSC   [serNo][i] = new double[Vs+Vb+1];
            intInEndSC   [serNo][i] = new double[Vs+Vb+1];
            intOutNewSC  [serNo][i] = new double[Vs+Vb+1];
            intOutEndSC  [serNo][i] = new double[Vs+Vb+1];

            noInNewSC    [serNo][i] = new unsigned long int[Vs+Vb+1];
            noInEndSC    [serNo][i] = new unsigned long int[Vs+Vb+1];
            noOutNewSC   [serNo][i] = new unsigned long int[Vs+Vb+1];
            noOutEndSC   [serNo][i] = new unsigned long int[Vs+Vb+1];
        }

        trDistrib[serNo]            = new double*[Vs+1];
        for (int n=0; n<=Vs; n++)
            trDistrib[serNo][n]     = new double[Vb+1];
        trDistribServ[serNo]        = new double[Vs+1];
        trDistribSys[serNo]         = new double[Vs+Vb+1];
        trDistribQeue[serNo]        = new double[Vb+1];

        intInNew[serNo]             = new double[Vs+Vb+1];
        intInEnd[serNo]             = new double[Vs+Vb+1];
        intOutNew[serNo]            = new double[Vs+Vb+1];
        intOutEnd[serNo]            = new double[Vs+Vb+1];

        noInNew[serNo]             = new unsigned long int[Vs+Vb+1];
        noInEnd[serNo]             = new unsigned long int[Vs+Vb+1];
        noOutNew[serNo]            = new unsigned long int[Vs+Vb+1];
        noOutEnd[serNo]            = new unsigned long int[Vs+Vb+1];

    }
}

simulationResults::~simulationResults()
{
    delete []__act_yQ;
    delete []__act_y;
    delete []__act_ytQ;
    delete []__act_t;
    delete []__act_tQeue;
    delete []__act_tS;
    delete []__act_tServer;
    delete []__act_tPlanedServer;
    delete __act_Qlen;
    delete []__act_E;
    delete []__act_Y;
    delete []__act_noOfLostCallsBeforeQeue;
    delete []__act_noOfLostCallsInQeue;
    delete []__act_noOfCalls;
    delete []__act_noOfServicedCalls;

    delete []__act_trDistribServ;
    delete []__act_trDistribSys;
    delete []__act_trDistribQeue;

    for (int n=0; n<=Vs; n++)
        delete []__act_trDistrib[n];
    delete []__act_trDistrib;

    for (int i=0; i<m; i++)
    {
        delete []__act_LOC_qeue_yt[i];
        delete []__act_LOC_server_yt[i];
        delete []__act_SYS_qeue_yt[i];
        delete []__act_SYS_server_yt[i];
        delete []__act_SYS_yt[i];
    }

    delete []__act_LOC_qeue_yt;
    delete []__act_LOC_server_yt;
    delete []__act_SYS_qeue_yt;
    delete []__act_SYS_server_yt;
    delete []__act_SYS_yt;

    for (int serNo=0; serNo<noOfSeries; serNo++)
    {
        for (int i=0; i<m; i++)
        {
            delete []LOC_qeue_yt  [serNo][i];
            delete []LOC_server_yt[serNo][i];
            delete []SYS_qeue_yt  [serNo][i];
            delete []SYS_server_yt[serNo][i];
            delete []SYS_yt       [serNo][i];

            delete []intInNewSC  [serNo][i];
            delete []intInEndSC  [serNo][i];
            delete []intOutNewSC [serNo][i];
            delete []intOutEndSC [serNo][i];

            delete []noInNewSC   [serNo][i];
            delete []noInEndSC   [serNo][i];
            delete []noOutNewSC  [serNo][i];
            delete []noOutEndSC  [serNo][i];
        }

        delete []yQ           [serNo];
        delete []y            [serNo];
        delete []ytQ          [serNo];
        delete []t            [serNo];
        delete []tQeue        [serNo];
        delete []tS           [serNo];
        delete []tServer      [serNo];
        delete []tPlanedServer[serNo];

        delete []E  [serNo];
        delete []Y  [serNo];

        delete []noOfLostCallsBeforeQeue[serNo];
        delete []noOfLostCallsInQeue[serNo];
        delete []noOfCalls[serNo];
        delete []noOfServicedCalls[serNo];

        delete []LOC_qeue_yt  [serNo];
        delete []LOC_server_yt[serNo];
        delete []SYS_qeue_yt  [serNo];
        delete []SYS_server_yt[serNo];
        delete []SYS_yt       [serNo];

        delete []trDistribSys[serNo];
        delete []trDistribServ[serNo];
        delete []trDistribQeue[serNo];
        for (int n=0; n<=Vs; n++)
            delete []trDistrib[serNo][n];
        delete []trDistrib[serNo];

        delete []intInNew[serNo];
        delete []intInEnd[serNo];
        delete []intOutNew[serNo];
        delete []intOutEnd[serNo];

        delete []noInNew[serNo];
        delete []noInEnd[serNo];
        delete []noOutNew[serNo];
        delete []noOutEnd[serNo];

        delete []intInNewSC[serNo];
        delete []intInEndSC[serNo];
        delete []intOutNewSC[serNo];
        delete []intOutEndSC[serNo];


        delete []noInNewSC[serNo];
        delete []noInEndSC[serNo];
        delete []noOutNewSC[serNo];
        delete []noOutEndSC[serNo];
    }

    delete []yQ;
    delete []y;
    delete []ytQ;
    delete []t;
    delete []tQeue;
    delete []tS;
    delete []tPlanedServer;
    delete []tServer;
    delete []Qlen;
    delete []E;
    delete []Y;

    delete []noOfLostCallsBeforeQeue;
    delete []noOfLostCallsInQeue;
    delete []noOfCalls;
    delete []noOfServicedCalls;

    delete []LOC_qeue_yt;
    delete []LOC_server_yt;
    delete []SYS_qeue_yt;
    delete []SYS_server_yt;
    delete []SYS_yt;

    delete []trDistribSys;
    delete []trDistribServ;
    delete []trDistribQeue;
    delete []trDistrib;

    delete []intOutNew;
    delete []intOutEnd;
    delete []intInNew;
    delete []intInEnd;

    delete []noOutNew;
    delete []noOutEnd;
    delete []noInNew;
    delete []noInEnd;

    delete []intOutNewSC;
    delete []intOutEndSC;
    delete []intInNewSC;
    delete []intInEndSC;

    delete []noOutNewSC;
    delete []noOutEndSC;
    delete []noInNewSC;
    delete []noInEndSC;

}

void simulationResults::disableStatisticCollection()
{
    act_yQ             = __act_yQ;
    act_y              = __act_y;
    act_ytQ            = __act_ytQ;
    act_t              = __act_t;
    act_tQeue          = __act_tQeue;
    act_tS             = __act_tS;
    act_tServer        = __act_tServer;
    act_tPlanedServer  = __act_tPlanedServer;
    act_Qlen           = __act_Qlen;
    act_E              = __act_E;
    act_Y              = __act_Y;

    act_noOfLostCallsBeforeQeue = __act_noOfLostCallsBeforeQeue;
    act_noOfLostCallsInQeue     = __act_noOfLostCallsInQeue;
    act_noOfCalls               = __act_noOfCalls;
    act_noOfServicedCalls       = __act_noOfServicedCalls;

    act_LOC_qeue_yt             = __act_LOC_qeue_yt;
    act_LOC_server_yt           = __act_LOC_server_yt;
    act_SYS_qeue_yt             = __act_SYS_qeue_yt;
    act_SYS_server_yt           = __act_SYS_server_yt;
    act_SYS_yt                  = __act_SYS_yt;

    act_trDistrib               = __act_trDistrib;
    act_trDistribServ           = __act_trDistribServ;
    act_trDistribSys            = __act_trDistribSys;
    act_trDistribQeue           = __act_trDistribQeue;

    *act_Qlen = 0;
    for (int i=0; i<m; i++)
    {
        act_yQ[i]            = 0;
        act_y[i]             = 0;
        act_ytQ[i]           = 0;
        act_t[i]             = 0;
        act_tQeue[i]         = 0;
        act_tS[i]            = 0;
        act_tServer[i]       = 0;
        act_tPlanedServer[i] = 0;
        act_E[i]             = 0;
        act_Y[i]             = 0;

        act_noOfLostCallsBeforeQeue[i] = 0;
        act_noOfLostCallsInQeue[i]     = 0;
        act_noOfCalls[i]               = 0;
        act_noOfServicedCalls[i]       = 0;

        for (int n=0; n<=Vb; n++)
            act_LOC_qeue_yt[i][n]=0;
        for (int n=0; n<=Vs; n++)
            act_LOC_server_yt[i][n]=0;

        for (int n=0; n<=Vs+Vb; n++)
        {
            act_SYS_qeue_yt[i][n]=0;
            act_SYS_server_yt[i][n]=0;
            act_SYS_yt[i][n]=0;
        }
    }
    for (int n=0; n<=Vs; n++)
    {
        act_trDistribServ[n] = 0;
        for (int q=0; q<=Vb; q++)
            act_trDistrib[n][q] = 0;
    }
    for (int n=0; n<=Vb; n++)
        act_trDistribQeue[n] = 0;

    for (int n=0; n<=Vs+Vb; n++)
        act_trDistribSys[n] = 0;

}

void simulationResults::enableStatisticscollection(int serNo)
{
    _simulationTime = 0;

    act_yQ            = yQ[serNo];
    act_y             = y[serNo];
    act_ytQ           = ytQ[serNo];
    act_t             = t[serNo];
    act_tQeue         = tQeue[serNo];
    act_tS            = tS[serNo];
    act_tServer       = tServer[serNo];
    act_tPlanedServer = tPlanedServer[serNo];
    act_Qlen          = &Qlen[serNo];
    act_E             = E[serNo];
    act_Y             = Y[serNo];

    act_noOfLostCallsBeforeQeue = noOfLostCallsBeforeQeue[serNo];
    act_noOfLostCallsInQeue     = noOfLostCallsInQeue[serNo];
    act_noOfCalls               = noOfCalls[serNo];
    act_noOfServicedCalls       = noOfServicedCalls[serNo];

    act_LOC_qeue_yt             = LOC_qeue_yt[serNo];
    act_LOC_server_yt           = LOC_server_yt[serNo];
    act_SYS_qeue_yt             = SYS_qeue_yt[serNo];
    act_SYS_server_yt           = SYS_server_yt[serNo];
    act_SYS_yt                  = SYS_yt[serNo];

    act_trDistrib               = trDistrib[serNo];
    act_trDistribServ           = trDistribServ[serNo];
    act_trDistribSys            = trDistribSys[serNo];
    act_trDistribQeue           = trDistribQeue[serNo];

    act_intInNew                = intInNew[serNo];
    act_intInEnd                = intInEnd[serNo];
    act_intOutNew               = intOutNew[serNo];
    act_intOutEnd               = intOutEnd[serNo];

    act_noInNew                 = noInNew[serNo];
    act_noInEnd                 = noInEnd[serNo];
    act_noOutNew                = noOutNew[serNo];
    act_noOutEnd                = noOutEnd[serNo];

    act_intInNewSC              = intInNewSC[serNo];
    act_intInEndSC              = intInEndSC[serNo];
    act_intOutNewSC             = intOutNewSC[serNo];
    act_intOutEndSC             = intOutEndSC[serNo];

    act_noInNewSC               = noInNewSC[serNo];
    act_noInEndSC               = noInEndSC[serNo];
    act_noOutNewSC              = noOutNewSC[serNo];
    act_noOutEndSC              = noOutEndSC[serNo];

    *act_Qlen = 0;
    for (int i=0; i<m; i++)
    {
        act_yQ[i]            = 0;
        act_y[i]             = 0;
        act_ytQ[i]           = 0;
        act_t[i]             = 0;
        act_tQeue[i]         = 0;
        act_tS[i]            = 0;
        act_tServer[i]       = 0;
        act_tPlanedServer[i] = 0;
        act_E[i]             = 0;
        act_Y[i]             = 0;

        act_noOfLostCallsBeforeQeue[i] = 0;
        act_noOfLostCallsInQeue[i]     = 0;
        act_noOfCalls[i]               = 0;
        act_noOfServicedCalls[i]       = 0;

        for (int n=0; n<=Vb; n++)
            act_LOC_qeue_yt[i][n]=0;
        for (int n=0; n<=Vs; n++)
            act_LOC_server_yt[i][n]=0;

        for (int n=0; n<=Vs+Vb; n++)
        {
            act_SYS_qeue_yt[i][n]=0;
            act_SYS_server_yt[i][n]=0;
            act_SYS_yt[i][n]=0;
        }
    }
    for (int n=0; n<=Vs; n++)
    {
        act_trDistribServ[n]=0;
        for (int q=0; q<=Vb; q++)
            act_trDistrib[n][q]=0;
    }
    for (int n=0; n<=Vb; n++)
        act_trDistribQeue[n]=0;

    for (int n=0; n<=Vs+Vb; n++)
        act_trDistribSys[n]=0;
}


} // namespace Algorithms
