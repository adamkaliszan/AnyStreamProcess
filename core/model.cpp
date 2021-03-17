#include <string.h>
#include <QString>
#include <QTextStream>
#include <QDebug>
#include <QtAlgorithms>
#include <QtCore/QList>
#include <QThread>
#include <QThreadPool>
#include <QRandomGenerator>

#include <queue>
#include <math.h>
#include "model.h"
#include "utils/probDistributions.h"
#include <stack>


QString serverResourcessSchedulerToString(ResourcessScheduler value)
{
    QString result;
    switch (value)
    {
    case ResourcessScheduler::Random:
        result = "Random";
        break;
    case ResourcessScheduler::Sequencial:
        result = "Sequencial";
        break;
    }
    return result;
}

QString bufferResourcessSchedulerToString(SystemPolicy value)
{
    QString result;
    switch (value)
    {
    case SystemPolicy::cFifo:
        result = "Continuos";
        break;

    case SystemPolicy::dFIFO:
        result = "Discrette";
        break;

    case SystemPolicy::qFIFO:
        result = "Discrette qFIFO";
        break;

    case SystemPolicy::SD_FIFO:
        result = "State dependent";
        break;

    case SystemPolicy::NoBuffer:
        result = "No buffer";
        break;

    }
    return result;
}


QString ModelTrClass::streamTypeToString(ModelTrClass::StreamType str)
{
    switch(str)
    {
        case StreamType::Poisson:
            return "Poisson";
        case StreamType::Uniform:
            return "Uniform";
        case StreamType::Normal:
            return "Normal";
        case StreamType::Gamma:
            return "Gamma";
        case StreamType::Pareto:
            return "Pareto";
    }
    qFatal("Unknown stream");
}

QString ModelTrClass::streamTypeToShortString(StreamType str)
{
    switch(str)
    {
    case StreamType::Poisson:
        return "M";
    case StreamType::Uniform:
        return "U";
    case StreamType::Normal:
        return "N";
    case StreamType::Gamma:
        return "G";
    case StreamType::Pareto:
        return "P";
    }
    qFatal("Unknown stream");
}


void ModelTrClass::setNewCallStrType(ModelTrClass::StreamType strType, ModelTrClass::SourceType srcType)
{
    _srcType = srcType;
    _newCallStr = strType;
}

void ModelTrClass::setCallServStrType(ModelTrClass::StreamType strType)
{
    _callServStr = strType;
}

ModelSubResourcess::ModelSubResourcess(int k, int v)
{
    _k = k;
    _v = v;
}

void ModelSubResourcess::set_k(int k)
{
    _k = k;
}

void ModelSubResourcess::set_v(int v)
{
    _v = v;
}

int ModelSubResourcess::v() const
{
    return _v;
}

int ModelSubResourcess::k() const
{
    return _k;
}

int ModelSubResourcess::V() const
{
    return _v * _k;
}

bool ModelSubResourcess::operator==(const ModelSubResourcess &rho) const
{
    if (_v != rho._v)
        return false;
    if (_k != rho._k)
        return false;
    return true;
}

bool ModelSubResourcess::operator!=(const ModelSubResourcess &rho) const
{
    if (_v != rho._v)
        return true;
    if (_k != rho._k)
        return true;
    return false;
}

bool ModelSubResourcess::operator>(const ModelSubResourcess &rho) const
{
    if (_v*_k > rho._v * rho._k)
        return true;
    if (_v > rho._v)
        return true;
    return false;
}

bool ModelSubResourcess::operator<(const ModelSubResourcess &rho) const
{
    if (_v*_k < rho._v * rho._k)
        return true;
    if (_v < rho._v)
        return true;
    return false;
}

ModelTrClass::ModelTrClass(const ModelTrClass& rho):
    _newCallStr(rho._newCallStr)
  , _callServStr(rho._callServStr)
  , _srcType(rho._srcType)
  , _noOfSourcess(rho._noOfSourcess)
  , _propAt(rho._propAt)
  , _t(rho._t)
  , _mu(rho._mu)
  , _ServiceExPerDx(rho._ServiceExPerDx)
  , _IncommingExPerDx(rho._IncommingExPerDx)
{
}

ModelTrClass::ModelTrClass():
    _noOfSourcess(0)
  , _propAt(1)
  , _t(0)
  , _mu(0)
  , _ServiceExPerDx(0)
  , _IncommingExPerDx(0)
{
    _newCallStr  = StreamType::Poisson;
    _callServStr = StreamType::Poisson;
    _srcType     = SourceType::Independent;
}

QString ModelTrClass::shortName(bool enableUtf8) const
{
    QString result;
    QTextStream resultStream(&result);
    resultStream.setRealNumberPrecision(1);

    if ((newCallStr()==StreamType::Poisson) && (callServStr() == StreamType::Poisson))
    {
        switch (srcType())
        {
        case SourceType::Independent:
            if ((_propAt != 1) || (! qFuzzyCompare(_mu, 1)))
                resultStream<<"Erl. at="<<_propAt<<" t="<<_t<<" µ="<<_mu;
            else
                resultStream<<"Erlang t="<<_t;
            break;
        case SourceType::DependentMinus:
            if ((_propAt != 1) || (! qFuzzyCompare(_mu, 1)))
                resultStream<<"Eng. t=" << _t << " S=" << _noOfSourcess<<" at="<<_propAt<<" µ="<<_mu;
            else
                resultStream<<"Engset t="<<_t<<" S="<<_noOfSourcess;
            break;
        case SourceType::DependentPlus:
            if ((_propAt != 1) || (! qFuzzyCompare(_mu, 1)))
                resultStream << "Pas. t=" << _t << " S=" << _noOfSourcess << _noOfSourcess << " at=" <<_propAt << " µ=" << _mu;
            else
                resultStream << "Pascal t=" << _t << " S=" << _noOfSourcess;
            break;
        }
    }
    else
    {
        switch (srcType())
        {
        case SourceType::Independent:
            break;
        case SourceType::DependentMinus:
                resultStream << "Dep- S="<<_noOfSourcess;
            break;
        case SourceType::DependentPlus:
                resultStream << "Dep+ S="<<_noOfSourcess;
            break;
        }

        switch (newCallStr())
        {
        case StreamType::Poisson:
            resultStream << streamTypeToString(newCallStr());
            break;
        default:
            if (!enableUtf8)
                resultStream << streamTypeToString(newCallStr()) << " E2D=" << _IncommingExPerDx;
            else
                resultStream << streamTypeToString(newCallStr()) << QString::fromUtf8(" E²/ð²=") << _IncommingExPerDx;
            break;
        }
        resultStream<<" ";


        switch (callServStr())
        {
        case StreamType::Poisson:
            //streamTypeToShortString(callServStr());
            break;
        default:
            if (!enableUtf8)
                resultStream << " " << streamTypeToString(callServStr()) << " E2D=" <<_ServiceExPerDx;
            else
                resultStream << " " << streamTypeToString(callServStr()) << QString::fromUtf8(" E²/ð²=") <<_ServiceExPerDx;
            break;
        }

        if (_t != 1)
            resultStream << " t=" << _t;

        if ((_propAt != 1) || ( ! qFuzzyCompare(_mu, 1)))
            resultStream << " at=" << _propAt << "µ=" << _mu;
    }
    return result;
}

void ModelTrClass::doSimExpUnlimitedSoNo(TrClVector &states, int Vs, int Vb, double Aoffered, double IncommingEx2perDxDNewCall, double EsingleCallServ, double DsingleCallServ, int noOfSeries, int noOfEventsPerUnit) const
{
    QThreadPool *threadPool = QThreadPool::globalInstance();

    threadPool->setMaxThreadCount(noOfSeries);

    ModelTrClassSimulationWork *simulationTask;
    TrClVector *simStates = new TrClVector[noOfSeries];
    for (int simNo=0; simNo<noOfSeries; simNo++)
    {
        simStates[simNo] = TrClVector(states);
        simulationTask = new ModelTrClassSimulationWork(&simStates[simNo], Vs, Vb, Aoffered, _t
          , srcType(), newCallStr(), IncommingEx2perDxDNewCall
          , callServStr(), EsingleCallServ, DsingleCallServ, noOfEventsPerUnit);
        simulationTask->setAutoDelete(true);
        threadPool->start(simulationTask);
    }
    threadPool->waitForDone();

    for (int serNo=0; serNo < noOfSeries; serNo++)
    {
        for (int n=0; n<=Vs+Vb; n++)
        {
            states[n]+= simStates[serNo][n];
            states.getState(n).tIntInEnd+=simStates[serNo].getState(n).tIntInEnd;
            states.getState(n).tIntInNew+=simStates[serNo].getState(n).tIntInNew;
            states.getState(n).tIntOutEnd+=simStates[serNo].getState(n).tIntOutEnd;
            states.getState(n).tIntOutNew+=simStates[serNo].getState(n).tIntOutNew;
        }
    }

    for (int n=0; n<=Vs+Vb; n++)
    {
        states.getState(n).p/= noOfSeries;
        states.getState(n).tIntInEnd/= noOfSeries;
        states.getState(n).tIntInNew/= noOfSeries;
        states.getState(n).tIntOutEnd/= noOfSeries;
        states.getState(n).tIntOutNew/= noOfSeries;
    }
    delete []simStates;
}

void ModelTrClass::doSimExpLimitedSoNo(TrClVector &states, int Vs, int Vb, double Aoffered, double IncommingEx2perDxDNewCall, double EsingleCallServ, double DsingleCallServ) const
{
    SimulatorProcess *proc;
    SimulatorSingleServiceSystem *System = new SimulatorSingleServiceSystem(_noOfSourcess, srcType(), Vs, Vb, this->_t, states, Aoffered, IncommingEx2perDxDNewCall, EsingleCallServ, DsingleCallServ);

    for (int s=0; s< _noOfSourcess; s++)
    {
        proc = nullptr;
        switch (srcType())
        {
        case SourceType::Independent:
            qFatal("Wrong source type");
        case SourceType::DependentMinus:
            switch (newCallStr())
            {
            case StreamType::Poisson:
                switch (callServStr())
                {
                case StreamType::Poisson:
                    proc = new SimulatorProcess_DepMinusMM(System);
                    break;
                case StreamType::Uniform:
                    proc = new SimulatorProcess_DepMinusMU(System);
                    break;
                case StreamType::Normal:
                    proc = new SimulatorProcess_DepMinusMN(System);
                    break;
                case StreamType::Gamma:
                    proc = new SimulatorProcess_DepMinusMG(System);
                    break;
                case StreamType::Pareto:
                    proc = new SimulatorProcess_DepMinusMP(System);
                    break;
                }
                break;

            case StreamType::Uniform:
                switch (callServStr())
                {
                case StreamType::Poisson:
                    proc = new SimulatorProcess_DepMinusUM(System);
                    break;
                case StreamType::Uniform:
                    proc = new SimulatorProcess_DepMinusUU(System);
                    break;
                case StreamType::Normal:
                    proc = new SimulatorProcess_DepMinusUN(System);
                    break;
                case StreamType::Gamma:
                    proc = new SimulatorProcess_DepMinusUG(System);
                    break;
                case StreamType::Pareto:
                    proc = new SimulatorProcess_DepMinusUP(System);
                    break;
                }
                break;

            case StreamType::Normal:
                switch (callServStr())
                {
                case StreamType::Poisson:
                    proc = new SimulatorProcess_DepMinusNM(System);
                    break;
                case StreamType::Uniform:
                    proc = new SimulatorProcess_DepMinusNU(System);
                    break;
                case StreamType::Normal:
                    proc = new SimulatorProcess_DepMinusNN(System);
                    break;
                case StreamType::Gamma:
                    proc = new SimulatorProcess_DepMinusNG(System);
                    break;
                case StreamType::Pareto:
                    proc = new SimulatorProcess_DepMinusNP(System);
                    break;
                }
                break;

            case StreamType::Gamma:
                switch (callServStr())
                {
                case StreamType::Poisson:
                    proc = new SimulatorProcess_DepMinusGM(System);
                    break;
                case StreamType::Uniform:
                    proc = new SimulatorProcess_DepMinusGU(System);
                    break;
                case StreamType::Normal:
                    proc = new SimulatorProcess_DepMinusGN(System);
                    break;
                case StreamType::Gamma:
                    proc = new SimulatorProcess_DepMinusGG(System);
                    break;
                case StreamType::Pareto:
                    proc = new SimulatorProcess_DepMinusGP(System);
                    break;
                }
                break;

            case StreamType::Pareto:
                switch (callServStr())
                {
                case StreamType::Poisson:
                    proc = new SimulatorProcess_DepMinusPM(System);
                    break;
                case StreamType::Uniform:
                    proc = new SimulatorProcess_DepMinusPU(System);
                    break;
                case StreamType::Normal:
                    proc = new SimulatorProcess_DepMinusPN(System);
                    break;
                case StreamType::Gamma:
                    proc = new SimulatorProcess_DepMinusPG(System);
                    break;
                case StreamType::Pareto:
                    proc = new SimulatorProcess_DepMinusPP(System);
                    break;
                }
                break;
            }
        break;
        case SourceType::DependentPlus:
            switch (newCallStr())
            {
            case StreamType::Poisson:
                switch (callServStr())
                {
                case StreamType::Poisson:
                    proc = new SimulatorProcess_DepPlusMM(System);
                    break;
                case StreamType::Uniform:
                    proc = new SimulatorProcess_DepPlusMU(System);
                    break;
                case StreamType::Normal:
                    proc = new SimulatorProcess_DepPlusMN(System);
                    break;
                case StreamType::Gamma:
                    proc = new SimulatorProcess_DepPlusMG(System);
                    break;
                case StreamType::Pareto:
                    proc = new SimulatorProcess_DepPlusMP(System);
                    break;
                }
                break;

            case StreamType::Uniform:
                switch (callServStr())
                {
                case StreamType::Poisson:
                    proc = new SimulatorProcess_DepPlusUM(System);
                    break;
                case StreamType::Uniform:
                    proc = new SimulatorProcess_DepPlusUU(System);
                    break;
                case StreamType::Normal:
                    proc = new SimulatorProcess_DepPlusUN(System);
                    break;
                case StreamType::Gamma:
                    proc = new SimulatorProcess_DepPlusUG(System);
                    break;
                case StreamType::Pareto:
                    proc = new SimulatorProcess_DepPlusUP(System);
                    break;
                }
                break;

            case StreamType::Normal:
                switch (callServStr())
                {
                case StreamType::Poisson:
                    proc = new SimulatorProcess_DepPlusNM(System);
                    break;
                case StreamType::Uniform:
                    proc = new SimulatorProcess_DepPlusNU(System);
                    break;
                case StreamType::Normal:
                    proc = new SimulatorProcess_DepPlusNN(System);
                    break;
                case StreamType::Gamma:
                    proc = new SimulatorProcess_DepPlusNG(System);
                    break;
                case StreamType::Pareto:
                    proc = new SimulatorProcess_DepPlusNP(System);
                    break;
                }
                break;

            case StreamType::Gamma:
                switch (callServStr())
                {
                case StreamType::Poisson:
                    proc = new SimulatorProcess_DepPlusGM(System);
                    break;
                case StreamType::Uniform:
                    proc = new SimulatorProcess_DepPlusGU(System);
                    break;
                case StreamType::Normal:
                    proc = new SimulatorProcess_DepPlusGN(System);
                    break;
                case StreamType::Gamma:
                    proc = new SimulatorProcess_DepPlusGG(System);
                    break;
                case StreamType::Pareto:
                    proc = new SimulatorProcess_DepPlusGP(System);
                    break;
                }
                break;

            case StreamType::Pareto:
                switch (callServStr())
                {
                case StreamType::Poisson:
                    proc = new SimulatorProcess_DepPlusPM(System);
                    break;
                case StreamType::Uniform:
                    proc = new SimulatorProcess_DepPlusPU(System);
                    break;
                case StreamType::Normal:
                    proc = new SimulatorProcess_DepPlusPN(System);
                    break;
                case StreamType::Gamma:
                    proc = new SimulatorProcess_DepPlusPG(System);
                    break;
                case StreamType::Pareto:
                    proc = new SimulatorProcess_DepPlusPP(System);
                    break;
                }
                break;
            }
        break;
        }
        if (proc == nullptr)
            qFatal("Simulation critical error: not supported traffic class");
        proc->initialize();
    }

    System->stabilize((1+Vs+Vb) * 1000 / t());
    System->doSimExperiment((1+Vs+Vb) * (1+Vs+Vb) * 10000 / t(), states);

    double sum = 0;
    for (int n=0; n<=Vs+Vb; n++)
        sum += states[n];
    for (int n=0; n<=Vs+Vb; n++)
        states[n] /= sum;
    sum = 1;

    delete System;
}

void ModelTrClass::setPropAt(int propAt)
{
    _propAt = propAt;
}

void ModelTrClass::setT(int t)
{
    _t = t;
}

void ModelTrClass::setMu(double mu)
{
    _mu = mu;
}

void ModelTrClass::setNoOfSourcess(int noOfSourcess)
{
    _noOfSourcess = noOfSourcess;
}

int ModelTrClass::s (void) const
{
    if (this->_srcType == SourceType::Independent)
        return -1;
    return _noOfSourcess;
}

double ModelTrClass::intensityNewCallTotal(double a, size_t V, int sumPropAT) const
{
    return a * V * _propAt / sumPropAT / _t * _mu;
}

TrClVector ModelTrClass::trDistribution(int classIdx, double A, int Vs, int Vb, int noOfOseries, int noOfEventsPerUnit) const
{
    if (srcType() == ModelTrClass::SourceType::DependentMinus)
        if (A >= _noOfSourcess)
            qFatal("Wrong system data");

    int lastState = Vs+Vb;
    TrClVector result(Vs+Vb);
    result.addClass(classIdx, t());

    double sum;
    int n;

    double y = 0;
    double EservSingleCall      = 1/_mu;
    double DservSingleCall      = EservSingleCall*EservSingleCall/getServiceExPerDx();
    double LambdaZero;


    double A_cor = A;
    if (_srcType == ModelTrClass::SourceType::DependentMinus)
        A_cor = A * _noOfSourcess / (_noOfSourcess - A);
    if (_srcType == ModelTrClass::SourceType::DependentPlus)
        A_cor = A * _noOfSourcess / (_noOfSourcess + A);

    LambdaZero = A_cor * _mu;





    if ((newCallStr()==StreamType::Poisson) && (callServStr()==StreamType::Poisson))
    {
        result[0] = 1;

        result.setIntOutNew(0, classIdx, intensityNewCallForState(LambdaZero, 0));
        result.setIntOutEnd(0, classIdx, 0);

        sum = result[0];

        for (n=_t; n <= lastState; n+=_t)
        {
            y = (n<=Vs) ? n/_t : static_cast<double>(Vs)/_t;

            double intNewCall = intensityNewCallForState(LambdaZero, qMin(n - _t, Vs));
            result[n] = intNewCall * result[n-_t] / (y*_mu);
            sum += result[n];

            if (std::isnan(result[n]))
                qFatal("N/A value for state %d", n);

            result.setIntOutNew(n, classIdx, intensityNewCallForState(LambdaZero, qMin(n, Vs)));
            result.setIntOutEnd(n, classIdx, _mu * (n/_t));
        }

        for (n=0; n <= lastState; n+=_t)
        {
            result[n] /= sum;
            result.setIntInNew(n, classIdx, (n>=_t) ? result.getIntOutNew(n-_t, classIdx) : 0);
            result.setIntInEnd(n, classIdx, (n+_t<=lastState) ? result.getIntOutEnd(n+_t, classIdx) : 0);

            result.getState(n).tIntInNew = result.getIntInNew(n, classIdx);
            result.getState(n).tIntInEnd = result.getIntInEnd(n, classIdx);
            result.getState(n).tIntOutNew = result.getIntOutNew(n, classIdx);
            result.getState(n).tIntOutEnd = result.getIntOutEnd(n, classIdx);

        }
        sum = 1;
    }
    else
    {
        if (srcType() == SourceType::Independent)
            doSimExpUnlimitedSoNo(result, Vs, Vb, A, getIncommingExPerDx(), EservSingleCall, DservSingleCall, noOfOseries, noOfEventsPerUnit);
        else
            doSimExpLimitedSoNo(result, Vs, Vb, A, getIncommingExPerDx(), EservSingleCall, DservSingleCall);

        result.normalize();
    }

    for (int n=0; n<=Vs+Vb; n+=t())
    {
        result.setY(n, classIdx, n/t());
    }
    return result;
}

bool ModelTrClass::operator ==(const ModelTrClass &rho) const
{
    if ((_t != rho._t)
            || (_propAt != rho._propAt)
            || ( !qFuzzyCompare(_mu, rho._mu))
            || (_srcType != rho._srcType)
            || (_noOfSourcess != rho._noOfSourcess)
            || (_newCallStr != rho._newCallStr)
            || ( !qFuzzyCompare(_IncommingExPerDx, rho._IncommingExPerDx))
            || (_callServStr != rho._callServStr)
            || ( !qFuzzyCompare(_ServiceExPerDx, rho._ServiceExPerDx))
            ) return false;
    return true;
}

bool ModelTrClass::operator !=(const ModelTrClass &rho) const
{
    return !(*this == rho);
}

bool ModelTrClass::operator <(const ModelTrClass &rho) const
{
    if (_t != rho._t)
        return _t < rho._t;

    if (_propAt != rho._propAt)
        return _propAt < rho._propAt;

    if ( !qFuzzyCompare(_mu, rho._mu))
        return _mu < rho._mu;

    if (_srcType != rho._srcType)
        return _srcType < rho._srcType;

    if (_noOfSourcess != rho._noOfSourcess)
        return _noOfSourcess < rho._noOfSourcess;

    if (_newCallStr != rho._newCallStr)
        return _newCallStr < rho._newCallStr;

    if ( !qFuzzyCompare(_IncommingExPerDx, rho._IncommingExPerDx))
        return _IncommingExPerDx < rho._IncommingExPerDx;

    if (_callServStr != rho._callServStr)
        return _callServStr < rho._callServStr;

    if ( !qFuzzyCompare(_ServiceExPerDx, rho._ServiceExPerDx))
        return _ServiceExPerDx < rho._ServiceExPerDx;
    return false;
}

bool ModelTrClass::operator >(const ModelTrClass &rho) const
{
    if (_t != rho._t)
        return _t > rho._t;

    if (_propAt != rho._propAt)
        return _propAt > rho._propAt;

    if (!qFuzzyCompare(_mu, rho._mu))
        return _mu > rho._mu;

    if (_srcType != rho._srcType)
        return _srcType > rho._srcType;

    if (_noOfSourcess != rho._noOfSourcess)
        return _noOfSourcess > rho._noOfSourcess;

    if (_newCallStr != rho._newCallStr)
        return _newCallStr > rho._newCallStr;

    if (!qFuzzyCompare(_IncommingExPerDx, rho._IncommingExPerDx))
        return _IncommingExPerDx > rho._IncommingExPerDx;

    if (_callServStr != rho._callServStr)
        return _callServStr > rho._callServStr;

    if (!qFuzzyCompare(_ServiceExPerDx, rho._ServiceExPerDx))
        return _ServiceExPerDx > rho._ServiceExPerDx;
    return false;
}

bool ModelTrClass::operator <=(const ModelTrClass &rho) const
{
    if (*this == rho) return true;
    return *this < rho;
}

bool ModelTrClass::operator >=(const ModelTrClass &rho) const
{
    if (*this == rho) return true;
    return *this > rho;
}

double ModelTrClass::intensityNewCallForState(
          double lambdaZero
        , int stateN
        ) const
{
    double y = static_cast<double>(stateN)/_t;

    switch (srcType())
    {
    case ModelTrClass::SourceType::Independent:
        return lambdaZero;
    case ModelTrClass::SourceType::DependentMinus:
        return lambdaZero * (_noOfSourcess - y) / _noOfSourcess;
    case ModelTrClass::SourceType::DependentPlus:
        return lambdaZero * (_noOfSourcess + y) / _noOfSourcess;
    }
    return -1;
}

double ModelTrClass::intensityNewCallForY(double lambdaZero, double y) const
{
    switch (srcType())
    {
    case ModelTrClass::SourceType::Independent:
        return lambdaZero;
    case ModelTrClass::SourceType::DependentMinus:
        return lambdaZero * (_noOfSourcess - y) / _noOfSourcess;
    case ModelTrClass::SourceType::DependentPlus:
        return lambdaZero * (_noOfSourcess + y) / _noOfSourcess;
    }
    return -1;
}

bool MCRsc::operator>(const MCRsc &rho) const
{
    if (resourcess > rho.resourcess)
        return true;

    if (scheduler > rho.scheduler)
        return true;

    return false;
}

bool MCRsc::operator<(const MCRsc &rho) const
{
    if (resourcess < rho.resourcess)
        return true;

    if (scheduler < rho.scheduler)
        return true;

    return false;
}


ModelCreator::ModelCreator():
    _systemPolicy(SystemPolicy::NoBuffer)
  , id(0)
{
}

ModelCreator::~ModelCreator()
{
}

void ModelCreator::addClass(ModelTrClass *newClass)
{
    _traffic.trClasses.push_back(*newClass);
}

void ModelCreator::addServerGroups(ModelSubResourcess newGroup, bool optimize)
{
    if (optimize)
    {
        foreach(ModelSubResourcess tmp, _server.resourcess)
        {
            if (tmp.v() == newGroup.v())
            {
                tmp.set_k(tmp.k() + newGroup.k());
                return;
            }
        }
    }
    _server.resourcess.push_back(newGroup);
}

void ModelCreator::addBufferGroups(ModelSubResourcess qeue, bool optimize)
{
    if (optimize)
    {
        foreach(ModelSubResourcess tmp, _buffer.resourcess)
        {
            if (tmp.v() == qeue.v())
            {
                tmp.set_k(tmp.k() + qeue.k());
                return;
            }
        }
    }
    _buffer.resourcess.push_back(qeue);
}

void ModelCreator::setServerSchedulerAlgorithm(ResourcessScheduler algorithm)
{
    this->_server.scheduler = algorithm;
}

void ModelCreator::setBufferSchedulerAlgorithm(ResourcessScheduler algorithm)
{
    this->_buffer.scheduler = algorithm;
}

void ModelCreator::setSystemSchedulerAlgorithm(SystemPolicy algorithm)
{
    this->_systemPolicy = algorithm;
}

void ModelCreator::clearAll()
{
    _traffic.trClasses.clear();
    _server.resourcess.clear();
    _buffer.resourcess.clear();
}

QString ModelCreator::getGnuplotDescription() const
{
    QString result;
    QTextStream str;
    str.setString(&result);

    str<<"S"<<vk_s();
    if (k_s() > 1)
    {
        str<<"(";
        switch (_server.scheduler)
        {
        case ResourcessScheduler::Random:
            str<<"R";
            break;
        case ResourcessScheduler::Sequencial:
            str<<"S";
            break;
        }
        str<<k_s()<<")";
    }
    if (vk_b() > 0)
        str<<" B"<<vk_b();
    str<<" m"<<m();
    if (m()>0)
    {
        str<<"(";
        for(int i=0; i < m(); i++)
        {
            if (i > 0)
                str<<" ";
            const ModelTrClass tmp = getClass(i);
            str<<tmp;
        }
        str<<")";
    }
    return result;
}

const ModelTrClass& ModelCreator::getClass(int idx) const
{
    return _traffic.trClasses[idx];
}

ModelTrClass *ModelCreator::getClassClone(int idx) const
{
    ModelTrClass *result = new ModelTrClass(getClass(idx));
    return result;
}

const ModelSystem ModelCreator::getConstSyst() const
{
    ModelResourcess server(_server.resourcess, _server.scheduler);
    ModelResourcess buffer(_buffer.resourcess, _buffer.scheduler);
    ModelSystem result(_traffic.trClasses, server, buffer, _systemPolicy);
    return result;
}

ResourcessScheduler ModelCreator::getGroupsSchedulerAlgorithm() const
{
    return _server.scheduler;
}

bool ModelCreator::operator==(const ModelCreator &rho) const
{
    if (
            m()   != rho.m()
            || V()   != rho.V()
            || vk_s() != rho.vk_s()
            || vk_b() != rho.vk_b()
            )
        return false;

    bool result = true;


    if (_traffic != rho._traffic)
        return false;


    return result;
}

bool ModelCreator::operator !=(const ModelCreator &rho) const
{
    return !(*this == rho);
}

bool ModelCreator::operator >(const ModelCreator &rho) const
{
    if (V() > rho.V())
        return true;

    if (m() > rho.m())
        return true;

    if (vk_s() > rho.vk_s())
        return true;

    if (vk_b() > rho.vk_b())
        return true;

    if (_traffic > rho._traffic)
        return true;

    if (_server > rho._server)
        return true;

    if (_buffer > rho._buffer)
        return true;

    if (_systemPolicy > rho._systemPolicy)
        return true;

    return false;
}

bool ModelCreator::operator <(const ModelCreator &rho) const
{
    if (V() < rho.V())
        return true;

    if (m() < rho.m())
        return true;

    if (vk_s() < rho.vk_s())
        return true;

    if (vk_b() < rho.vk_b())
        return true;

    if (_traffic < rho._traffic)
        return true;

    if (_server < rho._server)
        return true;

    if (_buffer < rho._buffer)
        return true;

    if (_systemPolicy < rho._systemPolicy)
        return true;

    return false;
}

int ModelCreator::V() const
{
    int result = vk_s() + vk_b();
    return result;
}

int ModelCreator::v_sMax() const
{
    int result=0;
    for (int grType=0; grType< _server.resourcess.length(); grType++)
        if (result < _server.resourcess[grType].v())
            result = _server.resourcess[grType].v();
    return result;
}

int ModelCreator::k_s() const
{
    int result = 0;

    foreach (ModelSubResourcess tmp, _server.resourcess)
        result+= tmp.k();

    return result;
}

int ModelCreator::v_s(int groupClNo) const
{
    if (groupClNo < _server.resourcess.length())
        return _server.resourcess[groupClNo].v();
    return -1;
}

int ModelCreator::vk_s() const
{
    int result = 0;
    foreach (ModelSubResourcess tmp, _server.resourcess)
        result+= tmp.V();
    return result;
}

int ModelCreator::vk_s(int groupClNo) const
{
    if (groupClNo < _server.resourcess.length())
        return _server.resourcess[groupClNo].V();
    return -1;
}

int ModelCreator::k_s(int groupClNo) const
{
    if (groupClNo < _server.resourcess.length())
        return _server.resourcess[groupClNo].k();
    return -1;
}

int ModelCreator::k_sType() const
{
    return _server.resourcess.length();
}

int ModelCreator::v_b(int bufferClNo) const
{
    if (bufferClNo < _buffer.resourcess.length())
        return _buffer.resourcess[bufferClNo].v();
    return -1;
}

int ModelCreator::vk_b() const
{
    int result = 0;
    foreach (ModelSubResourcess tmp, _buffer.resourcess)
        result+= tmp.V();

    return result;
}

int ModelCreator::vk_b(int bufferClNo) const
{
    if (bufferClNo < _buffer.resourcess.length())
        return _buffer.resourcess[bufferClNo].v();
    return -1;
}

int ModelCreator::k_b() const
{
    int result = 0;
    foreach (ModelSubResourcess tmp, _buffer.resourcess)
        result+= tmp.k();
    return result;
}

int ModelCreator::k_b(int i) const
{
    if (i < _buffer.resourcess.length())
        return _buffer.resourcess[i].k();
    return -1;
}

int ModelCreator::k_bType() const
{
    return _buffer.resourcess.length();
}


QTextStream &operator<<(QTextStream &stream, enum ModelTrClass::StreamType strType)
{
    switch (strType) {
    case ModelTrClass::StreamType::Poisson: stream<<"Poisson";    break;
    case ModelTrClass::StreamType::Uniform: stream<<"Uniform";    break;
    case ModelTrClass::StreamType::Pareto:  stream<<"Pareto";     break;
    case ModelTrClass::StreamType::Gamma:   stream<<"Gamma";      break;
    case ModelTrClass::StreamType::Normal:  stream<<"NormalLike"; break;
    }
    return stream;
}
QDebug &operator<<(QDebug &stream, enum ModelTrClass::StreamType strType)
{
    switch (strType) {
    case ModelTrClass::StreamType::Poisson: stream<<"Poisson";    break;
    case ModelTrClass::StreamType::Uniform: stream<<"Uniform";    break;
    case ModelTrClass::StreamType::Pareto:  stream<<"Pareto";     break;
    case ModelTrClass::StreamType::Gamma:   stream<<"Gamma";      break;
    case ModelTrClass::StreamType::Normal:  stream<<"NormalLike"; break;
    }
    return stream;
}

QTextStream& operator<<(QTextStream &stream, const ModelCreator &model)
{
    stream<<"S"<<model.vk_s();
    if (model.k_s() > 1)
    {
        stream<<"(";
        switch (model._server.scheduler)
        {
        case ResourcessScheduler::Random:
            stream<<"R";
            break;
        case ResourcessScheduler::Sequencial:
            stream<<"S";
            break;
        }
        stream<<model.k_s()<<")";
    }
    if (model.vk_b() > 0)
        stream<<"_B"<<model.vk_b();
    stream<<"_m"<<model.m();
    if (model.m()>0)
    {
        bool first = true;
        stream<<"(";
        foreach(ModelTrClass tmp, model._traffic.trClasses)
        {
            if (!first)
                stream<<"_";
            first = false;
            stream<<tmp;
        }
        stream<<")";
    }
    return stream;
}

QTextStream& operator<<(QTextStream &stream, const ModelSystem &model)
{
    stream<<"S"<<model.getServer().V();
    if (model.getServer().k() > 1)
    {
        stream<<"(";
        switch (model.getServer().schedulerAlg)
        {
        case ResourcessScheduler::Random:
            stream<<"R";
            break;
        case ResourcessScheduler::Sequencial:
            stream<<"S";
            break;
        }
        stream<<model.getServer().k()<<")";
    }
    if (model.getBuffer().V() > 0)
        stream<<"_B"<<model.getBuffer().V();
    stream<<"_m"<<model.m();
    if (model.m()>0)
    {
        bool first = true;
        stream<<"(";
        foreach(ModelTrClass tmp, model.getTrClasses())
        {
            if (!first)
                stream<<"_";
            first = false;
            stream<<tmp;
        }
        stream<<")";
    }
    return stream;
}

QTextStream& operator<<(QTextStream &stream2, const ModelTrClass &trClass)
{
    stream2<<qSetRealNumberPrecision(2);

    if ((trClass.newCallStr()==ModelTrClass::StreamType::Poisson) && (trClass.callServStr()==ModelTrClass::StreamType::Poisson))
    {
        if (trClass.srcType()==ModelTrClass::SourceType::Independent)
            stream2<<"Er-"<<trClass.t();
        if (trClass.srcType()==ModelTrClass::SourceType::DependentMinus)
            stream2<<"En-"<<trClass.t()<<"("<<trClass.s()<<")";
        if (trClass.srcType()==ModelTrClass::SourceType::DependentPlus)
            stream2<<"Pa-"<<trClass.t()<<"("<<trClass.s()<<")";
    }
    else
    {
        if (trClass.srcType()==ModelTrClass::SourceType::Independent)
            stream2<<"Ind";
        if (trClass.srcType()==ModelTrClass::SourceType::DependentMinus)
            stream2<<"Dep-";
        if (trClass.srcType()==ModelTrClass::SourceType::DependentPlus)
            stream2<<"Dep+";

        stream2<<ModelTrClass::streamTypeToShortString(trClass.newCallStr());
        if (trClass.newCallStr()!=ModelTrClass::StreamType::Poisson)
            stream2<<trClass.getIncommingExPerDx();
        stream2<<ModelTrClass::streamTypeToShortString(trClass.callServStr());
        if (trClass.callServStr()!=ModelTrClass::StreamType::Poisson)
            stream2<<trClass.getServiceExPerDx();

        if (trClass.srcType()==ModelTrClass::SourceType::Independent)
            stream2<<"-"<<trClass.t();
        else
            stream2<<"-"<<trClass.t()<<"("<<trClass.s()<<")";
    }

    if ( !qFuzzyCompare(trClass.getMu(), 1) || (trClass.propAt() != 1))
            stream2<<"-"<<trClass.propAt();
    if ( !qFuzzyCompare(trClass.getMu(), 1))
        stream2<<"-"<<trClass.getMu();

    return stream2;
}

QDebug &operator<<(QDebug &stream, const ModelCreator &model)
{
    QDebug stream2 = stream.nospace();
    stream2<<"S"<<model.vk_s();
    if (model.k_s() > 1)
    {
        stream<<"(";
        switch (model._server.scheduler)
        {
        case ResourcessScheduler::Random:
            stream<<"R";
            break;
        case ResourcessScheduler::Sequencial:
            stream<<"S";
            break;
        }
        stream<<model.k_s()<<")";
    }
    stream2<<"_B"<<model.vk_b();
    stream2<<"_m"<<model.m();
    if (model.m()>0)
    {
        stream2<<"(";
        bool first = true;
        foreach(ModelTrClass tmp, model._traffic.trClasses)
        {
            if (!first)
                stream2<<"_";
            first = false;
            stream2<<tmp;
        }
        stream2<<")";
    }
    return stream;
}

QDebug &operator<<(QDebug &stream, const ModelTrClass &trClass)
{
    QDebug stream2 = stream.nospace();
    stream2<<qSetRealNumberPrecision(2);
    if ((trClass.newCallStr()==ModelTrClass::StreamType::Poisson) && (trClass.callServStr()==ModelTrClass::StreamType::Poisson))
    {
        if (trClass.srcType()==ModelTrClass::SourceType::Independent)
            stream2<<"Er-"<<trClass.t();
        if (trClass.srcType()==ModelTrClass::SourceType::DependentMinus)
            stream2<<"En-"<<trClass.t()<<"("<<trClass.s()<<")";
        if (trClass.srcType()==ModelTrClass::SourceType::DependentPlus)
            stream2<<"Pa-"<<trClass.t()<<"("<<trClass.s()<<")";
    }
    else
    {
        if (trClass.srcType()==ModelTrClass::SourceType::Independent)
            stream2<<"Ind";
        if (trClass.srcType()==ModelTrClass::SourceType::DependentMinus)
            stream2<<"Dep-";
        if (trClass.srcType()==ModelTrClass::SourceType::DependentPlus)
            stream2<<"Dep+";

        stream2<<ModelTrClass::streamTypeToShortString(trClass.newCallStr());
        if (trClass.newCallStr()!=ModelTrClass::StreamType::Poisson)
            stream2<<trClass.getIncommingExPerDx();
        stream2<<ModelTrClass::streamTypeToShortString(trClass.callServStr());
        if (trClass.callServStr()!=ModelTrClass::StreamType::Poisson)
            stream2<<trClass.getServiceExPerDx();

        if (trClass.srcType()==ModelTrClass::SourceType::Independent)
            stream2<<"-"<<trClass.t();
        else
            stream2<<"-"<<trClass.t()<<"("<<trClass.s()<<")";
    }

    if ( !qFuzzyCompare(trClass.getMu(), 1) || (trClass.propAt() != 1))
            stream2<<"-"<<trClass.propAt();
    if ( !qFuzzyCompare(trClass.getMu(), 1))
        stream2<<"-"<<trClass.getMu();
    return stream;
}

QDebug &operator<<(QDebug &stream, ModelTrClass &trClass)
{
    QDebug stream2 = stream.nospace();
    stream2<<qSetRealNumberPrecision(2);
    if ((trClass.newCallStr()==ModelTrClass::StreamType::Poisson) && (trClass.callServStr()==ModelTrClass::StreamType::Poisson))
    {
        if (trClass.srcType()==ModelTrClass::SourceType::Independent)
            stream2<<"Er-"<<trClass.t();
        if (trClass.srcType()==ModelTrClass::SourceType::DependentMinus)
            stream2<<"En-"<<trClass.t()<<"("<<trClass.s()<<")";
        if (trClass.srcType()==ModelTrClass::SourceType::DependentPlus)
            stream2<<"Pa-"<<trClass.t()<<"("<<trClass.s()<<")";
    }
    else
    {
        if (trClass.srcType()==ModelTrClass::SourceType::Independent)
            stream2<<"Ind";
        if (trClass.srcType()==ModelTrClass::SourceType::DependentMinus)
            stream2<<"Dep-";
        if (trClass.srcType()==ModelTrClass::SourceType::DependentPlus)
            stream2<<"Dep+";

        stream2<<ModelTrClass::streamTypeToShortString(trClass.newCallStr());
        if (trClass.newCallStr()!=ModelTrClass::StreamType::Poisson)
            stream2<<trClass.getIncommingExPerDx();
        stream2<<ModelTrClass::streamTypeToShortString(trClass.callServStr());
        if (trClass.callServStr()!=ModelTrClass::StreamType::Poisson)
            stream2<<trClass.getServiceExPerDx();

        if (trClass.srcType()==ModelTrClass::SourceType::Independent)
            stream2<<"-"<<trClass.t();
        else
            stream2<<"-"<<trClass.t()<<"("<<trClass.s()<<")";
    }

    if ( !qFuzzyCompare(trClass.getMu(), 1) || (trClass.propAt() != 1))
            stream2<<"-"<<trClass.propAt();
    if ( !qFuzzyCompare(trClass.getMu(), 1))
        stream2<<"-"<<trClass.getMu();
    return stream;
}


template <class P>  void ModelTrClass::SimulatorProcess_Indep::initializeT(double timeOfNewCall)
{
    this->procFun = P::newCall;
    system->addProcess(this, timeOfNewCall);
}

template <class P> bool ModelTrClass::SimulatorProcess_Indep::newCall(ModelTrClass::SimulatorSingleServiceSystem *system, ModelTrClass::SimulatorProcess *proc, double EOS_TIME)
{
    P *newProc = new P(system);
    newProc->initialize();

    proc->procFun = SimulatorProcess_Indep::endOfCallService;
    return system->addCall(proc, EOS_TIME);
}

ModelTrClass::SimulatorProcess_Indep::SimulatorProcess_Indep(ModelTrClass::SimulatorSingleServiceSystem *system): SimulatorProcess(system) {}

bool ModelTrClass::SimulatorProcess_Indep::endOfCallService(ModelTrClass::SimulatorSingleServiceSystem *system, ModelTrClass::SimulatorProcess *proc)
{
    system->endCallService(proc);
    return false;
}

template <class P> bool ModelTrClass::SimulatorProcess_DepMinus::newCall(ModelTrClass::SimulatorSingleServiceSystem *system, ModelTrClass::SimulatorProcess *proc, double EOS_TIME)
{

    proc->procFun = P::endOfCallService;
    bool result = system->addCall(proc, EOS_TIME);
    if (result == false)
    {
        P *newProc = new P(system);
        newProc->initialize();
    }
    return result;
}


template <class P> bool ModelTrClass::SimulatorProcess_DepMinus::endOfCallService(ModelTrClass::SimulatorSingleServiceSystem *system, ModelTrClass::SimulatorProcess *proc)
{
    system->endCallService(proc);

    P *newProc = new P(system);
    newProc->initialize();

    return false;
}

template <class P> bool ModelTrClass::SimulatorProcess_DepPlus::newCall(ModelTrClass::SimulatorSingleServiceSystem *system, ModelTrClass::SimulatorProcess *proc, double endOfServiceTime)
{
    P *procPascal = static_cast<P *>(proc);

    P *newProc = new P(system);
    newProc->initialize();

    if (procPascal->parent != nullptr)
    {
        procPascal->parent->child = newProc;
        newProc->parent = procPascal->parent;
        procPascal->parent = nullptr;
        procPascal->child = nullptr;
    }


    procPascal->procFun = P::endOfCallService;
    bool result = system->addCall(procPascal, endOfServiceTime /*system->funGetServEndTime()*/);
    if(result)
    {
        procPascal->child = new P(system);
        procPascal->child->initialize();
        procPascal->child->parent = procPascal;
    }
    return result;
}


template <class P> bool ModelTrClass::SimulatorProcess_DepPlus::endOfCallService(ModelTrClass::SimulatorSingleServiceSystem *system, ModelTrClass::SimulatorProcess *proc)
{
    P *procPascal = static_cast<P *>(proc);
    system->endCallService(procPascal);

    if (procPascal->child != nullptr)
    {
        if (procPascal->child->child != nullptr)
            qFatal("Something is wrong");
        system->removeProcess(procPascal->child);
        procPascal->child = nullptr;
    }


    return false;
}

std::random_device ModelTrClass::SimulatorSingleServiceSystem::rd;

QRandomGenerator generator = 123;

double ModelTrClass::SimulatorSingleServiceSystem::distrLambda(double Ex)
{
    double randomNumber;
    do
    {
        randomNumber = generator.generateDouble();// static_cast<double>(generator.generate()) / generator.max();
    }
    while (qFuzzyIsNull(randomNumber) || qFuzzyCompare(randomNumber, 1));

    double result = -log(randomNumber)*Ex;
    return result;  //return -Math.Log(Los, Math.E) / wspInt
}

double ModelTrClass::SimulatorSingleServiceSystem::distrUniform(double tMin, double tMax)
{
    double x = (tMax - tMin) * generator.generateDouble();//(static_cast<double>(generator.generate())/generator.max());
    return tMin+x;
}

ModelTrClass::SimulatorSingleServiceSystem::SimulatorSingleServiceSystem(int noOfSourcess, SourceType srcType, int Vs, int Vb, int t, TrClVector &states, double Aoffered, double IncommingEx2perDxDNewCall, double E_service, double D_service)
{
    agendaTimeOffset = 0;
    gen = std::mt19937_64(rd());

    n_inServer = 0;
    n_total = 0;
    this->Vs = Vs;
    this->Vb = Vb;
    this->states = &states;
    this->t = t;

    double deltaTServCall, deltaTnewCall;

    switch (srcType)
    {
    case ModelTrClass::SourceType::Independent:
        this->E_newCall = E_service / Aoffered;
        this->D_newCall = E_newCall * E_newCall / IncommingEx2perDxDNewCall;
        break;

    case ModelTrClass::SourceType::DependentMinus:
        this->E_newCall = E_service / Aoffered * (noOfSourcess - Aoffered);
        this->D_newCall = this->E_newCall * this->E_newCall / IncommingEx2perDxDNewCall;
        break;

    case ModelTrClass::SourceType::DependentPlus:
        //New Call
        this->E_newCall = E_service / Aoffered * (noOfSourcess + Aoffered);
        this->D_newCall = E_newCall * E_newCall / IncommingEx2perDxDNewCall;
        break;
    }

    deltaTnewCall =  sqrt(12*D_newCall);
    t_newCallMin = E_newCall-0.5*deltaTnewCall;
    t_newCallMax = E_newCall+0.5*deltaTnewCall;
    //Call service ending
    this->E_service = E_service;
    this->D_service = D_service;
    deltaTServCall =  sqrt(12*D_service);
    t_serviceMin = E_service-0.5*deltaTServCall;
    t_serviceMax = E_service+0.5*deltaTServCall;

    //Normal distribution
    distribNormalNewCall = std::normal_distribution<>(E_newCall, D_newCall);
    distribNormalService = std::normal_distribution<>(E_service, D_service);



    double betaNewCall  = D_newCall/E_newCall;
    double betaServ     = D_service/E_service;

    double alphaNewCall = E_newCall / betaNewCall;
    double alphaServ    = E_service / betaServ;


    distribGammaNewCall = std::gamma_distribution<double> (alphaNewCall, betaNewCall);
    distribGammaService = std::gamma_distribution<double> (alphaServ, betaServ);


    distribParetoNewCall =paretoDistrib(E_newCall, D_newCall);
    distribParetoService =paretoDistrib(E_service, D_service);

    callPartialyServiced = nullptr;
    resPartOccupied = 0;
}

ModelTrClass::SimulatorSingleServiceSystem::~SimulatorSingleServiceSystem()
{
    while (!agenda.empty())
    {
        delete agenda.top();
        agenda.pop();
    }
    while (!qeue.isEmpty())
        delete qeue.takeFirst();
}


double ModelTrClass::SimulatorSingleServiceSystem::timeNewCallNormal()
{
    //std::normal_distribution<>distribNormal(Ex, D);
    double result = 0;
    do
    {
        result = distribNormalNewCall(gen);
    }
    while (result <= 0);
    return result;

}

double ModelTrClass::SimulatorSingleServiceSystem::timeNewCallGamma()
{
    //std::gamma_distribution<double> distribution(2.0,2.0)
    double result = 0;
    do
    {
        result = distribGammaNewCall(gen);
    }
    while (result <= 0);
    return result;
}

double ModelTrClass::SimulatorSingleServiceSystem::timeNewCallPareto()
{
    double result = distribParetoNewCall(gen);
    return result;
}

double ModelTrClass::SimulatorSingleServiceSystem::timeServEndNormal()
{
//    std::normal_distribution<>distribNormal(Ex, D);
    double result = 0;
    do
    {
        result = distribNormalService(gen);
    }
    while (result <= 0);
    return result;
}

double ModelTrClass::SimulatorSingleServiceSystem::timeServEndGamma()
{
//    std::normal_distribution<>distribNormal(Ex, D);
    double result = 0;
    do
    {
        result = distribGammaService(gen);
    }
    while (result <= 0);
    return result;
}

double ModelTrClass::SimulatorSingleServiceSystem::timeServEndPareto()
{
   return distribParetoService(gen);
}

void ModelTrClass::SimulatorSingleServiceSystem::addProcess(ModelTrClass::SimulatorProcess *newProc, double relativeTime)
{
    newProc->time = relativeTime + agendaTimeOffset;
    agenda.push(newProc);
}

void ModelTrClass::SimulatorSingleServiceSystem::removeProcess(ModelTrClass::SimulatorProcess *proc)
{
    std::stack<SimulatorProcess *> tmpStack;
    while(!agenda.empty())
    {
        SimulatorProcess *tmpProc = agenda.top();
        if (tmpProc != proc)
            tmpStack.push(tmpProc);
        agenda.pop();
    }
    while (!tmpStack.empty())
    {
        agenda.push(tmpStack.top());
        tmpStack.pop();
    }
    delete proc;
}

void ModelTrClass::SimulatorSingleServiceSystem::stabilize(int noOfEvents)
{
    for( ;noOfEvents>0; noOfEvents--)
    {
        //SimulatorProcess *proc = agenda.takeFirst();

        SimulatorProcess *proc = agenda.top();
        agenda.pop();

        double tmp = proc->time;
        proc->time -= agendaTimeOffset;

        agendaTimeOffset = tmp;

        if (noOfEvents % 1024 == 0)
        {
            std::list<SimulatorProcess *> tmpAgenda;
            while (!agenda.empty())
            {
                tmpAgenda.push_front(agenda.top());
                agenda.pop();
            }
            for (auto iter = tmpAgenda.begin(); iter != tmpAgenda.end(); iter++)
            {
                (*iter)->time-=agendaTimeOffset;
                agenda.push(*iter);
            }
            agendaTimeOffset = 0;
        }

        if (!proc->execute(this))
            delete proc;
    }
}

void ModelTrClass::SimulatorSingleServiceSystem::doSimExperiment(long int noOfEvents, TrClVector &states)
{
    if (states.m() !=1)
        qFatal("Wrong distriobution");

    int classId = states.classId(0);

    for (int n=0; n<=this->Vs+Vb; n++)
    {
        states[n] = 0;

        states.getState(n).tIntOutNew = 0;
        states.getState(n).tIntOutEnd = 0;

        states.getState(n).tIntInNew = 0;
        states.getState(n).tIntInEnd = 0;

        states.setIntInNew(n, classId, 0);
        states.setIntInEnd(n, classId, 0);

        states.setIntOutNew(n, classId, 0);
        states.setIntOutEnd(n, classId, 0);
    }
    for( ;noOfEvents>0; noOfEvents--)
    {
        int oldState = n_total;
        SimulatorProcess *proc = agenda.top();
        agenda.pop();

        double tmp = proc->time;
        proc->time -= agendaTimeOffset;
        agendaTimeOffset = tmp;

        if (noOfEvents % 1024 == 0)
        {
            std::list<SimulatorProcess *> tmpAgenda;

            while(!agenda.empty())
            {
                tmpAgenda.push_front(agenda.top());
                agenda.pop();
            }
            for (auto iterator = tmpAgenda.begin(); iterator != tmpAgenda.end(); iterator++)
            {
                (*iterator)->time -= agendaTimeOffset;
                agenda.push(*iterator);
            }
            agendaTimeOffset = 0;
        }

        double deltaTime = proc->time;
        states[n_total] += deltaTime;

        if (!proc->execute(this))
            delete proc;


        if (n_total > oldState)
        {
            states.getState(oldState).tIntOutNew++;
            states.getState(n_total).tIntInNew++;
        }
        if (n_total < oldState)
        {
            states.getState(oldState).tIntOutEnd++;
            states.getState(n_total).tIntInEnd++;
        }
        if (n_total == oldState)
        {
#ifdef QT_DEBUG
            if (n_total + t < Vs+Vb)
                qFatal("Wimulation error, wrong assumptions");
#endif
            states.getState(oldState).tIntOutNew++;
        }
    }
    for (int n=0; n<=this->Vs+Vb; n++)
    {
        double time = states[n];
        if (qFuzzyIsNull(time))
        {
            states.getState(n).tIntOutNew = 0;
            states.getState(n).tIntOutEnd = 0;
        }
        else
        {
            states.getState(n).tIntOutNew /= time;
            states.getState(n).tIntOutEnd /= time;
        }

        if (n >= t)
        {
            time = states[n-t];
            if (qFuzzyIsNull(time))
                states.getState(n).tIntInNew = 0;
            else
                states.getState(n).tIntInNew /= time;
        }
        else
            states.getState(n).tIntInNew = 0;

        if (n+t <= Vs+Vb)
        {
            time = states[n+t];
            if (qFuzzyIsNull(time))
                states.getState(n).tIntInEnd = 0;
            else
                states.getState(n).tIntInEnd /= time;
        }
        else
            states.getState(n).tIntInEnd = 0;

        states.setIntInNew(n, classId, states.getState(n).tIntInNew);
        states.setIntInEnd(n, classId, states.getState(n).tIntInEnd);
        states.setIntOutNew(n, classId, states.getState(n).tIntOutNew);
        states.setIntOutEnd(n, classId, states.getState(n).tIntOutEnd);
    }
}

bool ModelTrClass::SimulatorSingleServiceSystem::addCall(ModelTrClass::SimulatorProcess *call, double timeOfService)
{
    if (n_inServer+t <= Vs)
    {
        n_inServer +=t;
        addProcess(call, timeOfService);

        n_total +=t;
        return true;
    }

    if (n_total + t > Vs+Vb)
        return false;

    if (n_inServer < Vs)
    {
        timeOfService *= t;
        timeOfService /= (Vs-n_inServer);
        n_inServer = Vs;
        addProcess(call, timeOfService);

        n_total +=t;
        return true;
    }

    call->time = timeOfService;
    qeue.append(call);

    n_total +=t;
    return true;
}

void ModelTrClass::SimulatorSingleServiceSystem::endCallService(ModelTrClass::SimulatorProcess *call)
{
    n_total -=t;
    if (callPartialyServiced == call)
    {
        n_inServer -= resPartOccupied;
        resPartOccupied = 0;
        callPartialyServiced = nullptr;
    }
    else
    {
        n_inServer -= t;
    }

    //Take call from qeue
    if (callPartialyServiced)
    {
        double timeToTheEnd = callPartialyServiced->time;
        timeToTheEnd /= t;
        timeToTheEnd *=resPartOccupied;
        callPartialyServiced->time = timeToTheEnd;

        n_inServer +=t;
        n_inServer -= resPartOccupied;
        resPartOccupied = 0;
        callPartialyServiced = nullptr;

        std::list<SimulatorProcess *> tmpList;
        while (!agenda.empty())
        {
            tmpList.push_back(agenda.top());
            agenda.pop();
        }
        for (auto iterator = tmpList.begin(); iterator != tmpList.end(); iterator++)
        {
            agenda.push(*iterator);
        }
    }

    while (n_inServer < Vs)
    {
        if (qeue.length() > 0)
        {
            if (n_inServer + t <= Vs)
            {
                n_inServer += t;
                SimulatorProcess *tmp = qeue.takeFirst();
                double time = tmp->time;
                addProcess(tmp, time);
            }
            else
            {
                callPartialyServiced = qeue.takeFirst();
                double ratio = static_cast<double>(t)/static_cast<double>(Vs-n_inServer);
                double time = callPartialyServiced->time * ratio;
                addProcess(callPartialyServiced, time);
                n_inServer = Vs;
            }
        }
        else
            break;
    }
}

bool ModelTrClass::SimulatorSingleServiceSystem::simProcComparer(const ModelTrClass::SimulatorProcess *a, const ModelTrClass::SimulatorProcess *b) { return a->time < b->time; }


bool ModelTrClass::SimulatorProcess::operator<(const ModelTrClass::SimulatorProcess &rho) const
{
    return time < rho.time;
}

ModelTrClass::SimulatorProcess::SimulatorProcess(ModelTrClass::SimulatorSingleServiceSystem *system): system(system) {}

ModelTrClass::SimulatorProcess::~SimulatorProcess() {}

bool ModelTrClass::SimulatorProcess::execute(ModelTrClass::SimulatorSingleServiceSystem *system)
{
    return procFun(system, this);
}

ModelTrClass::SimulatorProcess_DepMinus::SimulatorProcess_DepMinus(ModelTrClass::SimulatorSingleServiceSystem *system): SimulatorProcess(system) {}

ModelTrClass::SimulatorProcess_DepPlus::SimulatorProcess_DepPlus(ModelTrClass::SimulatorSingleServiceSystem *system): SimulatorProcess(system), child(nullptr), parent(nullptr) {}


#define CLASS_SIMULATOR_INDEP_CPP(X,Y,FUN_TIME_NEW,FUN_TIME_END) \
void ModelTrClass::SimulatorProcess_Indep##X##Y::initialize()\
{\
    initializeT<SimulatorProcess_Indep##X##Y>(system->time##FUN_TIME_NEW());\
}\
\
bool ModelTrClass::SimulatorProcess_Indep##X##Y::newCall(SimulatorSingleServiceSystem *system, SimulatorProcess *proc)\
{\
    return SimulatorProcess_Indep::newCall<SimulatorProcess_Indep##X##Y>(system, proc, system->time##FUN_TIME_END());\
}

#define CLASS_SIMULATOR_DEP_MINUS_CPP(X,Y,FUN_TIME_NEW,FUN_TIME_END) \
void ModelTrClass::SimulatorProcess_DepMinus##X##Y::initialize()\
{\
    this->procFun = SimulatorProcess_DepMinus##X##Y::newCall;\
    system->addProcess(this, system->time##FUN_TIME_NEW());\
}\
bool ModelTrClass::SimulatorProcess_DepMinus##X##Y::newCall(SimulatorSingleServiceSystem *system, SimulatorProcess *proc)\
{\
    return SimulatorProcess_DepMinus::newCall<SimulatorProcess_DepMinus##X##Y>(system, proc, system->time##FUN_TIME_END());\
}\
bool ModelTrClass::SimulatorProcess_DepMinus##X##Y::endOfCallService(SimulatorSingleServiceSystem *system, SimulatorProcess *proc)\
{\
    return SimulatorProcess_DepMinus::endOfCallService<SimulatorProcess_DepMinus##X##Y>(system, proc);\
}

#define CLASS_SIMULATOR_DEP_PLUS_CPP(X,Y,FUN_TIME_NEW,FUN_TIME_END) \
void ModelTrClass::SimulatorProcess_DepPlus##X##Y::initialize()\
{\
    this->procFun = SimulatorProcess_DepPlus##X##Y::newCall;\
    system->addProcess(this, system->time##FUN_TIME_NEW());\
}\
\
bool ModelTrClass::SimulatorProcess_DepPlus##X##Y::newCall(SimulatorSingleServiceSystem *system, SimulatorProcess *proc)\
{\
    return SimulatorProcess_DepPlus::newCall<SimulatorProcess_DepPlus##X##Y>(system, proc, system->time##FUN_TIME_END());\
}\
bool ModelTrClass::SimulatorProcess_DepPlus##X##Y::endOfCallService(SimulatorSingleServiceSystem *system, SimulatorProcess *proc)\
{\
    return SimulatorProcess_DepPlus::endOfCallService<SimulatorProcess_DepPlus##X##Y>(system, proc);\
}


ModelSystem::ModelSystem(const ModelSystem &system)
  : _trClasses(system._trClasses)
  , _server(system._server)
  , _buffer(system._buffer)
  , _bufferPolicy(system._bufferPolicy)
  , _totalAt(system._totalAt)
  , _V(system._V)
{
}

ModelSystem::ModelSystem(const QVector<ModelTrClass> &trClasses, const ModelResourcess &server, const ModelResourcess &buffer, SystemPolicy bufferPolicy)
  : _trClasses(trClasses)
  , _server(server)
  , _buffer(buffer)
  , _bufferPolicy(bufferPolicy)
{
    _totalAt = 0;
    foreach(ModelTrClass tmp, trClasses)
        _totalAt+= tmp.propAt();
    _V = _server.V() + _buffer.V();
}

bool ModelSystem::operator==(const ModelSystem &rho) const
{
    if (_trClasses != rho._trClasses)
        return false;

    if (_server != rho._server)
        return false;

    if (_buffer != rho._buffer)
        return false;

    if (_bufferPolicy != rho._bufferPolicy)
        return false;

    return true;
}

QDebug &operator<<(QDebug &stream, const ModelSystem &model)
{
    QDebug stream2 = stream.nospace();
    stream2<<"S"<<model.getServer().V();
    if (model.getServer().k() > 1)
    {
        stream<<"(";
        switch (model.getServer().schedulerAlg)
        {
        case ResourcessScheduler::Random:
            stream<<"R";
            break;
        case ResourcessScheduler::Sequencial:
            stream<<"S";
            break;
        }
        stream<<model.getServer().k()<<")";
    }
    stream2<<"_B"<<model.getBuffer().V();
    stream2<<"_m"<<model.m();
    if (model.m()>0)
    {
        stream2<<"(";
        bool first = true;
        foreach(ModelTrClass tmp, model.getTrClasses())
        {
            if (!first)
                stream2<<"_";
            first = false;
            stream2<<tmp;        }
        stream2<<")";
    }
    return stream;
}


bool ModelSystem::isInBlockingState(int classNo, const QVector<int> &serverGroupsState, const QVector<int> bufferGroupsState) const
{
    bool result = true;

    int groupTypeNo = 0;
    int localSubgroupNo = 0;

    for (int subgroupNo=0; subgroupNo<this->_server.k(); subgroupNo++)
    {
        if (localSubgroupNo > _server.k(groupTypeNo))
        {
            localSubgroupNo = 0;
            groupTypeNo++;
        }
        if (serverGroupsState[subgroupNo] + t(classNo) <= _server.V(groupTypeNo, localSubgroupNo))
        {
            result = false;
            break;
        }
    }


    groupTypeNo = 0;
    localSubgroupNo = 0;

    if (result)
    {
        for (int subgroupNo=0; subgroupNo<this->_buffer.k(); subgroupNo++)
        {
            if (localSubgroupNo > _buffer.k(groupTypeNo))
            {
                localSubgroupNo = 0;
                groupTypeNo++;
            }
            if (bufferGroupsState[subgroupNo] + t(classNo) <= _buffer.V(groupTypeNo, localSubgroupNo))
            {
                result = false;
                break;
            }
        }
    }
    return result;
}

bool ModelSystem::isServerAvailable(int classNo, const QVector<int> &serverGroupsState) const
{
    for (int sgrNo=0; sgrNo<getServer().k(); sgrNo++)
        if(getServer().V(sgrNo) - t(classNo) - serverGroupsState[classNo] >= 0)
            return true;
    return false;
}

bool ModelSystem::isBufferAvailable(int classNo, const QVector<int> &bufferGroupsState) const
{
    for (int sgrNo=0; sgrNo<getBuffer().k(); sgrNo++)
        if(getBuffer().V(sgrNo) - t(classNo) - bufferGroupsState[classNo] >= 0)
            return true;
    return false;
}

ModelResourcess::ModelResourcess(QList<ModelSubResourcess> listSubRes, ResourcessScheduler schedulerAlg)
    : schedulerAlg(schedulerAlg)
    , _listSubRes(listSubRes)
    , _k(listSubRes.length())
{
    _V = 0;
    _vMax = 0;
    foreach(ModelSubResourcess tmp, listSubRes)
    {
        _V+= tmp.V();
        if (tmp.v() > _vMax)
            _vMax = tmp.v();

        for (int i=0; i<tmp.k(); i++)
            _subgrpCapacity.append(tmp.v());
    }
}

bool ModelResourcess::operator==(const ModelResourcess &rho) const
{
    if (_listSubRes != rho._listSubRes)
        return false;
    return true;
}

bool ModelResourcess::operator!=(const ModelResourcess &rho) const
{
    if (_listSubRes != rho._listSubRes)
        return true;

    return false;
}

int ModelResourcess::V(int groupClassNo, int groupNo) const
{
    return ((groupClassNo>=0) && (groupClassNo < _listSubRes.length())) ?
                ((groupNo>=0) && (groupNo < _listSubRes[groupClassNo].k())) ? _listSubRes[groupClassNo].v() : 0
    : 0;
}

bool MCTrCl::operator>(const MCTrCl &rho) const
{
    return trClasses > rho.trClasses;
}

bool MCTrCl::operator<(const MCTrCl &rho) const
{
    return trClasses < rho.trClasses;
}

bool MCTrCl::operator==(const MCTrCl &rho) const
{
    if (this->trClasses.length() != rho.trClasses.length())
        return false;
    if (totalAt() != rho.totalAt())
        return false;

    for(int clNo = 0; clNo < trClasses.length(); clNo++)
    {
        if (trClasses[clNo] != rho.trClasses[clNo])
                return false;
    }

    return true;
}

bool MCTrCl::operator!=(const MCTrCl &rho) const
{
    if (this->trClasses.length() != rho.trClasses.length())
        return true;
    if (totalAt() != rho.totalAt())
        return true;

    for(int clNo = 0; clNo < trClasses.length(); clNo++)
    {
        if (trClasses[clNo] != rho.trClasses[clNo])
                return true;
    }
    return false;
}



ModelTrClassSimulationWork::ModelTrClassSimulationWork(TrClVector *states, int Vs, int Vb, double Aoffered, int t
    , ModelTrClass::SourceType srcNewCallSrcType, ModelTrClass::StreamType newCallStreamType, double IncommingEx2perDxDNewCall
    , ModelTrClass::StreamType endCallStreamType, double EsingleCallServ, double DsingleCallServ, int noOfEventsPerUnit)
{
    this->states = states;
    system = new ModelTrClass::SimulatorSingleServiceSystem(0, srcNewCallSrcType, Vs, Vb, t, *states, Aoffered, IncommingEx2perDxDNewCall, EsingleCallServ, DsingleCallServ);
    proc = nullptr;
    this->noOfEventsPerUnit = noOfEventsPerUnit;

    switch (newCallStreamType)
    {
    case ModelTrClass::StreamType::Poisson:
        switch (endCallStreamType)
        {
        case ModelTrClass::StreamType::Poisson:
            proc = new ModelTrClass::SimulatorProcess_IndepMM(system);
            break;
        case ModelTrClass::StreamType::Uniform:
            proc = new ModelTrClass::SimulatorProcess_IndepMU(system);
            break;
        case ModelTrClass::StreamType::Normal:
            proc = new ModelTrClass::SimulatorProcess_IndepMN(system);
            break;
        case ModelTrClass::StreamType::Gamma:
            proc = new ModelTrClass::SimulatorProcess_IndepMG(system);
            break;
        case ModelTrClass::StreamType::Pareto:
            proc = new ModelTrClass::SimulatorProcess_IndepMP(system);
            break;
        }
        break;
    case ModelTrClass::StreamType::Uniform:
        switch (endCallStreamType)
        {
        case ModelTrClass::StreamType::Poisson:
            proc = new ModelTrClass::SimulatorProcess_IndepUM(system);
            break;
        case ModelTrClass::StreamType::Uniform:
            proc = new ModelTrClass::SimulatorProcess_IndepUU(system);
            break;
        case ModelTrClass::StreamType::Normal:
            proc = new ModelTrClass::SimulatorProcess_IndepUN(system);
            break;
        case ModelTrClass::StreamType::Gamma:
            proc = new ModelTrClass::SimulatorProcess_IndepUG(system);
            break;
        case ModelTrClass::StreamType::Pareto:
            proc = new ModelTrClass::SimulatorProcess_IndepUP(system);
            break;
        }
        break;
    case ModelTrClass::StreamType::Normal:
        switch (endCallStreamType)
        {
        case ModelTrClass::StreamType::Poisson:
            proc = new ModelTrClass::SimulatorProcess_IndepNM(system);
            break;
        case ModelTrClass::StreamType::Uniform:
            proc = new ModelTrClass::SimulatorProcess_IndepNU(system);
            break;
        case ModelTrClass::StreamType::Normal:
            proc = new ModelTrClass::SimulatorProcess_IndepNN(system);
            break;
        case ModelTrClass::StreamType::Gamma:
            proc = new ModelTrClass::SimulatorProcess_IndepNG(system);
            break;
        case ModelTrClass::StreamType::Pareto:
            proc = new ModelTrClass::SimulatorProcess_IndepNP(system);
            break;
        }
        break;
    case ModelTrClass::StreamType::Gamma:
        switch (endCallStreamType)
        {
        case ModelTrClass::StreamType::Poisson:
            proc = new ModelTrClass::SimulatorProcess_IndepGM(system);
            break;
        case ModelTrClass::StreamType::Uniform:
            proc = new ModelTrClass::SimulatorProcess_IndepGU(system);
            break;
        case ModelTrClass::StreamType::Normal:
            proc = new ModelTrClass::SimulatorProcess_IndepGN(system);
            break;
        case ModelTrClass::StreamType::Gamma:
            proc = new ModelTrClass::SimulatorProcess_IndepGG(system);
            break;
        case ModelTrClass::StreamType::Pareto:
            proc = new ModelTrClass::SimulatorProcess_IndepGP(system);
            break;
        }
        break;

    case ModelTrClass::StreamType::Pareto:
        switch (endCallStreamType)
        {
        case ModelTrClass::StreamType::Poisson:
            proc = new ModelTrClass::SimulatorProcess_IndepPM(system);
            break;
        case ModelTrClass::StreamType::Uniform:
            proc = new ModelTrClass::SimulatorProcess_IndepPU(system);
            break;
        case ModelTrClass::StreamType::Normal:
            proc = new ModelTrClass::SimulatorProcess_IndepPN(system);
            break;
        case ModelTrClass::StreamType::Gamma:
            proc = new ModelTrClass::SimulatorProcess_IndepPG(system);
            break;
        case ModelTrClass::StreamType::Pareto:
            proc = new ModelTrClass::SimulatorProcess_IndepPP(system);
            break;
        }
        break;
    }
    if (proc == nullptr)
        qFatal("Simulation Critical error: not supported traffic class");

}

ModelTrClassSimulationWork::~ModelTrClassSimulationWork()
{
    delete system;
}

void ModelTrClassSimulationWork::run()
{
    proc->initialize();

    for (int n=0; n<=system->Vs+system->Vb; n++)
    {
        states->getState(n).p = 0;
        states->getState(n).tIntInEnd = 0;
        states->getState(n).tIntInNew = 0;
        states->getState(n).tIntOutEnd = 0;
        states->getState(n).tIntOutNew = 0;
    }

    int stabilizationLen = (1+system->Vs+system->Vb) * (1+system->Vs+system->Vb) * noOfEventsPerUnit / 100 / system->getT();
    long int expLen = (1+system->Vs+system->Vb) * (1 + system->Vs+system->Vb);
    expLen*= (noOfEventsPerUnit / system->getT());
    system->stabilize(stabilizationLen);
    system->doSimExperiment(expLen, *states);

    double sum = 0;
    for (int n=0; n<=system->Vs+system->Vb; n++)
        sum += states->getState(n).p;
    for (int n=0; n<=system->Vs+system->Vb; n++)
        states->getState(n).p/= sum;
    sum = 1;
}

CLASS_SIMULATOR_INDEP_CPP(M, M, NewCallExp, ServEndExp)
CLASS_SIMULATOR_INDEP_CPP(M, U, NewCallExp, ServEndUni)
CLASS_SIMULATOR_INDEP_CPP(M, N, NewCallExp, ServEndNormal)
CLASS_SIMULATOR_INDEP_CPP(M, G, NewCallExp, ServEndGamma)
CLASS_SIMULATOR_INDEP_CPP(M, P, NewCallExp, ServEndPareto)

CLASS_SIMULATOR_INDEP_CPP(U, M, NewCallUni, ServEndExp)
CLASS_SIMULATOR_INDEP_CPP(U, U, NewCallUni, ServEndUni)
CLASS_SIMULATOR_INDEP_CPP(U, N, NewCallUni, ServEndNormal)
CLASS_SIMULATOR_INDEP_CPP(U, G, NewCallUni, ServEndGamma)
CLASS_SIMULATOR_INDEP_CPP(U, P, NewCallUni, ServEndPareto)

CLASS_SIMULATOR_INDEP_CPP(N, M, NewCallNormal, ServEndExp)
CLASS_SIMULATOR_INDEP_CPP(N, U, NewCallNormal, ServEndUni)
CLASS_SIMULATOR_INDEP_CPP(N, N, NewCallNormal, ServEndNormal)
CLASS_SIMULATOR_INDEP_CPP(N, G, NewCallNormal, ServEndGamma)
CLASS_SIMULATOR_INDEP_CPP(N, P, NewCallNormal, ServEndPareto)

CLASS_SIMULATOR_INDEP_CPP(G, M, NewCallGamma, ServEndExp)
CLASS_SIMULATOR_INDEP_CPP(G, U, NewCallGamma, ServEndUni)
CLASS_SIMULATOR_INDEP_CPP(G, N, NewCallGamma, ServEndNormal)
CLASS_SIMULATOR_INDEP_CPP(G, G, NewCallGamma, ServEndGamma)
CLASS_SIMULATOR_INDEP_CPP(G, P, NewCallGamma, ServEndPareto)

CLASS_SIMULATOR_INDEP_CPP(P, M, NewCallPareto, ServEndExp)
CLASS_SIMULATOR_INDEP_CPP(P, U, NewCallPareto, ServEndUni)
CLASS_SIMULATOR_INDEP_CPP(P, N, NewCallPareto, ServEndNormal)
CLASS_SIMULATOR_INDEP_CPP(P, G, NewCallPareto, ServEndGamma)
CLASS_SIMULATOR_INDEP_CPP(P, P, NewCallPareto, ServEndPareto)

CLASS_SIMULATOR_DEP_MINUS_CPP(M, M, NewCallExp, ServEndExp)
CLASS_SIMULATOR_DEP_MINUS_CPP(M, U, NewCallExp, ServEndUni)
CLASS_SIMULATOR_DEP_MINUS_CPP(M, N, NewCallExp, ServEndNormal)
CLASS_SIMULATOR_DEP_MINUS_CPP(M, G, NewCallExp, ServEndGamma)
CLASS_SIMULATOR_DEP_MINUS_CPP(M, P, NewCallExp, ServEndPareto)

CLASS_SIMULATOR_DEP_MINUS_CPP(U, M, NewCallUni, ServEndExp)
CLASS_SIMULATOR_DEP_MINUS_CPP(U, U, NewCallUni, ServEndUni)
CLASS_SIMULATOR_DEP_MINUS_CPP(U, N, NewCallUni, ServEndNormal)
CLASS_SIMULATOR_DEP_MINUS_CPP(U, G, NewCallUni, ServEndGamma)
CLASS_SIMULATOR_DEP_MINUS_CPP(U, P, NewCallUni, ServEndPareto)

CLASS_SIMULATOR_DEP_MINUS_CPP(N, M, NewCallNormal, ServEndExp)
CLASS_SIMULATOR_DEP_MINUS_CPP(N, U, NewCallNormal, ServEndUni)
CLASS_SIMULATOR_DEP_MINUS_CPP(N, N, NewCallNormal, ServEndNormal)
CLASS_SIMULATOR_DEP_MINUS_CPP(N, G, NewCallNormal, ServEndGamma)
CLASS_SIMULATOR_DEP_MINUS_CPP(N, P, NewCallNormal, ServEndPareto)

CLASS_SIMULATOR_DEP_MINUS_CPP(G, M, NewCallGamma, ServEndExp)
CLASS_SIMULATOR_DEP_MINUS_CPP(G, U, NewCallGamma, ServEndUni)
CLASS_SIMULATOR_DEP_MINUS_CPP(G, N, NewCallGamma, ServEndNormal)
CLASS_SIMULATOR_DEP_MINUS_CPP(G, G, NewCallGamma, ServEndGamma)
CLASS_SIMULATOR_DEP_MINUS_CPP(G, P, NewCallGamma, ServEndPareto)

CLASS_SIMULATOR_DEP_MINUS_CPP(P, M, NewCallPareto, ServEndExp)
CLASS_SIMULATOR_DEP_MINUS_CPP(P, U, NewCallPareto, ServEndUni)
CLASS_SIMULATOR_DEP_MINUS_CPP(P, N, NewCallPareto, ServEndNormal)
CLASS_SIMULATOR_DEP_MINUS_CPP(P, G, NewCallPareto, ServEndGamma)
CLASS_SIMULATOR_DEP_MINUS_CPP(P, P, NewCallPareto, ServEndPareto)

CLASS_SIMULATOR_DEP_PLUS_CPP(M, M, NewCallExp, ServEndExp)
CLASS_SIMULATOR_DEP_PLUS_CPP(M, U, NewCallExp, ServEndUni)
CLASS_SIMULATOR_DEP_PLUS_CPP(M, N, NewCallExp, ServEndNormal)
CLASS_SIMULATOR_DEP_PLUS_CPP(M, G, NewCallExp, ServEndGamma)
CLASS_SIMULATOR_DEP_PLUS_CPP(M, P, NewCallExp, ServEndPareto)

CLASS_SIMULATOR_DEP_PLUS_CPP(U, M, NewCallUni, ServEndExp)
CLASS_SIMULATOR_DEP_PLUS_CPP(U, U, NewCallUni, ServEndUni)
CLASS_SIMULATOR_DEP_PLUS_CPP(U, N, NewCallUni, ServEndNormal)
CLASS_SIMULATOR_DEP_PLUS_CPP(U, G, NewCallUni, ServEndGamma)
CLASS_SIMULATOR_DEP_PLUS_CPP(U, P, NewCallUni, ServEndPareto)

CLASS_SIMULATOR_DEP_PLUS_CPP(N, M, NewCallNormal, ServEndExp)
CLASS_SIMULATOR_DEP_PLUS_CPP(N, U, NewCallNormal, ServEndUni)
CLASS_SIMULATOR_DEP_PLUS_CPP(N, N, NewCallNormal, ServEndNormal)
CLASS_SIMULATOR_DEP_PLUS_CPP(N, G, NewCallNormal, ServEndGamma)
CLASS_SIMULATOR_DEP_PLUS_CPP(N, P, NewCallNormal, ServEndPareto)

CLASS_SIMULATOR_DEP_PLUS_CPP(G, M, NewCallGamma, ServEndExp)
CLASS_SIMULATOR_DEP_PLUS_CPP(G, U, NewCallGamma, ServEndUni)
CLASS_SIMULATOR_DEP_PLUS_CPP(G, N, NewCallGamma, ServEndNormal)
CLASS_SIMULATOR_DEP_PLUS_CPP(G, G, NewCallGamma, ServEndGamma)
CLASS_SIMULATOR_DEP_PLUS_CPP(G, P, NewCallGamma, ServEndPareto)

CLASS_SIMULATOR_DEP_PLUS_CPP(P, M, NewCallPareto, ServEndExp)
CLASS_SIMULATOR_DEP_PLUS_CPP(P, U, NewCallPareto, ServEndUni)
CLASS_SIMULATOR_DEP_PLUS_CPP(P, N, NewCallPareto, ServEndNormal)
CLASS_SIMULATOR_DEP_PLUS_CPP(P, G, NewCallPareto, ServEndGamma)
CLASS_SIMULATOR_DEP_PLUS_CPP(P, P, NewCallPareto, ServEndPareto)
