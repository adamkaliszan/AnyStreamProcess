#ifndef UTILSMISC_H
#define UTILSMISC_H

#include <QVector>

namespace Utils
{


class UtilsMisc
{
public:
    UtilsMisc();

    static void suffle(QVector<int> &vector);
};

} //namespace Utils

#endif // UTILSMISC_H
