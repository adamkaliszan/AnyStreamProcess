#include "algorithms/algRekLag.h"
#include "utils/lag.h"

namespace Algorithms
{

Utils::UtilsLAG lag; //TODO korzystać ze zmiennej globalnej w UTILS

algRekLAG::algRekLAG()
{
    myQoS_Set
       <<Results::Type::BlockingProbability
       <<Results::Type::OccupancyDistribution;
}

bool algRekLAG::possible(const ModelSyst *system) const
{
    if ((system->vk_s() == 0) || (system->k_sType() > 1) || (system->vk_b() > 0))
        return false;

    return Investigator::possible(system);
}

double algRekLAG::getSigma(int classNumber, int state)
{
    double result = 1;
    uint x = static_cast<uint>(system->V() - state);
    uint t = static_cast<uint>(system->getClass(classNumber)->t());
    uint k = static_cast<uint>(system->k_s());
    uint f = static_cast<uint>(system->v_s(0));

    if (x <= (t-1) * k)
    {
        double A = lag.F(x, k, f,   0);
        double B = lag.F(x, k, t-1, 0);
        result = (A-B)/A;
    }
    return result;
}

} // namespace Algorithms
