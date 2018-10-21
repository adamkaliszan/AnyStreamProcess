#include "algRekLagGS.h"
#include "utils/vectorUtils.h"
#include "utils/lag.h"

namespace Algorithms
{

algRekLagGS::algRekLagGS()
{
    myQoS_Set
     << Results::Type::BlockingProbability
     << Results::Type::OccupancyDistribution
     << Results::Type::LossProbability
     << Results::Type::AllSugbrupsInGivenCombinationAndClassAvailable
     << Results::Type::AvailableSubroupDistribution;
}

void algRekLagGS::calculateSystem(const ModelSyst *system, double a, RInvestigator *results, SimulationParameters *simParameters)
{
    (void) simParameters;

    Utils::UtilsLAG lag;
    int f = system->v_s(0);
    int k = system->k_s(0);
    int V = system->vk_s();
    int m = system->m();


    prepareTemporaryData(system, a);

    QVector<double> statesFAG;
    statesFAG.resize(V+1);

    states[0] = 1;
    statesFAG[0] = 1;

    for (int n=1; n<=V; n++)
    {
        states[n] = 0;
        statesFAG[n] = 0;
        for (int i=0; i<m; i++)
        {
            int t = classes[i].t;
            if (t <= n)
            {
                double sigma = getSigma(i, n-t);
                states[n] += states[n - t] * classes[i].A * t * sigma;
                statesFAG[n] += statesFAG[n - t] * classes[i].A * t;
            }
        }
        states[n] /= n;
        statesFAG[n] /= n;
    }
    vectorUtils::normalize(statesFAG);
    vectorUtils::normalize(states);



    QVector<QVector <double>> RDP;                                   // Rozkład dostępnych s podgrup - mających (jedna z nich ma ns wolnych zasobów) - kolumna
    QVector<QVector <double>> tRDP;                                  // transponowany Rozkład dostępnych s podgrup
    RDP.resize(k+1);
    tRDP.resize(f+1);

    RDP[0].fill(0, f+1);
    RDP[0][0] = 1;


    RDP[0][0] = 0;
    for (int t=1; t<=f; t++)
    {
        RDP[0][t] = 0;
        for (int n=0; n<=V; n++)
        {
            double A = lag.F(static_cast<uint>(V-n), static_cast<uint>(k), static_cast<uint>(f), 0);                          // Liczba możliwych rozmieszczeń wolnych zasobów w LAG
            double B = (V-n > k*(t-1)) ? 0 : lag.F(static_cast<uint>(V-n), static_cast<uint>(k), static_cast<uint>(t-1), 0);  // Liczba możliwych rozmieszczeń wolnych zasobów gdy w żadnej z podgrup nie ma dostępnych t zasobów
            double X = B/A;                                          // Prawdopodobieństwo, że w stanie n nie można przyjąć zgłoszenia żądającego t zasobów

            RDP[0][t] += states[n] * X;
        }
    }
    for (int s=1; s <= k; s++)
    {
        double dwumianL = lag.binomial(k, s);

        RDP[s].fill(0, f+1);
        for (int ti=1; ti <= f; ti++)                               // Liczba wymaganych zasobów w dowlonej z s podgrup
        {
            QVector<double> WRDP;                                   // Warunkowy rozkład dostępnych s podgrup.
            WRDP.fill(1, V+1);
            for (int x=0; x <= V; x++)                              // x - liczba wolnych zasobów
            {
                WRDP[x] = 0;
                double fi = qMin<double>(s*f, x);                   // Minimum z (liczby zajętych zasobów oraz liczby rozważanych podgrup i minimalnej zajętości każdej z nich)
                for (int w=s*ti; w<=fi; w++)                        // w liczba wolnych zasobów w s wiązkach
                {
                    if ((x-w) > (k-s)*(ti-1))
                        continue;
                    if (s * ti > w)
                        continue;

                    double product = lag.F(static_cast<uint>(w), static_cast<uint>(s), static_cast<uint>(f), static_cast<uint>(ti));             // Liczba rozmieszczeń w zasobów w s podgrupach gdy każda z podrup ma minumim ti wolnych zasobów
                    product *= lag.F(static_cast<uint>(x-w), static_cast<uint>(k-s), static_cast<uint>(ti-1), 0);             // Liczba rozmieszczeń x-w zasobów w k-s podgrupach, gdy żadna z tych podgrup nie ma dostępnych ti wolnych zasobów
                    WRDP[x] += product;
                }
                WRDP[x]*= dwumianL;
                WRDP[x]/= lag.F(static_cast<uint>(x), static_cast<uint>(k), static_cast<uint>(f), 0);                         // Liczba wszystkich możliwych rozmieszczeń wolnych zasobów

                if ( WRDP[x] < 0 || WRDP[x] > 1 )
                    qFatal("Wrong WRDP");
            }

            RDP[s][ti] = 0;
            for (int n=0; n<=V; n++)
            {
                RDP[s][ti]+= states[n] * WRDP[V-n];
            }
        }
    }
    RDP[k][0]=1;

    for (int s=0; s <= k; s++)
    {
        for (int ns=0; ns <= f; ns++)
        {
            (*results)->write(TypeStateForServerGroupsSet::AvailabilityOnlyInAllTheGroups, RDP[s][ns] , ns,  s);
        }
        for (int i=0; i<m; i++)
        {
            (*results)->write(TypeClassForServerExactGroupsSet::ServPossibilityOnlyInAllTheSubgroups, RDP[s][classes[i].t], i, s);
        }
    }
    for (int i=0; i<m; i++)
        (*results)->write(TypeForClass::BlockingProbability, RDP[0][classes[i].t], i);

    for (int ns=0; ns <= f; ns++)
    {
        tRDP[ns].resize(k+1);
        for (int s=0; s <= k; s++)
            tRDP[ns][s] = RDP[s][ns];
    }

    QVector<QPair <QVector<int>, QVector<int> > > combinations = Utils::UtilsLAG::getPossibleCombinations(k);

    for (int combNo=0; combNo < combinations.length(); combNo++)
    {
        uint xd = static_cast<uint>(combinations[combNo].first.length());

        for (int i=0; i<m; i++)
        {
            double H;

            H = lag.Hprime_Wariant1(xd, tRDP[classes[i].t], static_cast<uint>(k));
            (*results)->write(TypeClassForServerGroupsCombination::SerPossibilityInAllTheSubgroups, H, i, combNo);

            H = lag.H_Wariant1(xd, tRDP[classes[i].t], static_cast<uint>(k));
            (*results)->write(TypeClassForServerGroupsCombination::SerImpossibilityInAllTheSubgroups, H, i, combNo);
        }

        for (int ns=0; ns <= f; ns++)
        {
            double H;

            H = lag.Hprime_Wariant1(xd, tRDP[ns], static_cast<uint>(k));
            (*results)->write(TypeStateForServerGroupsCombination::AvailabilityInAllTheGroups, H, ns, combNo);

            H = lag.H_Wariant1(xd, tRDP[ns], static_cast<uint>(k));
            (*results)->write(TypeStateForServerGroupsCombination::InavailabilityInAllTheGroups, H, ns, combNo);

        }
    }
    //emit this->sigCalculationDone();
}

bool algRekLagGS::possible(const ModelSyst *system) const
{
    if ((system->vk_s() == 0) || (system->K_sType() > 1) || (system->V_b() > 0))
        return false;

    return Investigator::possible(system);
}

double algRekLagGS::getSigma(int classNumber, int state)
{
    Utils::UtilsLAG lag;
    double result = 1;
    int x = system->V() - state;
    int t = system->getClass(classNumber)->t();
    int k = system->Ks();
    int f = system->v_s(0);

    if (x <= (t-1) * k)
    {
        double A = lag.F(static_cast<uint>(x), static_cast<uint>(k), static_cast<uint>(f),   0);
        double B = lag.F(static_cast<uint>(x), static_cast<uint>(k), static_cast<uint>(t-1), 0);
        result = (A-B)/A;
    }
    return result;
}

algRekLagGS2::algRekLagGS2()
{
    myQoS_Set
     << Results::Type::BlockingProbability
     << Results::Type::OccupancyDistribution
     << Results::Type::LossProbability
     << Results::Type::AllSugbrupsInGivenCombinationAndClassAvailable
     << Results::Type::AvailableSubroupDistribution;
}

void algRekLagGS2::calculateSystem(const ModelSyst *system, double a, RInvestigator *results, SimulationParameters *simParameters)
{
    (void) simParameters;

    Utils::UtilsLAG utilsLag;

    int f = system->v_s(0);
    int k = system->k_s(0);
    //int V = system->V_s();
    int m = system->m();

    prepareTemporaryData(system, a);

    states[0] = 1;
    for (int n=1; n<=system->V(); n++)
    {
        states[n] = 0;
        for (int i=0; i<system->m(); i++)
        {
            int t = classes[i].t;
            if (t <= n)
            {
                double sigma = getSigma(i, n-t);
                states[n] += states[n - t] * classes[i].A * t * sigma;
            }
        }
        states[n] /= n;
    }
    vectorUtils::normalize(states);

    for (int classNo=0; classNo<system->m(); classNo++)
    {
        double Hk; //prawdopodobieńśtwo zajętości k łączy

        Hk = utilsLag.H_Wariant2(static_cast<uint>(system->Ks()), states, static_cast<uint>(system->getClass(classNo)->t()), static_cast<uint>(system->Ks()), static_cast<uint>(system->v_s(0)));
        (*results)->write(TypeForClass::BlockingProbability, Hk, classNo);
    }

    QVector<QPair <QVector<int>, QVector<int> > > combinations = Utils::UtilsLAG::getPossibleCombinations(k);

    for (int combNo=0; combNo < combinations.length(); combNo++)
    {
        uint xd = static_cast<uint>(combinations[combNo].first.length());

        for (int i=0; i<m; i++)
        {
            double H = utilsLag.H_Wariant2(xd, states,  static_cast<uint>(classes[i].t), static_cast<uint>(k), static_cast<uint>(f));
            (*results)->write(TypeClassForServerGroupsCombination::SerImpossibilityInAllTheSubgroups, H, i, combNo);
        }

        for (int ns=0; ns <= f; ns++)
        {
            double H = utilsLag.H_Wariant2(xd, states,  static_cast<uint>(ns), static_cast<uint>(k), static_cast<uint>(f));
            (*results)->write(TypeStateForServerGroupsCombination::InavailabilityInAllTheGroups, H, ns, combNo);
        }
    }
    //emit this->sigCalculationDone();
}

bool algRekLagGS2::possible(const ModelSyst *system) const
{
    if ((system->vk_s() == 0) || (system->K_sType() > 1) || (system->V_b() > 0))
        return false;

    return Investigator::possible(system);
}

double algRekLagGS2::getSigma(int classNumber, int state)
{
    Utils::UtilsLAG lag;
    double result = 1;
    uint x = static_cast<uint>(system->V() - state);
    int t = system->getClass(classNumber)->t();
    uint k = static_cast<uint>(system->Ks());
    uint f = static_cast<uint>(system->v_s(0));

    if (x <= static_cast<uint>(t-1) * k)
    {
        double A = lag.F(x, k, f,   0);
        double B = lag.F(x, k, static_cast<uint>(t-1), 0);
        result = (A-B)/A;
    }
    return result;
}

} //namespace Algorithms
