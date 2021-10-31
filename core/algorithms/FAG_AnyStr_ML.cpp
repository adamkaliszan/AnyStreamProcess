#define PY_SSIZE_T_CLEAN
#include "FAG_AnyStr_ML.h"

#include <tensorflow/c/c_api.h>

#include <tensorflow/core/platform/env.h>
#include <tensorflow/core/public/session.h>


#include "results/resultsInvestigator.h"

/*
(tf) adam@piaskun-gtr:~/GitHub/AnyStreamProcess/core/ml/trained_models/10$ saved_model_cli show --dir ./Uniform/model_all_points/
The given SavedModel contains the following tag-sets:
'serve'

(tf) adam@piaskun-gtr:~/GitHub/AnyStreamProcess/core/ml/trained_models/10$ saved_model_cli show --dir ./Uniform/model_all_points/ --tag_set serve
The given SavedModel MetaGraphDef contains SignatureDefs with the following keys:
SignatureDef key: "__saved_model_init_op"
SignatureDef key: "serving_default"

(tf) adam@piaskun-gtr:~/GitHub/AnyStreamProcess/core/ml/trained_models/10$ saved_model_cli show --dir ./Uniform/model_all_points/ --tag_set serve --signature_def serving_default
The given SavedModel SignatureDef contains the following input(s):
  inputs['dense_3_input'] tensor_info:
      dtype: DT_FLOAT
      shape: (-1, 22)
      name: serving_default_dense_3_input:0
The given SavedModel SignatureDef contains the following output(s):
  outputs['dense_5'] tensor_info:
      dtype: DT_FLOAT
      shape: (-1, 54)
      name: StatefulPartitionedCall:0
Method name is: tensorflow/serving/predict
*/

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
    initialized = true;
}

void FAG_AnyStr_ML::finish()
{
    initialized = false;
}

void NoOpDeallocator(void* data, size_t a, void* b) {}

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

    prepareTemporaryData(system, a);
    p_single = new TrClVector[system.m()];



    int m = system.m();

    int NumInputs = (system.V()+1) * 2;
    int NumOutputs = (system.V()+1) * (system.V()) / 2;

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

            TF_SessionOptions *sesionOptions = TF_NewSessionOptions();
            TF_Buffer *bufferRunOptions = nullptr; //TF_NewBuffer();
            const char* const tags[] = {"serve"};

            TF_Graph* graph = TF_NewGraph();
            TF_Buffer* meta_graph_def = TF_NewBuffer();
            TF_Status* status = TF_NewStatus();


            TF_Session* model = TF_LoadSessionFromSavedModel(
               sesionOptions, bufferRunOptions, (const char *)modelNameStream.string()->toStdWString().c_str(),
               tags, sizeof (tags)/sizeof(const char*),
               graph, meta_graph_def, status);

            if(TF_GetCode(status) == TF_OK)
            {
                qDebug("TF_LoadSessionFromSavedModel OK\n");
            }
            else
            {
                qDebug("Failed to load model %s",TF_Message(status));
                return;
            }

            //****** Get input tensor
            TF_Output* Input = (TF_Output*)malloc(sizeof(TF_Output) * NumInputs);

            for (int inpNo = 0; inpNo < NumInputs; inpNo++)
            {
                Input[inpNo] = {TF_GraphOperationByName(graph, "serving_default_dense_3_input"), inpNo};

                if(Input[inpNo].oper == NULL)
                    qDebug("ERROR: Failed TF_GraphOperationByName serving_default_dense_3_input:%d", inpNo);
            }


            //********* Get Output tensor
            TF_Output* Output = (TF_Output*)malloc(sizeof(TF_Output) * NumOutputs);

            for (int outputNo = 0; outputNo < NumOutputs; outputNo++)
            {
                Output[outputNo] = {TF_GraphOperationByName(graph, "StatefulPartitionedCall"), outputNo};
                if(Output[outputNo].oper == NULL)
                    qDebug("ERROR: Failed TF_GraphOperationByName StatefulPartitionedCall:%d", outputNo);
            }

            //********* Allocate data for inputs & outputs
            TF_Tensor** InputValues  = (TF_Tensor**)malloc(sizeof(TF_Tensor*)*NumInputs);
            TF_Tensor** OutputValues = (TF_Tensor**)malloc(sizeof(TF_Tensor*)*NumOutputs);

            int ndims = 2;
            int64_t dims[] = {1, 1}; //{22, 1} ???
            float data; //TODO replace it
            size_t ndata = sizeof(float); // This is tricky, it number of bytes not number of element

            for (int stN=0; stN < NumInputs; stN++)
            {
                data = (stN<= system.V()) ? p_single[i].getState(stN).tIntOutNew : p_single[i].getState(stN-system.V()-1).tIntOutEnd;
                InputValues[i] = TF_NewTensor(TF_FLOAT, dims, ndims, (void*)&data, ndata, &NoOpDeallocator, nullptr);
                if (InputValues[i] == nullptr)
                    qDebug("Fail: TF_NewTensor -> nullptr !!!");
            }

            // //Run the Session
            TF_SessionRun(model, NULL, Input, InputValues, NumInputs, Output, OutputValues, NumOutputs, NULL, 0,NULL , status);



            if(TF_GetCode(status) == TF_OK)
            {
                qDebug("Session is OK\n");
            }
            else
            {
                qDebug("%s",TF_Message(status));
            }

            TrClVector *tmp = &p_single[i];
            int lclState = 0;
            int tmpV = system.V()-1;
            for (int stN = 0; stN < NumOutputs; stN++)
            {
                if (lclState == 0)
                {
                    tmp->previous = new TrClVector(tmpV, tmp->aggregatedClasses);
                }


                void* buff = TF_TensorData(OutputValues[stN]);
                float* item = (float *) buff;

                tmp->previous->getState(lclState).tIntOutNew = *item;
                tmp->previous->getState(lclState).tIntOutEnd = lclState;

                lclState++;
                if (lclState > tmpV)
                {
                    tmpV--;
                    lclState = 0;
                    tmp->previous->calculateP_baseOnOutIntensities();
                    tmp = tmp->previous;
                }
            }
            tmp->previous = new TrClVector(0, tmp->aggregatedClasses);
            tmp->previous->getState(0).tIntOutNew = 1;
            tmp->previous->getState(0).tIntOutEnd = 0;
            tmp->previous->calculateP_baseOnOutIntensities();
            tmp->previous->previous = nullptr;


            // //Free memory
            TF_DeleteGraph(graph);
            TF_DeleteSession(model, status);
            TF_DeleteSessionOptions(sesionOptions);
            TF_DeleteStatus(status);

            //TF: tensorflow::SavedModelBundle model;

    /// Building list with intensities for python
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

/*
 * #!/bin/env python 3
from tensorflow import keras
from numpy import expand_dims

import numpy as np
import os, sys

from sklearn import preprocessing

np.set_printoptions(precision=3, suppress=True)


#Użycie zapisanego modelu
def calculate(intensities, modelFileName):

    try:
        loaded_model = keras.models.load_model(modelFileName)
        print("Załadowano model, Obliczam rozkłady:\n")

    except ImportError:
        sys.stderr.write(f"Nie mogę załadować modelu. Jestem w katalogu {os.getcwd()}")
        return None
    except IOError:
        sys.stderr.write(f"Nie mogę załadować modelu. Jestem w katalogu {os.getcwd()}")
        return None
    except Exception:
        sys.stderr.write("Coś poszło nie tak")
        return None

    #return intensities
    return calculateP_baseOnOutIntensities(loaded_model, intensities)

def calculateP_baseOnOutIntensities(model, intensities):

    intensities2 = np.array(intensities)
    #print(intensities2.shape)
    intensities2 = expand_dims(intensities2, axis=0)
    #print(intensities2.shape)
    results = model.predict(intensities2)
    return results[0, :].tolist()
*/
