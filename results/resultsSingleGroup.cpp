#include "resultsSingleGroup.h"

namespace Results
{

AvailabilityAU &AvailabilityAU::operator+=(const AvailabilityAU &rho)
{
    freeInBestGroup+= rho.freeInBestGroup;
    freeInAllTheGroups+= rho.freeInAllTheGroups;
    availabilityOnlyInAllTheGroups+= rho.availabilityOnlyInAllTheGroups;
    availabilityInAllTheGroups+= rho.availabilityInAllTheGroups;
    inavailabilityInAllTheGroups+= rho.inavailabilityInAllTheGroups;
    return *this;
}

AvailabilityAU AvailabilityAU::operator-(const AvailabilityAU &rho) const
{
    AvailabilityAU result;
    result.freeInBestGroup = freeInBestGroup - rho.freeInBestGroup;
    result.freeInAllTheGroups = freeInAllTheGroups - rho.freeInAllTheGroups;;
    result.availabilityOnlyInAllTheGroups = availabilityOnlyInAllTheGroups - rho.availabilityOnlyInAllTheGroups;
    result.availabilityInAllTheGroups = availabilityInAllTheGroups - rho.availabilityInAllTheGroups;
    result.inavailabilityInAllTheGroups = inavailabilityInAllTheGroups - rho.inavailabilityInAllTheGroups;
    return result;
}

AvailabilityAU AvailabilityAU::operator^(double rho) const
{
    AvailabilityAU result;
    result.freeInBestGroup = qPow(freeInBestGroup, rho);
    result.freeInAllTheGroups = qPow(freeInAllTheGroups, rho);
    result.availabilityOnlyInAllTheGroups = qPow(availabilityOnlyInAllTheGroups, rho);
    result.availabilityInAllTheGroups = qPow(availabilityInAllTheGroups, rho);
    result.inavailabilityInAllTheGroups = qPow(inavailabilityInAllTheGroups, rho);
    return result;
}

AvailabilityAU AvailabilityAU::operator*(const AvailabilityAU &rho) const
{
    AvailabilityAU result;
    result.freeInBestGroup = rho.freeInBestGroup * freeInBestGroup;
    result.freeInAllTheGroups = rho.freeInAllTheGroups * freeInAllTheGroups;
    result.availabilityOnlyInAllTheGroups = rho.availabilityOnlyInAllTheGroups * availabilityOnlyInAllTheGroups;
    result.availabilityInAllTheGroups = rho.availabilityInAllTheGroups * availabilityInAllTheGroups;
    result.inavailabilityInAllTheGroups = rho.inavailabilityInAllTheGroups * inavailabilityInAllTheGroups;
    return result;
}

AvailabilityAU &AvailabilityAU::operator/=(double rho)
{
    freeInBestGroup/= rho;
    freeInAllTheGroups/= rho;
    availabilityOnlyInAllTheGroups/= rho;
    availabilityInAllTheGroups/= rho;
    inavailabilityInAllTheGroups/= rho;
    return *this;
}

AvailabilityAU &AvailabilityAU::operator*=(double rho)
{
    freeInBestGroup*= rho;
    freeInAllTheGroups*= rho;
    availabilityOnlyInAllTheGroups*= rho;
    availabilityInAllTheGroups*= rho;
    inavailabilityInAllTheGroups*= rho;
    return *this;
}

AvailabilityAU &AvailabilityAU::sqrt()
{
    freeInBestGroup = qSqrt(freeInBestGroup);
    freeInAllTheGroups = qSqrt(freeInAllTheGroups);
    availabilityOnlyInAllTheGroups = qSqrt(availabilityOnlyInAllTheGroups);
    availabilityInAllTheGroups = qSqrt(availabilityInAllTheGroups);
    inavailabilityInAllTheGroups = qSqrt(inavailabilityInAllTheGroups);
    return *this;
}

AvailabilityAU AvailabilityAU::pow(double b) const
{
    AvailabilityAU result;
    result.freeInBestGroup = qPow(freeInBestGroup, b);
    result.freeInAllTheGroups = qPow(freeInAllTheGroups, b);
    result.availabilityOnlyInAllTheGroups = qPow(availabilityOnlyInAllTheGroups, b);
    result.availabilityInAllTheGroups = qPow(availabilityInAllTheGroups, b);
    result.inavailabilityInAllTheGroups = qPow(inavailabilityInAllTheGroups, b);
    return result;
}

AvailabilityClass &AvailabilityClass::operator+=(const AvailabilityClass &rho)
{
    availabilityInBestSubgroup+= rho.availabilityInBestSubgroup;
    availabilityOnlyInAllSubgroups+= rho.availabilityOnlyInAllSubgroups;
    availabilityInAllSubgroups+= rho.availabilityInAllSubgroups;
    inavailabilityInAllSubgroups+= rho.inavailabilityInAllSubgroups;
    return *this;
}

AvailabilityClass AvailabilityClass::operator-(const AvailabilityClass &rho) const
{
    AvailabilityClass result;
    result.availabilityInBestSubgroup = availabilityInBestSubgroup - rho.availabilityInBestSubgroup;
    result.availabilityOnlyInAllSubgroups = availabilityOnlyInAllSubgroups - rho.availabilityOnlyInAllSubgroups;
    result.availabilityInAllSubgroups = availabilityInAllSubgroups - rho.availabilityInAllSubgroups;
    result.inavailabilityInAllSubgroups = inavailabilityInAllSubgroups - rho.inavailabilityInAllSubgroups;
    return result;
}

AvailabilityClass AvailabilityClass::operator^(double rho) const
{
    AvailabilityClass result;
    result.availabilityInBestSubgroup = qPow(availabilityInBestSubgroup, rho);
    result.availabilityOnlyInAllSubgroups = qPow(availabilityOnlyInAllSubgroups, rho);
    result.availabilityInAllSubgroups = qPow(availabilityInAllSubgroups, rho);
    result.inavailabilityInAllSubgroups = qPow(inavailabilityInAllSubgroups, rho);
    return result;
}

AvailabilityClass AvailabilityClass::operator*(const AvailabilityClass &rho) const
{
    AvailabilityClass result;
    result.availabilityInBestSubgroup = rho.availabilityInBestSubgroup * availabilityInBestSubgroup;
    result.availabilityOnlyInAllSubgroups = rho.availabilityOnlyInAllSubgroups * availabilityOnlyInAllSubgroups;
    result.availabilityInAllSubgroups = rho.availabilityInAllSubgroups * availabilityInAllSubgroups;
    result.inavailabilityInAllSubgroups = rho.inavailabilityInAllSubgroups * inavailabilityInAllSubgroups;
    return result;
}

AvailabilityClass &AvailabilityClass::operator/=(double rho)
{
    availabilityInBestSubgroup/= rho;
    availabilityOnlyInAllSubgroups/= rho;
    availabilityInAllSubgroups/= rho;
    inavailabilityInAllSubgroups/= rho;
    return *this;
}

AvailabilityClass &AvailabilityClass::operator*=(double rho)
{
    availabilityInBestSubgroup*= rho;
    availabilityOnlyInAllSubgroups*= rho;
    availabilityInAllSubgroups*= rho;
    inavailabilityInAllSubgroups*= rho;
    return *this;
}

AvailabilityClass &AvailabilityClass::sqrt()
{
    availabilityInBestSubgroup = qSqrt(availabilityInBestSubgroup);
    availabilityOnlyInAllSubgroups = qSqrt(availabilityOnlyInAllSubgroups);
    availabilityInAllSubgroups = qSqrt(availabilityInAllSubgroups);
    inavailabilityInAllSubgroups = qSqrt(inavailabilityInAllSubgroups);
    return *this;
}

AvailabilityClass AvailabilityClass::pow(double b) const
{
    AvailabilityClass result;
    result.availabilityInBestSubgroup = qPow(availabilityInBestSubgroup, b);
    result.availabilityOnlyInAllSubgroups = qPow(availabilityOnlyInAllSubgroups, b);
    result.availabilityInAllSubgroups = qPow(availabilityInAllSubgroups, b);
    result.inavailabilityInAllSubgroups = qPow(inavailabilityInAllSubgroups, b);

    return result;
}


} //namespace Results
