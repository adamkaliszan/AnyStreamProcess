#ifndef RESULTS_API_TYPES_H
#define RESULTS_API_TYPES_H

#include <QHash>

namespace Results
{

enum class Type
{
// Basic parameters
    BlockingProbability,
    LossProbability,

    OccupancyDistribution,
    NumberOfCallsInSystemVsSystemState,

  //Service and arrival intensity
    NewCallOfSingleClassIntensityOut_inSystemVsSystemState,
    NewCallOfSingleClassIntensityIn_inSystemVsSystemState,
    EndCallOfSingleClassIntensityOut_inSystemVsSystemState,
    EndCallOfSingleClassIntensityIn_inSystemVsSystemState,

    NewCallOfAllClassesIntensityOut_inSystemVsSystemState,
    NewCallOfAllClassesIntensityIn_inSystemVsSystemState,
    EndCallOfAllClassesIntensityOut_inSystemVsSystemState,
    EndCallOfAllClassesIntensityIn_inSystemVsSystemState,

//Parameters for systems with buffer
    OccupancyDistributionServerOnly,
    OccupancyDistributionBufferOnly,

    NumberOfCallsInServerVsServerState,
    NumberOfCallsInServerVsSystemState,
    NumberOfCallsInBufferVsBufferState,
    NumberOfCallsInBufferVsSystem,

    NewCallOutIntensityServerVsSystemState,
    NewCallInIntensityServerVsSystemState,
    EndCallOutIntensityServerVsSystemState,
    EndCallInIntensityServerVsSystemState,

    NewCallOutIntensityBufferVsSystemState,
    NewCallInIntensityBufferVsSystemState,
    EndCallOutIntensityBufferVsSystemState,
    EndCallInIntensityBufferVsSystemState,

//Parameters for LAG systems
    AllSugbrupsInGivenCombinationAvailableForCallsOfGivenClass,        // LAG, all subgroups in a given combination and class are available
    AllSugbrupsInGivenCombinationNotAvailableForCallsOfGivenClass,     // LAG, all subgroups in a given combination and class are NOT available
    AvailableSubroupDistribution
};

inline unsigned int qHash(Type key, unsigned int seed)
{
     return ::qHash(static_cast<unsigned int>(key), seed);
}


}

#endif // RESULTS_API_TYPES_H
