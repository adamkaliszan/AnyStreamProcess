#ifndef SIMULATIONPARAMETERS_H
#define SIMULATIONPARAMETERS_H

#include <QMetaType>
#include <qstring.h>


class SimulationParameters
{
public:
    int noOfSeries;
    int noOfLostCalls;
    int noOfServicedCalls;

    int spaceBetweenSeries;
    int spaceOnStart;

    SimulationParameters(int noOfSeries = 10, int noOfLostCalls = 10000, int spaceBetweenSeries = 100, int spaceOnStart = 10, int noOfServicedCalls = 0);
    QString showDescription();
};

Q_DECLARE_METATYPE(SimulationParameters*)

#endif // SIMULATIONPARAMETERS_H
