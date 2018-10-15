#include "resultsSystem.h"

#include "utils/lag.h"

namespace Results
{

RSystem::RSystem(const ModelSyst &model)
    : model(model)
{

}

RInvestigator& RSystem::createNewInvestigation(Investigator *algorithm, decimal aPerAU, unsigned noOfSeries)
{
    if (!_aPerAU.contains(aPerAU))
        _aPerAU.append(aPerAU);

    if (!_algorithms.contains(algorithm))
        _algorithms.append(algorithm);

    if (!resultsForInvestigatorsAndOfferedTraffic[algorithm].contains(aPerAU))
    {
        resultsForInvestigatorsAndOfferedTraffic[algorithm].insert(aPerAU, RInvestigator(&model));
        resultsForInvestigatorsAndOfferedTraffic[algorithm].find(aPerAU)->init(noOfSeries);
    }
    return *(resultsForInvestigatorsAndOfferedTraffic[algorithm].find(aPerAU));
}

RInvestigator* RSystem::getInvestigationResults(Investigator *algorithm, decimal aPerAU)
{
    RInvestigator* result = nullptr;
//  if ( _aPerAU.contains(aPerAU) && _algorithms.contains(algorithm))
//  const QMap<decimal, RInvestigator > tmp = *(resultsForInvestigatorsAndOfferedTraffic.find(algorithm));

    if (this->_aPerAU.contains(aPerAU))
    {
        result = &(resultsForInvestigatorsAndOfferedTraffic.find(algorithm)->find(aPerAU).value());
    }
    else
    {
        double aTmp = static_cast<double>(aPerAU);
        qFatal("Wrong a %f", aTmp);
    }

    return result;
}

int RSystem::getNumberOrCalculatedAlgorithms() const
{
    int result = 0;

    foreach (Investigator *algorithm, _algorithms)
    {
        if (algorithm->isSelected)
            result++;
    }
    return result;
}

double RSystem::getMinAperAU() const
{
    double result = -1;

    if (getAvailableAperAU().length()>0)
    {
        result = static_cast<double>(getAvailableAperAU()[0]);
        foreach(decimal a, getAvailableAperAU())
        {
            double tmp = static_cast<double>(a);
            if (result > tmp)
                result = tmp;
        }
    }
    return result;
}

double RSystem::getMaxAperAU() const
{
    double result = -1;

    if (getAvailableAperAU().length()>0)
    {
        result = static_cast<double>(getAvailableAperAU()[0]);
        foreach(decimal a, getAvailableAperAU())
        {
            double tmp = static_cast<double>(a);
            if (result < tmp)
                result = tmp;
        }
    }
    return result;
}

int RSystem::getNoOfGroupsCombinations() const
{
    if (groupCombinations.length() == 0)
        groupCombinations = Utils::UtilsLAG::getPossibleCombinations(model.Ks());

    return groupCombinations.length();
}

QVector<int> RSystem::getGroupCombination(int combinationNo)
{
    if (groupCombinations.length() == 0)
        groupCombinations = Utils::UtilsLAG::getPossibleCombinations(model.Ks());

    return groupCombinations[combinationNo].first;
}

QString RSystem::getGroupCombinationStr(int combinationNo)
{
    QVector<int>combination = getGroupCombination(combinationNo);
    QString result;
    foreach (int groupNo, combination)
    {
        result+= QString::number(groupNo + 1);
        result+= " ";
    }

    return result;
}

} //namespace Results

