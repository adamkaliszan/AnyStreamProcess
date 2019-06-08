#ifndef SIMULATORDATACOLLECTION_H
#define SIMULATORDATACOLLECTION_H


#include <qstack.h>
#include <qvector.h>
#include <qlist.h>
#include <QtGlobal>
#include <QtDebug>

#include "results/resultsInvestigator.h"

namespace Algorithms
{

template<class P>
class SimulatorDataCollection
{
private:
    QStack<P*> outOfDateProcesses;

    struct BinaryHeap
    {
#ifdef INC_AGENDA_PERFORMANCE
        double timeOffset;                                           //The ralative time of an events, that are stored on the heap is a sum of its (not always updated) relative time and timeOffset
#endif
        u_int32_t capacity;
        u_int32_t len;
        QVector<P*> array;

#ifndef DO_NOT_USE_SECUTIRY_CHECKS
        void consistencyCheck()
        {
            P *tmpProc;
            for (u_int32_t i=0; i<len; i++)
            {
                tmpProc = array[i];

                if (tmpProc->hasCallData() == false)
                    qFatal("Proces without call");

                if (tmpProc->idx != i)
                    qFatal("Wrong index %d should be %d", array[i]->idx, i);
                if (tmpProc->time < timeOffset)
                    qFatal("Time %lf smaller then offset %lf", array[i]->time, timeOffset);
                if (tmpProc->time < array[0]->time)
                    qFatal("Binary heap is not sorted");

                u_int32_t leftIdx = 2*i+1;
                u_int32_t rightIdx = 2*i+2;
                if (leftIdx < len)
                {
                    P* childLeft = array[leftIdx];
                    if (childLeft->time < tmpProc->time)
                        qFatal("Binary heap is not sorted (left child)");
                }
                if (rightIdx < len)
                {
                    P* childRight = array[rightIdx];
                    if (childRight->time < tmpProc->time)
                        qFatal("Binary heap is not sorted (Right child)");
                }
            }
        }
#endif

        void _swap(u_int32_t idx1, u_int32_t idx2)
        {
            P *tmpProc = array[idx1];
            array[idx1] = array[idx2];
            array[idx2] = tmpProc;

            array[idx1]->idx = idx1;
            array[idx2]->idx = idx2;
        }
        void _fixTheHeapUp()
        {
            //qDebug()<<"Fix the heap UP: "<<array;
            u_int32_t tmpIdx = len-1;
            u_int32_t parIdx = len/2-1;

            while (tmpIdx != 0)
            {
                if (array[parIdx]->time > array[tmpIdx]->time)
                    _swap(parIdx, tmpIdx);

                tmpIdx = parIdx;
                parIdx = (parIdx-1)/2;
            }
            //qDebug()<<"               : "<<array;
        }
        void _fixTheHeapUp(int idx)
        {
            //qDebug()<<"Fix the heap UP: "<<array;
            u_int32_t tmpIdx = idx;
            u_int32_t parIdx = (tmpIdx-1)/2;

            while (tmpIdx != 0)
            {
                if (array[parIdx]->time > array[tmpIdx]->time)
                    _swap(parIdx, tmpIdx);

                tmpIdx = parIdx;
                parIdx = (parIdx-1)/2;
            }
            //qDebug()<<"               : "<<array;
        }
        void _fixTheHeapDown(u_int32_t idx)
        {
#ifndef DO_NOT_USE_SECUTIRY_CHECKS
            if (idx >= len)
                qFatal("Wrong index");
#endif
            u_int32_t leftIdx = 2*idx+1;
            u_int32_t rightIdx = leftIdx+1;
            u_int32_t smalerIdx;

            while (leftIdx < len)
            {
                smalerIdx = leftIdx;
                if (rightIdx < len)
                    if (array[leftIdx]->time > array[rightIdx]->time)
                        smalerIdx = rightIdx;

                if (array[idx]->time > array[smalerIdx]->time)
                    _swap(idx, smalerIdx);

                idx = smalerIdx;
                leftIdx = 2*idx+1;
                rightIdx = leftIdx + 1;
            }
        }

        void add(P *newProc)
        {
#ifndef DO_NOT_USE_SECUTIRY_CHECKS
            if (newProc->time < 0)
                qFatal("Negative time value");
            consistencyCheck();
#endif
#if INC_AGENDA_PERFORMANCE
            newProc->time += timeOffset;
#endif
            if (capacity == len)
            {
                capacity *=2;
                array.resize(capacity);
            }
            array[len] = newProc;
            newProc->idx = len;
            len++;
            _fixTheHeapUp();
#ifndef DO_NOT_USE_SECUTIRY_CHECKS
            consistencyCheck();
#endif
        }
        void remove(P *proc)
        {
#ifndef DO_NOT_USE_SECUTIRY_CHECKS
            consistencyCheck();
#endif
            u_int32_t idx = proc->idx;

            if (idx == len-1)
            {
                len--;
                array[len] = NULL;
            }
            else
            {
                len--;
                _swap(idx, len);
                array[len] = NULL;
                _fixTheHeapDown(idx);
                _fixTheHeapUp(idx);
            }
#ifndef DO_NOT_USE_SECUTIRY_CHECKS
            consistencyCheck();
#endif
        }


        void changeTime(P *proc, double newTime)
        {
#ifdef INC_AGENDA_PERFORMANCE
            proc->time = timeOffset+newTime;
#else
            proc->time = newTime;
#endif
            _fixTheHeapDown(proc->idx);
            _fixTheHeapUp(proc->idx);
#ifndef DO_NOT_USE_SECUTIRY_CHECKS
            consistencyCheck();
#endif
        }

        P *takeFirst()
        {
#ifndef DO_NOT_USE_SECUTIRY_CHECKS
            if (this->len == 0)
                qFatal("Agenda is empty");
            consistencyCheck();
#endif
            P *result=array[0];
            len--;
            if (len > 0)
            {
                _swap(0, len);
                array[len] = NULL;
                _fixTheHeapDown(0);
            }
#ifdef INC_AGENDA_PERFORMANCE
            result->time-=timeOffset;
#endif
#ifndef DO_NOT_USE_SECUTIRY_CHECKS
            consistencyCheck();
#endif
            return result;
        }

        void decreaseTime(double time)
        {
#ifdef INC_AGENDA_PERFORMANCE
            timeOffset += time;
            if (timeOffset > 1024)
            {
                for (u_int32_t idx = 0; idx < len; idx++)
                    array[idx]->time -= timeOffset;
                timeOffset = 0;
            }
#else
        for (u_int32_t idx = 0; idx < len; idx++)
            array[idx]->time -= time;
#endif
        }
    } processes;                                                     // Struct, that sorts the processes according to the "time" field

public:
    SimulatorDataCollection()
    {
#ifdef INC_AGENDA_PERFORMANCE
        processes.timeOffset = 0;
#endif
        processes.capacity = 4;
        processes.len = 0;
        processes.array.resize(processes.capacity);
    }

    ~SimulatorDataCollection()
    {
        foreach(P *proc, processes.array)
        {
            delete proc;
        }
        while(outOfDateProcesses.length())
        {
            P *tmp = outOfDateProcesses.pop();
            delete tmp;
        }
    }

    P *takeFirstProcess()
    {
        P *result=processes.takeFirst();
#ifndef DO_NOT_USE_SECUTIRY_CHECKS
        if (result == nullptr)
        {
            qFatal("Can't take next process. The list is empty");
            return nullptr;
        }
#endif
        double time = result->time;

        processes.decreaseTime(time);

        return result;
    }
    inline void addProcess(P *newCall) {processes.add(newCall);}
    void removeProcess(P **outOfDateProcess)
    {

        processes.remove(*outOfDateProcess);
        reuseProcess(*outOfDateProcess);
        outOfDateProcess = nullptr;
    }
    inline void changeProcessWakeUpTime(P *proc, double time) { processes.changeTime(proc, time); }
    P* getNewProcess()
    {
        if (this->outOfDateProcesses.empty())
            return new P();

        P* result = outOfDateProcesses.pop();
        return result;
    }
    void reuseProcess(P *uselessProcess)
    {
        outOfDateProcesses.push(uselessProcess);
    }
};


class SimulationResults
{
public:
    const int m;
    const int Vs;
    const int Vb;
    const int noOfSeries;

    double _simulationTime;

private:
    unsigned int *__act_noOfLostCallsBeforeQeue;          // Liczba zgłoszeń utraconych przed kolejką (bufor był zapełniony)
    unsigned int *__act_noOfLostCallsInQeue;              // Liczba zgłoszeń utraconych w kolejce
    unsigned int *__act_noOfCalls;                        // Liczba zgłoszeń (wszystkich - utraconych i obsłużonych)
    unsigned int *__act_noOfServicedCalls;                // Średnia liczba obsłużonych zgłoszeń

    double *__act_yQ;                             // Średnia liczba zgłoszeń w kolejce - ignorowane statystyki
    double *__act_y;                              // Średnia liczba zgłoszeń w całym systemie
    double *__act_ytQ;                            // Średnia liczba zajętych zasobów przez klasy w kolejce
    double *__act_t;                              // Efektywny czas oczekiwania w kolejce
    double *__act_tQeue;                          // Średni czas oczekiwania w kolejce;
    double *__act_tS;                             // Średni czas obsługi
    double *__act_tServer;                        // Średni czas pobytu w serwerze
    double *__act_tPlanedServer;                  // Średni zaplanowany czas pobytu w serwerze
    double *__act_Qlen;                           // Średnia długość kolejki
    double *__act_E;                              // Prawdopodobieństwo blokady
    double *__act_Y;                              // Ruch obsługiwany

    double ** __act_LOC_qeue_yt;                  // Średnia liczba zgłoszeń w kolejce w zależności od stanu zajętości kolejki
    double ** __act_LOC_server_yt;                // Średnia liczba zgłoszeń w serwerze w zależności od jętości serwera
    double ** __act_SYS_yt;                       // Średnia liczba zgłoszeń w systemie w zależności od stanu zajętości systemu
    double ** __act_SYS_server_yt;                // Średnia liczba zgłoszeń w serwerze w zależności od stanu zajętości systemu
    double ** __act_SYS_qeue_yt;                  // Średnia liczba zgłoszeń w serwerze w zależności od stanu zajętości systemu

    double *__act_trDistribServ;                  // Rozkład zajętości w serwerze
    double *__act_trDistribSys;                   // Rozkład zajętości w systemie
    double *__act_trDistribQeue;                  // Rozkład zajętości w kolejce
    double **__act_trDistrib;                     // Rozkład zajętości [server][kolejka]

public:
    double **yQ,             *act_yQ;             // Średnia liczba zgłoszeń w kolejce
    double **y,              *act_y;              // Średnia liczba zgłoszeń w całym systemie
    double **ytQ,            *act_ytQ;            // Średnia liczba zajętych zasobów przez klasy w kolejce
    double **t,              *act_t;              // Efektywny czas oczekiwania w kolejce
    double **tQeue,          *act_tQeue;          // Średni czas oczekiwania w kolejce
    double **tS,             *act_tS;             // Średni czas obsługi w systemie
    double **tServer,        *act_tServer;        // Średni czas obsługi przez serwer
    double **tPlanedServer,  *act_tPlanedServer;  // Średni zaplanowany czas obsługi

    double *Qlen,            *act_Qlen;           // Średnia długość kolejki
    double **E,              *act_E;              // Prawdopodobieństwo blokady
    double **Y,              *act_Y;              // Ruch obsługiwany

    double **trDistribServ,  *act_trDistribServ;  // Rozkład zajętości w serwerze
    double **trDistribSys,   *act_trDistribSys;   // Rozkład zajętości w systemie
    double **trDistribQeue,  *act_trDistribQeue;  // Rozkład zajętości w kolejce
    double ***trDistrib,     **act_trDistrib;     // Rozkład zajętości server/kolejka

    double **intOutNew,      *act_intOutNew;      // Intensywność wyjścia ze stanu po przyjęciu nowego zgłoszenia
    double **intOutEnd,      *act_intOutEnd;      // Intensywność wyjścia ze stanu po zakończeniu obsługi zgłoszenia
    double **intInNew,       *act_intInNew;       // Intensywność przejścia do stanu po przyjęciu nowego zgłoszenia
    double **intInEnd,       *act_intInEnd;       // Intensywność przejścia do stanu po zakończeniu obsługi zgłoszenia

    unsigned long int **noOutNew,     *act_noOutNew;       // Intensywność wyjścia ze stanu po przyjęciu nowego zgłoszenia
    unsigned long int **noOutEnd,     *act_noOutEnd;       // Intensywność wyjścia ze stanu po zakończeniu obsługi zgłoszenia
    unsigned long int **noInNew,      *act_noInNew;        // Intensywność przejścia do stanu po przyjęciu nowego zgłoszenia
    unsigned long int **noInEnd,      *act_noInEnd;        // Intensywność przejścia do stanu po zakończeniu obsługi zgłoszenia

    double  ***intInNewSC,   **act_intInNewSC;    // Intensywność wyjścia ze stanu po przyjęciu nowego zgłoszenia określonej klasy
    double  ***intInEndSC,   **act_intInEndSC;    // Intensywność wyjścia ze stanu po zakończeniu obsługi zgłoszenia określonej klasy

    double  ***intOutNewSC,  **act_intOutNewSC;   // Intensywność wyjścia ze stanu po przyjęciu nowego zgłoszenia określonej klasy
    double  ***intOutEndSC,  **act_intOutEndSC;   // Intensywność wyjścia ze stanu po zakończeniu obsługi zgłoszenia określonej klasy

    unsigned long int ***noInNewSC,   **act_noInNewSC;     // Liczba wejść do stanu po przyjęciu nowego zgłoszenia określonej klasy
    unsigned long int ***noInEndSC,   **act_noInEndSC;     // Liczba wejść do stanu po zakończeniu obsługi zgłoszenia określonej klasy

    unsigned long int ***noOutNewSC,  **act_noOutNewSC;    // Liczba wyjść ze stanu po przyjęciu nowego zgłoszenia określonej klasy
    unsigned long int ***noOutEndSC,  **act_noOutEndSC;    // Liczba wyjść ze stanu po zakończeniu obsługi zgłoszenia określonej klasy

    double ***LOC_qeue_yt,   **act_LOC_qeue_yt;   // Średnia liczba zgłoszeń w kolejce w zależności od stanu zajętości kolejki
    double ***LOC_server_yt, **act_LOC_server_yt; // Średnia liczba zgłoszeń w serwerze w zależności od jętości serwera
    double ***SYS_yt,        **act_SYS_yt;        // Średnia liczba zgłoszeń w systemie w zależności od stanu zajętości systemu
    double ***SYS_server_yt, **act_SYS_server_yt; // Średnia liczba zgłoszeń w serwerze w zależności od stanu zajętości systemu
    double ***SYS_qeue_yt,   **act_SYS_qeue_yt;   // Średnia liczba zgłoszeń w serwerze w zależności od stanu zajętości systemu

    unsigned int **noOfLostCallsBeforeQeue, *act_noOfLostCallsBeforeQeue;  // Liczba zgłoszeń utraconych przed kolejką (bufor był zapełniony)
    unsigned int **noOfLostCallsInQeue, *act_noOfLostCallsInQeue;          // Liczba zgłoszeń utraconych w kolejce
    unsigned int **noOfCalls, *act_noOfCalls;                              // Liczba zgłoszeń (wszystkich - utraconych i obsłużonych)
    unsigned int **noOfServicedCalls, *act_noOfServicedCalls;              // Średnia liczba obsłużonych zgłoszeń

    SimulationResults(int m, int Vs, int Vb, int noOfSer);
    ~SimulationResults();
    void disableStatisticCollection();
    void enableStatisticscollection(int serNo);

};

} // namespace Algorithms

#endif // SIMULATORDATACOLLECTION_H
