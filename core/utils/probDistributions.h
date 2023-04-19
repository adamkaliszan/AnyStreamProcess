#ifndef UTILS_PROB_DISTRIB_H
#define UTILS_PROB_DISTRIB_H

#include <math.h>
#include <random>

class paretoDistrib
{
private:
    double poz;
    double shape;

    double RevShape;

    std::uniform_real_distribution<double> uniformDis01;//(0.0, 1.0);

    double reverseCDF(double pVal);

public:
    paretoDistrib(double E = 1, double D = 1);

    double operator()(std::mt19937_64 &gen);
};

#endif // UTILS_PROB_DISTRIB_H
