#include<QDebug>
#include<QVector>
#include<QList>
#include <iostream>
#include <fstream>

#include <QJsonDocument>

#include "../core/model.h"

ModelSystem prepareSystem()
{
    QVector<ModelTrClass> trClasses;

    QList<ModelSubResourcess> serverGroups;
    ResourcessScheduler serverScheduler = ResourcessScheduler::Random;
    ModelResourcess resServer( serverGroups, serverScheduler);

    QList<ModelSubResourcess> bufferGroups;
    ResourcessScheduler bufferScheduler = ResourcessScheduler::Random;
    ModelResourcess resBuffer(bufferGroups, bufferScheduler);

    SystemPolicy policy = SystemPolicy::NoBuffer;

    return ModelSystem(trClasses, resServer, resBuffer, policy);
}

int main(int argc, char *argv[])
{
    int v = 120;

    ModelSystem model = prepareSystem();

    ModelTrClass trClass;
    trClass.setT(1);
    trClass.setMu(1);
    trClass.setNewCallStrType(ModelTrClass::StreamType::Poisson, ModelTrClass::SourceType::Independent);
    trClass.setCallServStrType(ModelTrClass::StreamType::Poisson);

    std::ofstream file;
    file.open("results.js");

    QJsonObject jsonTrClass;

    ModelTrClass::StreamType strType[] = {ModelTrClass::StreamType::Gamma, ModelTrClass::StreamType::Normal, ModelTrClass::StreamType::Pareto, ModelTrClass::StreamType::Poisson, ModelTrClass::StreamType::Uniform};






    file<< "{";
    for (int aNum=1; aNum<=4; aNum++)
    {
        if (aNum > 1)
            file<<",";
        for (int n=0; n<=10; n++)
        {
            if (n > 0)
                file<<",";
            double A =  static_cast<double>(aNum)/10.0;
            file<< QString("\"A%1\" : ").arg(A).toStdString();
            QJsonObject jsonAV = trClass.trDistribution(0, A, n, 0).getJson();
            file<< QJsonDocument(jsonAV).toJson(QJsonDocument::JsonFormat::Compact).toStdString();
        }
    }
    file<<"}";

    return 0;
}
