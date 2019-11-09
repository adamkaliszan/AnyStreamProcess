#include<QDebug>
#include<QVector>
#include<QList>
#include <iostream>
#include <fstream>

#include <QJsonDocument>

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QUrl>
#include <QVariantMap>
#include <QVariant>

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
    QVariantMap map;
//    map.insert("api_key",WRKey);
    map.insert("V", QVariant(50));
    QByteArray payload=QJsonDocument::fromVariant(map).toJson();
    qDebug()<<QVariant(payload).toString();

    QUrl myurl;
    myurl.setScheme("http"); //https also applicable
    myurl.setHost("api.thingspeak.com");
    myurl.setPath("/update.json");

    QNetworkRequest request;
    request.setUrl(myurl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

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

    ModelTrClass::SourceType srcType[] = {ModelTrClass::SourceType::Independent, ModelTrClass::SourceType::DependentPlus, ModelTrClass::SourceType::DependentMinus};
    ModelTrClass::StreamType strType[] = {ModelTrClass::StreamType::Gamma, ModelTrClass::StreamType::Normal, ModelTrClass::StreamType::Pareto, ModelTrClass::StreamType::Poisson, ModelTrClass::StreamType::Uniform};





    QJsonObject jsonAV;
    for (int aNum=1; aNum<=2400; aNum++)
    {
        for (int n=1; n<=v; n++)
        {
            double A =  static_cast<double>(aNum)/10.0;
            jsonAV.insert(QString("V%1A%2").arg(n).arg(A), trClass.trDistribution(0, A, n, 0).getJson());
        }
    }


    QJsonDocument doc(jsonTrClass);
    QString jsonStr(doc.toJson(QJsonDocument::JsonFormat::Compact));


    std::cout<<jsonStr.toStdString();
    file<<jsonStr.toStdString();





    std::cout<<"To jest test";
    return 0;
}
