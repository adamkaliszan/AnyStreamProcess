#include "simulatorStatistics.h"

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

}

BufferStatistics::BufferStatistics(const ModelSyst * const system)
{

}



}
