#if 0
#include "results/resultsOld.h"
#include "algorithms/investigator.h"

QString ResultsTmp::_NA;
QString ResultsTmp::_logScale;
QString ResultsTmp::_linearScale;
QMap<resultsType, ResultsTmp::QoSdetails> ResultsTmp::_resType2details;
ResultsTmp::_init ResultsTmp::_initializer;

int ResultsTmp::get_m() const
{
    if (lastSystem == nullptr)
        return 0;
    return lastSystem->m();
}

const ModelTrClass *ResultsTmp::getClass(int idx) const
{
    if (lastSystem == nullptr)
        return nullptr;

    return lastSystem->getClass(idx);
}

int ResultsTmp::get_V() const
{
    if (lastSystem == nullptr)
        return 0;

    return lastSystem->V_s();
}

int ResultsTmp::get_C() const
{
    if (lastSystem == nullptr)
        return 0;

    return lastSystem->V_b();
}

double ResultsTmp::get_sumPropAt() const
{
    double sumPropAt = 0;
    for (int i=0; i<this->get_m(); i++)
    {
        const ModelTrClass *tmpTrClass = getClass(i);
        sumPropAt += tmpTrClass->propAt();
    }
    return sumPropAt;
}

ResultsTmp::ResultsTmp()
{
    lastSystem = nullptr;

    QoSPar.append(resultsType::E);
    QoSPar.append(resultsType::B);
    QoSPar.append(resultsType::Qlen);
}


QString &ResultsTmp::resType2string(resultsType type)
{
    if (_resType2details.contains(type))
    {
        ResultsTmp::QoSdetails &tmp = _resType2details[type];
        QString &result = tmp.description;
        return result;
    }
    return _NA;
}

QString &ResultsTmp::resType2shortString(resultsType type)
{
    if (_resType2details.contains(type))
    {
        ResultsTmp::QoSdetails &tmp = _resType2details[type];
        QString &result = tmp.shortDescr;
        return result;
    }
    return _NA;
}

QString &ResultsTmp::resType2yScale(resultsType type)
{
    if (_resType2details.contains(type))
    {
        if (_resType2details[type].logScaleY)
            return _logScale;
        else
            return _linearScale;
    }
    return _NA;
}

resultFormat ResultsTmp::resType2resFormat(resultsType type)
{
    if (_resType2details.contains(type))
        return _resType2details[type].result;

    return resultFormat::typeDouble;
}

bool ResultsTmp::resTypeHasLogScale(resultsType type)
{
    if (_resType2details.contains(type))
        return _resType2details[type].logScaleY;
    return false;
}

bool ResultsTmp::isTrClassDependent(resultsType type)
{
    if (_resType2details.contains(type))
        return _resType2details[type].dependsOnClass;
    return false;
}

double ResultsTmp::min_a()
{
    if (a.length() < 1)
        return -1;
    double result = a[0];


    foreach (double tmp, a)
        result = qMin(result, tmp);
    return result;
}

double ResultsTmp::max_a()
{
    if (a.length() < 1)
        return -1;
    double result = a[0];


    foreach (double tmp, a)
        result = qMax(result, tmp);
    return result;
}




AlgorithmResults *ResultsTmp::getAlgResult(const Investigator *alg) const
{
    if (_algResultsDepracated.contains(alg))
        return _algResultsDepracated.value(alg);
    return NULL;
}

QVector<double> *ResultsTmp::get_a()
{
    return &this->a;
}

void ResultsTmp::addAlgorithmDepracated(Investigator *alg, AlgorithmResults *algResults)
{
    lastSystem = algResults->system;
    _algResultsDepracated.insert(alg, algResults);
}

void AlgorithmResults::set(
          resultsType type
        , const ModelTrClass *trClass
        , double a
        , double value
        , double valueCi
        )
{
    a = round(a * 10000)/10000;
    QVector<double> *tmpVector   = nullptr;
    QVector<double> *tmpVectorCI = nullptr;

    bool contains_a = this->a->contains(a);
    if (contains_a==false)
        this->a->append(a);

    int idx = this->a->indexOf(a);

    switch (type)
    {
    case resultsType::E:
        if (this->E.contains(trClass) == false)
            E.insert(trClass, new QVector<double>);
        if ((valueCi != 0) && (conf_E.contains(trClass) == false))
            conf_E.insert(trClass, new QVector<double>);

        tmpVector= E[trClass];
        if (valueCi != 0)
            tmpVectorCI = conf_E[trClass];
        break;

    case resultsType::Y:
        if (this->Y.contains(trClass) == false)
            Y.insert(trClass, new QVector<double>);
        if ((valueCi != 0) && (conf_Y.contains(trClass) == false))
            conf_Y.insert(trClass, new QVector<double>);

        tmpVector= Y[trClass];
        if (valueCi != 0)
            tmpVectorCI = conf_Y[trClass];

        break;

    case resultsType::B:
        if (this->B.contains(trClass) == false)
            B.insert(trClass, new QVector<double>);
        if ((valueCi != 0) && (conf_B.contains(trClass) == false))
            conf_B.insert(trClass, new QVector<double>);

        tmpVector= B[trClass];
        if (valueCi != 0)
            tmpVectorCI = conf_B[trClass];
        break;

    case resultsType::Qlen:
        tmpVector= &qLen;
        if (valueCi != 0)
            tmpVectorCI = &conf_qLen;
        break;

    case resultsType::yQ:
        if (yQ.contains(trClass) == false)
            yQ.insert(trClass, new QVector<double>);
        if ((valueCi != 0) && (conf_yQ.contains(trClass) == false))
            conf_yQ.insert(trClass, new QVector<double>);

        tmpVector= yQ[trClass];
        if (valueCi != 0)
            tmpVectorCI= conf_yQ[trClass];
        break;

    case resultsType::y:
        if (y.contains(trClass) == false)
            y.insert(trClass, new QVector<double>);
        if ((valueCi != 0) && (conf_y.contains(trClass) == false))
            conf_y.insert(trClass, new QVector<double>);

        tmpVector= y[trClass];
        if (valueCi != 0)
            tmpVectorCI= conf_y[trClass];
        break;

    case resultsType::ytQ:
        if (ytQ.contains(trClass) == false)
            ytQ.insert(trClass, new QVector<double>);
        if ((valueCi != 0) && (conf_ytQ.contains(trClass) == false))
            conf_ytQ.insert(trClass, new QVector<double>);

        tmpVector= ytQ[trClass];
        if (valueCi != 0)
            tmpVectorCI= conf_ytQ[trClass];
        break;

    case resultsType::tWait:
        if (t.contains(trClass) == false)
            t.insert(trClass, new QVector<double>);
         if ((valueCi != 0) && (conf_t.contains(trClass) == false))
             conf_t.insert(trClass, new QVector<double>);

        tmpVector= t[trClass];
        if (valueCi != 0)
            tmpVectorCI= conf_t[trClass];
        break;

    case resultsType::tService:
        if (tS.contains(trClass) == false)
            tS.insert(trClass, new QVector<double>);

        if ((valueCi != 0) && (conf_tS.contains(trClass) == false))
                conf_tS.insert(trClass, new QVector<double>);

        tmpVector= tS[trClass];
        if (valueCi != 0)
            tmpVectorCI= conf_tS[trClass];
        break;

    case resultsType::tServer:
        if (tServer.contains(trClass) == false)
            tServer.insert(trClass, new QVector<double>);

        if ((valueCi != 0) && (conf_tServer.contains(trClass) == false))
            conf_tServer.insert(trClass, new QVector<double>);

        tmpVector= tServer[trClass];
        if (valueCi != 0)
            tmpVectorCI= conf_tServer[trClass];
        break;

    case resultsType::tQeue:
        if (tQeue.contains(trClass) == false)
            tQeue.insert(trClass, new QVector<double>);

        if ((valueCi != 0) && (conf_tQeue.contains(trClass) == false))
            conf_tQeue.insert(trClass, new QVector<double>);

        tmpVector= tQeue[trClass];
        if (valueCi != 0)
            tmpVectorCI= conf_tQeue[trClass];
        break;

    case resultsType::tPlandedServer:
        if (tPlanedServer.contains(trClass) == false)
            tPlanedServer.insert(trClass, new QVector<double>);

        if ((valueCi != 0) && (conf_tPlanedServer.contains(trClass) == false))
            conf_tPlanedServer.insert(trClass, new QVector<double>);

        tmpVector= tPlanedServer[trClass];
        if (valueCi != 0)
            tmpVectorCI= conf_tPlanedServer[trClass];
        break;

    case resultsType::tServerMult:
        if (tServerMult.contains(trClass) == false)
            tServerMult.insert(trClass, new QVector<double>);

        if ((valueCi != 0) && (conf_tServerMult.contains(trClass) == false))
            conf_tServerMult.insert(trClass, new QVector<double>);

        tmpVector= tServerMult[trClass];
        if (valueCi != 0)
            tmpVectorCI= conf_tServerMult[trClass];
        break;

    default:
        qFatal("Not implemented");
        break;
    }

    if ((valueCi != 0) && (tmpVectorCI == nullptr))
        qFatal("No confidence intervall vector");

    int size =tmpVector->size();

    if (size < idx)
        tmpVector->fill(0, idx);

    size =tmpVector->size();
    if (size == idx)
        tmpVector->insert(idx, value);
    else
        tmpVector->replace(idx, value);

    if (tmpVectorCI != nullptr)
    {
        int size =tmpVectorCI->size();
        if (size < idx)
        {
            tmpVectorCI->fill(0, idx);
        }

        size =tmpVectorCI->size();
        if (size == idx)
        {
            tmpVectorCI->insert(idx, valueCi);
        }
        else
        {
            tmpVectorCI->replace(idx, valueCi);
        }
    }
}

bool AlgorithmResults::getVal(
          double&            value
        , double&            ci
        , resultsType        type
        , const ModelTrClass *trClass
        , const double&      a
        , CiTrust trust
        ) const
{
    const QVector<double> *tmpVector = NULL;
    const QVector<double> *tmpVectorCI = NULL;

    bool contains_a = this->a->contains(a);
    if (contains_a==false)
    {
        return false;
    }
    int idx = this->a->indexOf(a);

    switch (type)
    {
    case resultsType::E:
        if (this->E.contains(trClass) == false)
            return false;
        tmpVector= E[trClass];
        tmpVectorCI = conf_E[trClass];
        break;

    case resultsType::Y:
        if (this->Y.contains(trClass) == false)
            return false;
        tmpVector= Y[trClass];
        tmpVectorCI = conf_Y[trClass];
        break;

    case resultsType::B:
        if (this->B.contains(trClass) == false)
            return false;
        tmpVector= B[trClass];
        tmpVectorCI = conf_B[trClass];
        break;

    case resultsType::Qlen:
        tmpVector= &qLen;
        tmpVectorCI = &conf_qLen;
        break;

    case resultsType::yQ:
        if (yQ.contains(trClass) == false)
            return false;
        tmpVector= yQ[trClass];
        tmpVectorCI= conf_yQ[trClass];
        break;

    case resultsType::y:
        if (y.contains(trClass) == false)
            return false;
        tmpVector= y[trClass];
        tmpVectorCI= conf_y[trClass];
        break;

    case resultsType::ytQ:
        if (ytQ.contains(trClass) == false)
            return false;
        tmpVector= ytQ[trClass];
        tmpVectorCI= conf_ytQ[trClass];
        break;

    case resultsType::tWait:
        if (t.contains(trClass) == false)
            return false;
        tmpVector= t[trClass];
        tmpVectorCI= conf_t[trClass];
        break;

    case resultsType::tService:
        if (tS.contains(trClass) == false)
            return false;
        tmpVector= tS[trClass];
        tmpVectorCI= conf_tS[trClass];
        break;

    case resultsType::tServer:
        if (tServer.contains(trClass) == false)
            return false;
        tmpVector= tServer[trClass];
        tmpVectorCI= conf_tServer[trClass];
        break;

    case resultsType::tQeue:
        if (tQeue.contains(trClass) == false)
            return false;
        tmpVector= tQeue[trClass];
        tmpVectorCI= conf_tQeue[trClass];
        break;

    case resultsType::tPlandedServer:
        if (tPlanedServer.contains(trClass) == false)
            return false;
        tmpVector= tPlanedServer[trClass];
        tmpVectorCI= conf_tPlanedServer[trClass];
        break;

    case resultsType::tServerMult:
        if (tServerMult.contains(trClass) == false)
            return false;
        tmpVector= tServerMult[trClass];
        tmpVectorCI= conf_tServerMult[trClass];
        break;

    default:
        qFatal("Not implemented");
        break;
    }

    int size =tmpVector->size();
    if (size <= idx)
        return false;
    else
    {
        value = tmpVector->value(idx);
        ci = 0;
        if (tmpVectorCI != NULL)
        {
            ci = tmpVectorCI->value(idx, 0);
            switch (trust) {
            case CiTrust::trust95:
                ci*=WspStFish95[noOfSer];
                break;
            case CiTrust::trust99:
                ci*=WspStFish99[noOfSer];
                break;
            default:
                break;
            }
        }
    }
    return true;
}

AlgorithmResults::AlgorithmResults(const ModelSyst *system, QVector<double> *a, int noOfSer)
{
    this->noOfSer = noOfSer;
    this->system = system;
    this->a = a;

    resultsAS = new AU(system->V_s(), system->V_b());
}

void AlgorithmResults::calculateExAndWariance(double *samples, int noOfSamples, double *medValue, double *variance)
{
    *medValue = 0;
    for (int samplNo=0; samplNo<noOfSamples; samplNo++)
        (*medValue)+=samples[samplNo];

    (*medValue) /= noOfSamples;

    *variance = 0;

    for (int samplNo=0; samplNo<noOfSamples; samplNo++)
        (*variance) += ((*medValue - samples[samplNo])*((*medValue - samples[samplNo])));

    *variance = sqrt(*variance);

}

void AlgorithmResults::calculateExAndWarianceLongInt(long int *samples, int noOfSamples, long int *medValue, double *variance)
{
    *medValue = 0;
    for (int samplNo=0; samplNo<noOfSamples; samplNo++)
        (*medValue)+=samples[samplNo];

    (*medValue) /= noOfSamples;

    *variance = 0;

    for (int samplNo=0; samplNo<noOfSamples; samplNo++)
        (*variance) += ((*medValue - samples[samplNo])*((*medValue - samples[samplNo])));

    *variance = sqrt(*variance);
}

void AlgorithmResults::set_ltQ(const ModelTrClass *trClass, const double &a, double value, double ConfInt)
{
    set(resultsType::ytQ, trClass, a, value, ConfInt);
}

void AlgorithmResults::set_lQ(const ModelTrClass *trClass, const double &a, double value, double ConfInt)
{
    set(resultsType::yQ, trClass, a, value, ConfInt);
}

void AlgorithmResults::set_lSys(
          const ModelTrClass *trClass
        , const double&      a
        , double             value
        , double             ConfInt
        )
{
    set(resultsType::y, trClass, a, value, ConfInt);
}

void AlgorithmResults::set_t(const ModelTrClass *trClass, const double &a, double value, double ConfInt)
{
    set(resultsType::tWait, trClass, a, value, ConfInt);
}

/**
 * @brief set the total service time
 * @param trClass
 * @param a offered traffic per AS
 * @param value of the avarage total service time
 * @param ConfInt has conficence intervall (yes for simulation experiment)
 */
void AlgorithmResults::set_tService(
          const ModelTrClass *trClass
        , const double&      a
        , double             value
        , double             ConfInt
        )
{
    set(resultsType::tService, trClass, a, value, ConfInt);
}

void AlgorithmResults::set_tServer(const ModelTrClass *trClass, const double &a, double value, double ConfInt)
{
    set(resultsType::tServer, trClass, a, value, ConfInt);
}

void AlgorithmResults::set_tQeue(
          const ModelTrClass *trClass
        , const double&      a
        , double             value
        , double             ConfInt
        )
{
    set(resultsType::tQeue, trClass, a, value, ConfInt);
}

void AlgorithmResults::set_tPlannedServer(const ModelTrClass *trClass, const double &a, double value, double ConfInt)
{
    set(resultsType::tPlandedServer, trClass, a, value, ConfInt);
}

void AlgorithmResults::set_tServerMult(const ModelTrClass *trClass, const double &a, double value, double ConfInt)
{
    set(resultsType::tServerMult, trClass, a, value, ConfInt);
}

void AlgorithmResults::set_Qlen(const double &a, double value, double ConfInt)
{
    set(resultsType::Qlen, NULL, a, value, ConfInt);
}

void AlgorithmResults::set_Lost(const ModelTrClass *trClass, const double &a, double value, double ConfInt)
{
    set(resultsType::B, trClass, a, value, ConfInt);
}

void AlgorithmResults::set_Block(const ModelTrClass *trClass, const double &a, double value, double ConfInt)
{
    set(resultsType::E, trClass, a, value, ConfInt);
}

void AlgorithmResults::set_ServTraffic(const ModelTrClass *trClass, const double &a, double value, double ConfInt)
{
    set(resultsType::Y, trClass, a, value, ConfInt);
}


QTextStream &operator<<(QTextStream &str, const ResultsTmp &results)
{
    int clColumns = 0;
    AlgorithmResults *algRes = NULL;

    str<<"#\t";                     //# QoS[0]  QoS[1] ... QoS[n]
    foreach (resultsType type, results.QoSPar)
    {
        int noOfQoS_Col = 0;
        str<<results.resType2string(type);
        foreach(const Investigator *alg, results.algorithms())
        {

            algRes = results.getAlgResult(alg);// _algResults[alg];
            int m = algRes->system->m();
            for (int i=0; i<m; i++)
            {
                if (alg->hasConfIntervall())
                {
                    noOfQoS_Col +=2;
                    str<<"\t\t";
                }
                else
                {
                    noOfQoS_Col +=1;
                    str<<"\t";
                }
            }
            clColumns += (alg->hasConfIntervall()) ? 2 : 1;
        }
    }

    str<<"\n#\t";                   //# Cl1 .. Cln Cl1 .. Cln
    for (int QosIdx = 0; QosIdx<results.QoSPar.length(); QosIdx++)
    {
        int m = algRes->system->m();
        for (int i=0; i<m; i++)
        {
            str<<*(algRes->system->getClass(i));
            for (int sep=0; sep<clColumns; sep++)
                str<<"\t";
        }
    }

    str<<"\n#\t";                  //# alg1 .. algn alg1 .. algn
    for (int QosIdx = 0; QosIdx<results.QoSPar.length(); QosIdx++)
    {
        int m = algRes->system->m();
        for (int i=0; i<m; i++)
        {
            foreach(const Investigator *alg, results.algorithms())
            {
                if (alg->hasConfIntervall())
                    str<<alg->shortName()<<"\t+/-\t";
                else
                    str<<alg->shortName()<<"\t";
            }
        }
    }

    str<<"\n";
    const QVector<double> aVect = results.get_a();
    for (int x=0; x< aVect.length(); x++)
    {
        double a = aVect[x];
        str<<a;

        foreach (resultsType type, results.QoSPar)
        {
            if (algRes == NULL)
                continue;
            int m = algRes->system->m();
            for (int i=0; i<m; i++)
            {
                const ModelTrClass *tmpTrClass = algRes->system->getClass(i);

                foreach(const Investigator *alg, results.algorithms())
                {
                    const AlgorithmResults *algRes = results.getAlgResult(alg);
                    double value, ci;

                    bool found = algRes->getVal(value, ci,type, tmpTrClass, a);
                    if (found)
                    {
                        str<<"\t"<<QString::number(value, 'e', 5);
                        if (alg->hasConfIntervall())
                            str<<"\t"<<QString::number(ci, 'e', 5);
                    }
                    else
                    {
                        str<<"\t";
                        if (alg->hasConfIntervall())
                            str<<"\t";
                    }
                }
            }
        }
        str<<"\n";
    }

    return str;
}


QDebug &operator<<(QDebug &stream, const ResultsTmp &results)
{
    QDebug str=stream.nospace();

    str<<"\t";
    foreach(const Investigator *alg, results.algorithms())
    {
        const AlgorithmResults *algRes = results.getAlgResult(alg);
        int m = algRes->system->m();
        str<<alg->shortName();
        for (int i=0; i<m; i++)
        {
            if (alg->hasConfIntervall())
                str<<"\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t";
            else
                str<<"\t\t\t\t\t\t\t\t";
        }
    }
    str<<"\n\t";
    foreach(const Investigator *alg, results.algorithms())
    {
        const AlgorithmResults *algRes = results.getAlgResult(alg);
        int m = algRes->system->m();
        for (int i=0; i<m; i++)
        {
            str<<*(algRes->system->getClass(i));
            if (alg->hasConfIntervall())
                str<<"\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t";
            else
                str<<"\t\t\t\t\t\t\t\t";
        }
    }
    str<<"\na\t";
    foreach(const Investigator *alg, results.algorithms())
    {
        const AlgorithmResults *algRes = results.getAlgResult(alg);
        int m = algRes->system->m();
        for (int i=0; i<m; i++)
        {
            if (alg->hasConfIntervall())
                str<<"E\t+-\tB\t+-\tQlen\t+-\ty\t+-\tyQ\t+=\tytQ\t+-\tt in qeue\t+-\tt serv\t+-\t";
            else
                str<<"E\tB\tQlen\ty\tyQ\tytQ\tt in qeue\tt serv\t";
        }
    }
    str<<"\n";
    const QVector<double> aVect = results.get_a();
    for (int x=0; x< aVect.length(); x++)
    {
        double a = aVect[x];
        str<<a;

        foreach(const Investigator *alg, results.algorithms())
        {
            const AlgorithmResults *algRes = results.getAlgResult(alg);

            int m = algRes->system->m();
            for (int i=0; i<m; i++)
            {
                const ModelTrClass *tmpTrClass = algRes->system->getClass(i);

                double value, ci;
                bool found;

                found = algRes->getVal(value, ci, resultsType::E, tmpTrClass, a);
                if (found)
                {
                    str<<"\t"<<QString::number(value, 'e', 5);
                    if (alg->hasConfIntervall())
                        str<<"\t"<<QString::number(ci, 'e', 5);
                }
                else
                {
                    str<<"\t";
                    if (alg->hasConfIntervall())
                        str<<"\t";
                }

                found = algRes->getVal(value, ci, resultsType::Y, tmpTrClass, a);
                if (found)
                {
                    str<<"\t"<<QString::number(value, 'e', 5);
                    if (alg->hasConfIntervall())
                        str<<"\t"<<QString::number(ci, 'e', 5);
                }
                else
                {
                    str<<"\t";
                    if (alg->hasConfIntervall())
                        str<<"\t";
                }

                found = algRes->getVal(value, ci, resultsType::B, tmpTrClass, a);
                if (found)
                {
                    str<<"\t"<<QString::number(value, 'e', 5);
                    if (alg->hasConfIntervall())
                        str<<"\t"<<QString::number(ci, 'e', 5);
                }
                else
                {
                    str<<"\t";
                    if (alg->hasConfIntervall())
                        str<<"\t";
                }

                found = algRes->getVal(value, ci, resultsType::Qlen, tmpTrClass, a);
                if (found)
                {
                    str<<"\t"<<QString::number(value, 'e', 5);
                    if (alg->hasConfIntervall())
                        str<<"\t"<<QString::number(ci, 'e', 5);
                }
                else
                {
                    str<<"\t";
                    if (alg->hasConfIntervall())
                        str<<"\t";
                }

                found = algRes->getVal(value, ci, resultsType::y, tmpTrClass, a);
                if (found)
                {
                    str<<"\t"<<QString::number(value, 'e', 5);
                    if (alg->hasConfIntervall())
                        str<<"\t"<<QString::number(ci, 'e', 5);
                }
                else
                {
                    str<<"\t";
                    if (alg->hasConfIntervall())
                        str<<"\t";
                }

                found = algRes->getVal(value, ci, resultsType::yQ, tmpTrClass, a);
                if (found)
                {
                    str<<"\t"<<QString::number(value, 'e', 5);
                    if (alg->hasConfIntervall())
                        str<<"\t"<<QString::number(ci, 'e', 5);
                }
                else
                {
                    str<<"\t";
                    if (alg->hasConfIntervall())
                        str<<"\t";
                }

                found = algRes->getVal(value, ci, resultsType::ytQ, tmpTrClass, a);
                if (found)
                {
                    str<<"\t"<<QString::number(value, 'e', 5);
                    if (alg->hasConfIntervall())
                        str<<"\t"<<QString::number(ci, 'e', 5);
                }
                else
                {
                    str<<"\t";
                    if (alg->hasConfIntervall())
                        str<<"\t";
                }

                found = algRes->getVal(value, ci, resultsType::tWait, tmpTrClass, a);
                if (found)
                {
                    str<<"\t"<<QString::number(value, 'e', 5);
                    if (alg->hasConfIntervall())
                        str<<"\t"<<QString::number(ci, 'e', 5);
                }
                else
                {
                    str<<"\t";
                    if (alg->hasConfIntervall())
                        str<<"\t";
                }

                found = algRes->getVal(value, ci, resultsType::tService, tmpTrClass, a);
                if (found)
                {
                    str<<"\t"<<QString::number(value, 'e', 5);
                    if (alg->hasConfIntervall())
                        str<<"\t"<<QString::number(ci, 'e', 5);
                }
                else
                {
                    str<<"\t";
                    if (alg->hasConfIntervall())
                        str<<"\t";
                }
            }
        }

        str<<"\n";
    }
    return stream;
}

AlgorithmResults::AU_singleA_singleClass::AU_singleA_singleClass(int Vs, int Vb, bool hasCI): Vs(Vs), Vb(Vb), hasCI(hasCI)
{
    qeueAS_vs_qeue.resize(Vb+1);
    serverAS_vs_server.resize(Vs+1);
    qeueAS_vs_system.resize(Vs+Vb+1);
    serverAS_vs_system.resize(Vs+Vb+1);
    systemAS_vs_system.resize(Vs+Vb+1);

    IntOutNew.resize(Vs+Vb+1);
    IntOutEnd.resize(Vs+Vb+1);
    IntInNew.resize(Vs+Vb+1);
    IntInEnd.resize(Vs+Vb+1);

    NoOutNew.resize(Vs+Vb+1);
    NoOutEnd.resize(Vs+Vb+1);
    NoInNew.resize(Vs+Vb+1);
    NoInEnd.resize(Vs+Vb+1);

    hasQvsQ.resize(Vs+Vb+1);       hasQvsQ.fill(false);
    hasQvsSYS.resize(Vs+Vb+1);     hasQvsSYS.fill(false);
    hasSERVvsSERV.resize(Vs+Vb+1); hasSERVvsSERV.fill(false);
    hasSERVvsSYS.resize(Vs+Vb+1);  hasSERVvsSYS.fill(false);
    hasSYSvsSYS.resize(Vs+Vb+1);   hasSYSvsSYS.fill(false);

    hasIntOutNew.resize(Vs+Vb+1);  hasIntOutNew.fill(false);
    hasIntOutEnd.resize(Vs+Vb+1);  hasIntOutEnd.fill(false);

    hasIntInNew.resize(Vs+Vb+1);   hasIntInNew.fill(false);
    hasIntInEnd.resize(Vs+Vb+1);   hasIntInEnd.fill(false);

    hasNoOutNew.resize(Vs+Vb+1);   hasNoOutNew.fill(false);
    hasNoOutEnd.resize(Vs+Vb+1);   hasNoOutEnd.fill(false);

    hasNoInNew.resize(Vs+Vb+1);    hasNoInNew.fill(false);
    hasNoInEnd.resize(Vs+Vb+1);    hasNoInEnd.fill(false);


    serverQeueAS.resize(Vs+1);
    hasServerQeueAS.resize(Vs+1);

    for (int n=0; n<=Vs; n++)
    {
        serverQeueAS[n].resize(Vb+1);
        hasServerQeueAS[n].resize(Vb+1);
        hasServerQeueAS[n].fill(false);
    }

    if (hasCI)
    {
        qeueAS_vs_qeue_CI.resize(Vb+1);
        serverAS_vs_server_CI.resize(Vs+1);
        qeueAS_vs_system_CI.resize(Vs+Vb+1);
        serverAS_vs_system_CI.resize(Vs+Vb+1);
        systemAS_vs_system_CI.resize(Vs+Vb+1);
        serverQeueAS_CI.resize(Vs+1);
        for (int n=0; n<=Vs; n++)
            serverQeueAS_CI[n].resize(Vb+1);

        IntOutEnd_CI.resize(Vs+Vb+1);
        IntInEnd_CI.resize(Vs+Vb+1);
        IntInNew_CI.resize(Vs+Vb+1);
        IntOutNew_CI.resize(Vs+Vb+1);

        NoOutEnd_CI.resize(Vs+Vb+1);
        NoInEnd_CI.resize(Vs+Vb+1);
        NoInNew_CI.resize(Vs+Vb+1);
        NoOutNew_CI.resize(Vs+Vb+1);
    }
}

bool AlgorithmResults::AU_singleA_singleClass::getVal(
          resultsType type
        , int        n
        , double&     value
        , double&     CI
        ) const
{
    switch (type)
    {
    case resultsType::qeueYt_vs_q_n:
        if (!hasQvsQ.value(n, false))
            return false;
        value = qeueAS_vs_qeue[n];
        break;

    case resultsType::qeueYt_vs_sys_n:
        if (!hasQvsSYS.value(n, false))
            return false;
        value = qeueAS_vs_system[n];
        break;

    case resultsType::serverYt_vs_s_n:
        if (!hasSERVvsSERV.value(n, false))
            return false;
        value = serverAS_vs_server[n];
        break;

    case resultsType::serverYt_vs_sys_n:
        if (!hasSERVvsSYS.value(n, false))
            return false;
        value = serverAS_vs_system[n];
        break;

    case resultsType::systemYt_vs_sys_n:
        if (!hasSYSvsSYS.value(n, false))
            return false;
        value = systemAS_vs_system[n];
        break;

    case resultsType::intInNewSC:
        if (!hasIntInNew.value(n, false))
            return false;
        value = IntInNew[n];
        break;

    case resultsType::intInEndSC:
        if (!hasIntInEnd.value(n, false))
            return false;
        value = IntInEnd[n];
        break;

    case resultsType::intOutNewSC:
        if (!hasIntOutNew.value(n, false))
            return false;
        value = IntOutNew[n];
        break;

    case resultsType::intOutEndSC:
        if (!hasIntOutEnd.value(n, false))
            return false;
        value = IntOutEnd[n];
        break;

    default:
        qFatal("Not supported value type %d", (int)type);
    }

    if (hasCI)
    {
        switch (type)
        {
        case resultsType::qeueYt_vs_q_n:
            CI = qeueAS_vs_qeue_CI[n];
            break;

        case resultsType::qeueYt_vs_sys_n:
            CI = qeueAS_vs_system_CI[n];
            break;

        case resultsType::serverYt_vs_s_n:
            CI = serverAS_vs_server_CI[n];
            break;

        case resultsType::serverYt_vs_sys_n:
            CI = serverAS_vs_system_CI[n];
            break;

        case resultsType::systemYt_vs_sys_n:
            CI = systemAS_vs_system_CI[n];
            break;

        case resultsType::intInNewSC:
            CI = IntInNew_CI[n];
            break;

        case resultsType::intInEndSC:
            CI = IntInEnd_CI[n];
            break;

        case resultsType::intOutNewSC:
            CI = IntOutNew_CI[n];
            break;

        case resultsType::intOutEndSC:
            CI = IntOutEnd_CI[n];
            break;

        default:
            qFatal("Not supported value type %d", (int)type);
        }
    }
    return true;
}
bool AlgorithmResults::AU_singleA_singleClass::getValLongInt(
          resultsType type
        , int        n
        , long int&   value
        , double&     CI
        ) const
{
    switch (type)
    {
    case resultsType::qeueYt_vs_q_n:
        if (!hasQvsQ.value(n, false))
            return false;
        value = qeueAS_vs_qeue[n];
        break;

    case resultsType::qeueYt_vs_sys_n:
        if (!hasQvsSYS.value(n, false))
            return false;
        value = qeueAS_vs_system[n];
        break;

    case resultsType::serverYt_vs_s_n:
        if (!hasSERVvsSERV.value(n, false))
            return false;
        value = serverAS_vs_server[n];
        break;

    case resultsType::serverYt_vs_sys_n:
        if (!hasSERVvsSYS.value(n, false))
            return false;
        value = serverAS_vs_system[n];
        break;

    case resultsType::systemYt_vs_sys_n:
        if (!hasSYSvsSYS.value(n, false))
            return false;
        value = systemAS_vs_system[n];
        break;

    case resultsType::intInNewSC:
        if (!hasIntInNew.value(n, false))
            return false;
        value = IntInNew[n];
        break;

    case resultsType::intInEndSC:
        if (!hasIntInEnd.value(n, false))
            return false;
        value = IntInEnd[n];
        break;

    case resultsType::intOutNewSC:
        if (!hasIntOutNew.value(n, false))
            return false;
        value = IntOutNew[n];
        break;

    case resultsType::intOutEndSC:
        if (!hasIntOutEnd.value(n, false))
            return false;
        value = IntOutEnd[n];
        break;

    case resultsType::noInNewSC:
        if (!hasNoInNew.value(n, false))
            return false;
        value = NoInNew[n];
        break;

    case resultsType::noInEndSC:
        if (!hasNoInEnd.value(n, false))
            return false;
        value = NoInEnd[n];
        break;

    case resultsType::noOutNewSC:
        if (!hasNoOutNew.value(n, false))
            return false;
        value = NoOutNew[n];
        break;

    case resultsType::noOutEndSC:
        if (!hasNoOutEnd.value(n, false))
            return false;
        value = NoOutEnd[n];
        break;

    default:
        qFatal("Not supported value type %d", (int)type);
    }

    if (hasCI)
    {
        switch (type)
        {
        case resultsType::qeueYt_vs_q_n:
            CI = qeueAS_vs_qeue_CI[n];
            break;

        case resultsType::qeueYt_vs_sys_n:
            CI = qeueAS_vs_system_CI[n];
            break;

        case resultsType::serverYt_vs_s_n:
            CI = serverAS_vs_server_CI[n];
            break;

        case resultsType::serverYt_vs_sys_n:
            CI = serverAS_vs_system_CI[n];
            break;

        case resultsType::systemYt_vs_sys_n:
            CI = systemAS_vs_system_CI[n];
            break;

        case resultsType::intInNewSC:
            CI = IntInNew_CI[n];
            break;

        case resultsType::intInEndSC:
            CI = IntInEnd_CI[n];
            break;

        case resultsType::intOutNewSC:
            CI = IntOutNew_CI[n];
            break;

        case resultsType::intOutEndSC:
            CI = IntOutEnd_CI[n];
            break;

        case resultsType::noInNewSC:
            CI = NoInNew_CI[n];
            break;

        case resultsType::noInEndSC:
            CI = NoInEnd_CI[n];
            break;

        case resultsType::noOutNewSC:
            CI = NoOutNew_CI[n];
            break;

        case resultsType::noOutEndSC:
            CI = NoOutEnd_CI[n];
            break;

        default:
            qFatal("Not supported value type %d", (int)type);
        }
    }
    return true;
}

void AlgorithmResults::AU_singleA_singleClass::setVal       (resultsType type, int n, double    value, double  CI)
{
    switch (type)
    {
    case resultsType::qeueYt_vs_q_n:
        qeueAS_vs_qeue[n] = value;
        hasQvsQ[n] = true;
        break;

    case resultsType::qeueYt_vs_sys_n:
        qeueAS_vs_system[n] = value;
        hasQvsSYS[n] = true;
        break;

    case resultsType::serverYt_vs_s_n:
        serverAS_vs_server[n] = value;
        hasSERVvsSERV[n]= true;
        break;

    case resultsType::serverYt_vs_sys_n:
        serverAS_vs_system[n] = value;
        hasSERVvsSYS[n] = true;
        break;

    case resultsType::systemYt_vs_sys_n:
        systemAS_vs_system[n] = value;
        hasSYSvsSYS[n] = true;
        break;

    case resultsType::intInNewSC:
        IntInNew[n] = value;
        hasIntInNew[n] = true;
        break;

    case resultsType::intInEndSC:
        IntInEnd[n] = value;
        hasIntInEnd[n] = true;
        break;


    case resultsType::intOutNewSC:
        IntOutNew[n] = value;
        hasIntOutNew[n] = true;
        break;

    case resultsType::intOutEndSC:
        IntOutEnd[n] = value;
        hasIntOutEnd[n] = true;
        break;

    default:
        qFatal("Not supported value type %d", (int)type);
    }

    if (hasCI)
    {
        switch (type)
        {
        case resultsType::qeueYt_vs_q_n:
            qeueAS_vs_qeue_CI[n] = CI;
            break;

        case resultsType::qeueYt_vs_sys_n:
            qeueAS_vs_system_CI[n] = CI;
            break;

        case resultsType::serverYt_vs_s_n:
            serverAS_vs_server_CI[n] = CI;
            break;

        case resultsType::serverYt_vs_sys_n:
            serverAS_vs_system_CI[n] = CI;
            break;

        case resultsType::systemYt_vs_sys_n:
            systemAS_vs_system_CI[n] = CI;
            break;

        case resultsType::intInNewSC:
            IntInNew_CI[n] = CI;
            break;

        case resultsType::intInEndSC:
            IntInEnd_CI[n] = CI;
            break;

        case resultsType::intOutNewSC:
            IntOutNew_CI[n] = CI;
            break;

        case resultsType::intOutEndSC:
            IntOutEnd_CI[n] = CI;
            break;

        default:
            qFatal("Not supported value type %d", (int)type);
        }
    }
}
void AlgorithmResults::AU_singleA_singleClass::setValLongInt(resultsType type, int n, long int  value, double  CI)
{
    switch (type)
    {
    case resultsType::noInNewSC:
        NoInNew[n] = value;
        hasNoInNew[n] = true;
        break;

    case resultsType::noInEndSC:
        NoInEnd[n] = value;
        hasNoInEnd[n] = true;
        break;

    case resultsType::noOutNewSC:
        NoOutNew[n] = value;
        hasNoOutNew[n] = true;
        break;

    case resultsType::noOutEndSC:
        NoOutEnd[n] = value;
        hasNoOutEnd[n] = true;
        break;

    default:
        qFatal("Not supported value type %d", (int)type);
    }

    if (hasCI)
    {
        switch (type)
        {
        case resultsType::noInNewSC:
            NoInNew_CI[n] = CI;
            break;

        case resultsType::noInEndSC:
            NoInEnd_CI[n] = CI;
            break;

        case resultsType::noOutNewSC:
            NoOutNew_CI[n] = CI;
            break;

        case resultsType::noOutEndSC:
            NoOutEnd_CI[n] = CI;
            break;

        default:
            qFatal("Not supported value type %d", (int)type);
        }
    }
}

bool AlgorithmResults::AU_singleA_singleClass::getVal(resultsType type, int nServer, int nQeue, double &value, double &CI)
{
    if (type !=resultsType::serverQeueAS)
        return false;

    if (hasServerQeueAS[nServer][nQeue] == false)
        return false;

    value = serverQeueAS[nServer][nQeue];
    if (hasCI)
        CI = serverQeueAS_CI[nServer][nQeue];
    return false;
}

void AlgorithmResults::AU_singleA_singleClass::setVal(resultsType type, int nServer, int nQeue, double value, double CI)
{
    if (type !=resultsType::serverQeueAS)
        return;

    if (hasServerQeueAS[nServer][nQeue] == false)
        return;

    serverQeueAS[nServer][nQeue] = value;
    if (hasCI)
        serverQeueAS_CI[nServer][nQeue] = CI;

    hasServerQeueAS[nServer][nQeue] = true;
}


AlgorithmResults::AU_singleA::AU_singleA(int Vs, int Vb, bool hasCI): Vs(Vs), Vb(Vb), hasCI(hasCI)
{
    statesSystem.fill(-1, Vs+Vb+1);
    statesServer.fill(-1, Vs+1);
    statesQeue.fill(-1, Vb+1);

    intOutNew.fill(-1, Vs+Vb+1);
    intOutEnd.fill(-1, Vs+Vb+1);
    intInNew.fill(-1, Vs+Vb+1);
    intInEnd.fill(-1, Vs+Vb+1);

    noOutNew.fill(-1, Vs+Vb+1);
    noOutEnd.fill(-1, Vs+Vb+1);
    noInNew.fill(-1, Vs+Vb+1);
    noInEnd.fill(-1, Vs+Vb+1);

    states.resize(Vs+1);
    states_CI.resize(Vs+1);
    for (int n=0; n<=Vs; n++)
    {
        states[n].fill(-1, Vb+1);
        states_CI[n].fill(-1, Vb+1);
    }

    statesSystem_CI.fill(-1, Vs+Vb+1);
    statesServer_CI.fill(-1, Vs+1);
    statesQeue_CI.fill(-1, Vb+1);

    intOutNew_CI.fill(-1, Vs+Vb+1);
    intOutEnd_CI.fill(-1, Vs+Vb+1);
    intInNew_CI.fill(-1, Vs+Vb+1);
    intInEnd_CI.fill(-1, Vs+Vb+1);

    noOutNew_CI.fill(-1, Vs+Vb+1);
    noOutEnd_CI.fill(-1, Vs+Vb+1);
    noInNew_CI.fill(-1, Vs+Vb+1);
    noInEnd_CI.fill(-1, Vs+Vb+1);
}

bool AlgorithmResults::AU_singleA::getVal(resultsType type, int n, double &value, double &CI) const
{
    switch (type)
    {
    case resultsType::trDistribSystem:
        if (n >= (int) statesSystem.length())
            return false;
        if (statesSystem[n] == -1)
            return false;
        value = statesSystem[n];
        break;

    case resultsType::trDistribServer:
        if (n >= (int) statesServer.length())
            return false;
        if (statesServer[n] == -1)
            return false;
        value = statesServer[n];
        break;

    case resultsType::trDistribQeue:
        if (n >= (int) statesQeue.length())
            return false;
        if (statesQeue[n] == -1)
            return false;
        value = statesQeue[n];
        break;

    case resultsType::intInNew:
        if (n >= (int) intInNew.length())
            return false;
        if (intInNew[n] == -1)
            return false;
        value = intInNew[n];
        break;

    case resultsType::intInEnd:
        if (n >= (int) intInEnd.length())
            return false;
        if (intInEnd[n] == -1)
            return false;
        value = intInEnd[n];
        break;


    case resultsType::intOutNew:
        if (n >= (int) intOutNew.length())
            return false;
        if (intOutNew[n] == -1)
            return false;
        value = intOutNew[n];
        break;

    case resultsType::intOutEnd:
        if (n >= (int) intOutEnd.length())
            return false;
        if (intOutEnd[n] == -1)
            return false;
        value = intOutEnd[n];
        break;

    default:
        qFatal("Not supported value type %d", (int)type);
    }

    if (hasCI)
    {
        switch (type)
        {
        case resultsType::trDistribSystem:
            CI = statesSystem_CI[n];
            break;

        case resultsType::trDistribServer:
            CI = statesSystem_CI[n];
            break;

        case resultsType::trDistribQeue:
            CI = statesSystem_CI[n];
            break;

        case resultsType::intInNew:
            CI = intInNew_CI[n];
            break;

        case resultsType::intInEnd:
            CI = intInEnd_CI[n];
            break;

        case resultsType::intOutNew:
            CI = intOutNew_CI[n];
            break;

        case resultsType::intOutEnd:
            CI = intOutEnd_CI[n];
            break;

        default:
            qFatal("Not supported value type %d", (int)type);
        }
    }
    return true;
}
bool AlgorithmResults::AU_singleA::getValLongInt(
          resultsType type
        , int        n
        , long int&   value
        , double&     CI
        ) const
{
    switch (type)
    {
    case resultsType::noInNew:
        if (n >= (int) noInNew.length())
            return false;
        if (noInNew[n] == -1)
            return false;
        value = noInNew[n];
        break;

    case resultsType::noInEnd:
        if (n >= (int) noInEnd.length())
            return false;
        if (noInEnd[n] == -1)
            return false;
        value = noInEnd[n];
        break;


    case resultsType::noOutNew:
        if (n >= (int) noOutNew.length())
            return false;
        if (noOutNew[n] == -1)
            return false;
        value = noOutNew[n];
        break;

    case resultsType::noOutEnd:
        if (n >= (int) noOutEnd.length())
            return false;
        if (noOutEnd[n] == -1)
            return false;
        value = noOutEnd[n];
        break;

    default:
        qFatal("Not supported value type %d", (int)type);
    }

    if (hasCI)
    {
        switch (type)
        {
        case resultsType::noInNew:
            CI = noInNew_CI[n];
            break;

        case resultsType::noInEnd:
            CI = noInEnd_CI[n];
            break;

        case resultsType::noOutNew:
            CI = noOutNew_CI[n];
            break;

        case resultsType::noOutEnd:
            CI = noOutEnd_CI[n];
            break;

        default:
            qFatal("Not supported value type %d", (int)type);
        }
    }
    return true;
}
void AlgorithmResults::AU_singleA::setVal(
          resultsType type
        , int n
        , double value
        , double CI)
{
    switch (type)
    {
    case resultsType::trDistribSystem:
        statesSystem[n] = value;
        //hasState[n]=true;
        break;

    case resultsType::trDistribServer:
        statesServer[n] = value;
        //hasState[n]=true;
        break;

    case resultsType::trDistribQeue:
        statesQeue[n] = value;
        //hasState[n]=true;
        break;

    case resultsType::intInNew:
        intInNew[n] = value;
        break;

    case resultsType::intInEnd:
        intInEnd[n] = value;
        break;

    case resultsType::intOutNew:
        intOutNew[n] = value;
        break;

    case resultsType::intOutEnd:
        intOutEnd[n] = value;
        break;

    case resultsType::noInNew:
        noInNew[n] = value;
        break;

    case resultsType::noInEnd:
        noInEnd[n] = value;
        break;

    case resultsType::noOutNew:
        noOutNew[n] = value;
        break;

    case resultsType::noOutEnd:
        noOutEnd[n] = value;
        break;

    default:
        qFatal("Not supported value type %d", (int)type);
    }

    if (hasCI)
    {
        switch (type)
        {
        case resultsType::trDistribSystem:
            statesSystem_CI[n] = CI;
            break;

        case resultsType::trDistribServer:
            statesServer_CI[n] = CI;
            break;

        case resultsType::trDistribQeue:
            statesQeue_CI[n] = CI;
            break;

        case resultsType::intInNew:
            intInNew_CI[n] = CI;
            break;

        case resultsType::intInEnd:
            intInEnd_CI[n] = CI;
            break;

        case resultsType::intOutNew:
            intOutNew_CI[n] = CI;
            break;

        case resultsType::intOutEnd:
            intOutEnd_CI[n] = CI;
            break;

        case resultsType::noInNew:
            noInNew_CI[n] = CI;
            break;

        case resultsType::noInEnd:
            noInEnd_CI[n] = CI;
            break;

        case resultsType::noOutNew:
            noOutNew_CI[n] = CI;
            break;

        case resultsType::noOutEnd:
            noOutEnd_CI[n] = CI;
            break;


        default:
            qFatal("Not supported value type %d", (int)type);
        }
    }
}
void AlgorithmResults::AU_singleA::setValLongInt(resultsType type, int n, long int value, double CI)
{
    switch (type)
    {
    case resultsType::noInNew:
        noInNew[n] = value;
        break;

    case resultsType::noInEnd:
        noInEnd[n] = value;
        break;

    case resultsType::noOutNew:
        noOutNew[n] = value;
        break;

    case resultsType::noOutEnd:
        noOutEnd[n] = value;
        break;

    default:
        qFatal("Not supported value type %d", (int)type);
    }

    if (hasCI)
    {
        switch (type)
        {
        case resultsType::noInNew:
            noInNew_CI[n] = CI;
            break;

        case resultsType::noInEnd:
            noInEnd_CI[n] = CI;
            break;

        case resultsType::noOutNew:
            noOutNew_CI[n] = CI;
            break;

        case resultsType::noOutEnd:
            noOutEnd_CI[n] = CI;
            break;


        default:
            qFatal("Not supported value type %d", (int)type);
        }
    }
}

bool AlgorithmResults::AU_singleA::getVal(resultsType type, int n, int q, double &value, double &CI) const
{
    switch (type)
    {
    case resultsType::trDistrib:
        if (n >= (int) states.length())
            return false;

        if (q >= (int)states[n].length())
            return false;

        if (states[n][q] == -1)
            return false;
        value = states[n][q];
        break;


    default:
        qFatal("Not supported value type %d", (int)type);
    }

    if (hasCI)
    {
        switch (type)
        {
        case resultsType::trDistrib:
            CI = states_CI[n][q];
            break;

        default:
            qFatal("Not supported value type %d", (int)type);
        }
    }
    return true;
}
void AlgorithmResults::AU_singleA::setVal(resultsType type, int n, int q, double value, double CI)
{
    switch (type)
    {
    case resultsType::trDistrib:
        states[n][q] = value;
        //hasState[n]=true;
        break;

    default:
        qFatal("Not supported value type %d", (int)type);
    }

    if (hasCI)
    {
        switch (type)
        {
        case resultsType::trDistrib:
            states_CI[n][q] = CI;
            break;

        default:
            qFatal("Not supported value type %d", (int)type);
        }
    }
}

bool AlgorithmResults::AU_singleA::getVal(
          resultsType type
        , const ModelTrClass *trClass
        , int               n
        , double&            value
        , double&            CI
        ) const
{
    if (!AS_SinglClass.contains(trClass))
        return false;
    AU_singleA_singleClass *tmpPoint = AS_SinglClass.value(trClass);
    return tmpPoint->getVal(type, n, value, CI);
}

bool AlgorithmResults::AU_singleA::getValLongInt(
          resultsType        type
        , const ModelTrClass *trClass
        , int               n
        , long int&          value
        , double&            CI
        ) const
{
    if (!AS_SinglClass.contains(trClass))
        return false;
    AU_singleA_singleClass *tmpPoint = AS_SinglClass.value(trClass);
    return tmpPoint->getValLongInt(type, n, value, CI);
}

void AlgorithmResults::AU_singleA::setVal(
          resultsType        type
        , const ModelTrClass *trClass
        , int               n
        , double             value
        , double             CI
        )
{
    if (!AS_SinglClass.contains(trClass))
    {
        AU_singleA_singleClass *tmp = new AU_singleA_singleClass(Vs, Vb, hasCI);
        AS_SinglClass.insert(trClass, tmp);
    }
    AU_singleA_singleClass *tmpPoint = AS_SinglClass.value(trClass);
    tmpPoint->setVal(type, n, value, CI);
}

void AlgorithmResults::AU_singleA::setValLongInt(
          resultsType        type
        , const ModelTrClass *trClass
        , int               n
        , long int           value
        , double             CI
        )
{
    if (!AS_SinglClass.contains(trClass))
    {
        AU_singleA_singleClass *tmp = new AU_singleA_singleClass(Vs, Vb, hasCI);
        AS_SinglClass.insert(trClass, tmp);
    }
    AU_singleA_singleClass *tmpPoint = AS_SinglClass.value(trClass);
    tmpPoint->setValLongInt(type, n, value, CI);
}


bool AlgorithmResults::AU::getVal(
          resultsType type
        , double      a
        , int        n
        , double&     value
        , double&     CI
        ) const
{
    AU_singleA *tmpPoint = AS_A.value(a, NULL);

    if (tmpPoint == NULL)
    {
        foreach(double a2, AS_A.keys())
        {
            if (qAbs(a - a2) < 0.0001)
            {
                tmpPoint = AS_A.value(a2, NULL);
                break;
            }
        }
    }

    if (tmpPoint == NULL)
        return false;

    return tmpPoint->getVal(type, n, value, CI);
}

bool AlgorithmResults::AU::getValLongInt(resultsType type, double a, int n, long int &value, double &CI) const
{
    AU_singleA *tmpPoint = AS_A.value(a, NULL);

    if (tmpPoint == NULL)
    {
        foreach(double a2, AS_A.keys())
        {
            if (qAbs(a - a2) < 0.0001)
            {
                tmpPoint = AS_A.value(a2, NULL);
                break;
            }
        }
    }

    if (tmpPoint == NULL)
        return false;

    return tmpPoint->getValLongInt(type, n, value, CI);
}
bool AlgorithmResults::AU::setVal       (resultsType type, double a, int n, double    value, double  CI)
{
    if (std::isnan(value))
        return false;
    if (!AS_A.contains(a))
        AS_A.insert(a, new AU_singleA(Vs, Vb, hasCI));

    AU_singleA *tmpPoint = AS_A.value(a);
    tmpPoint->setVal(type, n, value, CI);

    return true;
}
bool AlgorithmResults::AU::setValLongInt(resultsType type, double a, int n, long int  value, double  CI)
{
    if (std::isnan(value))
        return false;
    if (!AS_A.contains(a))
        AS_A.insert(a, new AU_singleA(Vs, Vb, hasCI));

    AU_singleA *tmpPoint = AS_A.value(a);
    tmpPoint->setValLongInt(type, n, value, CI);

    return true;
}


bool AlgorithmResults::AU::getVal(resultsType type, double a, int n, int q, double &value, double &CI) const
{
    AU_singleA *tmpPoint = AS_A.value(a, NULL);

    if (tmpPoint == NULL)
    {
        foreach(double a2, AS_A.keys())
        {
            if (qAbs(a - a2) < 0.000001)
            {
                tmpPoint = AS_A.value(a2, NULL);
                break;
            }
        }
    }

    if (tmpPoint == NULL)
        return false;

    return tmpPoint->getVal(type, n, q, value, CI);
}
bool AlgorithmResults::AU::setVal(resultsType type, double a, int n, int q, double  value, double  CI)
{
    AU_singleA *tmpPoint = AS_A.value(a, NULL);

    if (tmpPoint == NULL)
    {
        foreach(double a2, AS_A.keys())
        {
            if (qAbs(a - a2) < 0.000001)
            {
                tmpPoint = AS_A.value(a2, NULL);
                break;
            }
        }
    }

    if (tmpPoint == NULL)
        return false;

    tmpPoint->setVal(type, n, q, value, CI);
    return true;
}


bool AlgorithmResults::AU::getVal       (resultsType type, double a, const ModelTrClass *trClass, int n, double   &value, double &CI) const
{
    AU_singleA *tmpPoint = AS_A.value(a, NULL);

    if (tmpPoint == NULL)
    {
        foreach(double a2, AS_A.keys())
        {
            if (qAbs(a - a2) < 0.0001)
            {
                tmpPoint = AS_A.value(a2, NULL);
                break;
            }
        }
    }

    if (tmpPoint == NULL)
        return false;

    return tmpPoint->getVal(type, trClass, n, value, CI);
}

bool AlgorithmResults::AU::getValLongInt(
          resultsType        type
        , double             a
        , const ModelTrClass *trClass
        , int               n
        , long int&          value
        , double&            CI
        ) const
{
    AU_singleA *tmpPoint = AS_A.value(a, NULL);

    if (tmpPoint == NULL)
    {
        foreach(double a2, AS_A.keys())
        {
            if (qAbs(a - a2) < 0.0001)
            {
                tmpPoint = AS_A.value(a2, NULL);
                break;
            }
        }
    }
    if (tmpPoint == NULL)
        return false;

    return tmpPoint->getValLongInt(type, trClass, n, value, CI);
}

bool AlgorithmResults::AU::setVal(
          resultsType        type
        , double             a
        , const ModelTrClass *trClass
        , int               n
        , double             value
        , double  CI
        )
{
    if (std::isnan(value))
        return false;
    if (!AS_A.contains(a))
        AS_A.insert(a, new AU_singleA(Vs, Vb, hasCI));

    AU_singleA *tmpPoint = AS_A.value(a);
    tmpPoint->setVal(type, trClass, n, value, CI);

    return true;
}
bool AlgorithmResults::AU::setValLongInt(resultsType type, double a, const ModelTrClass *trClass, int n, long int  value, double  CI)
{
    if (std::isnan(value))
        return false;
    if (!AS_A.contains(a))
        AS_A.insert(a, new AU_singleA(Vs, Vb, hasCI));

    AU_singleA *tmpPoint = AS_A.value(a);
    tmpPoint->setValLongInt(type, trClass, n, value, CI);

    return true;
}

ResultsTmp::_init::_init()
{
    _NA = QString("NA");
    _logScale = QString("logscale");
    _linearScale = QString("nologscale");

    _resType2details.insert(resultsType::yQ,                 QoSdetails("Avarage number of calls in qeue",                               "yQ",           true,  true,  false, false, false, false, resultFormat::typeDouble));
    _resType2details.insert(resultsType::y,                  QoSdetails("Avarage number of serviced calls in whole system",              "y",            false, true,  true,  false, false, false, resultFormat::typeDouble));
    _resType2details.insert(resultsType::ytQ,                QoSdetails("Avarage number of queues resourcess ocpupied by class class i", "ytQ",          true,  true,  false, false, false, false, resultFormat::typeDouble));

    _resType2details.insert(resultsType::tWait,              QoSdetails("Effective waiting time",                                        "tWait",        true,  true,  false, false, false, false, resultFormat::typeDouble));
    _resType2details.insert(resultsType::tService,           QoSdetails("Avarage service time",                                          "tOfServ",      false, true,  false, false, false, false, resultFormat::typeDouble));
    _resType2details.insert(resultsType::tServer,            QoSdetails("Avarage time on server",                                        "tOnServ",      true,  true,  false, false, false, false, resultFormat::typeDouble));
    _resType2details.insert(resultsType::tQeue,              QoSdetails("Avarage time on qeue",                                          "tOnQeue",      true,  true,  false, false, false, false, resultFormat::typeDouble));
    _resType2details.insert(resultsType::tPlandedServer,     QoSdetails("Planed service time",                                           "Pt",           false, true,  false, false, false, false, resultFormat::typeDouble));
    _resType2details.insert(resultsType::tServerMult,        QoSdetails("Time on server/planed time on server",                          "Tmult",        true,  true,  false, false, false, false, resultFormat::typeDouble));

    _resType2details.insert(resultsType::E,                  QoSdetails("Blocking probability",                                          "E",            false, true,  false, false, false, true , resultFormat::typeDouble));
    _resType2details.insert(resultsType::B,                  QoSdetails("Loss probability",                                              "B",            false, true,  false, false, false, true , resultFormat::typeDouble));
    _resType2details.insert(resultsType::Y,                  QoSdetails("Serviced traffic",                                              "Y",            false, true,  false, false, false, false, resultFormat::typeDouble));
    _resType2details.insert(resultsType::Qlen,               QoSdetails("Avarage qeue length",                                           "Qlen",         true,  false, false, false, false, false, resultFormat::typeDouble));

    _resType2details.insert(resultsType::qeueYt_vs_q_n,      QoSdetails("Avg AS in qeue by class when n res of qeue is occupied",        "yt Q/Q",       true,  true,  false, false, true,  false, resultFormat::typeDouble));
    _resType2details.insert(resultsType::serverYt_vs_s_n,    QoSdetails("Avg AS in server by class when n res of server is occupied",    "yt Serv/Serv", true,  true,  false, true,  false, false, resultFormat::typeDouble));
    _resType2details.insert(resultsType::qeueYt_vs_sys_n,    QoSdetails("AVG AS in qeue when n res of the system is occupied",           "yt Q/Sys",     true,  true,  true,  false, false, false, resultFormat::typeDouble));
    _resType2details.insert(resultsType::serverYt_vs_sys_n,  QoSdetails("AVG AS in server when n res of the system is occupied",         "yt Serv/Sys",  true,  true,  true,  false, false, false, resultFormat::typeDouble));
    _resType2details.insert(resultsType::systemYt_vs_sys_n,  QoSdetails("AVG AS in system when n res of the system is occupied",         "yt Sys/Sys",   false, true,  true,  false, false, false, resultFormat::typeDouble));
    _resType2details.insert(resultsType::serverQeueAS,       QoSdetails("AVG AS in the state (n_server, n_buffer)",                      "yt[Serv,Q]",   true,  true,  false, true,  true , false, resultFormat::typeDouble));

    _resType2details.insert(resultsType::trDistrib,          QoSdetails("State probabilities in [server][qeue]",                         "TrDistr",      true,  false, false, true,  true , true , resultFormat::typeDouble));
    _resType2details.insert(resultsType::trDistribSystem,    QoSdetails("State probabilities in whole system",                           "pSys",         false, false, true,  false, false, true , resultFormat::typeDouble));
    _resType2details.insert(resultsType::trDistribServer,    QoSdetails("State probabilities in server",                                 "pServ",        true,  false, false, true,  false, true , resultFormat::typeDouble));
    _resType2details.insert(resultsType::trDistribQeue,      QoSdetails("State probabilities in qeue",                                   "pQeue",        true,  false, false, false, true , true , resultFormat::typeDouble));

    _resType2details.insert(resultsType::intInNew,           QoSdetails("Intensity of reaching state after new call",                    "in+",          false, false, true,  false, false, false, resultFormat::typeDouble));
    _resType2details.insert(resultsType::intInEnd,           QoSdetails("Intensity of reaching state after call service ending",         "in-",          false, false, true,  false, false, false, resultFormat::typeDouble));
    _resType2details.insert(resultsType::intOutNew,          QoSdetails("Offered intensity of new calls",                                "out+",         false, false, true,  false, false, false, resultFormat::typeDouble));
    _resType2details.insert(resultsType::intOutEnd,          QoSdetails("Offered intensity of calls service",                            "out-",         false, false, true,  false, false, false, resultFormat::typeDouble));

    _resType2details.insert(resultsType::noOutNew,           QoSdetails("Number of passages from state after new call",                  "no out+",      false, false, true,  false, false, true , resultFormat::typeLongInt));
    _resType2details.insert(resultsType::noOutEnd,           QoSdetails("Number of passages from state after call service ending",       "no out-",      false, false, true,  false, false, true , resultFormat::typeLongInt));
    _resType2details.insert(resultsType::noInNew,            QoSdetails("Number of passages to state after new call",                    "no in+",       false, false, true,  false, false, true , resultFormat::typeLongInt));
    _resType2details.insert(resultsType::noInEnd,            QoSdetails("Number of passages to state after call service ending",         "no in-",       false, false, true,  false, false, true , resultFormat::typeLongInt));

    _resType2details.insert(resultsType::intInNewSC,         QoSdetails("Intensity of reaching new state after new call of class i",     "in+i",         false, true,  true,  false, false, false, resultFormat::typeDouble));
    _resType2details.insert(resultsType::intInEndSC,         QoSdetails("Intensity of reaching new state after call class i serv end",   "in-i",         false, true,  true,  false, false, false, resultFormat::typeDouble));
    _resType2details.insert(resultsType::intOutNewSC,        QoSdetails("Offered intensity of new calls of class i",                     "out+i",        false, true,  true,  false, false, false, resultFormat::typeDouble));
    _resType2details.insert(resultsType::intOutEndSC,        QoSdetails("Offered intensity of calls of class i service",                 "out-i",        false, true,  true,  false, false, false, resultFormat::typeDouble));

    _resType2details.insert(resultsType::noInNewSC,          QoSdetails("Number of passages to state after new call of class i",         "no in+i",      false, true,  true,  false, false, true , resultFormat::typeLongInt));
    _resType2details.insert(resultsType::noInEndSC,          QoSdetails("Number of passages to state after call class i serv end",       "no in-i",      false, true,  true,  false, false, true , resultFormat::typeLongInt));
    _resType2details.insert(resultsType::noOutNewSC,         QoSdetails("Number of passages from state after new calls of class i",      "no out+i",     false, true,  true,  false, false, true , resultFormat::typeLongInt));
    _resType2details.insert(resultsType::noOutEndSC,         QoSdetails("Number of passages from state after calls of class i service",  "no out-i",     false, true,  true,  false, false, true , resultFormat::typeLongInt));
}
#endif
