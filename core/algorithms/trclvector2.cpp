#include "trclvector2.h"
#include <QDebug>
#include <stdexcept>
#include <math.h>

#include <QJsonArray>

TrClVector::TrClVector(): previous(nullptr)
{
    _states.resize(1);
    _states[0].p = 1;

    aggregatedClasses.clear();
}

TrClVector::TrClVector(const TrClVector &rho): aggregatedClasses(rho.aggregatedClasses)
{
    _states = rho._states;//.fill(state(), rho._lastIdx+1);

    if (rho.previous != nullptr)
        previous = new TrClVector(*(rho.previous));
    else
        previous = nullptr;
}

TrClVector::TrClVector(int LastIdx): previous(nullptr)
{
    _states.resize(LastIdx + 1);
    _states[0].p = 1;
}

TrClVector::TrClVector(int V, const QVector<pairIdT> &aggregatedClasses): previous(nullptr), aggregatedClasses(aggregatedClasses)
{
    _states.resize(V+1);
    _states[0].p = 1;
}

TrClVector::~TrClVector()
{
    _states.clear();

    if (previous != nullptr)
        delete previous;
}

QJsonObject TrClVector::getJson() const
{
    QJsonObject result;

    QJsonArray p;
    QJsonArray arrivalIntensity;
    QJsonArray serviceIntensity;


    for (int n=0; n<=V(); n++)
    {
        p.push_back(QJsonValue(this->_states[n].p));
        arrivalIntensity.push_back(QJsonValue(this->_states[n].tIntOutNew));
        serviceIntensity.push_back(QJsonValue(this->_states[n].tIntOutEnd));
    }


    result.insert("V", V());
    result.insert("P", p);
    result.insert("newCallInt", arrivalIntensity);
    result.insert("servInt", serviceIntensity);

    return result;
}

QString TrClVector::getCvs(char cvsSeparator) const
{
    QString result;
    QTextStream stream(&result);

    for (int n=0; n<=V(); n++)
    {
        stream<<QString::number(_states[n].p, 'e', 7)<<cvsSeparator<<QString::number(_states[n].tIntOutNew, 'e', 7)<<cvsSeparator<<QString::number(_states[n].tIntOutEnd, 'e', 7);
        if (n<V())
            stream<<cvsSeparator;
    }
    return result;
}

double &TrClVector::operator[](int n)
{
    if (n > V())
        throw std::out_of_range("Vector's index is out of range");
    return _states[n].p;
}

void TrClVector::setState(int n, const State &refState)
{
    _states[n] = refState;
}

State &TrClVector::getState(int n)
{
    if (n > V())
        throw std::out_of_range("Vector's index is out of range");
    return _states[n];
}

void TrClVector::setIntInNew(int n, int i, double value)
{
    if (n > V())
        throw std::out_of_range("Vector's index is out of range");

    int classIdx = internalClassId(i);
    if (classIdx == -1)
        throw std::out_of_range("Can't find proper class");

    _states[n].intInNewSC[classIdx] = value;
}

void TrClVector::setIntInEnd(int n, int i, double value)
{
    if (n > V())
        throw std::out_of_range("Vector's index is out of range");

    int classIdx = internalClassId(i);
    if (classIdx == -1)
        throw std::out_of_range("Can't find proper class");

    _states[n].intInEndSC[classIdx] = value;
}

void TrClVector::setIntOutNew(int n, int i, double value)
{
    if (n > V())
        throw std::out_of_range("Vector's index is out of range");

    int classIdx = internalClassId(i);
    if (classIdx == -1)
        throw std::out_of_range("Can't find proper class");

    _states[n].intOutNewSC[classIdx] = value;
}

void TrClVector::setIntOutEnd(int n, int i, double value)
{
    if (n > V())
        throw std::out_of_range("Vector's index is out of range");

    int classIdx = internalClassId(i);
    if (classIdx == -1)
        throw std::out_of_range("Can't find proper class");

    _states[n].intOutEndSC[classIdx] = value;
}

void TrClVector::setY(int n, int i, double value)
{
    if (n > V())
        throw std::out_of_range("Vector's index is out of range");

    int classIdx = internalClassId(i);
    if (classIdx == -1)
        throw std::out_of_range("Can't find proper class");

    _states[n].y[classIdx] = value;
}

double TrClVector::getIntInNew(int n, int i) const
{
    if (n > V())
        throw std::out_of_range("Vector's index is out of range");

    int classIdx = internalClassId(i);
    if (classIdx == -1)
        throw std::out_of_range("Can't find proper class");

    double result = _states[n].intInNewSC[classIdx];
    return result;
}

double TrClVector::getIntInEnd(int n, int i) const
{
    if (n > V())
        throw std::out_of_range("Vector's index is out of range");

    int classIdx = internalClassId(i);
    if (classIdx == -1)
        throw std::out_of_range("Can't find proper class");

    return _states[n].intInEndSC[classIdx];
}

double TrClVector::getIntOutNew(int n, int i) const
{
    if (n > V())
        throw std::out_of_range("Vector's index is out of range");

    int classIdx = internalClassId(i);
    if (classIdx == -1)
        throw std::out_of_range("Can't find proper class");

    double result = _states[n].intOutNewSC[classIdx];
    return result;
}

double TrClVector::getIntOutEnd(int n, int i) const
{
    if (n > V())
        throw std::out_of_range("Vector's index is out of range");

    int classIdx = internalClassId(i);
    if (classIdx == -1)
        throw std::out_of_range("Can't find proper class");

    return _states[n].intOutEndSC[classIdx];
}

double TrClVector::getY(int n, int i) const
{
    if (n > V())
        throw std::out_of_range("Vector's index is out of range");

    int classIdx = internalClassId(i);
    if (classIdx == -1)
        throw std::out_of_range("Can't find proper class");

    return _states[n].y[classIdx];
}

TrClVector &TrClVector::operator=(const TrClVector &rho)
{
    //if(_states2 == rho._states2)
    //    return *this;    
    // if (this->V() != rho.V())
    //{///TODO don't remove all
    //    _states2.clear();
    //    _states2.fill(state(), rho._lastIdx + 1);
    //    _lastIdx = rho._lastIdx;
    //}
    //for (int n=0; n<=_lastIdx; n++)
    //    _states2[n] = rho._states2[n];

    _states = rho._states;

    aggregatedClasses = rho.aggregatedClasses;
    previous = (rho.previous == nullptr) ? nullptr : new TrClVector(*(rho.previous));

    return *this;
}

void TrClVector::normalize(double sumOfAllTheStates)
{
    if (qFuzzyIsNull(sumOfAllTheStates))
        qFatal("sum of all the states can't be 0");

    double sum = 0;
    for (int n=0; n<=V(); n++)
    {
#ifdef QT_DEBUG
        if (std::isnan(_states[n].p))
            qFatal("p_n = NAN");
#endif
        sum += _states[n].p;
    }

    for (int n=0; n<=V(); n++)
    {
        _states[n].p *= sumOfAllTheStates;
        _states[n].p /= sum;

#ifdef QT_DEBUG
        if (std::isnan(_states[n].p))
            qFatal("p_n = NAN");
#endif
    }
    if (previous != nullptr)
        previous->normalize(sumOfAllTheStates - _states[V()].p);
}

void TrClVector::generateNormalizedPoissonPrevDistrib()
{
    if (previous != nullptr)
        qFatal("Previous distribution exists");

    if (V() == 0)
        return;

    previous = new TrClVector(this->V()-1, this->aggregatedClasses);

    for (int n=0; n<V(); n++)
        previous->_states[n] = _states[n];
    previous->normalize();
    previous->generateNormalizedPoissonPrevDistrib();
}

void TrClVector::generateDeNormalizedPoissonPrevDistrib()
{
    if (previous != nullptr)
        qFatal("Previous distribution exists");

    if (V() == 0)
        return;

    previous = new TrClVector(V()-1, this->aggregatedClasses);

    for (int n=0; n<V(); n++)
        previous->_states[n] = _states[n];
    previous->normalize(1.0-_states[V()].p);
    previous->generateNormalizedPoissonPrevDistrib();
}

const TrClVector *TrClVector::getTruncatedVector(int lastIndex) const
{
    if (V() == lastIndex)
        return this;

    TrClVector *result = previous;

    while (result != nullptr)
    {
        if (result->V() == lastIndex)
            break;
        result = result->previous;
    }
    return result;
}

void TrClVector::prepareResult(TrClVector &result, const TrClVector &Pa, const TrClVector &Pb, int len)
{
    int n;

    if (len == 0)
        len = qMax(Pa.V(), Pb.V());

    result.aggregatedClasses.clear();
    for (int i=0; i<Pa.aggregatedClasses.length(); i++)
    {
        result.aggregatedClasses.append(Pa.aggregatedClasses[i]);

    }
    for (int i=0; i<Pb.aggregatedClasses.length(); i++)
    {
        result.aggregatedClasses.append(Pb.aggregatedClasses[i]);
    }
    for (n=0; n<=len; n++)
    {
        result._states[n].m = result.aggregatedClasses.length();
        result._states[n].intInNewSC = new double[result._states[n].m];
        result._states[n].intInEndSC = new double[result._states[n].m];
        result._states[n].intOutNewSC = new double[result._states[n].m];
        result._states[n].intOutEndSC = new double[result._states[n].m];
        result._states[n].y = new double[result._states[n].m];

        bzero(result._states[n].intInNewSC, static_cast<size_t>(result._states[n].m) * sizeof(double));
        bzero(result._states[n].intInEndSC, static_cast<size_t>(result._states[n].m) * sizeof(double));
        bzero(result._states[n].intOutNewSC, static_cast<size_t>(result._states[n].m) * sizeof(double));
        bzero(result._states[n].intOutEndSC, static_cast<size_t>(result._states[n].m) * sizeof(double));
        bzero(result._states[n].y, static_cast<size_t>(result._states[n].m) * sizeof(double));
    }
}

TrClVector TrClVector::convFAG(const TrClVector &Pa, const TrClVector &Pb, bool doNormalization, int len)
{
    int n, la, lb;
    if (len == 0)
        len = qMax(Pa.V(), Pb.V());

    double sum = 0;
    TrClVector result(len);

    prepareResult(result, Pa, Pb, len);

    for (n=0; n<=len; n++)
    {
        result[n]=0;
        for (la=0; la<=n; la++)
        {
            if (la > Pa.V())
                continue;

            lb = n-la;
            if (lb > Pb.V())
                continue;

            result[n] += Pa._states[la].p * Pb._states[lb].p;

            result._states[n].tIntOutNew += (Pa._states[la].p * Pb._states[lb].p * (Pa._states[la].tIntOutNew + Pb._states[lb].tIntOutNew));
            result._states[n].tIntOutEnd += (Pa._states[la].p * Pb._states[lb].p * (Pa._states[la].tIntOutEnd + Pb._states[lb].tIntOutEnd));

            for (int i=0; i<result.aggregatedClasses.length(); i++)
            {
                int clIdx = result.aggregatedClasses[i].id;

                int clIdxA = Pa.internalClassId(clIdx);
                int clIdxB = Pb.internalClassId(clIdx);

                double intOutNewA = (clIdxA == -1) ? 0 : Pa._states[la].intOutNewSC[clIdxA];
                double intOutNewB = (clIdxB == -1) ? 0 : Pb._states[lb].intOutNewSC[clIdxB];
                double intOutEndA = (clIdxA == -1) ? 0 : Pa._states[la].intOutEndSC[clIdxA];
                double intOutEndB = (clIdxB == -1) ? 0 : Pb._states[lb].intOutEndSC[clIdxB];

                double yA = (clIdxA == -1) ? 0 : Pa._states[la].y[clIdxA];
                double yB = (clIdxB == -1) ? 0 : Pb._states[lb].y[clIdxB];

                result._states[n].intOutNewSC[i] += Pa._states[la].p * Pb._states[lb].p * (intOutNewA + intOutNewB);
                result._states[n].intOutEndSC[i] += Pa._states[la].p * Pb._states[lb].p * (intOutEndA + intOutEndB);
                result._states[n].y[i] += Pa._states[la].p * Pb._states[lb].p * (yA + yB);
            }
        }
        if (!qFuzzyIsNull(result[n]))
        {
            result._states[n].tIntOutNew /= result[n];
            result._states[n].tIntOutEnd /= result[n];

            for (int i=0; i<result.aggregatedClasses.length(); i++)
            {
                result._states[n].intOutNewSC[i] /= result[n];
                result._states[n].intOutEndSC[i] /= result[n];
                result._states[n].y[i]           /= result[n];
            }
        }
        else
        {
            result._states[n].tIntOutNew = 0;
            result._states[n].tIntOutEnd = 0;
        }
        sum +=result[n];
    }

    for (n=0; n<=len; n++)
    {
        double sumNew = 0;
        double sumEnd = 0;

        for (int i=0; i<result.aggregatedClasses.length(); i++)
        {
            result._states[n].intInNewSC[i] = 0;
            result._states[n].intInEndSC[i] = 0;

            int t = result.aggregatedClasses[i].t;
            if (n >= static_cast<int>(t))
            {
                result._states[n].intInNewSC[i] = result._states[n-t].intOutNewSC[i];
                //result._states[n].tIntInNew    += result._states[n-t].intOutNewSC[i];
                sumNew += result._states[n-t].intOutNewSC[i];
            }

            if (n+t <=len)
            {
                result._states[n].intInEndSC[i] = result._states[n+t].intOutEndSC[i];
                //result._states[n].tIntInEnd    += result._states[n+t].intOutEndSC[i];
                sumEnd += result._states[n+t].intOutEndSC[i];
            }
        }
        result._states[n].tIntInNew = sumNew;
        result._states[n].tIntInEnd = sumEnd;

    }

    if (doNormalization)
        for (n=0; n<=len; n++)
            result[n] /= sum;

    return result;
}

TrClVector TrClVector::convFAGanyStream(const TrClVector &P_A, const TrClVector &P_B, int len)
{
    int n, la, lb;
    if (len == 0)
        len = qMax(P_A.V(), P_B.V());

    double sum = 0;
    TrClVector result(len);

    prepareResult(result, P_A, P_B, len);

    for (n=0; n<=len; n++)
    {
        result[n]=0;
        for (la=0; la<=n; la++)
        {
            lb = n-la;
            if (lb > P_B.V())
                continue;

            if (la > P_A.V())
                continue;

            const TrClVector *Pa = P_A.getTruncatedVector(result.V() - lb);
            const TrClVector *Pb = P_B.getTruncatedVector(result.V() - la);

            result[n] += Pa->_states[la].p * Pb->_states[lb].p;

            result._states[n].tIntOutNew += (Pa->_states[la].p * Pb->_states[lb].p * (Pa->_states[la].tIntOutNew + Pb->_states[lb].tIntOutNew));
            result._states[n].tIntOutEnd += (Pa->_states[la].p * Pb->_states[lb].p * (Pa->_states[la].tIntOutEnd + Pb->_states[lb].tIntOutEnd));

            for (int i=0; i<result.aggregatedClasses.length(); i++)
            {
                int clIdx = result.aggregatedClasses[i].id;

                int clIdxA = P_A.internalClassId(clIdx);
                int clIdxB = P_B.internalClassId(clIdx);

                double intOutNewA = (clIdxA == -1) ? 0 : Pa->_states[la].intOutNewSC[clIdxA];
                double intOutNewB = (clIdxB == -1) ? 0 : Pb->_states[lb].intOutNewSC[clIdxB];
                double intOutEndA = (clIdxA == -1) ? 0 : Pa->_states[la].intOutEndSC[clIdxA];
                double intOutEndB = (clIdxB == -1) ? 0 : Pb->_states[lb].intOutEndSC[clIdxB];

                double yA = (clIdxA == -1) ? 0 : Pa->_states[la].y[clIdxA];
                double yB = (clIdxB == -1) ? 0 : Pb->_states[lb].y[clIdxB];

                result._states[n].intOutNewSC[i] += Pa->_states[la].p * Pb->_states[lb].p * (intOutNewA + intOutNewB);
                result._states[n].intOutEndSC[i] += Pa->_states[la].p * Pb->_states[lb].p * (intOutEndA + intOutEndB);
                result._states[n].y[i] += Pa->_states[la].p * Pb->_states[lb].p * (yA + yB);
            }
        }
        if (!qFuzzyIsNull(result[n]))
        {
            result._states[n].tIntOutNew /= result[n];
            result._states[n].tIntOutEnd /= result[n];

            for (int i=0; i<result.aggregatedClasses.length(); i++)
            {
                result._states[n].intOutNewSC[i] /= result[n];
                result._states[n].intOutEndSC[i] /= result[n];
                result._states[n].y[i]           /= result[n];
            }
        }
        else
        {
            result._states[n].tIntOutNew = 0;
            result._states[n].tIntOutEnd = 0;
        }
        sum +=result[n];
    }

    for (n=0; n<=len; n++)
    {
        double sumNew = 0;
        double sumEnd = 0;

        for (int i=0; i<result.aggregatedClasses.length(); i++)
        {
            result._states[n].intInNewSC[i] = 0;
            result._states[n].intInEndSC[i] = 0;

            int t = result.aggregatedClasses[i].t;
            if (n >= static_cast<int>(t))
            {
                result._states[n].intInNewSC[i] = result._states[n-t].intOutNewSC[i];
                //result._states[n].tIntInNew    += result._states[n-t].intOutNewSC[i];
                sumNew += result._states[n-t].intOutNewSC[i];
            }

            if (n+t <=len)
            {
                result._states[n].intInEndSC[i] = result._states[n+t].intOutEndSC[i];
                sumEnd += result._states[n+t].intOutEndSC[i];

                result._states[n].intInEndSC[i] = result._states[n+t].intOutEndSC[i];
                sumEnd += result._states[n+t].intOutEndSC[i];
            }
        }
        result._states[n].tIntInNew = sumNew;
        result._states[n].tIntInEnd = sumEnd;

        result._states[n].tIntInNew = sumNew;
        result._states[n].tIntInEnd = sumEnd;

    }

    if (result.V() > 0)
    {
        result.previous = new TrClVector(result.V()-1, result.aggregatedClasses);
        *(result.previous) = convFAGanyStream(*(P_A.previous), *(P_B.previous));
    }
    return result;
}

void TrClVector::addClass(int id, int t)
{
    struct pairIdT tmp = {id, t};
    if (aggregatedClasses.contains(tmp))
        qFatal("This class is aggregated already");
    aggregatedClasses.append(tmp);

    for (int n=0; n< _states.length(); n++)
    {
        _states[n].addClassOnTheEnd();
    }
}

int TrClVector::internalClassId(int classId) const
{
    for(int intIdx = 0; intIdx<aggregatedClasses.length(); intIdx++)
        if (aggregatedClasses[intIdx].id == classId)
            return intIdx;
    return -1;
}

State::State(): p(0), tIntInNew(0), tIntInEnd(0), tIntOutNew(0), tIntOutEnd(0), intInNewSC(nullptr), intInEndSC(nullptr),
    intOutNewSC(nullptr), intOutEndSC(nullptr), y(nullptr), m(0)
{

}

State::State(const State &rho): p(rho.p), tIntInNew(rho.tIntInNew), tIntInEnd(rho.tIntInEnd), tIntOutNew(rho.tIntOutNew), tIntOutEnd(rho.tIntOutEnd), m(rho.m)
{
    if (rho.intInNewSC != nullptr)
    {
        intInNewSC = new double[rho.m];
        memcpy(intInNewSC, rho.intInNewSC, static_cast<size_t>(m)*sizeof(double));
    }
    if (rho.intInEndSC != nullptr)
    {
        intInEndSC = new double[rho.m];
        memcpy(intInEndSC, rho.intInEndSC, static_cast<size_t>(m)*sizeof(double));
    }

    if (rho.intOutNewSC != nullptr)
    {
        intOutNewSC = new double[rho.m];
        memcpy(intOutNewSC, rho.intOutNewSC, static_cast<size_t>(m)*sizeof(double));
    }
    if (rho.intOutEndSC != nullptr)
    {
        intOutEndSC = new double[rho.m];
        memcpy(intOutEndSC, rho.intOutEndSC, static_cast<size_t>(m)*sizeof(double));
    }

    if (rho.y != nullptr)
    {
        y = new double[rho.m];
        memcpy(y, rho.y, static_cast<size_t>(m)*sizeof(double));
    }
}

State::~State()
{
    if (intInNewSC != nullptr)
    {
        delete []intInNewSC;
        intInNewSC = nullptr;
    }
    if (intInEndSC != nullptr)
    {
        delete []intInEndSC;
        intInEndSC = nullptr;
    }
    if (intOutNewSC != nullptr)
    {
        delete []intOutNewSC;
        intOutNewSC = nullptr;
    }
    if (intOutEndSC != nullptr)
    {
        delete []intOutEndSC;
        intOutEndSC = nullptr;
    }
    if (y != nullptr)
    {
        delete []y;
        y = nullptr;
    }
}

State &State::operator=(const State &rho)
{
    this->p = rho.p;
    this->tIntOutNew = rho.tIntOutNew;
    this->tIntOutEnd = rho.tIntOutEnd;
    this->tIntInNew  = rho.tIntInNew;
    this->tIntInEnd  = rho.tIntInEnd;
    this->m = rho.m;

    if (rho.intInNewSC != nullptr)
    {
        intInNewSC = new double[rho.m];
        memcpy(intInNewSC, rho.intInNewSC, static_cast<size_t>(m)*sizeof(double));
    }
    else
        intInNewSC = nullptr;

    if (rho.intInEndSC != nullptr)
    {
        intInEndSC = new double[rho.m];
        memcpy(intInEndSC, rho.intInEndSC, static_cast<size_t>(m)*sizeof(double));
    }
    else
        intInEndSC = nullptr;


    if (rho.intOutNewSC != nullptr)
    {
        intOutNewSC = new double[rho.m];
        memcpy(intOutNewSC, rho.intOutNewSC, static_cast<size_t>(m)*sizeof(double));
    }
    else
        intOutNewSC = nullptr;

    if (rho.intOutEndSC != nullptr)
    {
        intOutEndSC = new double[rho.m];
        memcpy(intOutEndSC, rho.intOutEndSC, static_cast<size_t>(m)*sizeof(double));
    }
    else
        intOutEndSC = nullptr;

    if (rho.y != nullptr)
    {
        y = new double[rho.m];
        memcpy(y, rho.y, static_cast<size_t>(m)*sizeof(double));
    }
    else
        y = nullptr;

    return *this;
}

void State::addClassOnTheEnd()
{
    if (m == 0)
    {
        this->intInNewSC = new double[1];  this->intInNewSC[0] = 0;
        this->intInEndSC = new double[1];  this->intInEndSC[0] = 0;
        this->intOutNewSC = new double[1];  this->intOutNewSC[0] = 0;
        this->intOutEndSC = new double[1];  this->intOutEndSC[0] = 0;
        this->y           = new double[1];  this->y[0]           = 0;
    }
    else
    {
        double *tmp;

        tmp = new double[m+1];
        memcpy(tmp, intInNewSC, static_cast<size_t>(m)*sizeof(double));
        tmp[m] = 0;
        delete []intInNewSC;
        intInNewSC = tmp;

        tmp = new double[m+1];
        memcpy(tmp, intInEndSC, static_cast<size_t>(m)*sizeof(double));
        tmp[m] = 0;
        delete []intInEndSC;
        intInEndSC = tmp;

        tmp = new double[m+1];
        memcpy(tmp, intOutNewSC, static_cast<size_t>(m)*sizeof(double));
        tmp[m] = 0;
        delete []intOutNewSC;
        intOutNewSC = tmp;

        tmp = new double[m+1];
        memcpy(tmp, intOutEndSC, static_cast<size_t>(m)*sizeof(double));
        tmp[m] = 0;
        delete []intOutEndSC;
        intOutEndSC = tmp;

        tmp = new double[m+1];
        memcpy(tmp, y, static_cast<size_t>(m)*sizeof(double));
        tmp[m] = 0;
        delete []y;
        y = tmp;
    }
    m++;
}
/*
trClVector trClVector::convQUEUE(const trClVector &Pa, const trClVector &Pb, int V, bool doNormalization, int len)
{
    int n, la, lb;
    if (len == 0)
        len = qMax(Pa._lastIdx, Pb._lastIdx);

    double sum = 0;
    trClVector result(len);
    for (n=0; n<=len; n++)
    {
        result[n] = 0;

        for (la=0; la<=n; la++)
        {
            if (la > Pa._lastIdx)
                continue;

            lb = n-la;
            if (lb > Pb._lastIdx)
                continue;

            double Pa_la = Pa._states[la].p;
            double Pb_lb = Pb._states[lb].p;
            double Xa = QEUE2FAG(Pa_la, la, V);
            double Xb = QEUE2FAG(Pb_lb, lb, V);

            result[n] += (Xa * Xb);
        }
        double tmp = FAG2QEUE(result[n], n, V);
        result[n] = tmp;
        sum += result[n];
    }

    if (doNormalization)
        for (n=0; n<=len; n++)
            result[n] /= sum;

    return result;
}

double trClVector::FAG2QEUE(double value, int n, int V)
{
    if (n > V)
        value = (value * n / V);
    return value;
}

double trClVector::QEUE2FAG(double value, int n, int V)
{
    if (n > V)
        value = (value * V / n);
    return value;
}
*/
