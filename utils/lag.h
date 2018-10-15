#ifndef UTILS_LAG2_H
#define UTILS_LAG2_H

#include <QVector>

namespace Utils
{

class UtilsLAG
{
public:
    UtilsLAG();

    qint64 binomial(int n, int k);

    double F(unsigned x, unsigned k, unsigned f, unsigned t);
    double H(unsigned x, QVector<double> stateProbabilitues);

    double Hprime_Wariant1(unsigned x_d, QVector<double> groupAvailabilityProbabilities, unsigned numberOfGroup);
    double H_Wariant1(unsigned x_d, QVector<double> groupAvailabilityProbabilities, unsigned numberOfGroup);

    double H_Wariant2(unsigned x_d, QVector<double> stateProbabilities, unsigned t, unsigned numberOfGroups, unsigned groupCapacity);

    double H_Wariant2b(unsigned x_d, QVector<double> stateProbabilities, unsigned t, unsigned numberOfGroups, unsigned groupCapacity);


    static QVector<QPair<QVector<int>, QVector<int> > > getPossibleCombinations(int numberOfTheGroups);
    static QString getCombinationString(const QVector<int> &combination, QString start="{", QString stop="}", QString separator=", ");
private:
    void pascalTriangleAddRows(unsigned maxIndex);
    QVector<QVector<qint64>> pascalTriangle;


    static void getPossibleCombinationsAppend(QVector<QPair<QVector<int>, QVector<int> > > &result, QVector<int> level, int maxLevel, int endPoint);
};

extern UtilsLAG lag;

}

#endif // UTILS_LAG2_H
