#define PY_SSIZE_T_CLEAN
#include "FAG_AnyStr_ML.h"

#include <stack>

#include <tensorflow/cc/framework/ops.h>
#include <tensorflow/cc/client/client_session.h>
#include <tensorflow/cc/saved_model/loader.h>
#include <tensorflow/cc/saved_model/tag_constants.h>
#include <tensorflow/cc/ops/standard_ops.h>
#include <tensorflow/core/framework/tensor.h>

using namespace tensorflow;
using namespace tensorflow::ops;

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
        qErrnoWarning("Nie mogę zainiclializować algorytmu z uczeniem maszynowym");
        return;
    }

    prepareTemporaryData(system, a);
    p_single = new TrClVector[system.m()];


    int m = system.m();

    int NumInputs = (system.V()+1);
    int NumOutputs = ((system.V()+1) * (system.V()) / 2) - 1;

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

            auto scope = Scope::NewRootScope();
            tensorflow::SessionOptions session_options_;
            tensorflow::RunOptions run_options_;
            tensorflow::SavedModelBundle model_;

            auto status = tensorflow::LoadSavedModel(session_options_, run_options_, modelName.toStdString(), {tensorflow::kSavedModelTagServe}, &model_);
            if (!status.ok())
            {
                qDebug("Failed to load model %s: %s", modelName.toStdString().c_str(), status.ToString().c_str());
                return;
            }

            auto sig_map = model_.GetSignatures();
            auto model_def = sig_map.at("serving_default");

            qDebug("Model Signature");
            for (auto const& p : sig_map) {
                qDebug("key: %s", p.first.c_str());
            }

            qDebug("Model Input Nodes");
            for (auto const& p : model_def.inputs()) {
                qDebug("key: %s value: %s", p.first.c_str(), p.second.name().c_str());
            }

            qDebug("Model Output Nodes");
            for (auto const& p : model_def.outputs()) {
                qDebug("key: %s value: %s", p.first.c_str(), p.second.name().c_str());
            }

            std::vector<Tensor> bbox_output;

            // fill the input tensors with data
            TensorShape inputShape({1, NumInputs});
            TensorShape outputShape({1, NumOutputs});


            p_single[i] = system.getTrClass(i).trDistribution(i, classes[i].A, system.V(), 0);

            Tensor tens(DataType::DT_FLOAT, inputShape);
            auto tensMapped = tens.tensor<float, 2>();

            qDebug("Preparing input data");
            for (int n=0; n<= system.V(); n++)
            {
                tensMapped(0, n) = p_single[i].getState(n).tIntOutNew;
                //tensMapped(0, system.V() + 1 + n) = p_single[i].getState(n).tIntOutEnd;
            }

            qDebug("Calculating data model");
            status = model_.session->Run({ {"serving_default_input:0", tens} }, {"StatefulPartitionedCall:0"}, {}, &bbox_output);
            if (!status.ok()) {
                std::cerr << "Inference failed: " << status;
                return;
            }

            qDebug("Rewriting results");
            auto resultsMapped = bbox_output[0].tensor<float, 2>();


            TrClVector *tmp = &p_single[i];
            int tmpV = system.V()-1;

            //TODO poprawić

            std::stack<TrClVector *> distributions;

            while (tmpV >= 0)
            {
                tmp->previous = new TrClVector(tmpV, tmp->aggregatedClasses);
                tmp = tmp->previous;
                distributions.push(tmp);
                tmpV--;
            }
            tmp = distributions.top();
            distributions.pop();
            tmp->getState(0).tIntOutNew = 1;
            tmp->getState(0).tIntOutEnd = 0;
            tmp->calculateP_baseOnOutIntensities();
            tmp->previous = nullptr;

            tmp = distributions.top();
            distributions.pop();
            tmpV = 1;
            int lclState = 0;
            for (int stN = 0; stN < NumOutputs; stN++)
            {
                tmp->getState(lclState).tIntOutNew = resultsMapped(0, stN);
                tmp->getState(lclState).tIntOutEnd = lclState;

                lclState++;
                if (lclState > tmpV)
                {
                    tmpV++;
                    tmp->calculateP_baseOnOutIntensities();
                    lclState = 0;
                    if (tmpV < system.V())
                    {
                        tmp = distributions.top();
                        distributions.pop();
                    }
                }
            }
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
