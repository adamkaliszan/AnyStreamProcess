#include "probDistributions.h"


double paretoDistrib::reverseCDF(double pVal)
{
    double negPval = 1.0-pVal;


    double pierw = pow(negPval, RevShape);
    double result = poz / pierw;
    return result;
}

paretoDistrib::paretoDistrib(double E, double D)
{
    double mySqrtVal = sqrt(1+E*E/D);

    shape = 1+mySqrtVal;
    RevShape = 1.0/shape;
    poz = E*mySqrtVal/(1.0 + mySqrtVal);

    uniformDis01 = std::uniform_real_distribution<double>(0.000001, 1);
}

double paretoDistrib::operator()(std::mt19937_64 &gen)
{
    double randVal = this->uniformDis01(gen);
    double result = reverseCDF(randVal);
    return result;
}
