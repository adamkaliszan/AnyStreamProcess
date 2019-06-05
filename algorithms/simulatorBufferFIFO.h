#ifndef SIMULATOR_BUFFER_FIFO_H
#define SIMULATOR_BUFFER_FIFO_H

#include <qglobal.h>
#include <qstring.h>
#include <QStack>
#include <QList>
#include <math.h>
#include "simulator.h"
#include "simulatordatacollection.h"
#include "results/resultsInvestigator.h"

namespace Algorithms
{

//template <class P, class C>class simulatorDataCollection;
class ProcBufferFifo;
class CallData;

class SimulatorBufferFifo: public Simulator
{
public:
    SimulatorBufferFifo();


    QString shortName() { return "Sim. Buffer FIFO"; }
    int complexity()    { return 100;}
    void calculateSystem(const ModelSyst *system, double a, Results::RInvestigator *results, SimulationParameters *simParameters);
    bool possible(ModelSyst *system);

    inline double getAvarageNumberOfCalls(int i) {return systemForSimulation->avgNumberOfCalls[i];}


public:
    class System;
    class Server;
    class Buffer;
    struct Call;

    class System
    {
    private:
        Server *server;
        Buffer *buffer;
        QStack<Call *> uselessCalls;
        QList<Call *> callsInSystem;
        SimulatorDataCollection<ProcBufferFifo> *agenda;

        double **yQ, *act_yQ, *__act_yQ;        //Średnia liczba zgłoszeń w kolejce
        double **y, *act_y, *__act_y;           //Średnia liczba zgłoszeń w systemie
        double **t, *act_t, *__act_t;           //Średni czas oczekiwania w kolejce
        double **tS, *act_tS, *__act_tS;        //Średni czas obsługi
        double *Qlen, *act_Qlen, *__act_Qlen;   //Średnia długość kolejki
        double **E, *act_E, *__act_E;           //Prawdopodobieństwo blokady
        double **Y, *act_Y, *__act_Y;           //Ruch obsługiwany
        //double *B, *act_B, *__act_B;

        int **noOfLostCallsBeforeQeue, *act_noOfLostCallsBeforeQeue, *__act_noOfLostCallsBeforeQeue;  //Liczba zgłoszeń utraconych przed kolejką (bufor był zapełniony)
        int **noOfLostCallsInQeue, *act_noOfLostCallsInQeue, *__act_noOfLostCallsInQeue;              //Liczba zgłoszeń utraconych w kolejce
        int **noOfCalls, *act_noOfCalls, *__act_noOfCalls;                                            //Liczba zgłoszeń (wszystkich - utraconych i obsłużonych)
        int **noOfServicedCalls, *act_noOfServicedCalls, *__act_noOfServicedCalls;                    //Średnia liczba obsłużonych zgłoszeń

        double _simulationTime;

        Call *_getNewCall();
        void reuseCall(Call **callToReuse);
        void removeCallFromServer(Call *call);
        void removeCallFromBuffer(Call *call);

        void clearTheStats();

    public:
        double *avgNumberOfCalls;
        int totalNumberOfLostCalls;

        const ModelSyst *systemData;

        System(const ModelSyst *system, int numberOfSeries);

        void initialize(double a, int sumPropAt, int V);
        void doSimExperiment(int numberOfLostCall, int seed);

        void writeResults(RSingle &results);


        void collectTheStat(double time);
        void disableStatisticCollection();
        void enableStatisticscollection(int serNo);
        void writesResultsOfSingleExperiment();

        int getServerNumberOfFreeAS();
        double getQeueNumberOfFreeDU();
        double getBufferNumberOfFreeAS();


        Call *getNewCall(Call *parent);
        Call *getNewCall(const ModelTrClass *trClass, int classIdx, double sourceIntensity, double serviceIntensity);


        void addCall(Call *newCall);
        void FinishCall(Call **call, bool acceptedToService, bool servicedToTheEnd);

        ProcBufferFifo* getNewProcess();
        void addProcess(ProcBufferFifo *proc);
        void changeProcessWakeUpTime(ProcBufferFifo *proc, double newTime);
        ProcBufferFifo *takeFirstProcess();
        void RemoveProcess(ProcBufferFifo **proc);
        void reuseProcess(ProcBufferFifo **proc);

        void serveCallsInEque();


    };

    class Server
    {
    private:
        System *system;
        int V;
        int n;
        double *occupancyTimes;

    public:
        void clearTheStats();
        void collectTheStats(double time);
        inline int getNoOfMaxFreeAS() { return V - n; }

        void addCall(Call *call);
        void changeCallsResourcess(Call *call, int newServerDataRate);

        void release(int NoOfAS);

        Server(int noOfAS, System *system);
    };

    class Buffer
    {
    private:
        System *system;
        int V;                          /// Number of Allocated Slots, that the buffer is able to handle
        int n;                           /// Number of AS that is used by the calls
        double *occupancyTimes;

        double V_du;                     /// Buffer Capacity (in Data Units)
        double n_du;                     /// Number of occupied bytes in the buffer

        QStack<Call *> callsWaitingInBuffer;     /// FiFo Qeue with calls
        QList<Call *> callsInServerAndBuffer;       /// Calls that are read by the server

        int   *numberOfCalls;           /// Actual number of calls that are awainting in qeue
        double *avgNumberOfCalls;        /// Number of calls multiplied by the time in qeue

    public:
        Buffer(int V, double V_du, System *system);

        void clearTheStats();
        void collectTheStats(double time);

        double getOccupancyTimeOfState(int n);
        void writeOccupancyTime(double time);

        inline double getAvarageNumberOfCalls(int classIdx) {return avgNumberOfCalls[classIdx];}

        int   getV();

        void addCall(Call *newCall);
        void   removeCallAndReleaseRes(Call *call);
        Call   *getNextCall();

        inline double getNoOfFreeDU() { return (double)V_du - n_du; }
        inline int getNoOfFreeAS() { return V - n;}
    };
    struct Call
    {
        const ModelTrClass *trClass;   /// Traffic Class data (parameters like call service intensity, number of BBUs, ...)
        int classIdx;                 /// Class number
        int reqAS;                    /// Required number of allocation slots.

        int  serverDataRate;          /// Number of data, that are sent to the server in one time unit.
        int  bufferAS;                /// Buffer Allocation Slots, that are in use
        int   bufferDataRate;          /// Number of data, that are sent/received from the server.

        double sourceIntensity;
        double serviceIntensity;

        double timeAndDU;              /// DU * time
        double bufferDU;               /// Number of data units, that are stored in buffer

        double noOfDataUnitInMessage;  /// Number of bytes to send
        double noOfTransmittedDU;      /// Number of already transmitted Bytes ny the server.
        double noOfReceivedDU;

        double plannedServiceTime;     /// Expected time of service ending
        double timeOnBuffer;
        double timeOnSystem;

        ProcBufferFifo *procCall;

        void fillData(struct Call *src);
        void collectTheStats(double time);
    };

private:
    System *systemForSimulation;
};

class ProcBufferFifo
{
    friend void SimulatorBufferFifo::System::serveCallsInEque();
public:
    enum PROC_STATE
    {
        USELESS,
        WAITING_FOR_NEW,
        REC_DATA,                       /// receiving data from souce and sending them to serwer or server and buffer or buffer only
        FLUSHING_BUFFER,                /// sending data, that were stored in buffer
        WAITING_IN_BUFFER               /// call is waiting in buffer
    };

private:
    PROC_STATE state;

    void initializeErlang(SimulatorBufferFifo::System *system, const ModelTrClass *trClass, int idx, double a, int sumPropAt, int V);
    static void newCall(ProcBufferFifo *proc, SimulatorBufferFifo::System *system);
    static void sourceTransmissionEnded(ProcBufferFifo *proc, SimulatorBufferFifo::System *system);
    static void callTransmisionEnded(ProcBufferFifo *proc, SimulatorBufferFifo::System *system);
    static void buffOverflow(ProcBufferFifo *proc, SimulatorBufferFifo::System *system);

public:
    SimulatorBufferFifo::Call *callData;

    inline void setUseless()     { state = USELESS; }
    inline void removeCallData() { callData = NULL; }
    inline void setCallData(SimulatorBufferFifo::Call *newCallData) { callData = newCallData; }

    void configure(PROC_STATE state, void (*funPtr)(ProcBufferFifo *simClass, SimulatorBufferFifo::System *system), SimulatorBufferFifo::Call *callData, double time);
    inline bool hasCallData()                       { return (bool)(callData != NULL); }
    inline bool hasCallData(ProcBufferFifo *proc)   { return (callData == NULL) ? false :( bool)(callData->procCall == proc); }

    double time;                /// Thos value can be out of date. Use them only on object taken from agenda
    uint   idx;                 /// Index on the binary heap

    static void initialize(
              SimulatorBufferFifo::System *system
            , const ModelTrClass *trClass
            , int classIdx
            , double a
            , int sumPropAt
            , int V
            );
    void (*execute)(ProcBufferFifo *simClass, SimulatorBufferFifo::System *system);

    static double distrLambda(double lambda)
    {
        double randomNumber = (double)(qrand())/RAND_MAX;
        while (randomNumber == 0 || randomNumber == 1)
            randomNumber = (double)(qrand())/RAND_MAX;

        double result = -log(randomNumber)/lambda;
        return result;  //return -Math.Log(Los, Math.E) / wspInt
    }
};

} // namespace Algorithms

#endif // SIMULATOR_BUFFER_FIFO_H
