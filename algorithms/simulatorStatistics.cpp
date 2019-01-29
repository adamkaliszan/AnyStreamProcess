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
    timesPerSystemState[n_s+n_b].occupancyTime+= time;

    timesPerServerAndBufferState[n_s][n_b].occupancyTime+= time;

    for (int i=0; i< mSystem->m(); i++)
    {
        double blockingTime = time;
        for (int k=0; k<mSystem->vk_s(); k++)
        {
            if (mSystem->getConstSyst().vs[k] - n_sk[i] >= mSystem->getConstSyst().t[i])
            {
                blockingTime = 0;
                break;
            }
        }
        if (blockingTime > 0)
        {
            for (int k=0; k<mSystem->vk_b(); k++)
            {
                if (mSystem->getConstSyst().vb[k] - n_bk[i] >= mSystem->getConstSyst().t[i])
                {
                    blockingTime = 0;
                    break;
                }
            }
        }
        this->timesPerClasses[i].blockingTime+= blockingTime;

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


    timesPerState.resize(system->V()+1);
    timesPerState.fill(TimeStatisticsMacroState());

    timesPerClassAndState.resize(system->m());
    for (int classIdx=0; classIdx<system->m(); classIdx++)
    {
        timesPerClassAndState[classIdx].resize(system->V()+1);
        timesPerClassAndState[classIdx].fill(TimeStatisticsMicroState());
    }


    timesPerGroupSets.resize(system->k_s()+1);
    timesPerGroupSetsSC.resize(system->k_s()+1);
    timesPerBestGroupSets.resize(system->k_s()+1);
    timesPerBestGroupSetsSC.resize(system->k_s()+1);
    for (int noOfNotConsideredGroups=0; noOfNotConsideredGroups <= system->k_s(); noOfNotConsideredGroups++)
    {
        timesPerGroupSets[noOfNotConsideredGroups].resize(system->v_sMax()+1);
        timesPerGroupSets[noOfNotConsideredGroups].fill(GroupSetStatistics(), system->v_sMax()+1);

        timesPerGroupSetsSC[noOfNotConsideredGroups].resize(system->v_sMax()+1);
        timesPerGroupSetsSC[noOfNotConsideredGroups].fill(GroupSetStatistics(), system->v_sMax()+1);

        timesPerBestGroupSets[noOfNotConsideredGroups].resize(system->v_sMax()+1);
        timesPerBestGroupSets[noOfNotConsideredGroups].fill(GroupSetStatistics(), system->v_sMax()+1);

        timesPerBestGroupSetsSC[noOfNotConsideredGroups].resize(system->v_sMax()+1);
        timesPerBestGroupSetsSC[noOfNotConsideredGroups].fill(GroupSetStatistics(), system->v_sMax()+1);
    }

    timesPerGroupsCombinations.resize(combinationList.length());
    for (int combinationNo=0; combinationNo<combinationList.length(); combinationNo++)
    {

        timesPerGroupsCombinations[combinationNo].fill(GroupSetStatistics(), system->v_sMax()+1);
    }


    timesPerState.resize(system->vk_s()+1);
}

void ServerStatistics::collectPre(double time, int n, const QVector<int> &n_i)
{
    timesPerState[n].occupancyTime+= time;
    for (int i=0; i< n_i.length(); i++)
        timesPerClassAndState[i][n].occupancyUtilization = time * n_i[i];

}

void ServerStatistics::collectPre(const ModelSyst *mSystem, double time, int n, const QVector<int> &n_i, const QVector<int> &n_k)
{
    timesPerState[n].occupancyTime+= time;
    for (int i=0; i< mSystem->getConstSyst().m; i++)
        timesPerClassAndState[i][n].occupancyUtilization = time * n_i[i];

    timesPerBestGroupSets tmpAvailabilityInGroups.resize(k);
    /*
    // Uaktualnianie informacji o liczbie dostępnych zasobów w danej grupie
    for (int tmpK=0; tmpK<k; tmpK++)
        tmpAvailabilityInGroups[tmpK] = groups[tmpK]->getNoOfFreeAUs(true);

    // Zapis statystyk dla kombinacji podgrup
    for (int combinatinNo=0; combinatinNo < combinationList.length(); combinatinNo++)
    {
        int minAvailability = V;
        int maxAvailability = 0;
        int complementaryMaxAvailability = -1;
        foreach (int groupNo, combinationList[combinatinNo].first)
        {
            minAvailability = qMin<int>(minAvailability, tmpAvailabilityInGroups[groupNo]);
            maxAvailability = qMax<int>(maxAvailability, tmpAvailabilityInGroups[groupNo]);
        }
        foreach (int complGroupNo, combinationList[combinatinNo].second)
        {
            complementaryMaxAvailability
                    = qMax<int>(complementaryMaxAvailability, tmpAvailabilityInGroups[complGroupNo]);
        }

        freeAUsInWorstGroupInCombination[combinatinNo][minAvailability]+= time;
        freeAUsInBestGroupInCombination[combinatinNo][maxAvailability]+= time;

        for (int n=minAvailability; n>complementaryMaxAvailability; n--)
            availabilityOnlyInAllGroupsInCombination[combinatinNo][n]+= time;

        for (int n=minAvailability; n>=0; n--)
            availabilityInAllGroupsInCombination[combinatinNo][n]+= time;

        for (int n=maxAvailability+1; n<=vMax; n++)
            inavailabilityInAllGroupsInCombination[combinatinNo][n]+= time;
    }

    // Zapis statystyk dla określonej liczby dowolnych podgrup


    availabilityTimeInGroupSet[0][0] += time;

    for (int availableResourcess=0; availableResourcess <= vMax; availableResourcess++)
    {
        int noOfGroups = 0;

        for (int i=0; i<k; i++)
        {
            if (tmpAvailabilityInGroups[i] >= availableResourcess)
                noOfGroups++;
        }
        for (int i=0; i<=noOfGroups; i++)
            availabilityTimeInGroupSet[i][availableResourcess]+= time;
        availabilityTimeOnlyInExactNoOfGroups[noOfGroups][availableResourcess] += time;
    }

    for (int i=0; i<m; i++)
    {
        resourceUtilizationByClass[i]          += (time*n_i[i]);
        resourceUtilizationByClassInState[i][n]+= (time*n_i[i]);
    }
    timePerState[n].occupancyTime += time;

    int maxCallRequirement = getMaxNumberOfAsInSingleGroup();
    //timePerState[V-maxCallRequirement].availabilityTime += time;

    foreach(Call *tmpCall, this->calls)
        tmpCall->IncrTimeInServer(time);

    for (int groupNo=0; groupNo < k; groupNo++)
        groups[groupNo]->statsColectPre(time);


    // Uaktualnianie informacji o liczbie dostępnych zasobów w danej grupie
    tmpAvailabilityInGroups.resize(k);
    for (int tmpK=0; tmpK<k; tmpK++)
        tmpAvailabilityInGroups[tmpK] = groups[tmpK]->getNoOfFreeAUs(true);

    // Zapis statystyk dla kombinacji podgrup
    for (int combinatinNo=0; combinatinNo < combinationList.length(); combinatinNo++)
    {
        int minAvailability = V;
        int maxAvailability = 0;
        int complementaryMaxAvailability = -1;
        foreach (int groupNo, combinationList[combinatinNo].first)
        {
            minAvailability = qMin<int>(minAvailability, tmpAvailabilityInGroups[groupNo]);
            maxAvailability = qMax<int>(maxAvailability, tmpAvailabilityInGroups[groupNo]);
        }
        foreach (int complGroupNo, combinationList[combinatinNo].second)
        {
            complementaryMaxAvailability
                    = qMax<int>(complementaryMaxAvailability, tmpAvailabilityInGroups[complGroupNo]);
        }

        freeAUsInWorstGroupInCombination[combinatinNo][minAvailability]+= time;
        freeAUsInBestGroupInCombination[combinatinNo][maxAvailability]+= time;

        for (int n=minAvailability; n>complementaryMaxAvailability; n--)
            availabilityOnlyInAllGroupsInCombination[combinatinNo][n]+= time;

        for (int n=minAvailability; n>=0; n--)
            availabilityInAllGroupsInCombination[combinatinNo][n]+= time;

        for (int n=maxAvailability+1; n<=vMax; n++)
            inavailabilityInAllGroupsInCombination[combinatinNo][n]+= time;
    }

    // Zapis statystyk dla określonej liczby dowolnych podgrup


    availabilityTimeInGroupSet[0][0] += time;

    for (int availableResourcess=0; availableResourcess <= vMax; availableResourcess++)
    {
        int noOfGroups = 0;

        for (int i=0; i<k; i++)
        {
            if (tmpAvailabilityInGroups[i] >= availableResourcess)
                noOfGroups++;
        }
        for (int i=0; i<=noOfGroups; i++)
            availabilityTimeInGroupSet[i][availableResourcess]+= time;
        availabilityTimeOnlyInExactNoOfGroups[noOfGroups][availableResourcess] += time;
    }
     */

}

void ServerStatistics::collectPost(int classIdx, int old_n, int n)
{
    //for (int groupNo=0; groupNo < k; groupNo++)
    //    groups[groupNo]->statsCollectPost(classIdx);
}

void ServerStatistics::clear()
{
    /*
    for (int n=0; n<=V; n++)
        timePerState[n].statsClear();

    for (int groupNo=0; groupNo < k; groupNo++)
    {
        groups[groupNo]->statsClear();
    }

    for (int combinatinNo=0; combinatinNo < combinationList.length(); combinatinNo++)
    {        freeAUsInWorstGroupInCombination[combinatinNo].fill(0, vMax+1);
        freeAUsInBestGroupInCombination[combinatinNo].fill(0, vMax+1);
        availabilityOnlyInAllGroupsInCombination[combinatinNo].fill(0, vMax+1);
        availabilityInAllGroupsInCombination[combinatinNo].fill(0, vMax+1);
        inavailabilityInAllGroupsInCombination[combinatinNo].fill(0, vMax+1);
    }

    for (int noOfGroups=0; noOfGroups <= k; noOfGroups++)
    {
        availabilityTimeInGroupSet[noOfGroups].fill(0, vMax+1);
        availabilityTimeOnlyInExactNoOfGroups[noOfGroups].fill(0, vMax+1);
    }
    */
}

BufferStatistics::BufferStatistics(const ModelSyst * const system)
{

}

void BufferStatistics::clear()
{

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
