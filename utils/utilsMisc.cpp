#include "utilsMisc.h"

namespace Utils
{

UtilsMisc::UtilsMisc()
{

}

void UtilsMisc::suffle(QVector<int> &vector)
{
    int length = vector.length();
    for (int i=0; i<length; i++)
    {
        int tmp = vector[i];
        int randIndex = qrand() % length;

        vector[i] = vector[randIndex];
        vector[randIndex] = tmp;
    }
}

} //namespace Utils

