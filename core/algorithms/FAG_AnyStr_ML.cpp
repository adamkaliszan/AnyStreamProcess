#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "FAG_AnyStr_ML.h"

#include "results/resultsInvestigator.h"

namespace Algorithms
{
FAG_AnyStr_ML::FAG_AnyStr_ML(): Investigator()
{
    initialized = false;
    myQoS_Set
       <<Results::Type::BlockingProbability
       <<Results::Type::OccupancyDistribution;
}

FAG_AnyStr_ML::~FAG_AnyStr_ML()
{
    if (initialized)
        finish();
}

void FAG_AnyStr_ML::initialize()
{
    Py_Initialize();
    initialized = true;

    PyRun_SimpleString(
        "import sys, os\n"
        "sys.path.append(os.getcwd())\n"
        "print (sys.version)\n"
    );

    PyObject *pName = PyUnicode_DecodeFSDefault("core.ml.trDistributionML");

    pModule = PyImport_Import(pName);
    if (pModule == nullptr)
    {
        PyErr_Print();
        initialized = false;



        Py_DECREF(pName);
        Py_Finalize();
        return;
    }
    else
    {
        Py_DECREF(pName);
    }

    pFuncArrivalDistribution = PyObject_GetAttrString(pModule, "calculate");
    if (pFuncArrivalDistribution == nullptr)
    {
        PyErr_Print();
        Py_DECREF(pModule);
        Py_Finalize();
        initialized = false;
        return;
    }
}

void FAG_AnyStr_ML::finish()
{
    Py_DECREF(pFuncArrivalDistribution);
    Py_DECREF(pModule);
    Py_Finalize();

    initialized = false;
}
void FAG_AnyStr_ML::calculateSystem(const ModelSystem &system
      , double a
      , RInvestigator *results
      , SimulationParameters *simParameters
      )
{
    (void) simParameters;
    if (!initialized)
        initialize();
    if (!initialized)
    {
        qErrnoWarning("Nie mogę zainiclializować alorytmu z uczeniem maszynowym");
        return;
    }
    PyObject *pArgs;
    PyObject *pList;

    prepareTemporaryData(system, a);
    p_single = new TrClVector[system.m()];


    int m = system.m();
    for (int i=0; i<m; i++)
    {
        p_single[i] = system.getTrClass(i).trDistribution(i, classes[i].A, system.V(), 0);

        if (system.getTrClass(i).newCallStr() == ModelTrClass::StreamType::Poisson)
        {
            TrClVector *tmp = &p_single[i];
            for (int tmpV = system.V() - 1; tmpV >= 0; tmpV--)
            {
                tmp->previous = new TrClVector(tmpV, tmp->aggregatedClasses);
                *(tmp->previous) = system.getTrClass(i).trDistribution(i, classes[i].A, tmpV, 0);
                tmp = tmp->previous;
            }
        }
        else
        {
            QString modelName;
            QTextStream modelNameStream(&modelName);
            modelNameStream<<"./core/ml/trained_models/"<<system.V()<<"/"<<ModelTrClass::streamTypeToString(system.getTrClass(i).newCallStr())<< "/model_all_points";

    /// Building list with intensities for python
            pArgs = PyTuple_New(2);
            pList = PyList_New(0);//2*(p_single[i].V() + 1));
            for (int n=0; n<=p_single[i].V(); n++)
            {
                PyObject *pValue = PyFloat_FromDouble(p_single[i].getState(n).tIntOutNew);
                PyList_Append(pList, pValue);
            }
            for (int n=0; n<=p_single[i].V(); n++)
            {
                PyObject *pValue = PyFloat_FromDouble(p_single[i].getState(n).tIntOutEnd);
                PyList_Append(pList, pValue);
            }

            PyObject *pName = PyUnicode_DecodeFSDefault(modelName.toStdString().c_str());
            PyTuple_SetItem(pArgs, 0, pList);
            PyTuple_SetItem(pArgs, 1, pName);

            PyObject *pResult = PyObject_CallObject(pFuncArrivalDistribution, pArgs);
            if (pResult == nullptr)
            {
                PyErr_Print();
                qDebug("Dostałem nulla");
                Py_DECREF(pList);
                Py_DECREF(pName);
                Py_DECREF(pArgs);
                delete []p_single;
                deleteTemporaryData();
                return;
            }

    /// Odbieranie rezultatu
            int x = PyList_Size(pResult);
            int y = PyTuple_Size(pResult);
            if (x == -1)
            {
                //PyErr_Print();
                qDebug("Zły obiekt listy x = %d, y = %d", x, y);
                Py_DECREF(pResult);

                Py_DECREF(pList);
                Py_DECREF(pName);
                Py_DECREF(pArgs);
                delete []p_single;
                deleteTemporaryData();
                return;
            }
            int idx = 0;
            qDebug("Odebrałem listę o długości %d", x);

            TrClVector *tmp = &p_single[i];
            for (int tmpV = system.V() - 1; tmpV > 0; tmpV--)
            {
                tmp->previous = new TrClVector(tmpV, tmp->aggregatedClasses);
                for (int n=0; n<=tmpV; n++)
                {
                    PyObject *tmpObj = PyList_GetItem(pResult, idx++);
                    if (tmpObj == nullptr)
                        qFatal("tIntOutNew: Wrong python object, can't get an list item. List len = %x, list idx = %d, tmpV = %d, n = %d", x, idx, tmpV, n);
                    if (!PyFloat_Check(tmpObj))
                        qFatal("List item is not a float type %d/%d", idx-1, x);

                    tmp->previous->getState(n).tIntOutNew = PyFloat_AsDouble(tmpObj);
                    tmp->previous->getState(n).tIntOutEnd = n; continue;//TODO PyFloat_AsDouble(tmpObj);

             //       tmpObj = PyList_GetItem(pResult, idx++);
             //       if (tmpObj == nullptr)
             //           qFatal("tIntOutNew: Wrong python object, can't get an list item. List len = %x, list idx = %d, tmpV = %d, n = %d", x, idx, tmpV, n);
             //       if (!PyFloat_Check(tmpObj))
             //           qFatal("List item is not a float type %d/%d", idx-1, x);

             //       tmp->previous->getState(n).tIntOutEnd = PyFloat_AsDouble(tmpObj);
                }
                tmp->previous->calculateP_baseOnOutIntensities();
                tmp = tmp->previous;
            }
            Py_DECREF(pResult);
            //Py_DECREF(pList);
            //Py_DECREF(pName);
            //Py_DECREF(pArgs);

            tmp->previous = new TrClVector(0, tmp->aggregatedClasses);
            tmp->previous->getState(0).tIntOutNew = 1;
            tmp->previous->getState(0).tIntOutEnd = 0;
            tmp->previous->calculateP_baseOnOutIntensities();
            tmp->previous->previous = nullptr;
        }
        p_single[i].normalize();
        qDebug()<<".";//p_single[i];
    }

    TrClVector P(system.V());
    P = TrClVector(system.V());
    P.generateNormalizedPoissonPrevDistrib();
    int V = system.V();
    for (int i=0; i < system.m(); i++)
    {
        P = TrClVector::convFAGanyStream(P, p_single[i],V);
        P.normalize();
    }

    for (int i=0; i<system.m(); i++)
    {
        //Prawdopodobieństwo blokady i strat
        double E = 0;
        double B_n = 0;
        double B_d = 0;
        for (int n=system.V() + 1 - classes[i].t; n <= system.V(); n++)
        {
            E+=P[n];
            B_n+=(P[n] * P.getIntOutNew(n, i));
        }
        for (int n=0; n <= system.V(); n++)
        {
            B_d+=(P[n] * P.getIntOutNew(n, i));
        }

        (*results)->write(TypeForClass::BlockingProbability, E, i);
        (*results)->write(TypeForClass::LossProbability, B_n/B_d, i);


        //Średnia liczba obsługiwanych zgłoszeń
        //TODO for (int n=0; n <= system->V(); n++)
        //{
        //    algResults->set_lSys(system->getClass(i), a, P.getY(n, i));
        //}
        //Obsługiwany ruch
        double yS = 0;

        for (int n=0; n <= system.V(); n++)
        {
            yS+=(P[n] * P.getY(n, i));
        }
        //TODO algResults->set_ServTraffic(system->getClass(i), a, yS * classes[i].t);


        //Średni czas obsługi
        //algRes->set_tService(system->getClass(i), a, lQeue / A[i] * system->getClass(i)->getMu());
        double avgToS = 0;
        for (int n=0; n <= system.getServer().V(); n++)
        {
            avgToS += P[n] / P.getIntOutEnd(n, i);
        }
        //TODO algResults->set_tService(system->getClass(i), a, avgToS);


        for (int n=0; n <= system.getServer().V(); n++)
        {
            (*results)->write(TypeForClassAndServerState::Usage, P.getY(n, i)*classes[i].t, i, n);
        }
        for (int n=0; n <= system.V(); n++)
        {
            (*results)->write(TypeForClassAndSystemState::UsageForSystem, P.getY(n, i)*classes[i].t, i, n);

        //Intensywności przejść klas
            (*results)->write(TypeForClassAndSystemState::NewCallIntensityInForSystem, P.getIntInNew(n, i), i, n);
            (*results)->write(TypeForClassAndSystemState::EndCallIntensityInForSystem, P.getIntInEnd(n, i), i, n);

            (*results)->write(TypeForClassAndSystemState::OfferedNewCallIntensityOutForSystem, P.getIntOutNew(n, i), i, n);
            (*results)->write(TypeForClassAndSystemState::EndCallIntensityOutForSystem,        P.getIntOutEnd(n, i), i, n);
        }
    }

    for (int n=0; n <= system.V(); n++)
    {
        //Rozkład zajętości
        (*results)->write(TypeForSystemState::StateProbability, P[n], n);

        //Intensywności przejść (wejście do stanu)
        (*results)->write(TypeForSystemState::IntensityNewCallIn, P.getState(n).tIntInNew, n);
        (*results)->write(TypeForSystemState::IntensityEndCallIn, P.getState(n).tIntInEnd, n);

        //Intensywności przejść (wyjście ze stanu)
        (*results)->write(TypeForSystemState::IntensityNewCallOutOffered, P.getState(n).tIntOutNew, n);
        (*results)->write(TypeForSystemState::IntensityEndCallOut, P.getState(n).tIntOutEnd, n);
    }

    deleteTemporaryData();
    delete []p_single;
    //emit this->sigCalculationDone();
}

bool FAG_AnyStr_ML::possible(const ModelSystem &system) const
{
    if (system.getBuffer().V() > 0)
        return false;

    if (system.getServer().k() > 1)
        return false;

    return Investigator::possible(system);
}

} // namespace Algorithms
