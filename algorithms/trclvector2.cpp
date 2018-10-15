#include "trclvector2.h"
#include <QDebug>
#include <stdexcept>
#include <math.h>

TrClVector::TrClVector(): previous(nullptr)
{
    _states2.clear();
    _states2.fill(state(), 1);
    _states2[0].p = 1;

    aggregatedClasses.clear();
}

TrClVector::TrClVector(const TrClVector &rho): aggregatedClasses(rho.aggregatedClasses)
{
    _states2 = rho._states2;//.fill(state(), rho._lastIdx+1);

    if (rho.previous != nullptr)
        previous = new TrClVector(*(rho.previous));
    else
        previous = nullptr;
}

TrClVector::TrClVector(int LastIdx): previous(nullptr)
{
    _states2.fill(state(), LastIdx + 1);
    _states2[0].p = 1;
}

TrClVector::TrClVector(int V, const QVector<pairIdT> &aggregatedClasses): previous(nullptr), aggregatedClasses(aggregatedClasses)
{
    _states2.clear();
    _states2.fill(state(), V+1);
    _states2[0].p = 1;
}

TrClVector::~TrClVector()
{
    _states2.clear();

    if (previous != nullptr)
        delete previous;
}

double &TrClVector::operator[](int n)
{
    if (n > V())
        throw std::out_of_range("Vector's index is out of range");
    return _states2[n].p;
}

void TrClVector::setState(int n, const state &refState)
{
    _states2[n] = refState;
}

state &TrClVector::getState(int n)
{
    if (n > V())
        throw std::out_of_range("Vector's index is out of range");
    return _states2[n];
}

void TrClVector::setIntInNew(int n, int i, double value)
{
    if (n > V())
        throw std::out_of_range("Vector's index is out of range");

    int classIdx = internalClassId(i);
    if (classIdx == -1)
        throw std::out_of_range("Can't find proper class");

    _states2[n].intInNewSC[classIdx] = value;
}

void TrClVector::setIntInEnd(int n, int i, double value)
{
    if (n > V())
        throw std::out_of_range("Vector's index is out of range");

    int classIdx = internalClassId(i);
    if (classIdx == -1)
        throw std::out_of_range("Can't find proper class");

    _states2[n].intInEndSC[classIdx] = value;
}

void TrClVector::setIntOutNew(int n, int i, double value)
{
    if (n > V())
        throw std::out_of_range("Vector's index is out of range");

    int classIdx = internalClassId(i);
    if (classIdx == -1)
        throw std::out_of_range("Can't find proper class");

    _states2[n].intOutNewSC[classIdx] = value;
}

void TrClVector::setIntOutEnd(int n, int i, double value)
{
    if (n > V())
        throw std::out_of_range("Vector's index is out of range");

    int classIdx = internalClassId(i);
    if (classIdx == -1)
        throw std::out_of_range("Can't find proper class");

    _states2[n].intOutEndSC[classIdx] = value;
}

void TrClVector::setY(int n, int i, double value)
{
    if (n > V())
        throw std::out_of_range("Vector's index is out of range");

    int classIdx = internalClassId(i);
    if (classIdx == -1)
        throw std::out_of_range("Can't find proper class");

    _states2[n].y[classIdx] = value;
}

double TrClVector::getIntInNew(int n, int i) const
{
    if (n > V())
        throw std::out_of_range("Vector's index is out of range");

    int classIdx = internalClassId(i);
    if (classIdx == -1)
        throw std::out_of_range("Can't find proper class");

    double result = _states2[n].intInNewSC[classIdx];
    return result;
}

double TrClVector::getIntInEnd(int n, int i) const
{
    if (n > V())
        throw std::out_of_range("Vector's index is out of range");

    int classIdx = internalClassId(i);
    if (classIdx == -1)
        throw std::out_of_range("Can't find proper class");

    return _states2[n].intInEndSC[classIdx];
}

double TrClVector::getIntOutNew(int n, int i) const
{
    if (n > V())
        throw std::out_of_range("Vector's index is out of range");

    int classIdx = internalClassId(i);
    if (classIdx == -1)
        throw std::out_of_range("Can't find proper class");

    double result = _states2[n].intOutNewSC[classIdx];
    return result;
}

double TrClVector::getIntOutEnd(int n, int i) const
{
    if (n > V())
        throw std::out_of_range("Vector's index is out of range");

    int classIdx = internalClassId(i);
    if (classIdx == -1)
        throw std::out_of_range("Can't find proper class");

    return _states2[n].intOutEndSC[classIdx];
}

double TrClVector::getY(int n, int i) const
{
    if (n > V())
        throw std::out_of_range("Vector's index is out of range");

    int classIdx = internalClassId(i);
    if (classIdx == -1)
        throw std::out_of_range("Can't find proper class");

    return _states2[n].y[classIdx];
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

    _states2 = rho._states2;

    aggregatedClasses = rho.aggregatedClasses;
    previous = (rho.previous == nullptr) ? nullptr : new TrClVector(*(rho.previous));

    return *this;
}

void TrClVector::normalize(double sumOfAllTheStates)
{
    if (sumOfAllTheStates == 0)
        qFatal("sum of all the states can't be 0");

    double sum = 0;
    for (int n=0; n<=V(); n++)
    {
#ifdef QT_DEBUG
        if (std::isnan(_states2[n].p))
            qFatal("p_n = NAN");
#endif
        sum += _states2[n].p;
    }

    for (int n=0; n<=V(); n++)
    {
        _states2[n].p *= sumOfAllTheStates;
        _states2[n].p /= sum;

#ifdef QT_DEBUG
        if (std::isnan(_states2[n].p))
            qFatal("p_n = NAN");
#endif
    }
    if (previous != nullptr)
        previous->normalize(sumOfAllTheStates - _states2[V()].p);
}

void TrClVector::generateNormalizedPoissonPrevDistrib()
{
    if (previous != nullptr)
        qFatal("Previous distribution exists");

    if (V() == 0)
        return;

    previous = new TrClVector(this->V()-1, this->aggregatedClasses);

    for (int n=0; n<V(); n++)
        previous->_states2[n] = _states2[n];
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
        previous->_states2[n] = _states2[n];
    previous->normalize(1.0-_states2[V()].p);
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
        result._states2[n].m = result.aggregatedClasses.length();
        result._states2[n].intInNewSC = new double[result._states2[n].m];
        result._states2[n].intInEndSC = new double[result._states2[n].m];
        result._states2[n].intOutNewSC = new double[result._states2[n].m];
        result._states2[n].intOutEndSC = new double[result._states2[n].m];
        result._states2[n].y = new double[result._states2[n].m];

        bzero(result._states2[n].intInNewSC, result._states2[n].m * sizeof(double));
        bzero(result._states2[n].intInEndSC, result._states2[n].m * sizeof(double));
        bzero(result._states2[n].intOutNewSC, result._states2[n].m * sizeof(double));
        bzero(result._states2[n].intOutEndSC, result._states2[n].m * sizeof(double));
        bzero(result._states2[n].y, result._states2[n].m * sizeof(double));
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

            result[n] += Pa._states2[la].p * Pb._states2[lb].p;

            result._states2[n].tIntOutNew += (Pa._states2[la].p * Pb._states2[lb].p * (Pa._states2[la].tIntOutNew + Pb._states2[lb].tIntOutNew));
            result._states2[n].tIntOutEnd += (Pa._states2[la].p * Pb._states2[lb].p * (Pa._states2[la].tIntOutEnd + Pb._states2[lb].tIntOutEnd));

            for (int i=0; i<result.aggregatedClasses.length(); i++)
            {
                int clIdx = result.aggregatedClasses[i].id;

                int clIdxA = Pa.internalClassId(clIdx);
                int clIdxB = Pb.internalClassId(clIdx);

                double intOutNewA = (clIdxA == -1) ? 0 : Pa._states2[la].intOutNewSC[clIdxA];
                double intOutNewB = (clIdxB == -1) ? 0 : Pb._states2[lb].intOutNewSC[clIdxB];
                double intOutEndA = (clIdxA == -1) ? 0 : Pa._states2[la].intOutEndSC[clIdxA];
                double intOutEndB = (clIdxB == -1) ? 0 : Pb._states2[lb].intOutEndSC[clIdxB];

                double yA = (clIdxA == -1) ? 0 : Pa._states2[la].y[clIdxA];
                double yB = (clIdxB == -1) ? 0 : Pb._states2[lb].y[clIdxB];

                result._states2[n].intOutNewSC[i] += Pa._states2[la].p * Pb._states2[lb].p * (intOutNewA + intOutNewB);
                result._states2[n].intOutEndSC[i] += Pa._states2[la].p * Pb._states2[lb].p * (intOutEndA + intOutEndB);
                result._states2[n].y[i] += Pa._states2[la].p * Pb._states2[lb].p * (yA + yB);
            }
        }
        if (result[n] != 0)
        {
            result._states2[n].tIntOutNew /= result[n];
            result._states2[n].tIntOutEnd /= result[n];

            for (int i=0; i<result.aggregatedClasses.length(); i++)
            {
                result._states2[n].intOutNewSC[i] /= result[n];
                result._states2[n].intOutEndSC[i] /= result[n];
                result._states2[n].y[i]           /= result[n];
            }
        }
        else
        {
            result._states2[n].tIntOutNew = 0;
            result._states2[n].tIntOutEnd = 0;
        }
        sum +=result[n];
    }

    for (n=0; n<=len; n++)
    {
        double sumNew = 0;
        double sumEnd = 0;

        for (int i=0; i<result.aggregatedClasses.length(); i++)
        {
            result._states2[n].intInNewSC[i] = 0;
            result._states2[n].intInEndSC[i] = 0;

            int t = result.aggregatedClasses[i].t;
            if (n >= (int)
                    t)
            {
                result._states2[n].intInNewSC[i] = result._states2[n-t].intOutNewSC[i];
                //result._states[n].tIntInNew    += result._states[n-t].intOutNewSC[i];
                sumNew += result._states2[n-t].intOutNewSC[i];
            }

            if (n+t <=len)
            {
                result._states2[n].intInEndSC[i] = result._states2[n+t].intOutEndSC[i];
                //result._states[n].tIntInEnd    += result._states[n+t].intOutEndSC[i];
                sumEnd += result._states2[n+t].intOutEndSC[i];
            }
        }
        result._states2[n].tIntInNew = sumNew;
        result._states2[n].tIntInEnd = sumEnd;

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

            result[n] += Pa->_states2[la].p * Pb->_states2[lb].p;

            result._states2[n].tIntOutNew += (Pa->_states2[la].p * Pb->_states2[lb].p * (Pa->_states2[la].tIntOutNew + Pb->_states2[lb].tIntOutNew));
            result._states2[n].tIntOutEnd += (Pa->_states2[la].p * Pb->_states2[lb].p * (Pa->_states2[la].tIntOutEnd + Pb->_states2[lb].tIntOutEnd));

            for (int i=0; i<result.aggregatedClasses.length(); i++)
            {
                int clIdx = result.aggregatedClasses[i].id;

                int clIdxA = P_A.internalClassId(clIdx);
                int clIdxB = P_B.internalClassId(clIdx);

                double intOutNewA = (clIdxA == -1) ? 0 : Pa->_states2[la].intOutNewSC[clIdxA];
                double intOutNewB = (clIdxB == -1) ? 0 : Pb->_states2[lb].intOutNewSC[clIdxB];
                double intOutEndA = (clIdxA == -1) ? 0 : Pa->_states2[la].intOutEndSC[clIdxA];
                double intOutEndB = (clIdxB == -1) ? 0 : Pb->_states2[lb].intOutEndSC[clIdxB];

                double yA = (clIdxA == -1) ? 0 : Pa->_states2[la].y[clIdxA];
                double yB = (clIdxB == -1) ? 0 : Pb->_states2[lb].y[clIdxB];

                result._states2[n].intOutNewSC[i] += Pa->_states2[la].p * Pb->_states2[lb].p * (intOutNewA + intOutNewB);
                result._states2[n].intOutEndSC[i] += Pa->_states2[la].p * Pb->_states2[lb].p * (intOutEndA + intOutEndB);
                result._states2[n].y[i] += Pa->_states2[la].p * Pb->_states2[lb].p * (yA + yB);
            }
        }
        if (result[n] != 0)
        {
            result._states2[n].tIntOutNew /= result[n];
            result._states2[n].tIntOutEnd /= result[n];

            for (int i=0; i<result.aggregatedClasses.length(); i++)
            {
                result._states2[n].intOutNewSC[i] /= result[n];
                result._states2[n].intOutEndSC[i] /= result[n];
                result._states2[n].y[i]           /= result[n];
            }
        }
        else
        {
            result._states2[n].tIntOutNew = 0;
            result._states2[n].tIntOutEnd = 0;
        }
        sum +=result[n];
    }

    for (n=0; n<=len; n++)
    {
        double sumNew = 0;
        double sumEnd = 0;

        for (int i=0; i<result.aggregatedClasses.length(); i++)
        {
            result._states2[n].intInNewSC[i] = 0;
            result._states2[n].intInEndSC[i] = 0;

            int t = result.aggregatedClasses[i].t;
            if (n >= (int)
                    t)
            {
                result._states2[n].intInNewSC[i] = result._states2[n-t].intOutNewSC[i];
                //result._states[n].tIntInNew    += result._states[n-t].intOutNewSC[i];
                sumNew += result._states2[n-t].intOutNewSC[i];
            }

            if (n+t <=len)
            {
                result._states2[n].intInEndSC[i] = result._states2[n+t].intOutEndSC[i];
                sumEnd += result._states2[n+t].intOutEndSC[i];

                result._states2[n].intInEndSC[i] = result._states2[n+t].intOutEndSC[i];
                sumEnd += result._states2[n+t].intOutEndSC[i];
            }
        }
        result._states2[n].tIntInNew = sumNew;
        result._states2[n].tIntInEnd = sumEnd;

        result._states2[n].tIntInNew = sumNew;
        result._states2[n].tIntInEnd = sumEnd;

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

    for (int n=0; n< _states2.length(); n++)
    {
        _states2[n].addClassOnTheEnd();
    }
}

int TrClVector::internalClassId(int classId) const
{
    for(int intIdx = 0; intIdx<aggregatedClasses.length(); intIdx++)
        if (aggregatedClasses[intIdx].id == classId)
            return intIdx;
    return -1;
}

state::state(): p(0), tIntInNew(0), tIntInEnd(0), tIntOutNew(0), tIntOutEnd(0), intInNewSC(nullptr), intInEndSC(nullptr),
    intOutNewSC(nullptr), intOutEndSC(nullptr), y(nullptr), m(0)
{

}

state::state(const state &rho): p(rho.p), tIntInNew(rho.tIntInNew), tIntInEnd(rho.tIntInEnd), tIntOutNew(rho.tIntOutNew), tIntOutEnd(rho.tIntOutEnd), m(rho.m)
{
    if (rho.intInNewSC == nullptr)
    {
        intInNewSC = new double[rho.m];
        memcpy(intInNewSC, rho.intInNewSC, m*sizeof(double));
    }
    if (rho.intInEndSC == nullptr)
    {
        intInEndSC = new double[rho.m];
        memcpy(intInEndSC, rho.intInEndSC, m*sizeof(double));
    }

    if (rho.intOutNewSC == nullptr)
    {
        intOutNewSC = new double[rho.m];
        memcpy(intOutNewSC, rho.intOutNewSC, m*sizeof(double));
    }
    if (rho.intOutEndSC == nullptr)
    {
        intOutEndSC = new double[rho.m];
        memcpy(intOutEndSC, rho.intOutEndSC, m*sizeof(double));
    }

    if (rho.y == nullptr)
    {
        y = new double[rho.m];
        memcpy(y, rho.y, m*sizeof(double));
    }
}

state::~state()
{
    if (intOutNewSC != nullptr)
        delete []intInNewSC;
    if (intOutEndSC != nullptr)
        delete []intInEndSC;
    if (intOutNewSC != nullptr)
        delete []intOutNewSC;
    if (intOutEndSC != nullptr)
        delete []intOutEndSC;
    if (y != nullptr)
        delete []y;

}

state &state::operator=(const state &rho)
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
        memcpy(intInNewSC, rho.intInNewSC, m*sizeof(double));
    }
    else
        intInNewSC = nullptr;

    if (rho.intInEndSC != nullptr)
    {
        intInEndSC = new double[rho.m];
        memcpy(intInEndSC, rho.intInEndSC, m*sizeof(double));
    }
    else
        intInEndSC = nullptr;


    if (rho.intOutNewSC != nullptr)
    {
        intOutNewSC = new double[rho.m];
        memcpy(intOutNewSC, rho.intOutNewSC, m*sizeof(double));
    }
    else
        intOutNewSC = nullptr;

    if (rho.intOutEndSC != nullptr)
    {
        intOutEndSC = new double[rho.m];
        memcpy(intOutEndSC, rho.intOutEndSC, m*sizeof(double));
    }
    else
        intOutEndSC = nullptr;

    if (rho.y != nullptr)
    {
        y = new double[rho.m];
        memcpy(y, rho.y, m*sizeof(double));
    }
    else
        y = nullptr;

    return *this;
}

void state::addClassOnTheEnd()
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
        memcpy(tmp, intInNewSC, m*sizeof(double));
        tmp[m] = 0;
        delete []intInNewSC;
        intInNewSC = tmp;

        tmp = new double[m+1];
        memcpy(tmp, intInEndSC, m*sizeof(double));
        tmp[m] = 0;
        delete []intInEndSC;
        intInEndSC = tmp;

        tmp = new double[m+1];
        memcpy(tmp, intOutNewSC, m*sizeof(double));
        tmp[m] = 0;
        delete []intOutNewSC;
        intOutNewSC = tmp;

        tmp = new double[m+1];
        memcpy(tmp, intOutEndSC, m*sizeof(double));
        tmp[m] = 0;
        delete []intOutEndSC;
        intOutEndSC = tmp;

        tmp = new double[m+1];
        memcpy(tmp, y, m*sizeof(double));
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
