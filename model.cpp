#include <string.h>
#include <QString>
#include <QTextStream>
#include <QDebug>

#include <math.h>
#include "model.h"
#include "utils/probDistributions.h"

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

QString bufferResourcessSchedulerToString(BufferPolicy value)
{
    QString result;
    switch (value)
    {
    case BufferPolicy::Continuos:
        result = "Continuos";
        break;

    case BufferPolicy::dFIFO_Seq:
        result = "Discrette";
        break;

    case BufferPolicy::qFIFO_Seq:
        result = "Discrette qFIFO";
        break;

    case BufferPolicy::SD_FIFO:
        result = "State dependent";
        break;

    case BufferPolicy::Disabled:
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

int ModelSubResourcess::v()
{
    return _v;
}

int ModelSubResourcess::k()
{
    return _k;
}

int ModelSubResourcess::V()
{
    return _v * _k;
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

QString ModelTrClass::shortName() const
{
    QString result;


    if ((newCallStr()==StreamType::Poisson) && (callServStr() == StreamType::Poisson))
    {
        switch (srcType())
        {
        case SourceType::Independent:
            if ((_propAt != 1) || (! qFuzzyCompare(_mu, 1)))
                result.sprintf("Erl. at=%d t=%d µ=%.1f", _propAt, _t, _mu);
            else
                result.sprintf("Erlang t=%d", _t);
            break;
        case SourceType::DependentMinus:
            if ((_propAt != 1) || (! qFuzzyCompare(_mu, 1)))
                result.sprintf("Eng. t=%d S=%d at=%d µ=%.1f", _t, _noOfSourcess, _propAt, _mu);
            else
                result.sprintf("Engset t=%d S=%d", _t, _noOfSourcess);
            break;
        case SourceType::DependentPlus:
            if ((_propAt != 1) || (! qFuzzyCompare(_mu, 1)))
                result.sprintf("Pas+ t=%d S=%d at=%d  µ=%.1f", _t, _noOfSourcess, _propAt, _mu);
            else
                result.sprintf("Pascal t=%d S=%d", _t, _noOfSourcess);
            break;
        }
    }
    else
    {
        if (srcType() == SourceType::Independent)
            result= "Ind ";
        else if (srcType()==SourceType::DependentMinus)
            result = "Dep- ";
        else
            result = "Deb+ ";

        QString incStr;
        if (newCallStr()==StreamType::Poisson)
            incStr = QString("%1/").arg(streamTypeToShortString(newCallStr()));
        else
            incStr = QString("%1%2/").arg(streamTypeToShortString(newCallStr())).arg(_IncommingExPerDx);
        result.append(incStr);


        QString servStr;
        if (callServStr()==StreamType::Poisson)
            servStr = QString("%1 ").arg(streamTypeToShortString(callServStr()));
        else
            servStr = QString("%1%2 ").arg(streamTypeToShortString(callServStr())).arg(_ServiceExPerDx);

        result.append(servStr);

        if (_noOfSourcess != 0)
            result.append(QString("S=%1 ").arg(_noOfSourcess));

        if ((_propAt != 1) || ( ! qFuzzyCompare(_mu, 1)))
            result.append(QString("at=%1 t=%2 µ=%3").arg(_propAt).arg(_t).arg(_mu));
        else
            result.append(QString("t=%1").arg(_t));
    }
    return result;
}

void ModelTrClass::doSimExpUnlimitedSoNo(TrClVector &states, int Vs, int Vb, double Aoffered, double IncommingEx2perDxDNewCall, double EsingleCallServ, double DsingleCallServ) const
{
    SimulatorProcess *proc = nullptr;
    SimulatorSingleServiceSystem *System = new SimulatorSingleServiceSystem(0, srcType(), Vs, Vb, this->_t, states, Aoffered, IncommingEx2perDxDNewCall, EsingleCallServ, DsingleCallServ);



    switch (newCallStr())
    {
    case StreamType::Poisson:
        switch (callServStr())
        {
        case StreamType::Poisson:
            proc = new SimulatorProcess_IndepMM(System);
            break;
        case StreamType::Uniform:
            proc = new SimulatorProcess_IndepMU(System);
            break;
        case StreamType::Normal:
            proc = new SimulatorProcess_IndepMN(System);
            break;
        case StreamType::Gamma:
            proc = new SimulatorProcess_IndepMG(System);
            break;
        case StreamType::Pareto:
            proc = new SimulatorProcess_IndepMP(System);
            break;
        }
        break;
    case StreamType::Uniform:
        switch (callServStr())
        {
        case StreamType::Poisson:
            proc = new SimulatorProcess_IndepUM(System);
            break;
        case StreamType::Uniform:
            proc = new SimulatorProcess_IndepUU(System);
            break;
        case StreamType::Normal:
            proc = new SimulatorProcess_IndepUN(System);
            break;
        case StreamType::Gamma:
            proc = new SimulatorProcess_IndepUG(System);
            break;
        case StreamType::Pareto:
            proc = new SimulatorProcess_IndepUP(System);
            break;
        }
        break;
    case StreamType::Normal:
        switch (callServStr())
        {
        case StreamType::Poisson:
            proc = new SimulatorProcess_IndepNM(System);
            break;
        case StreamType::Uniform:
            proc = new SimulatorProcess_IndepNU(System);
            break;
        case StreamType::Normal:
            proc = new SimulatorProcess_IndepNN(System);
            break;
        case StreamType::Gamma:
            proc = new SimulatorProcess_IndepNG(System);
            break;
        case StreamType::Pareto:
            proc = new SimulatorProcess_IndepNP(System);
            break;
        }
        break;
    case StreamType::Gamma:
        switch (callServStr())
        {
        case StreamType::Poisson:
            proc = new SimulatorProcess_IndepGM(System);
            break;
        case StreamType::Uniform:
            proc = new SimulatorProcess_IndepGU(System);
            break;
        case StreamType::Normal:
            proc = new SimulatorProcess_IndepGN(System);
            break;
        case StreamType::Gamma:
            proc = new SimulatorProcess_IndepGG(System);
            break;
        case StreamType::Pareto:
            proc = new SimulatorProcess_IndepGP(System);
            break;
        }
        break;

    case StreamType::Pareto:
        switch (callServStr())
        {
        case StreamType::Poisson:
            proc = new SimulatorProcess_IndepPM(System);
            break;
        case StreamType::Uniform:
            proc = new SimulatorProcess_IndepPU(System);
            break;
        case StreamType::Normal:
            proc = new SimulatorProcess_IndepPN(System);
            break;
        case StreamType::Gamma:
            proc = new SimulatorProcess_IndepPG(System);
            break;
        case StreamType::Pareto:
            proc = new SimulatorProcess_IndepPP(System);
            break;
        }
        break;
    }
    if (proc == nullptr)
        qFatal("Simulation Critical error: not supported traffic class");

    proc->initialize();

    System->stabilize((1+Vs+Vb) * 10000 / t());
    System->doSimExperiment((Vs+Vb+1) * 100000 / t() , states);

    double sum = 0;
    for (int n=0; n<=Vs+Vb; n++)
        sum += states[n];
    for (int n=0; n<=Vs+Vb; n++)
        states[n] /= sum;
    sum = 1;

    delete System;
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

    System->stabilize((1+Vs+Vb) * 10000/ t());
    System->doSimExperiment((1+Vs+Vb) * 100000 / t(), states);

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

TrClVector ModelTrClass::trDistribution(int classIdx, double A, int Vs, int Vb) const
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
            doSimExpUnlimitedSoNo(result, Vs, Vb, A, getIncommingExPerDx(), EservSingleCall, DservSingleCall);
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

ModelCreator::ModelCreator():
    _noOfTrClasses(0)
  , _serverSchedulerAlgorithm(ResourcessScheduler::Sequencial)
  , _noOfTypesOfGroups(0)
  , _totalGroupsCapacity(0)
  , _totalNumberOfGroups(0)
  , _bufferPolicy(BufferPolicy::Disabled)
  , _noOfTypesOfBuffers(0)
  , _totalBufferCapacity(0)
  , _totalNumberOfBuffers(0)
  , _totalAt(0)
  , _parWasChanged(true)
  , id(0)
{
    _capacityTrClasses = 4;
    _trClasses = new ModelTrClass*[_capacityTrClasses];

    _capacityTypeOfGroups = 2;
    _servers = new ModelSubResourcess[_capacityTypeOfGroups];

    _capacityTypeOfQeues = 2;
    _bufers = new ModelSubResourcess[_capacityTypeOfQeues];
}

ModelCreator::~ModelCreator()
{
    for  (int i=0; i<_noOfTrClasses; i++)
        delete _trClasses[i];
    delete []_trClasses;

    delete []_servers;
    delete []_bufers;
}

void ModelCreator::getServerGroupDescription(int32_t **k
  , int32_t **v
  , int32_t *numberOfTypes
) const
{
    *numberOfTypes = this->_noOfTypesOfGroups;
    *k = new int32_t[this->_noOfTypesOfGroups];
    *v = new int32_t[this->_noOfTypesOfGroups];

    for (int32_t i=0; i<*numberOfTypes; i++)
    {
        *k[i] = this->_servers[i].k();
        *v[i] = this->_servers[i].v();
    }
}

void ModelCreator::getBufferGroupDescription(
          int32_t **k
        , int32_t **v
        , int32_t *numberOfTypes
        ) const
{
    *numberOfTypes = this->_noOfTypesOfBuffers;
    *k = new int32_t[this->_noOfTypesOfBuffers];
    *v = new int32_t[this->_noOfTypesOfBuffers];

    for (int32_t i=0; i<*numberOfTypes; i++)
    {
        *k[i] = this->_bufers[i].k();
        *v[i] = this->_bufers[i].v();
    }
}

const QVector<int> &ModelCreator::getServerGroupCapacityVector() const
{
    if (_parWasChanged)
        updateConstSyst();
    return constSyst.vs;
}

const QVector<int> &ModelCreator::getBufferCapacityVector() const
{
    if (_parWasChanged)
        updateConstSyst();
    return constSyst.vb;
}


void ModelCreator::updateConstSyst() const
{
    int index = 0;

    constSyst.m = m();
    constSyst.Vs = vk_s();
    constSyst.Vb = vk_b();

    constSyst.ks = k_s();
    constSyst.kb = k_b();

    constSyst.vs.resize(k_s());
    index = 0;
    for (int j=0; j < _noOfTypesOfGroups; j++)
        for (int j2=0; j2<this->k_s(j); j2++)
            constSyst.vs[index++] = this->v_s(j);

    constSyst.vb.resize(k_b());
    index = 0;
    for (int j=0; j < _noOfTypesOfBuffers; j++)
        for (int j2=0; j2<this->k_b(j); j2++)
            constSyst.vb[index++] = this->v_b(j);

    constSyst.t.resize(m());
    index = 0;
    for (int i=0; i < m(); i++)
    {
        constSyst.t[i] = getClass(i)->t();
    }

    _parWasChanged = false;
}


void ModelCreator::addClass(ModelTrClass *newClass)
{
    _parWasChanged = true;
    if (_noOfTrClasses == _capacityTrClasses)
    {
        ModelTrClass **newTrClasses = new ModelTrClass*[2*_capacityTrClasses];
        memcpy(newTrClasses, _trClasses, static_cast<size_t>(_capacityTrClasses) * sizeof(ModelTrClass *));
        delete _trClasses;
        _trClasses = newTrClasses;
        _capacityTrClasses *=2;
    }
    _trClasses[_noOfTrClasses] = new ModelTrClass();
    *_trClasses[_noOfTrClasses] = *newClass;
    _noOfTrClasses++;

    _totalAt += newClass->propAt();
}

void ModelCreator::addGroups(ModelSubResourcess newGroup, bool optimize)
{
    _parWasChanged = true;
    _totalNumberOfGroups += newGroup.k();
    _totalGroupsCapacity += newGroup.V();
    if (optimize)
    {
        for (int idx=0; idx < _noOfTypesOfGroups; idx++)
        {
            if (_servers[idx].v() == newGroup.v())
            {
                int k = _servers[idx].k();
                _servers[idx].set_k(k + newGroup.k());
                    return;
            }
        }
    }

    if (_noOfTypesOfGroups == _capacityTypeOfGroups)
    {
        ModelSubResourcess *newRes = new ModelSubResourcess[2*_capacityTypeOfGroups];
        memcpy(newRes, _servers, static_cast<size_t>(_capacityTypeOfGroups) * sizeof(ModelSubResourcess));
        delete _servers;
        _servers = newRes;
        _capacityTypeOfGroups *=2;
    }
    _servers[_noOfTypesOfGroups] = newGroup;
    _noOfTypesOfGroups++;
}

void ModelCreator::addQeues(ModelSubResourcess qeue, bool optimize)
{
    _parWasChanged = true;
    _totalNumberOfBuffers += qeue.k();
    _totalBufferCapacity += qeue.V();

    if (optimize)
    {
        for (int idx=0; idx < _noOfTypesOfBuffers; idx++)
        {
            if (_bufers[idx].v() == qeue.v())
            {
                int k = _bufers[idx].k();
                _bufers[idx].set_k(k + qeue.k());
                return;
            }
        }
    }

    if (_noOfTypesOfBuffers == _capacityTypeOfQeues)
    {
        ModelSubResourcess *newQue = new ModelSubResourcess[2*_capacityTypeOfQeues];
        memcpy(newQue, _bufers, static_cast<size_t>(_capacityTypeOfQeues) * sizeof(ModelSubResourcess));
        delete _bufers;
        _bufers = newQue;
        _capacityTypeOfQeues *=2;
    }
    _bufers[_noOfTypesOfBuffers] = qeue;
    _noOfTypesOfBuffers++;
}

void ModelCreator::setServerSchedulerAlgorithm(ResourcessScheduler algorithm)
{
    this->_serverSchedulerAlgorithm = algorithm;
}

void ModelCreator::setBufferSchedulerAlgorithm(BufferPolicy algorithm)
{
    this->_bufferPolicy = algorithm;
}

void ModelCreator::clearAll()
{
    _parWasChanged = true;
    for (int i=0; i<_noOfTrClasses; i++)
        delete _trClasses[i];
    _noOfTrClasses = 0;
    _totalAt = 0;

    _totalNumberOfGroups = 0;
    _totalGroupsCapacity = 0;
    _noOfTypesOfGroups=0;

    _totalBufferCapacity = 0;
    _noOfTypesOfBuffers = 0;
    _totalNumberOfBuffers = 0;
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
        switch (_serverSchedulerAlgorithm)
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
            const ModelTrClass *tmp = getClass(i);
            str<<*tmp;
        }
        str<<")";
    }
    return result;
}

const ModelTrClass *ModelCreator::getClass(int idx) const
{
    if (idx < _noOfTrClasses)
        return _trClasses[idx];
    return nullptr;
}

ModelTrClass *ModelCreator::getClassClone(int idx) const
{
    ModelTrClass *result = nullptr;
    if (idx < _noOfTrClasses)
        result = new ModelTrClass(*_trClasses[idx]);

    return result;
}

const ModelCreator::ConstSyst &ModelCreator::getConstSyst() const
{
    if (_parWasChanged)
        updateConstSyst();
    return constSyst;
}

ResourcessScheduler ModelCreator::getGroupsSchedulerAlgorithm() const
{
    return _serverSchedulerAlgorithm;
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

    //TODO dopisać operator == dla klas
    for(int classIdx=0; classIdx<m(); classIdx++)
    {
        if (*getClass(classIdx) != *rho.getClass(classIdx))
            return false;
    }


    int32_t *vA, *vB, *kA, *kB, cA, cB;

    getServerGroupDescription(&kA, &vA, &cA);
    rho.getServerGroupDescription(&kB, &vB, &cB);

    if (cA != cB)
        result = false;
    else
        for (int32_t idx=0; idx<cA; idx++)
        {
            if ((vA[idx] != vB[idx])||(kA[idx] != kB[idx]))
            {
                result = false;
                break;
            }
        }

    if (this->_serverSchedulerAlgorithm != rho._serverSchedulerAlgorithm)
        return false;

    delete []vA;
    delete []vB;
    delete []kA;
    delete []kB;

    getBufferGroupDescription(&kA, &vA, &cA);
    rho.getBufferGroupDescription(&kB, &vB, &cB);

    if (cA != cB)
        result = false;
    else
        for (int32_t idx=0; idx<cA; idx++)
        {
            if ((vA[idx] != vB[idx])||(kA[idx] != kB[idx]))
            {
                result = false;
                break;
            }
        }
    if ((cA > 0) && (_bufferPolicy != rho._bufferPolicy))
        return false;

    delete []vA;
    delete []vB;
    delete []kA;
    delete []kB;

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

    for(int classIdx=0; classIdx<m(); classIdx++)
        if (*(getClass(classIdx)) > *(rho.getClass(classIdx)))
                return true;

    bool result = true;
    int32_t *vA, *vB, *kA, *kB, cA, cB;

    getServerGroupDescription(&kA, &vA, &cA);
    rho.getServerGroupDescription(&kB, &vB, &cB);

    if (cA != cB)
        result = (cA > cB);
    else
    {
        for (int32_t idx=0; idx<cA; idx++)
        {
            if (vA[idx] != vB[idx])
            {
                result = (vA[idx] > vB[idx]);
                break;
            }
            if (kA[idx] != kB[idx])
            {
                result = (kA[idx] > kB[idx]);
                break;
            }
        }
    }

    delete []vA;
    delete []vB;
    delete []kA;
    delete []kB;
    return result;
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

    for(int classIdx=0; classIdx<m(); classIdx++)
        if (*(getClass(classIdx)) < *(rho.getClass(classIdx)))
                return true;

    bool result = true;
    int32_t *vA, *vB, *kA, *kB, cA, cB;

    getServerGroupDescription(&kA, &vA, &cA);
    rho.getServerGroupDescription(&kB, &vB, &cB);

    if (cA != cB)
        result = (cA < cB);
    else
    {
        for (int32_t idx=0; idx<cA; idx++)
        {
            if (vA[idx] != vB[idx])
            {
                result = (vA[idx] < vB[idx]);
                break;
            }
            if (kA[idx] != kB[idx])
            {
                result = (kA[idx] < kB[idx]);
                break;
            }
        }
    }

    delete []vA;
    delete []vB;
    delete []kA;
    delete []kB;
    return result;
}

int ModelCreator::v_sMax() const
{
    int result=0;
    for (int grType=0; grType<_noOfTypesOfGroups; grType++)
        if (result < _servers[grType].v())
            result = _servers[grType].v();
    return result;
}

int ModelCreator::v_s(int groupClNo) const
{
    if (groupClNo < _noOfTypesOfGroups)
        return _servers[groupClNo].v();
    return -1;
}

int ModelCreator::vk_s(int groupClNo) const
{
    if (groupClNo < _noOfTypesOfGroups)
        return _servers[groupClNo].V();
    return -1;
}

int ModelCreator::k_s(int groupClNo) const
{
    if (groupClNo < _noOfTypesOfGroups)
        return _servers[groupClNo].k();
    return -1;
}

int ModelCreator::v_b(int bufferClNo) const
{
    if (bufferClNo < _noOfTypesOfBuffers)
        return _bufers[bufferClNo].v();
    return -1;
}

int ModelCreator::vk_b(int bufferClNo) const
{
    if (bufferClNo < _noOfTypesOfBuffers)
        return _bufers[bufferClNo].v() * _bufers[bufferClNo].k();
    return -1;
}

int ModelCreator::k_b(int i) const
{
    if (i < _noOfTypesOfBuffers)
        return _bufers[i].k();
    return -1;
}


QTextStream& operator<<(QTextStream &stream, const ModelCreator &model)
{
    stream<<"S"<<model.vk_s();
    if (model.k_s() > 1)
    {
        stream<<"(";
        switch (model._serverSchedulerAlgorithm)
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
        stream<<"(";
        for(int i=0; i<model.m(); i++)
        {
            if (i > 0)
                stream<<"_";
            const ModelTrClass *tmp = model.getClass(i);
            stream<<*tmp;
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
        switch (model._serverSchedulerAlgorithm)
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
        for(int i=0; i<model.m(); i++)
        {
            if (i > 0)
                stream2<<"_";
            const ModelTrClass *tmp = model.getClass(i);
            stream2<<*tmp;
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

double ModelTrClass::SimulatorSingleServiceSystem::distrLambda(double Ex)
{
    double randomNumber;
    do
    {
        randomNumber = static_cast<double>(qrand())/RAND_MAX;
    }
    while (qFuzzyIsNull(randomNumber) || qFuzzyCompare(randomNumber, 1));

    double result = -log(randomNumber)*Ex;
    return result;  //return -Math.Log(Los, Math.E) / wspInt
}

double ModelTrClass::SimulatorSingleServiceSystem::distrUniform(double tMin, double tMax)
{
    double x = (tMax - tMin) * (static_cast<double>(qrand())/RAND_MAX);
    return tMin+x;
}

ModelTrClass::SimulatorSingleServiceSystem::SimulatorSingleServiceSystem(int noOfSourcess, SourceType srcType, int Vs, int Vb, int t, TrClVector &states, double Aoffered, double IncommingEx2perDxDNewCall, double E_service, double D_service)
{
    gen = std::mt19937(rd());

    n_inServer = 0;
    n_total = 0;
    this->Vs = Vs;
    this->Vb = Vb;
    this->states = &states;
    this->t = t;

    this->time = 0;


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
    while (!agenda.isEmpty())
        delete agenda.takeFirst();
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
    newProc->time = relativeTime+this->time;
    agenda.append(newProc);
    std::sort(agenda.begin(), agenda.end(), simProcComparer);
}

void ModelTrClass::SimulatorSingleServiceSystem::removeProcess(ModelTrClass::SimulatorProcess *proc)
{
    agenda.removeOne(proc);
    delete proc;
}

void ModelTrClass::SimulatorSingleServiceSystem::stabilize(int noOfEvents)
{
    for( ;noOfEvents>0; noOfEvents--)
    {
        SimulatorProcess *proc = agenda.takeFirst();
        time = proc->time;
        if (!proc->execute(this))
            delete proc;
    }
}

void ModelTrClass::SimulatorSingleServiceSystem::doSimExperiment(int noOfEvents, TrClVector &states)
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
        double lastTime = time;

        int oldState = n_total;
        SimulatorProcess *proc = agenda.takeFirst();
        time = proc->time;
        double deltaTime = time - lastTime;
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
            //TODO makesure that always n_total = Vs+Vb
#ifdef QT_DEBUG
            if (n_total + t < this->Vs+Vb)
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
        double timeToTheEnd = callPartialyServiced->time - time;
        timeToTheEnd /= t;
        timeToTheEnd *=resPartOccupied;
        callPartialyServiced->time = time + timeToTheEnd;

        n_inServer +=t;
        n_inServer -= resPartOccupied;
        resPartOccupied = 0;
        callPartialyServiced = nullptr;
        std::sort(agenda.begin(), agenda.end(), simProcComparer);
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




bool ModelCreator::ConstSyst::isInBlockingState(int classNo, const QVector<int> &serverGroupsState, const QVector<int> bufferGroupsState) const
{
    bool result = true;
    for (int x=0; x < vs.length(); x++)
    {
        if (serverGroupsState[x] + t[classNo] <= vs[x])
        {
            result = false;
            break;
        }
    }
    if (result)
    {
        for (int x=0; x < vb.length(); x++)
        {
            if (bufferGroupsState[x] + t[classNo] <= vb[x])
            {
                result = false;
                break;
            }
        }
    }
    return result;
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
