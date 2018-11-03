#ifndef SIMULATOR_STATISTICS_H
#define SIMULATOR_STATISTICS_H

#include <QVector>

#include "model.h"

namespace Algorithms
{

class TimeStatisticsState
{
public:
    double availabilityTime;  /// Duration time of "state n equivalent", where V-n AUs are available for a single call. FAG state equivalent
    double occupancyTime;     /// State duration time

    TimeStatisticsState() : availabilityTime(0), occupancyTime(0) { }
    inline void statsClear() { availabilityTime = 0; occupancyTime = 0; }
};

class EvenStatistics
{
public:
    long unsigned int inNew;         /// Number of events when this state was reached because of new call acceptance
    long unsigned int inEnd;         /// Number of events when this state was reached because of call service ending

    long unsigned int outNewOffered; /// Number of events when new call was offered in this state (don't care if was accepted or not). New state is possible, but astatistisc are enconted to old one
    long unsigned int outNewAccepted;/// Number of events when new call was offered and accepted. There is new state, but statistics are encounted to old one
    long unsigned int outNewLost;    /// Number of events when new call sas offered and wasn't accepted
    long unsigned int outEnd;        /// Number of events when the state was leaved because of call service ending

    EvenStatistics(): inNew(0), inEnd(0), outNewOffered(0), outNewAccepted(0), outNewLost(0), outEnd(0) { }
    inline void statsClear() { memset(this, 0, sizeof(class EvenStatistics)); }
};


class SystemStatistics
{
private:
    QVector<TimeStatisticsState>                      timesPerSystemState;
    QVector<QVector<TimeStatisticsState> >            timesPerServerAndBufferState;
    QVector<QVector<TimeStatisticsState> >            timesPerClassAndSystemState;
    QVector<QVector<QVector<TimeStatisticsState> > >  timesPerClassAndServerStateAndBuffer;

    QVector<EvenStatistics>                           eventsPerSystemState;
    QVector<QVector<EvenStatistics> >                 eventsPerServerAndBufferState;
    QVector<QVector <EvenStatistics> >                eventsPerClassAndSystemState;
    QVector<QVector <QVector <EvenStatistics> > >     eventsPerClassAndServerStateAndSystem;

public:
    SystemStatistics(const ModelSyst * const system);
    void clear();

    inline const TimeStatisticsState& getTimeStatistics(int systemState)                                   const { return timesPerSystemState[systemState]; }
    inline const TimeStatisticsState& getTimeStatistics(int serverState, int bufferState)                  const { return timesPerServerAndBufferState[serverState][bufferState]; }
    inline const TimeStatisticsState& getTimeStatisticsSC(int classNo, int systemState)                    const { return timesPerClassAndSystemState[classNo][systemState]; }
    inline const TimeStatisticsState& getTimeStatisticsSC(int classNo, int serverState, int bufferState)   const { return timesPerClassAndServerStateAndBuffer[classNo][serverState][bufferState]; }

    inline const EvenStatistics& getEventStatistics(int systemState)                                       const {return eventsPerSystemState[systemState] ;}
    inline const EvenStatistics& getEventStatistics(int serverState, int bufferState)                      const {return eventsPerServerAndBufferState[serverState][bufferState] ;}
    inline const EvenStatistics& getEventStatisticsSC(int classNo, int systemState)                        const {return eventsPerClassAndSystemState[classNo][systemState] ;}
    inline const EvenStatistics& getEventStatisticsSC(int classNo, int serverState, int bufferState)       const {return eventsPerClassAndServerStateAndSystem[classNo][serverState][bufferState] ;}
};

class ServerStatistics
{
public:
    ServerStatistics(const ModelSyst * const system);
};

class BufferStatistics
{
public:
    BufferStatistics(const ModelSyst * const system);
};

}
#endif // SIMULATOR_STATISTICS_H
