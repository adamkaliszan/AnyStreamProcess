#ifndef SIMULATOR_STATISTICS_H
#define SIMULATOR_STATISTICS_H

#include <QVector>

#include "model.h"

namespace Algorithms
{

class TimeStatisticsMacroState
{
public:
    double occupancyTime;                  /// State duration time

    TimeStatisticsMacroState() : occupancyTime(0) { }
    inline void statsClear() { occupancyTime = 0; }
};

class TimeStatisticsMicroState
{
public:
    double occupancyUtilization;           /// State duration time of state n * number of AUs occupied by class i

    TimeStatisticsMicroState() : occupancyUtilization(0) { }
    inline void statsClear() { occupancyUtilization = 0; }
};

class TimeStatisticsMicroStateDetail
{
public:
    double occupancyUtilizationServer;     /// State duration time of state n * number of AUs occupied by class i in server
    double occupancyUtilizationBuffer;     /// State duration time of state n * number of AUs occupied by class i in buffer

    TimeStatisticsMicroStateDetail() : occupancyUtilizationServer(0), occupancyUtilizationBuffer(0) { }
    inline void statsClear() { occupancyUtilizationServer = 0; occupancyUtilizationBuffer = 0; }
};

class EvenStatistics
{
public:
    long unsigned int inNew;               /// Number of events when this state was reached because of new call acceptance
    long unsigned int inEnd;               /// Number of events when this state was reached because of call service ending

    long unsigned int outNewOffered;       /// Number of events when new call was offered in this state (don't care if was accepted or not). New state is possible, but astatistisc are enconted to old one
    long unsigned int outNewAccepted;      /// Number of events when new call was offered and accepted. There is new state, but statistics are encounted to old one
    long unsigned int outNewLost;          /// Number of events when new call sas offered and wasn't accepted
    long unsigned int outEnd;              /// Number of events when the state was leaved because of call service ending

    EvenStatistics(): inNew(0), inEnd(0), outNewOffered(0), outNewAccepted(0), outNewLost(0), outEnd(0) { }
    inline void statsClear() { memset(this, 0, sizeof(class EvenStatistics)); }
};


class SystemStatistics
{
private:
    QVector<EvenStatistics>                           eventsPerClass;
    QVector<EvenStatistics>                           eventsPerSystemState;
    QVector<QVector<EvenStatistics> >                 eventsPerServerAndBufferState;
    QVector<QVector <EvenStatistics> >                eventsPerClassAndSystemState;
    QVector<QVector <QVector <EvenStatistics> > >     eventsPerClassAndServerStateAndSystem;

    QVector<TimeStatisticsMacroState>                            timesPerSystemState;
    QVector<QVector<TimeStatisticsMacroState> >                  timesPerServerAndBufferState;
    QVector<QVector<TimeStatisticsMicroState> >                  timesPerClassAndSystemState;
    QVector<QVector<QVector<TimeStatisticsMicroStateDetail> > >  timesPerClassAndServerAndBufferState;

public:
    SystemStatistics(const ModelSyst * const system);
    void clear();

    inline const TimeStatisticsMacroState& getTimeStatistics(int systemState)                                         const { return timesPerSystemState[systemState]; }
    inline const TimeStatisticsMacroState& getTimeStatistics(int serverState, int bufferState)                        const { return timesPerServerAndBufferState[serverState][bufferState]; }
    inline const TimeStatisticsMicroState& getTimeStatisticsSC(int classNo, int systemState)                          const { return timesPerClassAndSystemState[classNo][systemState]; }
    inline const TimeStatisticsMicroStateDetail& getTimeStatisticsSC(int classNo, int serverState, int bufferState)   const { return timesPerClassAndServerAndBufferState[classNo][serverState][bufferState]; }

    inline const EvenStatistics& getEventStatistics(int systemState)                                       const { return eventsPerSystemState[systemState] ;}
    inline const EvenStatistics& getEventStatistics(int serverState, int bufferState)                      const { return eventsPerServerAndBufferState[serverState][bufferState] ;}
    inline const EvenStatistics& getEventStatisticsSC(int classNo)                                         const { return eventsPerClass[classNo] ;}
    inline const EvenStatistics& getEventStatisticsSC(int classNo, int systemState)                        const { return eventsPerClassAndSystemState[classNo][systemState] ;}
    inline const EvenStatistics& getEventStatisticsSC(int classNo, int serverState, int bufferState)       const { return eventsPerClassAndServerStateAndSystem[classNo][serverState][bufferState] ;}


    void collectPre(double time, int n_s, int n_b, const QVector<int> &n_si, const QVector<int> &n_bi);
    void collectPost(int classIdx, int old_n, int n);
};

class GroupSetStatistics
{
public:
    double allInSetAvailableAllOutsideSetUnavailable;
    double allInSetAvailable;
    double atLeastOneInetAvailable;
};

class ServerStatistics
{
private:
    QVector<EvenStatistics>                      eventsPerClass;
    QVector<EvenStatistics>                      eventsPerState;
    QVector<QVector <EvenStatistics> >           eventsPerClassAndState;

    QVector<TimeStatisticsMacroState>            timesPerState;
    QVector<QVector<TimeStatisticsMicroState> >  timesPerClassAndState;


public:
    QVector<QVector <GroupSetStatistics> >       timesPerGroupSets;                         /// Each set is different combination, sec. dimension: State
    QVector<QVector <GroupSetStatistics> >       timesPerGroupSetsSC;                       /// Each set is different combination, sec. dimension: Traffic class
    QVector<QVector <GroupSetStatistics> >       timesPerBestGroupSets;                     /// Each element determines the set that consist of 0, 1, ... k elements
    QVector<QVector <GroupSetStatistics> >       timesPerBestGroupSetsSC;                   /// Each element determines the set that consist of 0, 1, ... k elements

//    QVector<QVector<double> >                    availabilityOnlyInAllGroupsInCombination;  /// Outside combination, the groups are not available
    QVector<QVector<double> >                    availabilityInAllGroupsInCombination;      /// Don't care about the groups outside the combination
    QVector<QVector<double> >                    inavailabilityInAllGroupsInCombination;    /// Don't care about the groups outside the combination

    QVector< QVector<int> > combinationList;

    QVector<QVector<double> >                    freeAUsInWorstGroupInCombination;
    QVector<QVector<double> >                    freeAUsInBestGroupInCombination;

    QVector<QVector<double> >                    availabilityTimeInGroupSet;                /// Availability in given number of groups, other groups are not considered
    QVector<QVector<double> >                    availabilityTimeOnlyInExactNoOfGroups;     /// Availability in EXACT number of groups, other groups are not available


    ServerStatistics(const ModelSyst * const system);

    inline const EvenStatistics& getEventStatistics(int state)                            const { return eventsPerState[state]; }
    inline const EvenStatistics& getEventStatisticsSC(int classNo)                        const { return eventsPerClass[classNo]; }
    inline const EvenStatistics& getEventStatisticsSC(int classNo, int state)             const { return eventsPerClassAndState[classNo][state]; }
    inline const TimeStatisticsMacroState& getTimeStatistics(int state)                   const { return timesPerState[state]; }
    inline const TimeStatisticsMicroState& getTimeStatisticsSC(int classNo, int state)    const { return timesPerClassAndState[classNo][state]; }

    inline const GroupSetStatistics& getTimeGroupSet(int combinationNo, int state)        const { return timesPerGroupSets[combinationNo][state]; }
    inline const GroupSetStatistics& getTimeBestGroupSet(int setPower, int state)         const { return timesPerBestGroupSets[setPower][state]; }
    inline const GroupSetStatistics& getTimeGroupSetSC(int combinationNo, int classIdx)   const { return timesPerGroupSets[combinationNo][classIdx]; }
    inline const GroupSetStatistics& getTimeBestGroupSetSC(int setPower, int classIdx)    const { return timesPerBestGroupSets[setPower][classIdx]; }

    void collectPre(double time, int n, const QVector<int> &n_i);
    void collectPost(int classIdx, int old_n, int n);

    inline const QVector<int>& getSet(int combinationNo)                                  const { return combinationList[combinationNo]; }
    inline int getNoOfSets()                                                              const { return combinationList.length(); }

    void clear();
};

class BufferStatistics
{
private:
    QVector<EvenStatistics>                           eventsPerClass;
    QVector<EvenStatistics>                           eventsPerState;
    QVector<QVector <EvenStatistics> >                eventsPerClassAndState;

    QVector<TimeStatisticsMacroState>                 timesPerState;
    QVector<QVector<TimeStatisticsMicroState> >       timesPerClassAndState;

public:
    BufferStatistics(const ModelSyst * const system);

    inline const EvenStatistics& getEventStatistics(int state)                            const { return eventsPerState[state] ;}
    inline const EvenStatistics& getEventStatisticsSC(int classNo)                        const { return eventsPerClass[classNo] ;}
    inline const EvenStatistics& getEventStatisticsSC(int classNo, int state)             const { return eventsPerClassAndState[classNo][state] ;}
    inline const TimeStatisticsMacroState& getTimeStatistics(int state)                   const { return timesPerState[state]; }
    inline const TimeStatisticsMicroState& getTimeStatisticsSC(int classNo, int state)    const { return timesPerClassAndState[classNo][state]; }

    void clear();

    void collectPre(double time, int n, const QVector<int> &n_i);
    void collectPost(int classIdx, int old_n, int n);
};

}
#endif // SIMULATOR_STATISTICS_H
