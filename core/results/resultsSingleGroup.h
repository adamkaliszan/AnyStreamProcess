#ifndef RESULTS_SINGLE_GROUP_H
#define RESULTS_SINGLE_GROUP_H

#include <qmath.h>

namespace Results
{

class AvailabilityAU
{
public:
    AvailabilityAU(): freeInBestGroup(0), freeInAllTheGroups(0), availabilityOnlyInAllTheGroups(0), availabilityInAllTheGroups(0), inavailabilityInAllTheGroups(0) {}

    double freeInBestGroup;
    double freeInAllTheGroups;

    double availabilityOnlyInAllTheGroups;
    double availabilityInAllTheGroups;
    double inavailabilityInAllTheGroups;


    AvailabilityAU &operator+=(const AvailabilityAU& rho);
    AvailabilityAU operator-(const AvailabilityAU& rho) const;
    AvailabilityAU operator^(double rho) const;
    AvailabilityAU operator*(const AvailabilityAU& rho) const;
    AvailabilityAU &operator/=(double rho);
    AvailabilityAU &operator*=(double rho);

    AvailabilityAU &sqrt();
    AvailabilityAU pow(double rho) const;
    void clear();
};

class AvailabilityClass
{
public:
    AvailabilityClass(): availabilityInBestSubgroup(0), availabilityOnlyInAllSubgroups(0), availabilityInAllSubgroups(0), inavailabilityInAllSubgroups(0) {}

    double availabilityInBestSubgroup;             ///Probability, that the call can be accepted by at least one subgroup within the set
    double availabilityOnlyInAllSubgroups;         ///Probability, that the call can be accepted by all the sobgroup within the set, but can not be accepted in any subgroup outside the set
    double availabilityInAllSubgroups;             ///Probability, that the call can be accepted by all the sobgroup within the set, subgroups outside the set are not considered
    double inavailabilityInAllSubgroups;           ///Probability, that the call can not be accepted by all the sobgroup within the set, subgroups outside the set are not considered


    AvailabilityClass &operator+=(const AvailabilityClass& rho);
    AvailabilityClass operator-(const AvailabilityClass& rho) const;
    AvailabilityClass operator^(double rho) const;
    AvailabilityClass operator*(const AvailabilityClass& rho) const;
    AvailabilityClass &operator/=(double rho);
    AvailabilityClass &operator*=(double rho);

    AvailabilityClass &sqrt();
    AvailabilityClass pow(double rho) const;
    void clear();
};

} //namespace Results
#endif // RESULTS_SINGLE_GROUP_H
