#ifndef RESULTS_BASE_TYPES_H
#define RESULTS_BASE_TYPES_H

namespace Results
{

enum class TypeForClass: unsigned
{
    BlockingProbability = 0,    // Blocking probability
    LossProbability,            // Loss probability or Sigma coefficient
    CongestionTraffic           // Avarage service traffic (number of occupied resourcess)
    //Time,                       // Avarage time in system
    //TimeInServer,               // Avarage time in system
    //TimeInQeue                  // Avarage time in system
};

enum class TypeForServerState: unsigned
{
    StateProbability,
    IntensityNewCallOut,
    IntensityEndCallOut,
    IntensityNewCallIn,
    IntensityEndCallIn
};

enum class TypeForQueueState: unsigned
{
    StateProbability,
    IntensityNewCallOut,
    IntensityEndCallOut,
    IntensityNewCallIn,
    IntensityEndCallIn
};

enum class TypeForSystemState: unsigned
{
    StateProbability,
    IntensityNewCallOut,
    IntensityEndCallOut,
    IntensityNewCallIn,
    IntensityEndCallIn
};


enum class TypeForClassAndServerState: unsigned
{
    Usage,                /// (y_i(n) t_i)/n
    CAC_Probability,            /// \signa_i(n)
    OfferedNewCallIntensityOut, //
    RealNewCallIntensityOut,    //
    NewCallIntensityIn,     //
    EndCallIntensityOut,
    EndCallIntensityIn
};

enum class TypeForClassAndQueueState: unsigned
{
    Usage,                /// (y_i(n) t_i)/n
    CAC_Probability,            /// \signa_i(n)
    OfferedNewCallIntensityOut, //
    RealNewCallIntensityOut,    //
    NewCallIntensityIn,     //
    EndCallIntensityOut,
    EndCallIntensityIn
};

enum class TypeForClassAndSystemState: unsigned
{
    UsageForServer,          ///
    UsageForQueue,           ///
    UsageForSystem,          /// (y_i(n) t_i)/n

    CAC_ProbabilityForServer,              /// \signa_i(n)
    CAC_ProbabilityForQueue,               /// \signa_i(n)
    CAC_ProbabilityForSystem,              /// \signa_i(n)

    OfferedNewCallIntensityOutForServer, //
    OfferedNewCallIntensityOutForQueue, //
    OfferedNewCallIntensityOutForSystem, //

    RealNewCallIntensityOutForServer,
    RealNewCallIntensityOutForQueue,
    RealNewCallIntensityOutForSystem,

    NewCallIntensityInForServer,     //
    NewCallIntensityInForQueue,      //
    NewCallIntensityInForSystem,     //

    EndCallIntensityOutForServer,
    EndCallIntensityOutForQueue,
    EndCallIntensityOutForSystem,

    EndCallIntensityInForServer,
    EndCallIntensityInForQueue,
    EndCallIntensityInForSystem
};

enum class TypeStateForServerGroupsCombination: unsigned
{
    FreeAUsInBestGroup,             /// The best  subgroup has exactly n AUs available (not more)
    FreeAUsInEveryGroup,            /// The worst subgroup has exactly n AUs available (not more)
    AvailabilityOnlyInAllTheGroups, /// Each group in combination is available, but groups outside combination are not available
    AvailabilityInAllTheGroups,     /// Each group in combination is available, don't care about other groups
    InavailabilityInAllTheGroups,   /// Each group in combination is not available, don't care about other groups
};

enum class TypeStateForServerGroupsSet: unsigned
{
    AvailabilityOnlyInAllTheGroups,      /// All the subgroups in the set has this availability, groups outside the sat does not have such availability
    AvailabilityInAllTheGroups,          /// All the subgroups in the set has this availability, groups outside are not considered
    InavailabilityInAllTheGroups         /// All the subgroups in the set are not available, groups outside are not considered
};

enum class TypeClassForServerGroupsCombination: unsigned
{
    SerPossibilityInBestSubgroup,
    SerPossibilityOnlyInAllTheSubgroups,
    SerPossibilityInAllTheSubgroups,
    SerImpossibilityInAllTheSubgroups,
};

enum class TypeClassForServerBestGroupsSet: unsigned
{
    ServPossibilityInBestSubgroup,
    ServPossibilityOnlyInAllTheSubgroups,
    ServPossibilityInAllTheSubgroups,
    ServImpossibilityInAllTheSubgroups,
};

enum class TypeClassForServerExactGroupsSet: unsigned
{
    ServPossibilityInBestSubgroup,
    ServPossibilityOnlyInAllTheSubgroups,
    ServPossibilityInAllTheSubgroups,
    ServImpossibilityInAllTheSubgroups,
};

} // namespace Results

#endif // RESULTS_BASE_TYPES_H
