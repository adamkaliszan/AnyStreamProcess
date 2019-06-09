#include "simulator.h"

namespace Algorithms
{

Simulator::Simulator(): Investigator()
{
    _hasConfIntervall = true;
    myQoS_Set
       <<Results::Type::BlockingProbability
       <<Results::Type::LossProbability
       <<Results::Type::OccupancyDistribution;
}

std::random_device Simulator::rd;
std::mt19937 Simulator::gen = std::mt19937(Simulator::rd());

bool Simulator::isItTheSameSystem(ModelCreator *system)
{
    if (system == this->system)
        return true;

    return *(this->system) == *system;
}


double Simulator::distrLambdaED(double Ex, double D)
{
    (void) D;
    double randomNumber;
    do
    randomNumber = static_cast<double>(qrand())/RAND_MAX;
    while (qFuzzyIsNull(randomNumber) || qFuzzyCompare(randomNumber, 1));

    double result = -log(randomNumber)*Ex;
    return result;  //return -Math.Log(Los, Math.E) / wspInt
}

double Simulator::distrNormalED(double Ex, double D)
{
    std::normal_distribution<> tmpDistrib(Ex, D);

    double result = 0;
    do
    {
        result = tmpDistrib(Simulator::gen);
    }
    while (result <= 0);
    return result;
}

double Simulator::distrUniformED(double Ex, double D)
{
    double DeltaT = sqrt(12*D);

    double tMin = Ex-0.5*DeltaT;

    double x = DeltaT * static_cast<double>(qrand())/RAND_MAX;
    return tMin+x;
}

double Simulator::distrGammaED(double Ex, double D)
{
    double betha    = D/Ex;
    double alpha    = Ex / betha;

    std::gamma_distribution<double> tmpDistrib(alpha, betha);

    double result = 0;
    do
    {
        result = tmpDistrib(Simulator::gen);
    }
    while (result <= 0);
    return result;
}

double Simulator::distrParetoED(double Ex, double D)
{
    static std::uniform_real_distribution<double>uniformDis01(0.000001, 1);

    double mySqrtVal = sqrt(1+Ex*Ex/D);
    double shape = 1+mySqrtVal;
    double RevShape = 1.0/shape;
    double poz = Ex*mySqrtVal/(1.0 + mySqrtVal);

    double randVal = uniformDis01(Simulator::gen);
    double pierw = pow(1.0-randVal, RevShape);
    double result = poz / pierw;
    return result;
}

double Simulator::distrUniform(double tMin, double tMax)
{
    double x = (tMax - tMin) * (static_cast<double>(qrand())/RAND_MAX);
    return tMin+x;
}

} // namespace Algorithms
