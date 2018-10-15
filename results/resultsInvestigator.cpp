#include "resultsInvestigator.h"

#include <qmath.h>

namespace Results
{

RInvestigator::RInvestigator(const ModelSyst *model)
    :  model(model)
    , areStatisticsUpToDate(false)
{

}

RInvestigator::RInvestigator(const RInvestigator &rho)
    : model(rho.model)
    , areStatisticsUpToDate(rho.areStatisticsUpToDate)
    , series(rho.series)
    , avarage(rho.avarage)
    , confidencyIntervalls(rho.confidencyIntervalls)
{

}

void RInvestigator::init(int noOfseries)
{
    avarage.init(model);

    if (noOfseries > 0)
        confidencyIntervalls.init(model);

    series.resize(noOfseries);
    for (int serNo = 0; serNo < noOfseries; serNo++)
        series[serNo].init(model);
}



RSingle *RInvestigator::operator->()
{
    if (series.length() > 0)
        qFatal("Can't modify the avarage");
    areStatisticsUpToDate = false;
    return &avarage;
}

const RSingle *RInvestigator::operator->() const
{
    //if (ser)
    if (!areStatisticsUpToDate && series.length()>0)
        calculateStatistics();
    return &avarage;
}

const RSingle &RInvestigator::getConfidencyIntervall(ConfidencyIntervall trust) const
{
    if (series.length() <=0)
        qFatal("RInvestigator::getConfidencyInterval: not available for this object - no series");

    if (!areStatisticsUpToDate || lastTrust != trust)
        calculateStatistics(trust);
    return this->confidencyIntervalls;
}

RSingle &RInvestigator::operator[](int serialNumber)
{
    if (series.length() == 0)
        qFatal("RInvestigator::operator[]: Series are not supported by this object, use write read functions");

    if (serialNumber >= series.length())
        qFatal("RInvestigator::operator[]: Out of index");

    this->areStatisticsUpToDate = false;
    return series[serialNumber];
}

RSingle &RInvestigator::operator[](int serialNumber) const
{
    if (series.length() == 0)
        qFatal("RInvestigator::operator[]: Series are not supported by this object, use write read functions");

    if (serialNumber >= series.length())
        qFatal("RInvestigator::operator[]: Out of index");

    return series[serialNumber];
}

bool RInvestigator::hasConfidencyIntervall() const
{
    return (series.length() > 1);
}

void RInvestigator::calculateStatistics(ConfidencyIntervall trust) const
{
    if (!areStatisticsUpToDate)
    {
        avarage.clear();
        foreach(RSingle singleSerie, series)
            avarage+= singleSerie;
        avarage/= series.length();
    }

    confidencyIntervalls.clear();
    foreach(RSingle singleSerie, series)
    {
        //RSingle difference = avarage-singleSerie;
        //confidencyIntervalls+= (difference * difference);
        confidencyIntervalls+= ((avarage-singleSerie)^2);
    }
    confidencyIntervalls /= series.length();
    //confidencyIntervalls.sqrt();
    confidencyIntervalls = confidencyIntervalls^0.5;

    switch (trust)
    {
    case ConfidencyIntervall::Trust95:
        confidencyIntervalls*= coefficientStudentFisher95[series.length()];
        break;
    case ConfidencyIntervall::Trust99:
        confidencyIntervalls*= coefficientStudentFisher99[series.length()];
        break;
    }

    areStatisticsUpToDate = true;
    lastTrust = trust;
}

} // namespace Results

