#ifndef RESULTS_INVESTIGATOR2_H
#define RESULTS_INVESTIGATOR2_H

#include "results/baseTypes.h"
#include "results/resultsSingle.h"

#include "model.h"

namespace Results
{

const double coefficientStudentFisher95[] = {0,
    6.314, 2.920, 2.353, 2.132, 2.015, 1.943, 1.895, 1.860, 1.833, 1.812,
    1.796, 1.782, 1.771, 1.761, 1.753, 1.746, 1.740, 1.734, 1.729, 1.725,
    1.721, 1.717, 1.714, 1.711, 1.708, 1.706, 1.703, 1.701, 1.699, 1.697};

const double coefficientStudentFisher99[] = {0,
    63.656, 9.925, 5.841, 4.4604, 4.032, 3.707, 3.499, 3.355, 3.250, 3.169,
    3.108, 3.055, 3.012, 2.977, 2.947, 2.921, 2.898, 2.878, 2.861, 2.845,
    2.831, 2.819, 2.807, 2.797, 2.787, 2.779, 2.771, 2.763, 2.756, 2.750};

enum class ConfidencyIntervall
{
    Trust95,
    Trust99
};

class RInvestigator
{
public:
    RInvestigator();
    RInvestigator(const RInvestigator& rho);

    void init(const ModelSystem *model, int noOfseries = 0);

    bool hasConfidencyIntervall() const;
    const RSingle& getConfidencyIntervall(ConfidencyIntervall trust=ConfidencyIntervall::Trust95) const;

    RSingle* operator->();
    const RSingle* operator->() const;
    RSingle& operator[](int serialNumber) const;
    RSingle& operator[](int serialNumber);

    RInvestigator& operator=(const RInvestigator &rho);

private:
    const ModelSystem *model;

    mutable bool areStatisticsUpToDate;
    mutable ConfidencyIntervall lastTrust;

    mutable QVector<RSingle> series;
    mutable RSingle avarage;
    mutable RSingle confidencyIntervalls;

    void calculateStatistics(ConfidencyIntervall trust=ConfidencyIntervall::Trust95) const;
};

} // namespace Results


#endif // RESULTS_INVESTIGATOR2_H
