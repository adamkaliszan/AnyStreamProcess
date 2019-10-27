#include "utils/lag.h"

namespace Utils
{

extern UtilsLAG lag;

UtilsLAG::UtilsLAG(): pascalTriangle(1)
{
    pascalTriangle[0] = QVector<qint64>(1);
    pascalTriangle[0][0] = 1;
}

qint64 UtilsLAG::binomial(int n, int k)
{
    if (pascalTriangle.length() <= n)
        pascalTriangleAddRows(static_cast<uint>(n));
    return pascalTriangle[n][k];
}

/**
 * @brief UtilsLAG::F
 * @param x liczba jednostek do rozmieszczenia
 * @param k liczba zbiorów
 * @param f maksymalna pojemność zbioru
 * @param t minimalna liczba jednostek, która musi być przydzielona do zbioru
 * @return liczba możliwych rozmieszczeń
 */
double UtilsLAG::F(unsigned x, unsigned k, unsigned f, unsigned t)
{
    if (k == 0)
    {
        if ((x > 0) || (t > 0))
            qFatal("F: There is 0 groups");
        else
            return 1;
    }
    if (x > k*f)
        qFatal("F: Number os ressorces to allocate %d, system capacity %d", x, k*f);
    if (t * k > x)
        qFatal("F: Not enought resourcess to allocate");

    double wynik = 0;
    uint gr = ((x - k * t) / (f - t + 1));
    for (uint i = 0; i <= gr; i++)
    {
        int znak = (i % 2 == 1) ? -1 : 1;
        double dw1 = binomial(static_cast<int>(k), static_cast<int>(i));
        double dw2 = binomial(static_cast<int>(x - k * (t - 1) - 1 - i * (f - t + 1)), static_cast<int>(k - 1));
        wynik += (znak * dw1 * dw2);
    }
    return wynik;
}

double UtilsLAG::H(unsigned x, QVector<double> stateProbabilitues)
{
    double result = 0;
    int V = stateProbabilitues.length();
    for (int n=static_cast<int>(x); n<=V; n++)
        result += (static_cast<double>(binomial(V - static_cast<int>(x), n - static_cast<int>(x))) / static_cast<double>(binomial(V, n)) * stateProbabilitues[n]);
    return result;
}

/**
 * @brief Probability, that all the x_d groups are available
 * @param x_d number of exactly choosen groups
 * @param groupAvailabilityProbabilities
 * @param k numbed of subgroups
 * @return
 */
double UtilsLAG::Hprime_Wariant1(unsigned x_d, QVector<double> groupAvailabilityProbabilities, unsigned k)
{
    double result = 0;

    //s liczba dostępnych podgrup poza kombinacją
    //x_d liczba dostępnych podgrup w kombinacji (długość kombinacji)
    for (unsigned s=0; s <= k-x_d; s++)
    {
        double tmp = static_cast<double>(binomial(static_cast<int>(k-x_d), static_cast<int>(k-(s+x_d))));       //liczba możliwych rozmieszczeń zajętych podgrup (niesdostępnych) w obrębie podgrup spoza kombinacji
        tmp /= static_cast<double>(binomial(static_cast<int>(k), static_cast<int>(k - (s+x_d))));               //liczba możliwych rozmieszczeń dostępnych podgrup spoza kombinacji we wszystkich podgrupach
        tmp *= groupAvailabilityProbabilities[static_cast<int>(s+x_d)];

        result += tmp;
    }

    return result;
}

/**
 * @brief Probability, that all the x_d groups are NOT available
 * @param x_d number of exactly choosen groups
 * @param groupAvailabilityProbabilities - Distribution of Available Groups
 * @param k numbed of subgroups
 * @return
 */
double UtilsLAG::H_Wariant1(unsigned x_d, QVector<double> groupAvailabilityProbabilities, unsigned k)
{
    double result = 0;

    //s liczba niedostępnych podgrup poza kombinacją
    //x_d liczba niedostępnych podgrup w kombinacji (długość kombinacji)
    for (unsigned s=0; s <= k-x_d; s++)
    {
        double tmp = static_cast<double>(binomial(static_cast<int>(k-x_d), static_cast<int>(k-(s+x_d))));             //liczba możliwych rozmieszczeń dostępnych podgrup w obrębie podgrup spoza kombinacji
        tmp /= static_cast<double>(binomial(static_cast<int>(k), static_cast<int>(k - (s+x_d))));                     //liczba możliwych rozmieszczeń dostępnych podgrup spoza kombinacji we wszystkich podgrupach
        tmp *= groupAvailabilityProbabilities[static_cast<int>(k-(s+x_d))];

        result += tmp;
    }

    return result;
}


double UtilsLAG::H_Wariant2(unsigned x_d, QVector<double> stateProbabilities, unsigned t, unsigned numberOfGroups, unsigned groupCapacity)
{
    double result = 0;
    uint V = numberOfGroups*groupCapacity;

    if (x_d == numberOfGroups)
    {
        for (uint n=static_cast<uint>(groupCapacity-t+1)*x_d; n <= V; n++)                           // n - liczba zajętych zasobów
        {
            double tmp = F(n, x_d, groupCapacity, groupCapacity-t+1);                   // liczba możliwych zrozmieszczeń n_d zasobów w x_d podgrupach, gdy w każdej podgrupie nie ma t dostępnych zasobów
            tmp /= F(n, numberOfGroups, groupCapacity, 0);                              // liczba wszystkich możliwych rozmieszczeń n zasobów w
            tmp *= stateProbabilities[static_cast<int>(n)];
            result +=tmp;
        }
    }
    else
    {
        for (uint n=(groupCapacity-t+1)*x_d; n <= V; n++)                           // n - liczba zajętych zasobów
        {
            unsigned psi = (n <= x_d * groupCapacity) ? n : x_d * groupCapacity;        // psi - maksymalna liczba możliwych zajętoch zasobóœ w x_D podgrupach, gdy łącznie zajęto n zasobów

            for (uint n_d = (groupCapacity-t+1)*x_d; n_d <= psi; n_d++)             // n_d - liczba zajętych zasobów w x_d podgrupach grupach
            {   //w wolne zasoby w x_d łączach
                if ((n-n_d) > (numberOfGroups-x_d)*groupCapacity)
                    continue;
                double tmp = F(n_d, x_d, groupCapacity, groupCapacity-t+1);         // liczba możliwych zrozmieszczeń n_d zasobów w x_d podgrupach, gdy w każdej podgrupie nie ma t dostępnych zasobów
                tmp *= F(n-n_d, numberOfGroups-x_d, groupCapacity, 0);              // liczba możliwych rozmieszczeń pozostałych zasobów w pozostałych podgrupach
                tmp /= F(n, numberOfGroups, groupCapacity, 0);                      // liczba wszystkich możliwych rozmieszczeń n zasobów w
                tmp *= stateProbabilities[static_cast<int>(n)];
                result +=tmp;
            }
        }
    }
    return result;
}

double UtilsLAG::H_Wariant2b(unsigned x_d, QVector<double> stateProbabilities, unsigned t, unsigned numberOfGroups, unsigned groupCapacity)
{
    double result = 0;
    uint V = numberOfGroups*groupCapacity;

    for (uint n=static_cast<uint>(groupCapacity-t+1)*x_d; n <= V; n++)
    {
        uint psi = (n <= x_d * groupCapacity) ? V-n : V-x_d*groupCapacity;

        for (uint w_d = 0; w_d <= psi; w_d++)
        {   //w wolne zasoby w x_d łączach

                double tmp = F(w_d, x_d, groupCapacity, t-1);
                tmp *= F(V-n-w_d, numberOfGroups-x_d, groupCapacity, 0);
                tmp /= F(V-n, numberOfGroups, groupCapacity, 0);
                tmp *= stateProbabilities[static_cast<int>(n)];
                result +=tmp;
        }
    }
    return result;
}

void UtilsLAG::getPossibleCombinationsAppend(
        QVector<QPair<QVector<int>, QVector<int> > >& result
      , QVector<int> level
      , int maxLevel
      , int endPoint)
{
    int startValue = (level.length() == 0) ? 0: level.last()+1;
    if (level.length() + 1 == maxLevel)
    {   //Koniec, nie trzeba więcej rekurencyjnie wywoływać funkcji
        for(int x=startValue; x<endPoint; x++)
        {
            QVector<int> tmpFirst = level;
            tmpFirst.append(x);             //To już ostatnia podgrupa w naszej kombinacji
            QVector<int> tmpSecond;         //Tworzymy listę podgrup, jakie nie są w naszej kombinacji
            for (int complGroup=0; complGroup<endPoint; complGroup++)
            {
                if (!tmpFirst.contains(complGroup))
                    tmpSecond.append(complGroup);
            }
            result.append(QPair<QVector<int>, QVector<int> >(tmpFirst, tmpSecond));
        }
    }
    else
    {   //Rekurencyjnie wywołujemy funkcję po wcześniejszym dodaniu podgrupy
        for(int x=startValue; x<endPoint; x++)
        {
            QVector<int> newLevels = level;
            newLevels.append(x);
            getPossibleCombinationsAppend(result, newLevels, maxLevel, endPoint);
        }
    }
}

QVector<QPair<QVector<int>, QVector<int> > > UtilsLAG::getPossibleCombinations(int numberOfTheGroups)
{
    QVector<QPair<QVector<int>, QVector<int> > > result;

    QVector<int> level;
    for (int noOfConsideredGroups = 1; noOfConsideredGroups<=numberOfTheGroups; noOfConsideredGroups++)
        getPossibleCombinationsAppend(result, level, noOfConsideredGroups, numberOfTheGroups);

    return result;
}

QVector<QVector<int> > UtilsLAG::getPossibleCombinationsFinal(int numberOfTheGroups)
{
    QVector<QPair<QVector<int>, QVector<int> > > tmp = getPossibleCombinations(numberOfTheGroups);
    QVector<QVector<int> > result;

    foreach (auto set, tmp)
    {
        result.append(set.first);
    }

    return result;
}

QString UtilsLAG::getCombinationString(const QVector<int> &combination, QString start, QString stop, QString separator)
{
    QString combinationDescr;

    bool first = true;
    foreach (int groupNo, combination)
    {
        combinationDescr += first ?
                    QString("%1%2").arg(start).arg(groupNo+1) :
                    QString("%1%2").arg(separator).arg(groupNo+1);
        first = false;
    }
    combinationDescr+=stop;
    return combinationDescr;
}

void UtilsLAG::pascalTriangleAddRows(unsigned maxIndex)
{
    uint firstRowIndexToCreate = static_cast<uint>(pascalTriangle.length());
    pascalTriangle.resize(static_cast<int>(maxIndex + 1));
    for (uint rowNo=static_cast<uint>(firstRowIndexToCreate); rowNo<=maxIndex; rowNo++)
    {
        pascalTriangle[static_cast<int>(rowNo)] = QVector<qint64>(static_cast<int>(rowNo+1));
        pascalTriangle[static_cast<int>(rowNo)][0] = pascalTriangle[static_cast<int>(rowNo)][static_cast<int>(rowNo)] = 1;
        for (uint colNo=1; colNo<rowNo; colNo++)
        {
            pascalTriangle[static_cast<int>(rowNo)][static_cast<int>(colNo)]
                    = pascalTriangle[static_cast<int>(rowNo-1)][static_cast<int>(colNo-1)]
                    + pascalTriangle[static_cast<int>(rowNo-1)][static_cast<int>(colNo)];
        }
    }
}

} // namespace Utils

