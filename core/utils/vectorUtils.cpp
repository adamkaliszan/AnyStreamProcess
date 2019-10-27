#include "vectorUtils.h"



vectorUtils::vectorUtils()
{
}

void vectorUtils::normalize(QVector<double> &unormalizedVector, int vectorLength)
{
    if (vectorLength == 0)
        vectorLength = unormalizedVector.length();

    double sum =0;
    for (int n=0; n < vectorLength; n++)
        sum += unormalizedVector[n];

    for (int n=0; n < vectorLength; n++)
        unormalizedVector[n] /= sum;
}
