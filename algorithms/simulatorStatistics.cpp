#include "simulatorStatistics.h"
#include "utils/lag.h"

namespace Algorithms
{

SystemStatistics::SystemStatistics(const ModelSystem &system)
{
    int vk_sb = system.V();
    int vk_s = system.getServer().V();
    int vk_b = system.getBuffer().V();
    int m = system.m();

    timesPerClasses.resize(m);
    timesPerSystemState.resize(vk_sb + 1);
    eventsPerSystemState.resize(vk_sb + 1);

    timesPerServerAndBufferState.resize(vk_s+1);
    eventsPerServerAndBufferState.resize(vk_s+1);
    for (int ns=0; ns <= vk_s; ns++)
    {
        timesPerServerAndBufferState[ns].resize(vk_b+1);
        eventsPerServerAndBufferState[ns].resize(vk_b+1);
    }

    timesPerClassAndSystemState.resize(m);
    timesPerClassAndServerAndBufferState.resize(m);

    eventsPerClass.resize(m);
    eventsPerClassAndSystemState.resize(m);
    eventsPerClassAndServerStateAndSystem.resize(m);
    for (int i=0; i< m; i++)
    {
        timesPerClassAndSystemState[i].resize(vk_sb+1);
        eventsPerClassAndSystemState[i].resize(vk_sb+1);

        timesPerClassAndServerAndBufferState[i].resize(vk_s+1);
        eventsPerClassAndServerStateAndSystem[i].resize(vk_s+1);

        for (int ns=0; ns <= vk_s; ns++)
        {
            timesPerClassAndServerAndBufferState[i][ns].resize(vk_b+1);
            eventsPerClassAndServerStateAndSystem[i][ns].resize(vk_b+1);
        }
    }
}

void SystemStatistics::clear()
{
    for (int n=0; n<eventsPerClass.length(); n++)
        eventsPerClass[n].statsClear();

    for (int n=0; n<eventsPerSystemState.length(); n++)
        eventsPerSystemState[n].statsClear();

    for (int i=0; i<eventsPerServerAndBufferState.length(); i++)
        for (int n=0; n<eventsPerServerAndBufferState[i].length(); n++)
            eventsPerServerAndBufferState[i][n].statsClear();

    for (int i=0; i<eventsPerClassAndSystemState.length(); i++)
        for (int n=0; n<eventsPerClassAndSystemState[i].length(); n++)
            eventsPerClassAndSystemState[i][n].statsClear();

    for (int i=0; i<eventsPerClassAndServerStateAndSystem.length(); i++)
        for (int n=0; n<eventsPerClassAndServerStateAndSystem[i].length(); n++)
            for (int l=0; l<eventsPerClassAndServerStateAndSystem[i][n].length(); l++)
                eventsPerClassAndServerStateAndSystem[i][n][l].statsClear();

    for (int i=0; i<timesPerClasses.length(); i++)
        timesPerClasses[i].statsClear();

    for (int i=0; i<timesPerSystemState.length(); i++)
        timesPerSystemState[i].statsClear();

    for (int i=0; i<timesPerServerAndBufferState.length(); i++)
        for (int n=0; n<timesPerServerAndBufferState[i].length(); n++)
            timesPerServerAndBufferState[i][n].statsClear();


    for (int i=0; i<timesPerClassAndSystemState.length(); i++)
        for (int n=0; n<timesPerClassAndSystemState[i].length(); n++)
            timesPerClassAndSystemState[i][n].statsClear();

    for (int i=0; i<timesPerClassAndServerAndBufferState.length(); i++)
        for (int n=0; n<timesPerClassAndServerAndBufferState[i].length(); n++)
            for (int l=0; l<timesPerClassAndServerAndBufferState[i][n].length(); l++)
                timesPerClassAndServerAndBufferState[i][n][l].statsClear();
}

void SystemStatistics::collectPre(const ModelSystem &mSystem, double time, int n_s, int n_b
  , const QVector<int> &n_si, const QVector<int> &n_bi, const QVector<int> &n_sk, const QVector<int> &n_bk)
{
    for (int i=0; i<mSystem.m(); i++)
    {
        if (mSystem.isInBlockingState(i, n_sk, n_bk))
            timesPerClasses[i].blockingTime+= time;
    }

    timesPerSystemState[n_s+n_b].occupancyTime+= time;

    timesPerServerAndBufferState[n_s][n_b].occupancyTime+= time;

    for (int i=0; i< mSystem.m(); i++)
    {
        timesPerClassAndSystemState[i][n_s+n_b].occupancyUtilization += (time*(n_si[i] + n_bi[i]));
        timesPerClassAndServerAndBufferState[i][n_s][n_b].occupancyUtilizationServer += (time*(n_si[i]));
        timesPerClassAndServerAndBufferState[i][n_s][n_b].occupancyUtilizationBuffer += (time*(n_bi[i]));
    }
}

void SystemStatistics::collectPost(int classIdx, int old_n, int n, StatisticEventType event)
{
    (void) event;
    if (n == old_n) //nowe zgłoszenie zostało odrzucone
    {
        eventsPerClass[classIdx].outNewOffered++;
        eventsPerClass[classIdx].outNewLost++;

        eventsPerSystemState[old_n].outNewOffered++;
        eventsPerSystemState[old_n].outNewLost++;
        eventsPerClassAndSystemState[classIdx][old_n].outNewOffered++;
        eventsPerClassAndSystemState[classIdx][old_n].outNewLost++;

    }
    else if (n < old_n) //zakończenie obsługi zgłoszenia
    {
        eventsPerClass[classIdx].outEnd++;
        eventsPerClass[classIdx].inEnd++;

        eventsPerSystemState[old_n].outEnd++;
        eventsPerClassAndSystemState[classIdx][old_n].outEnd++;

        eventsPerSystemState[n].inEnd++;
        eventsPerClassAndSystemState[classIdx][n].inEnd++;
    }
    else if (n > old_n) //przyjęcie do obsługi zgłoszenia
    {
        eventsPerClass[classIdx].outNewOffered++;
        eventsPerClass[classIdx].outNewAcceptedByServer++;
        eventsPerClass[classIdx].inNew++;

        eventsPerSystemState[old_n].outNewOffered++;
        eventsPerSystemState[old_n].outNewAcceptedByServer++;

        eventsPerClassAndSystemState[classIdx][old_n].outNewOffered++;
        eventsPerClassAndSystemState[classIdx][old_n].outNewAcceptedByServer++;

        eventsPerSystemState[n].inNew++;
        eventsPerClassAndSystemState[classIdx][n].inNew++;
    }
}

ServerStatistics::ServerStatistics(const ModelSystem &system)
{
    combinationList = Utils::UtilsLAG::getPossibleCombinationsFinal(system.getServer().k());

    eventsPerClass.resize(system.m());
    eventsPerClass.fill(EvenStatistics());

    eventsPerState.resize(system.V()+1);
    eventsPerState.fill(EvenStatistics());

    eventsPerClassAndState.resize(system.m());
    for (int classIdx=0; classIdx<system.m(); classIdx++)
    {
        eventsPerClassAndState[classIdx].resize(system.V()+1);
        eventsPerClassAndState[classIdx].fill(EvenStatistics());
    }

    timesPerState.resize(system.V()+1);
    timesPerState.fill(TimeStatisticsMacroState());

    timesPerClassAndState.resize(system.m());
    for (int classIdx=0; classIdx<system.m(); classIdx++)
    {
        timesPerClassAndState[classIdx].resize(system.V()+1);
        timesPerClassAndState[classIdx].fill(TimeStatisticsMicroState());
    }

    timesPerGroupSets.resize(system.getServer().k()+1);

    for (int noOfNotConsideredGroups=0; noOfNotConsideredGroups <= system.getServer().k(); noOfNotConsideredGroups++)
    {
        timesPerGroupSets[noOfNotConsideredGroups].resize(system.getServer().vMax()+1);
        timesPerGroupSets[noOfNotConsideredGroups].fill(GroupSetStatistics(), system.getServer().vMax()+1);
    }

    timesPerGroupsCombinations.resize(combinationList.length());
    for (int combinationNo=0; combinationNo<combinationList.length(); combinationNo++)
    {
        timesPerGroupsCombinations[combinationNo].resize(system.getServer().vMax()+1);
    }
}

void ServerStatistics::collectPre(const ModelSystem &system, double time, int n, const QVector<int> &n_i, const QVector<int> &n_k)
{
//timesPerState
    timesPerState[n].occupancyTime+= time;

//timesPerClassAndState
    for (int i=0; i< system.m(); i++)
    {
        timesPerClassAndState[i][n].occupancyUtilizationServer = time * n_i[i];
    }
//timesPerGroupSets
    QVector<int> availability;
    availability.fill(0, system.getServer().k()+1);

    availability[0] = 0;
    for (int k=0; k < system.getServer().k(); k++)
    {
        availability[k+1] = system.getServer().V(k) - n_k[k];
    }
    qSort(availability);
    std::reverse(availability.begin(), availability.end());//, std::back_inserter( availability ));
    timesPerGroupSets[0][0].allInSetAvailable+= time;
    timesPerGroupSets[0][0].allInSetAvailableAllOutsideSetUnavailable+= time;
    for (int k=1; k <= system.getServer().k(); k++)
    {
        for (int n=availability[k]+1; n <= availability[k-1]; n++)
            timesPerGroupSets[k][n].allInSetAvailableAllOutsideSetUnavailable+= time;

        for (int n=0; n <= availability[k-1]; n++)
            timesPerGroupSets[k][n].allInSetAvailable+= time;

        for (int n=availability[0]+1; n <= system.getServer().vMax(); n++)
            timesPerGroupSets[k][n].allUnavailable+= time;
    }

//timesPerGroupsCombinations
    for (int combinationNo = 0; combinationNo < combinationList.length(); combinationNo++)
    {
        int k = combinationList[combinationNo].length();
        availability.resize(k);

        int min = system.getServer().vMax();
        int max = 0;
        for (int groupNo=0; groupNo < k; groupNo++)
        {
            availability[groupNo] = system.getServer().V(groupNo) - n_k[groupNo];
            if (availability[groupNo] > max)
                max = availability[groupNo];
            if (availability[groupNo] < min)
                min = availability[groupNo];
        }
        for (int n=0; n<=max; n++)
            timesPerGroupsCombinations[combinationNo][n].oneOrMoreInCombinationAvailable+= time;

        for (int n=0; n<=min; n++)
            timesPerGroupsCombinations[combinationNo][n].allInCombinationAvailable+= time;

        for (int n=max+1; n<=system.getServer().vMax(); n++)
            timesPerGroupsCombinations[combinationNo][n].allInCombinationUnavailable+= time;
    }
}

void ServerStatistics::collectPost(int classIdx, int old_n, int n, StatisticEventType event)
{
//eventsPerClass
    switch (event)
    {
    case StatisticEventType::newCallAccepted:
        eventsPerClass[classIdx].outNewOffered++;
        eventsPerClass[classIdx].outNewAcceptedByServer++;
        eventsPerClass[classIdx].inNew++;
        break;

    case StatisticEventType::newCallRejected:
        eventsPerClass[classIdx].outNewOffered++;
        eventsPerClass[classIdx].outNewLost++;
        break;

    case StatisticEventType::callServiceEnded:
        eventsPerClass[classIdx].outEnd++;
        eventsPerClass[classIdx].inEnd++;
        break;
    }
//eventsPerState
    switch (event)
    {
    case StatisticEventType::newCallAccepted:
        eventsPerState[old_n].outNewOffered++;
        eventsPerState[old_n].outNewAcceptedByServer++;
        eventsPerState[n].inNew++;
        break;

    case StatisticEventType::newCallRejected:
        eventsPerState[old_n].outNewOffered++;
        eventsPerState[old_n].outNewLost++;
        break;

    case StatisticEventType::callServiceEnded:
        eventsPerState[old_n].outEnd++;
        eventsPerState[n].inEnd++;
        break;
    }
//eventsPerClassAndState
    switch (event)
    {
    case StatisticEventType::newCallAccepted:
        eventsPerClassAndState[classIdx][old_n].outNewOffered++;
        eventsPerClassAndState[classIdx][old_n].outNewAcceptedByServer++;
        eventsPerClassAndState[classIdx][n].inNew++;
        break;

    case StatisticEventType::newCallRejected:
        eventsPerClassAndState[classIdx][old_n].outNewOffered++;
        eventsPerClassAndState[classIdx][old_n].outNewLost++;
        break;

    case StatisticEventType::callServiceEnded:
        eventsPerClassAndState[classIdx][old_n].outEnd++;
        eventsPerClassAndState[classIdx][n].inEnd++;
        break;
    }
}

void ServerStatistics::clear()
{      
    eventsPerClass.fill(EvenStatistics());
    eventsPerState.fill(EvenStatistics());
    for (int classNo=0; classNo < eventsPerClassAndState.length(); classNo++)
        eventsPerClassAndState[classNo].fill(EvenStatistics());

    timesPerState.fill(TimeStatisticsMacroState());
    for (int classNo=0; classNo < timesPerClassAndState.length(); classNo++)
        timesPerClassAndState[classNo].fill(TimeStatisticsMicroState());
    for (int groupNo=0; groupNo < timesPerGroupSets.length(); groupNo++)
        timesPerGroupSets[groupNo].fill(GroupSetStatistics());
    for (int combinationNo=0; combinationNo < timesPerGroupsCombinations.length(); combinationNo++)
        timesPerGroupsCombinations[combinationNo].fill(GroupCombinationStatistics());
}

BufferStatistics::BufferStatistics(const ModelSystem &system)
{//TODO
    eventsPerClass.resize(system.m());
    eventsPerState.resize(system.getBuffer().V()+1);
    //QVector<QVector <EvenStatistics> >                eventsPerClassAndState;

    timesPerState.resize(system.getBuffer().V()+1);
    //QVector<QVector<TimeStatisticsMicroState> >       timesPerClassAndState;
}

void BufferStatistics::clear()
{
    eventsPerClass.fill(EvenStatistics());
    eventsPerState.fill(EvenStatistics());

    for (int classIdx=0; classIdx<eventsPerClassAndState.length(); classIdx++)
    {
        eventsPerClassAndState[classIdx].fill(EvenStatistics());
    }

    timesPerState.fill(TimeStatisticsMacroState());

    for (int classIdx=0; classIdx<timesPerClassAndState.length(); classIdx++)
    {
        timesPerClassAndState[classIdx].fill(TimeStatisticsMicroState());
    }
}

void BufferStatistics::collectPre(double time, int n, const QVector<int> &n_i)
{

}

void BufferStatistics::collectPost(int classIdx, int old_n, int n)
{

}

ServerStatistics::State::State(int m): state(0), CallClassIndex(-1), lastEvent(LastEvent::None)
{
    microstate.resize(m);
    servicePossibility.resize(m);
}



}
