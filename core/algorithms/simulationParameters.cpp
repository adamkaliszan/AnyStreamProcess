#include "simulationParameters.h"
#include <qstring.h>


SimulationParameters::SimulationParameters(int noOfSeries, int noOfLostCalls, int spaceBetweenSeries, int spaceOnStart, int noOfServicedCalls)
{
    this->noOfSeries = noOfSeries;
    this->noOfLostCalls = noOfLostCalls;
    this->spaceBetweenSeries = spaceBetweenSeries;
    this->spaceOnStart = spaceOnStart;
    this->noOfServicedCalls = noOfServicedCalls;
}

QString SimulationParameters::showDescription()
{
    QString result;

    if (noOfServicedCalls == 0)
    {
        result = "%1 series: %2 lost (start %3 lost, space %4 lost)";
        result = result.arg(noOfSeries).arg(noOfLostCalls).arg(noOfLostCalls/spaceOnStart).arg(noOfLostCalls/spaceBetweenSeries);
    }
    if (noOfServicedCalls > 0 && noOfLostCalls == 0)
    {
        result = "%1 series: %2 serv (start %3 serv, space %4 serv)";
        result = result.arg(noOfSeries).arg(noOfServicedCalls).arg(noOfServicedCalls/spaceOnStart).arg(noOfServicedCalls/spaceBetweenSeries);
    }
    if (noOfServicedCalls > 0 && noOfLostCalls > 0)
    {
        result = "%1: %2 l or %3 s (st %4 l or %5 s, sp %6 l or %7 s)";
        result = result.arg(noOfSeries).arg(noOfLostCalls).arg(noOfServicedCalls).arg(noOfLostCalls/spaceOnStart).arg(noOfServicedCalls/spaceOnStart).arg(noOfLostCalls/spaceBetweenSeries).arg(noOfServicedCalls/spaceBetweenSeries);
    }

    return result;
}
