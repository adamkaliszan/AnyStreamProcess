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

    //RSingle& write(Scope scope, Type type, double value, std::initializer_list<unsigned> params);
    //double read(Scope scope, Type type, std::initializer_list<unsigned> params);

    RSingle& write(TypeForClass type, double value, int classNo);
    bool read(double &result, TypeForClass type, int classNo) const;

    RSingle &write(TypeForServerState, double value, int serverState);
    bool read(double &result, TypeForServerState type, int serverState) const;

    RSingle &write(TypeForQueueState type, double value, int queueState);
    bool read(double &result, TypeForQueueState type, int queueState) const;

    RSingle& write(TypeForSystemState, double value, int systemState);
    bool read(double &result, TypeForSystemState, int systemState) const;

    RSingle& write(TypeForClassAndServerState type, double value, int classNo, int serverState);
    bool read(double &result, TypeForClassAndServerState type, int classNo, int serverState);

    RSingle& write(TypeForClassAndQueueState type, double value, int classNo, int queueState);
    bool read(double &result, TypeForClassAndQueueState type, int classNo, int queueState);

    RSingle& write(TypeForClassAndSystemState type, double value, int classNo, int systemState);
    bool read(double &result, TypeForClassAndSystemState type, int classNo, int systemState);

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

    struct DataForClasses;
    struct DataForStates;
    struct DataPerGroups;
    struct DataForClassesAndState;

    QVector<DataForClasses>             dataPerClasses;
    QVector<DataForStates>              dataPerServerState;
    QVector<DataForStates>              dataPerQueueState;
    QVector<DataForStates>              dataPerSystemState;
    QVector<DataForClassesAndState>     dataPerClassAndServerState;
    QVector<DataForClassesAndState>     dataPerClassAndQueueState;
    QVector<DataForClassesAndState>     dataPerClassAndSystemStateForServer;
    QVector<DataForClassesAndState>     dataPerClassAndSystemStateForQueue;
    QVector<DataForClassesAndState>     dataPerClassAndSystemStateForSystem;
    QVector<DataPerGroups>              dataPerGroupCombination;
    QVector<DataPerGroups>              dataPerBestGroups;
    QVector<DataPerGroups>              dataPerExactGroupNumber;

    struct DataForClasses
    {
        double blockingProbability;
        double lossProbability;
        double congestionTraffic;

        DataForClasses(): blockingProbability(0), lossProbability(0), congestionTraffic(0) {}

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
