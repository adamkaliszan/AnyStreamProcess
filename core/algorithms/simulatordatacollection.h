#ifndef SIMULATORDATACOLLECTION_H
#define SIMULATORDATACOLLECTION_H


#include <qstack.h>
#include <qvector.h>
#include <qlist.h>
#include <QtGlobal>
#include <QtDebug>

#include <set>

namespace Algorithms
{

template<class P>
class SimulatorDataCollection
{
private:
    QStack<P*> outOfDateProcesses;

    struct Comparator {
        int operator()(const P *lho, const P *rho) const {
            return lho->time < rho->time;
        }
    };

    struct Collection
    {
        double timeOffset;                                           //The ralative time of an events, that are stored on the heap is a sum of its (not always updated) relative time and timeOffset
        std::set<P*, Comparator> set;

        void add(P *newProc)
        {
            newProc->time += timeOffset;
            set.insert(newProc);
        }
        void remove(P *proc)
        {
            set.erase(proc);
        }


        void changeTime(P *proc, double newTime)
        {
            set.erase(proc);
            proc->time = timeOffset+newTime;
            set.insert(proc);
        }

        P *takeFirst()
        {
            auto first = set.begin();
            set.erase(first);
            (*first)->time -= timeOffset;
            return *first;
        }

        void decreaseTime(double time)
        {
            timeOffset += time;
            if (timeOffset > 1024) {
                for (auto iter = std::begin(set); iter != std::end(set); iter++) {
                    (*iter)->time -=timeOffset;
                }
                timeOffset = 0;
            }
        }
    } processes;                                                     // Struct, that sorts the processes according to the "time" field

public:
    SimulatorDataCollection()
    {
        processes.timeOffset = 0;
    }

    ~SimulatorDataCollection()
    {
        foreach(P *proc, processes.set)
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
#ifdef QT_DEBUG
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
