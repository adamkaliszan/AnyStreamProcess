#ifndef VECTORUTILS_H
#define VECTORUTILS_H

#include <QVector>

class vectorUtils
{
public:
    vectorUtils();

    static void normalize(QVector<double>& unormalizedVector, int vectorLength=0);
};

#endif // VECTORUTILS_H
