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

    ModelTrClass::StreamType arrivalStrType[] = {ModelTrClass::StreamType::Gamma, ModelTrClass::StreamType::Normal};//, ModelTrClass::StreamType::Pareto, ModelTrClass::StreamType::Poisson, ModelTrClass::StreamType::Uniform};
    ModelTrClass::StreamType serviceStrType[] = {ModelTrClass::StreamType::Gamma, ModelTrClass::StreamType::Normal};//, ModelTrClass::StreamType::Pareto, ModelTrClass::StreamType::Poisson, ModelTrClass::StreamType::Uniform};






    bool firstObject = true;
    file<< "[";
    for (ModelTrClass::StreamType arrivalStr : arrivalStrType)
    {
        for (double EaDa =3; EaDa <=3; EaDa++)
        {
            for (ModelTrClass::StreamType serviceStr : serviceStrType)
            {
                for (double EsDs =3; EsDs <=3; EsDs++)
                {
                    for (int aNum=1; aNum<=4; aNum++)
                    {
                        double A =  static_cast<double>(aNum)/10.0;

                        if (!firstObject)
                            file<<",";
                        firstObject = false;
                        file<< "{";

                        file<<"\"arrivalStr\": \""<<ModelTrClass::streamTypeToString(arrivalStr).toStdString()<<"\",";
                        file<<"\"Ea2Da\":"<<EaDa<<",";
                        file<<"\"serviceStr\": \""<<ModelTrClass::streamTypeToString(serviceStr).toStdString()<<"\",";
                        file<<"\"Es2Ds\":"<<EsDs<<",";
                        file<<"\"A\":"<<A;
                        file<< ",\"dta\": [";

                        for (int n=0; n<=2; n++)
                        {
                            if (n > 0)
                                file<<",";
                            file<< QJsonDocument(trClass.trDistribution(0, A, n, 0).getJson()).toJson(QJsonDocument::JsonFormat::Compact).toStdString();
                        }
                        file<< "]}";
                    }
                }
            }
        }
    }
    file<<"]";

    return 0;
}
