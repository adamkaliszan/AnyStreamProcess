#ifndef RESULTS_BASE_TYPES_H
#define RESULTS_BASE_TYPES_H

namespace Results
{

enum class TypeGeneral: unsigned
{
    SystemUtilization,     /// Expected value of system state probability distribution
    ServerUtilization,     /// Expected value of server state probability distribution
    BufferUtilization,     /// Expected value of buffer state probability distribution - buffer length
    TotalTime,             /// Tatal time between call arrival and leawing the system wait+service time
    ServiceTime,           /// Total time of service
    WaitingTime,           /// Total time of waiting in the buffer
};

enum class TypeForClass: unsigned
{
    BlockingProbability = 0,        /// Blocking probability
    LossProbability,                /// Loss probability or Sigma coefficient
    AvarageNumbersOfCallsInSystem,  /// Avarage number of calls in whole system (server + buffer)
    AvarageNumbersOfCallsInServer,  /// Avarage number of calls in server
    AvarageNumbersOfCallsInBuffer,  /// Avarage number of calls in buffer. Only for systems with buffer
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

enum class TypeForBufferState: unsigned
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

enum class TypeForServerAngBufferState: unsigned
{
    StateProbability
};


/**
 * @brief The TypeForClassAndSystemState enum
 * Depends on:
 * - system state
 * - traffic class
 */
enum class TypeForClassAndSystemState: unsigned
{
    UsageForServer,                      /// (yServer_i(nSystem) t_i / nSystem
    UsageForBuffer,                       /// (yBuffer_i(nSystem) t_i / nSystem
    UsageForSystem,                      /// (y_i(n) t_i)/n

    CAC_ProbabilityForServer,            /// \signa_i(n)
    CAC_ProbabilityForQueue,             /// \signa_i(n)
    CAC_ProbabilityForSystem,            /// \signa_i(n)

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

/**
 * @brief The TypeForClassAndServerState enum
 * Depends on:
 * - server state (buffer state is not considered)
 * - traffic class
 */
enum class TypeForClassAndServerState: unsigned
{
    Usage,                      ///< (y_i(n) t_i)/n
    CAC_Probability,            ///< \signa_i(n)
    OfferedNewCallIntensityOut,
    RealNewCallIntensityOut,
    NewCallIntensityIn,
    EndCallIntensityOut,
    EndCallIntensityIn
};

enum class TypeForClassAndBufferState: unsigned
{
    Usage,                      /// (y_i(n) t_i)/n
    CAC_Probability,            /// \signa_i(n)
    OfferedNewCallIntensityOut,
    RealNewCallIntensityOut,
    NewCallIntensityIn,
    EndCallIntensityOut,
    EndCallIntensityIn
};

enum class TypeResourcess_VsServerGroupsCombination: unsigned
{
    FreeAUsInBestGroup,                  /// The best  subgroup has exactly n AUs available (not more)
    AvailabilityOnlyInAllTheGroups,      /// Each group in combination is available, but groups outside combination are not available
    AvailabilityInAllTheGroups,          /// Each group in combination is available, don't care about other groups
    InavailabilityInAllTheGroups,        /// Each group in combination is not available, don't care about other groups
};

/**
 * @brief The QoS resultus vs: 1) minimum number resourcess in a single group 2) number of server groups
 */
enum class TypeForResourcessAndNumberOfServerGroups: unsigned
{
    AvailabilityOnlyInAllTheGroups,      /// All the subgroups in the set has this availability, groups outside the sat does not have such availability
    AvailabilityInAllTheGroups,          /// All the subgroups in the set has this availability, groups outside are not considered
    InavailabilityInAllTheGroups         /// All the subgroups in the set are not available, groups outside are not considered
};

enum class TypeClassForServerBestGroupsSet: unsigned
{
    ServPossibilityInBestSubgroup,
    ServPossibilityOnlyInAllTheSubgroups,
    ServPossibilityInAllTheSubgroups,
    ServImpossibilityInAllTheSubgroups,
};
} // namespace Results

#endif // RESULTS_BASE_TYPES_H
