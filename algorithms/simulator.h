#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "algorithms/investigator.h"
#include <boost/math/distributions/pareto.hpp>

namespace Algorithms
{

class simulator : public Investigator
{
public:
    static std::random_device rd;
    static std::mt19937 gen;

    simulator(BufferResourcessScheduler qDisc);

    bool isItTheSameSystem(ModelSyst *system);

    static double distrLambdaED(double Ex, double D = 0);
    static double distrNormalED(double Ex, double D);
    static double distrUniformED(double Ex, double D);
    static double distrGammaED(double Ex, double D);
    static double distrParetoED(double Ex, double D);

    static double distrUniform(double tMin, double tMax);

};

} // namespace Algorithms

#endif // SIMULATOR_H
