#ifndef RESULTS_API_TYPES_H
#define RESULTS_API_TYPES_H

#include <QHash>

namespace Results
{

enum class Type
{
    BlockingProbability,
    LossProbability,

    OccupancyDistribution,
    NumberOfCallsInStateN,

    //Parameters for Queue systems
    OccupancyDistributionServerOnly,
    OccupancyDistributionServerBufferOnly,
    NumberOfCallsInStateN_inServer,
    NumberOfCallsInStateN_inBuffer,

    //Parameters for LAG systems
    AllSugbrupsInGivenCombinationAndClassAvailable,     // LAG, all subgroups in a given combination and class are available
    AvailableSubroupDistribution,
};

inline unsigned int qHash(Type key, unsigned int seed)
{
     return ::qHash(static_cast<unsigned int>(key), seed);
}


}

#endif // RESULTS_API_TYPES_H
