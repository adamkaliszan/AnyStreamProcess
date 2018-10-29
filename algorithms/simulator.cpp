#include "simulator.h"

namespace Algorithms
{

simulator::simulator(BufferResourcessScheduler qDisc): Investigator(qDisc)
{
    _hasConfIntervall = true;
    myQoS_Set
       <<Results::Type::BlockingProbability
       <<Results::Type::OccupancyDistribution;
}

std::random_device simulator::rd;
std::mt19937 simulator::gen = std::mt19937(simulator::rd());

bool simulator::isItTheSameSystem(ModelSyst *system)
{
    if (system == this->system)
        return true;

    return *(this->system) == *system;
}


double simulator::distrLambdaED(double Ex, double D)
{
    (void) D;
/*
    std::poisson_distribution<double>tmpDistrib(Ex);
    double result = 0;
    do
    {
        result = tmpDistrib(simulator::gen);
    }
    while (result <=0);
*/
    double randomNumber;
    do
    randomNumber = static_cast<double>(qrand())/RAND_MAX;
    while (qFuzzyIsNull(randomNumber) || qFuzzyCompare(randomNumber, 1));

    double result = -log(randomNumber)*Ex;
    return result;  //return -Math.Log(Los, Math.E) / wspInt
}

double simulator::distrNormalED(double Ex, double D)
{
    std::normal_distribution<> tmpDistrib(Ex, D);

    double result = 0;
    do
    {
        result = tmpDistrib(simulator::gen);
    }
    while (result <= 0);
    return result;
}

double simulator::distrUniformED(double Ex, double D)
{
    double DeltaT = sqrt(12*D);

    double tMin = Ex-0.5*DeltaT;

    //double tMax = Ex+0.5*DeltaT;
    //std::uniform_real_distribution<double> tmpDistrib(tMin, tMax);
    //double result = 0;
    //do
    //{
    //    result = tmpDistrib(simulator::gen);
    //}
    //while(result<=0);


    double x = DeltaT * static_cast<double>(qrand())/RAND_MAX;
    return tMin+x;
}

double simulator::distrGammaED(double Ex, double D)
{
    double betha    = D/Ex;
    double alpha    = Ex / betha;

    std::gamma_distribution<double> tmpDistrib(alpha, betha);

    double result = 0;
    do
    {
        result = tmpDistrib(simulator::gen);
    }
    while (result <= 0);
    return result;
}

double simulator::distrParetoED(double Ex, double D)
{
    static std::uniform_real_distribution<double>uniformDis01(0.000001, 1);

    double mySqrtVal = sqrt(1+Ex*Ex/D);
    double shape = 1+mySqrtVal;
    double RevShape = 1.0/shape;
    double poz = Ex*mySqrtVal/(1.0 + mySqrtVal);

    double randVal = uniformDis01(simulator::gen);
    double pierw = pow(1.0-randVal, RevShape);
    double result = poz / pierw;
    return result;
}

double simulator::distrUniform(double tMin, double tMax)
{
    double x = (tMax - tMin) * (static_cast<double>(qrand())/RAND_MAX);
    return tMin+x;
}

} // namespace Algorithms
