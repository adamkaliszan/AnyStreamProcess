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
    timesPerClassAndServerStateAndBuffer.resize(m);

    eventsPerClassAndSystemState.resize(m);
    eventsPerClassAndServerStateAndSystem.resize(m);
    for (int i=0; i< m; i++)
    {
        timesPerClassAndSystemState[i].resize(vk_sb+1);
        eventsPerClassAndSystemState[i].resize(vk_sb+1);

        timesPerClassAndServerStateAndBuffer[i].resize(vk_s+1);
        eventsPerClassAndServerStateAndSystem[i].resize(vk_s+1);

        for (int ns=0; ns <= vk_s; ns++)
        {
            timesPerClassAndServerStateAndBuffer[i][ns].resize(vk_b+1);
            eventsPerClassAndServerStateAndSystem[i][ns].resize(vk_b+1);
        }
    }
}

void SystemStatistics::clear()
{

}

ServerStatistics::ServerStatistics(const ModelSyst * const system)
{

}

BufferStatistics::BufferStatistics(const ModelSyst * const system)
{

}



}
