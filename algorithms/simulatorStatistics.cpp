#include "simulatorStatistics.h"
#include "utils/lag.h"

namespace Algorithms
{

SystemStatistics::SystemStatistics(const ModelSyst * const system)
{
    int vk_sb = system->V();
    int vk_s = system->vk_s();
    int vk_b = system->vk_b();
    int m = system->m();
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

}

void SystemStatistics::collectPre(double time, int n_s, int n_b, const QVector<int> &n_si, const QVector<int> &n_bi)
{
    timesPerSystemState[n_s+n_b].occupancyTime+= time;

    timesPerServerAndBufferState[n_s][n_b].occupancyTime+= time;

    for (int i=0; i< timesPerClassAndSystemState.length(); i++)
    {
        timesPerClassAndSystemState[i][n_s+n_b].occupancyUtilization += (time*(n_si[i] + n_bi[i]));
        timesPerClassAndServerAndBufferState[i][n_s][n_b].occupancyUtilizationServer += (time*(n_si[i]));
        timesPerClassAndServerAndBufferState[i][n_s][n_b].occupancyUtilizationBuffer += (time*(n_bi[i]));
    }
}

void SystemStatistics::collectPre(const ModelSyst *mSystem, double time, int n_s, int n_b
  , const QVector<int> &n_si, const QVector<int> &n_bi, const QVector<int> &n_sk, const QVector<int> &n_bk)
{
    for (int i=0; i<mSystem->m(); i++)
    {
        if (mSystem->getConstSyst().isInBlockingState(i, n_sk, n_bk))
            timesPerClasses[i].blockingTime+= time;
    }

    timesPerSystemState[n_s+n_b].occupancyTime+= time;

    timesPerServerAndBufferState[n_s][n_b].occupancyTime+= time;

    for (int i=0; i< mSystem->m(); i++)
    {
        timesPerClassAndSystemState[i][n_s+n_b].occupancyUtilization += (time*(n_si[i] + n_bi[i]));
        timesPerClassAndServerAndBufferState[i][n_s][n_b].occupancyUtilizationServer += (time*(n_si[i]));
        timesPerClassAndServerAndBufferState[i][n_s][n_b].occupancyUtilizationBuffer += (time*(n_bi[i]));
    }
}

void SystemStatistics::collectPost(int classIdx, int old_n, int n)
{
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
        eventsPerClass[classIdx].outNewAccepted++;
        eventsPerClass[classIdx].inNew++;

        eventsPerSystemState[old_n].outNewOffered++;
        eventsPerSystemState[old_n].outNewAccepted++;

        eventsPerClassAndSystemState[classIdx][old_n].outNewOffered++;
        eventsPerClassAndSystemState[classIdx][old_n].outNewAccepted++;

        eventsPerSystemState[n].inNew++;
        eventsPerClassAndSystemState[classIdx][n].inNew++;
    }
}

ServerStatistics::ServerStatistics(const ModelSyst * const system)
{
    combinationList = Utils::UtilsLAG::getPossibleCombinationsFinal(system->k_s());

    eventsPerClass.resize(system->m());
    eventsPerClass.fill(EvenStatistics());

    eventsPerState.resize(system->V()+1);
    eventsPerState.fill(EvenStatistics());

    eventsPerClassAndState.resize(system->m());
    for (int classIdx=0; classIdx<system->m(); classIdx++)
    {
        eventsPerClassAndState[classIdx].resize(system->V()+1);
        eventsPerClassAndState[classIdx].fill(EvenStatistics());
    }

    timesPerState.resize(system->V()+1);
    timesPerState.fill(TimeStatisticsMacroState());

    timesPerClassAndState.resize(system->m());
    for (int classIdx=0; classIdx<system->m(); classIdx++)
    {
        timesPerClassAndState[classIdx].resize(system->V()+1);
        timesPerClassAndState[classIdx].fill(TimeStatisticsMicroState());
    }

    timesPerGroupSets.resize(system->k_s()+1);

    for (int noOfNotConsideredGroups=0; noOfNotConsideredGroups <= system->k_s(); noOfNotConsideredGroups++)
    {
        timesPerGroupSets[noOfNotConsideredGroups].resize(system->v_sMax()+1);
        timesPerGroupSets[noOfNotConsideredGroups].fill(GroupSetStatistics(), system->v_sMax()+1);
    }

    timesPerGroupsCombinations.resize(combinationList.length());
    for (int combinationNo=0; combinationNo<combinationList.length(); combinationNo++)
    {
        timesPerGroupsCombinations[combinationNo].resize(system->v_sMax()+1);
    }
}

void ServerStatistics::collectPre(const ModelSyst *mSystem, double time, int n, const QVector<int> &n_i, const QVector<int> &n_k)
{
//timesPerState
    timesPerState[n].occupancyTime+= time;

//timesPerClassAndState
    for (int i=0; i< mSystem->getConstSyst().m; i++)
        timesPerClassAndState[i][n].occupancyUtilization = time * n_i[i];

//timesPerGroupSets
    QVector<int> availability;
    availability.fill(0, mSystem->getConstSyst().ks+1);

    availability[0] = 0;
    for (int k=0; k < mSystem->getConstSyst().ks; k++)
    {
        availability[k+1] = mSystem->getConstSyst().vs[k] - n_k[k];
    }
    qSort(availability);
    std::reverse(availability.begin(), availability.end());//, std::back_inserter( availability ));
    timesPerGroupSets[0][0].allInSetAvailable+= time;
    timesPerGroupSets[0][0].allInSetAvailableAllOutsideSetUnavailable+= time;
    for (int k=1; k <= mSystem->getConstSyst().ks; k++)
    {
        for (int n=0; n <= availability[k-1]; n++)
            timesPerGroupSets[k][n].allInSetAvailable+= time;

        for (int n=availability[k]+1; n <= availability[k-1]; n++)
            timesPerGroupSets[k][n].allInSetAvailable+= time;
    }

//timesPerGroupsCombinations
    for (int combinationNo = 0; combinationNo < combinationList.length(); combinationNo++)
    {
        int k = combinationList[combinationNo].length();
        availability.resize(k);

        int min = mSystem->v_sMax();
        int max = 0;
        for (int groupNo=0; groupNo < k; groupNo++)
        {
            availability[groupNo] = mSystem->getConstSyst().vs[groupNo] - n_k[groupNo];
            if (availability[groupNo] > max)
                max = availability[groupNo];
            if (availability[groupNo] < min)
                min = availability[groupNo];
        }
        for (int n=0; n<=max; n++)
            timesPerGroupsCombinations[combinationNo][n].atLeasOneAvailable = time;

        for (int n=0; n<=min; n++)
            timesPerGroupsCombinations[combinationNo][n].allInCombinationAvailable = time;

        for (int n=max+1; n<=mSystem->v_sMax(); n++)
            timesPerGroupsCombinations[combinationNo][n].allInCombinationUnavailable = time;
    }
}

void ServerStatistics::collectPost(int classIdx, int old_n, int n, StatisticEventType event)
{
//eventsPerClass
    switch (event)
    {
    case StatisticEventType::newCallAccepted:
        eventsPerClass[classIdx].outNewOffered++;
        eventsPerClass[classIdx].outNewAccepted++;
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
        eventsPerClass[old_n].outNewAccepted++;
        eventsPerClass[n].inNew++;
        break;

    case StatisticEventType::newCallRejected:
        eventsPerClass[old_n].outNewOffered++;
        eventsPerClass[old_n].outNewLost++;
        break;

    case StatisticEventType::callServiceEnded:
        eventsPerClass[old_n].outEnd++;
        eventsPerClass[n].inEnd++;
        break;
    }
//eventsPerClassAndState
    switch (event)
    {
    case StatisticEventType::newCallAccepted:
        eventsPerClassAndState[classIdx][old_n].outNewOffered++;
        eventsPerClassAndState[classIdx][old_n].outNewAccepted++;
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

BufferStatistics::BufferStatistics(const ModelSyst * const system)
{

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
