#ifndef RESULTS_SINGLE_H
#define RESULTS_SINGLE_H

#include <QtCharts>

#include "model.h"
//#include "results/resultsApi.h"
#include "results/baseTypes.h"
#include "results/resultsSingleGroup.h"

namespace Results
{
class RInvestigator;

/**
 * @brief Rezultaty otrzymane za pomocą metody zanalitycznej lub pojedynczego eksperymentu symulacyjnego
 * Na tym etapie nie ma wyznaczania przedziałów ufności
 */
class RSingle
{
    friend class RInvestigator;
public:
    RSingle();

    void init(const ModelSyst *system);

    RSingle& write(TypeGeneral type, double value);
    bool read(double &result, TypeGeneral type) const;

    RSingle& write(TypeForClass type, double value, int classNo);
    bool read(double &result, TypeForClass type, int classNo) const;

    RSingle& write(TypeForSystemState, double value, int systemState);
    bool read(double &result, TypeForSystemState, int systemState) const;

    RSingle &write(TypeForServerState, double value, int serverState);
    bool read(double &result, TypeForServerState type, int serverState) const;

    RSingle &write(TypeForBufferState type, double value, int queueState);
    bool read(double &result, TypeForBufferState type, int queueState) const;

    RSingle &write(TypeForServerAngBufferState type, double value, int serverState, int bufferState);
    bool read(double &result, TypeForServerAngBufferState type, int serverState, int bufferState) const;

    RSingle& write(TypeForClassAndSystemState type, double value, int classNo, int systemState);
    bool read(double &result, TypeForClassAndSystemState type, int classNo, int systemState) const;

    RSingle& write(TypeForClassAndServerState type, double value, int classNo, int serverState);
    bool read(double &result, TypeForClassAndServerState type, int classNo, int serverState) const;

    RSingle& write(TypeForClassAndBufferState type, double value, int classNo, int queueState);
    bool read(double &result, TypeForClassAndBufferState type, int classNo, int queueState) const;

    RSingle &write(TypeStateForServerGroupsCombination, double value, int numberOfResourcess, int groupCombinationIndex);
    bool read(double &result, TypeStateForServerGroupsCombination type, int numberOfResourcess, int groupCombinationIndex) const;

    RSingle &write(TypeClassForServerGroupsCombination, double value, int classNumber, int groupCombinationIndex);
    bool read(double &result, TypeClassForServerGroupsCombination type, int classNumber, int groupCombinationIndex) const;

    RSingle &write(TypeClassForServerBestGroupsSet, double value, int classNumber, int numberOfGroups);
    bool read(double &result, TypeClassForServerBestGroupsSet type, int classNumber, int numberOfGroups) const;

    RSingle &write(TypeStateForServerGroupsSet type, double value, int numberOfResourcess, int numberOfGroups);
    bool read(double &result, TypeStateForServerGroupsSet type, int numberOfResourcess, int numberOfGroups) const;

    RSingle &write(TypeClassForServerExactGroupsSet type, double value, int classNumber, int numberOfGroups);
    bool read(double &result, TypeClassForServerExactGroupsSet type, int classNumber, int numberOfGroups) const;

    RSingle &operator+=(const RSingle& rho);
    RSingle operator-(const RSingle& rho) const;
    RSingle operator^(double rho) const;
    RSingle operator*(const RSingle& rho);
    RSingle &operator/=(double rho);
    RSingle &operator*=(double rho);

    //RSingle operator=(const RSingle& rho);

    void sqrt();
    void clear();

//signals:
//    void valueChanged();

private:    
    int m;
    int vMax;
    int V;
    int Vs;

    struct DataGeneral;
    struct DataForClasses;
    struct DataForStates;
    struct DataPerGroups;
    struct DataForClassesAndState;


    QVector<DataForClasses>             dataPerClasses;
    QVector<DataForStates>              dataPerSystemState;
    QVector<DataForStates>              dataPerServerState;
    QVector<DataForStates>              dataPerBufferState;
    QVector<DataForStates>              dataPerServerAndBufferState;
    QVector<DataForClassesAndState>     dataPerClassAndServerState;
    QVector<DataForClassesAndState>     dataPerClassAndQueueState;
    QVector<DataForClassesAndState>     dataPerClassAndSystemStateForServer;
    QVector<DataForClassesAndState>     dataPerClassAndSystemStateForQueue;
    QVector<DataForClassesAndState>     dataPerClassAndSystemStateForSystem;
    QVector<DataPerGroups>              dataPerGroupCombination;
    QVector<DataPerGroups>              dataPerBestGroups;
    QVector<DataPerGroups>              dataPerExactGroupNumber;


    struct DataGeneral
    {
        double systemUtilization;     /// Expected value of system state probability distribution
        double serverUtilization;     /// Expected value of server state probability distribution
        double bufferUtilization;     /// Expected value of buffer state probability distribution - buffer length
        double totalTime;             /// Tatal time between call arrival and leawing the system wait+service time
        double serviceTime;           /// Total time of service
        double waitingTime;           /// Total time of waiting in the buffer

        DataGeneral(): systemUtilization(0), serverUtilization(0), bufferUtilization(0), totalTime(0), serviceTime(0), waitingTime(0) {}

        DataGeneral &operator+=(const DataGeneral& rho);
        DataGeneral operator-(const DataGeneral& rho) const;
        DataGeneral operator^(double rho) const;
        DataGeneral operator*(const DataGeneral& rho) const;
        DataGeneral &operator/=(double rho);
        DataGeneral &operator*=(double rho);

        DataGeneral& sqrt();
        DataGeneral pow(double rho) const;
        void clear();
    };

    struct DataGeneral                  dataGeneral;

    struct DataForClasses
    {
        double blockingProbability;
        double lossProbability;
        double congestionTraffic;
        double avarageNumbersOfCallsInSystem;
        double avarageNumbersOfCallsInServer;
        double avarageNumbersOfCallsInBuffer;

        DataForClasses(): blockingProbability(0), lossProbability(0), congestionTraffic(0)
          , avarageNumbersOfCallsInSystem(0), avarageNumbersOfCallsInServer(0), avarageNumbersOfCallsInBuffer(0) {}

        DataForClasses &operator+=(const DataForClasses& rho);
        DataForClasses operator-(const DataForClasses& rho) const;
        DataForClasses operator^(double rho) const;
        DataForClasses operator*(const DataForClasses& rho) const;
        DataForClasses &operator/=(double rho);
        DataForClasses &operator*=(double rho);

        DataForClasses& sqrt();
        DataForClasses pow(double rho) const;
        void clear();
    };

    struct DataForStates
    {
        double probability;
        double newCallInIntensity;
        double endCallInIntensity;
        double newCallOutIntensity;
        double endCallOutIntensity;

        DataForStates(): probability(0), newCallInIntensity(0), endCallInIntensity(0), newCallOutIntensity(0),  endCallOutIntensity(0) {}

        DataForStates &operator+=(const DataForStates& rho);
        DataForStates operator-(const DataForStates& rho) const;
        DataForStates operator^(double rho) const;
        DataForStates operator*(const DataForStates& rho) const;
        DataForStates &operator/=(double rho);
        DataForStates &operator*=(double rho);

        DataForStates& sqrt();
        DataForStates pow(double rho) const;
        void clear();
    };

    struct DataPerGroups
    {
        QVector<AvailabilityAU> availabilityProbabilities;
        QVector<AvailabilityClass> availabilityClasses;

        DataPerGroups(int maxGroupLength=0, int m=0)
        {
            availabilityProbabilities.resize(maxGroupLength);
            availabilityClasses.resize(m);
        }

        DataPerGroups& operator+=(const DataPerGroups& rho);
        DataPerGroups  operator- (const DataPerGroups& rho) const;
        DataPerGroups  operator^ (double rho) const;
        DataPerGroups  operator* (const DataPerGroups& rho) const;
        DataPerGroups& operator/=(double rho);
        DataPerGroups& operator*=(double rho);

        DataPerGroups& sqrt();
        DataPerGroups pow(double rho) const;
        void clear();
    };

    struct DataForClassesAndState
    {
        double realNewCallIntensityOut;
        double offeredCallIntensityOut;
        double newCallIntensityIn;
        double endCallIntensityOut;
        double endCallIntensityIn;

        double cac_probability;
        double utilization;

        DataForClassesAndState(): realNewCallIntensityOut(0), offeredCallIntensityOut(0)
          , newCallIntensityIn(0), endCallIntensityOut(0), endCallIntensityIn(0), cac_probability(0), utilization(0) {}

        DataForClassesAndState& operator+=(const DataForClassesAndState& rho);
        DataForClassesAndState  operator- (const DataForClassesAndState& rho) const;
        DataForClassesAndState  operator^ (double rho) const;
        DataForClassesAndState  operator* (const DataForClassesAndState& rho) const;
        DataForClassesAndState& operator/=(double rho);
        DataForClassesAndState& operator*=(double rho);

        DataForClassesAndState& sqrt();
        DataForClassesAndState pow(double rho) const;
        void clear();
    };
};

} //namespace Results

#endif // RESULTS_SINGLE_H
