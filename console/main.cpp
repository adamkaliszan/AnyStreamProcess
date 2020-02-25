#include<QDebug>
#include<QVector>
#include<QList>
#include <iostream>
#include <fstream>

#include <QJsonDocument>
#include <QCommandLineParser>

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

int main(int argc, char *argv[]){
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("AnyStream generator");
    QCoreApplication::setApplicationVersion("0.61");

    QCommandLineParser parser;
    parser.setApplicationDescription("Tool for investigating any stream processes");
    parser.addHelpOption();
    parser.addVersionOption();


    QCommandLineOption optSservUnits(QStringList() << "V" << "volume"
                                    , QCoreApplication::translate("main", "set number of service units")
                                    , QCoreApplication::translate("main", "V")
                                    , "120");

    QCommandLineOption optAmin("Amin", QCoreApplication::translate("main", "Minimum traffic offered")     , QCoreApplication::translate("main","Amin"), "1.0");
    QCommandLineOption optAmax("Amax", QCoreApplication::translate("main", "Maximum traffic offered")     , QCoreApplication::translate("main","Amax"), "5.0");
    QCommandLineOption optAinc("Ainc", QCoreApplication::translate("main", "Increment of offered traffic"), QCoreApplication::translate("main","Ainc"), "0.1");


    QList<QCommandLineOption> optArrivalStreams;
    QList<QCommandLineOption> optServiceStreams;

    QVector<ModelTrClass::StreamType> supportedStreams = {ModelTrClass::StreamType::Poisson, ModelTrClass::StreamType::Uniform, ModelTrClass::StreamType::Gamma, ModelTrClass::StreamType::Pareto, ModelTrClass::StreamType::Normal};

    for (ModelTrClass::StreamType tmp: supportedStreams)
    {
        optArrivalStreams.append(QCommandLineOption("as"+ModelTrClass::streamTypeToString(tmp), "add arrival " + ModelTrClass::streamTypeToString(tmp) + " stream"));
        optServiceStreams.append(QCommandLineOption("ss"+ModelTrClass::streamTypeToString(tmp), "add service " + ModelTrClass::streamTypeToString(tmp) + " stream"));
    }


    QCommandLineOption optEaDaMin("EaDaMin", QCoreApplication::translate("main", "Minimum ratio of square Expected value to Variance in arrival stream")     , QCoreApplication::translate("main","EaDaMin"), "3.0");
    QCommandLineOption optEaDaMax("EaDaMax", QCoreApplication::translate("main", "Maximum ratio of square Expected value to Variance in arrival stream")     , QCoreApplication::translate("main","EaDaMax"), "5.0");
    QCommandLineOption optEaDaInc("EaDaInc", QCoreApplication::translate("main", "Increment of ratio of square Expected value to Variance in arrival stream"), QCoreApplication::translate("main","EaDaInc"), "0.1");

    QCommandLineOption optEsDsMin("EsDsMin", QCoreApplication::translate("main", "Minimum ratio of square Expected value to Variance in service stream")     , QCoreApplication::translate("main","EsDsMin"), "3.0");
    QCommandLineOption optEsDsMax("EsDsMax", QCoreApplication::translate("main", "Maximum ratio of square Expected value to Variance in service stream")     , QCoreApplication::translate("main","EsDsMax"), "5.0");
    QCommandLineOption optEsDsInc("EsDsInc", QCoreApplication::translate("main", "Increment of ratio of square Expected value to Variance in service stream"), QCoreApplication::translate("main","EsDsInc"), "0.1");

    parser.addOption(optSservUnits);
    parser.addOption(optAmin);
    parser.addOption(optAmax);

    parser.addOption(optEaDaMin);
    parser.addOption(optEaDaMax);
    parser.addOption(optEaDaInc);

    parser.addOption(optEsDsMin);
    parser.addOption(optEsDsMax);
    parser.addOption(optEsDsInc);

    parser.addOption(optAinc);
    parser.addOptions(optArrivalStreams);
    parser.addOptions(optServiceStreams);

    parser.process(app);

    int V = 120;

    double AMin = 1;
    double AMax = 30;
    double AIncrement = 1;

    double EaDaMin = 3;
    double EaDaMax = 3;
    double EaDaIncrement = 0.01;

    double EsDsMin = 3;
    double EsDsMax = 3;
    double EsDsIncrement = 0.01;


    QList<ModelTrClass::StreamType> arrivalStrType;
    QList<ModelTrClass::StreamType> serviceStrType;

    bool conversionResult = false;
    if (parser.isSet(optSservUnits))
        V = parser.value(optSservUnits).toUInt(&conversionResult);
    if (!conversionResult)
        V = 120;

    conversionResult = false;
    if (parser.isSet(optAmin))
        AMin = parser.value(optAmin).toUInt(&conversionResult);
    if (!conversionResult)
        AMin = 1;

    conversionResult = false;
    if (parser.isSet(optAmax))
        AMax = parser.value(optAmax).toUInt(&conversionResult);
    if (!conversionResult)
        AMax = 60;

    conversionResult = false;
    if (parser.isSet(optAinc))
        AIncrement = parser.value(optAinc).toUInt(&conversionResult);
    if (!conversionResult)
        AIncrement = 1;

    conversionResult = false;
    if (parser.isSet(optEaDaMin))
        EaDaMin = parser.value(optEaDaMin).toUInt(&conversionResult);
    if (!conversionResult)
        EaDaMin = 3;

    conversionResult = false;
    if (parser.isSet(optEaDaMax))
        EaDaMax = parser.value(optEaDaMax).toUInt(&conversionResult);
    if (!conversionResult)
        EaDaMax = 5;

    conversionResult = false;
    if (parser.isSet(optEaDaInc))
        EaDaIncrement = parser.value(optEaDaInc).toUInt(&conversionResult);
    if (!conversionResult)
        EaDaIncrement = 1;

    conversionResult = false;
    if (parser.isSet(optEsDsMin))
        EsDsMin = parser.value(optEsDsMin).toUInt(&conversionResult);
    if (!conversionResult)
        EsDsMin = 3;

    conversionResult = false;
    if (parser.isSet(optEsDsMax))
        EsDsMax = parser.value(optEsDsMax).toUInt(&conversionResult);
    if (!conversionResult)
        EsDsMax = 5;

    conversionResult = false;
    if (parser.isSet(optEaDaInc))
        EsDsIncrement = parser.value(optEsDsInc).toUInt(&conversionResult);
    if (!conversionResult)
        EsDsIncrement = 1;


    for (int i=0; i<supportedStreams.length(); i++)
    {
        if (parser.isSet(optArrivalStreams[i]))
            arrivalStrType.append(supportedStreams[i]);
    }
    if (arrivalStrType.length() == 0)
        arrivalStrType.append(ModelTrClass::StreamType::Poisson);


    for (int i=0; i<supportedStreams.length(); i++)
    {
        if (parser.isSet(optServiceStreams[i]))
            serviceStrType.append(supportedStreams[i]);
    }
    if (serviceStrType.length() == 0)
        serviceStrType.append(ModelTrClass::StreamType::Poisson);


    qDebug().resetFormat();
    qDebug()<<"V = "<<V<<",\tA min " << AMin << ",\tA max " << AMax <<",\tA inc " << AIncrement << ",\tu = 1";
    qDebug()<<"Arrival E2D min = "<<EaDaMin <<", E2D max " << EaDaMax <<", EaDa inc " << EaDaIncrement << "Streams:";
    for (auto tmp: arrivalStrType)
        qDebug()<<" " << ModelTrClass::streamTypeToString(tmp);

    qDebug()<<"Service E2D min = "<<EsDsMin <<", E2D max " << EsDsMax <<", EaDa inc " << EsDsIncrement << "Streams:";
    for (auto tmp: serviceStrType)
        qDebug()<<" " << ModelTrClass::streamTypeToString(tmp);



    ModelSystem model = prepareSystem();
    ModelTrClass trClass;

    trClass.setT(1);
    trClass.setMu(1);

    trClass.setNewCallStrType(ModelTrClass::StreamType::Poisson, ModelTrClass::SourceType::Independent);
    trClass.setCallServStrType(ModelTrClass::StreamType::Poisson);

    std::ofstream file;
    file.open("results.js");

    QJsonObject jsonTrClass;


    bool firstObject = true;
    file<< "[";
    for (ModelTrClass::StreamType arrivalStr : arrivalStrType)
    {
        trClass.setNewCallStrType(arrivalStr, ModelTrClass::SourceType::Independent);
        for (ModelTrClass::StreamType serviceStr : serviceStrType)
        {
            qDebug()<<"Arrival stream " << ModelTrClass::streamTypeToString(arrivalStr)<<", Service stream " << ModelTrClass::streamTypeToString(serviceStr);

            trClass.setCallServStrType(serviceStr);
            for (double EaDa =EaDaMin; EaDa <=EaDaMax; EaDa+= EaDaIncrement)
            {
                trClass.setIncommingExPerDx((arrivalStr == ModelTrClass::StreamType::Poisson) ? 1 : EaDa);

                for (double EsDs =EsDsMin; EsDs <=EsDsMax; EsDs+= EsDsIncrement)
                {
                    qDebug()<<"\tArrival stream E2D " << EaDa << ", Service stream E2D " << EsDs << "A <" << AMin << " .. " << AMax <<">";

                    trClass.setServiceExPerDx((serviceStr == ModelTrClass::StreamType::Poisson) ? 1 :EsDs);
                    for (double A = AMin; A <= AMax; A+= AIncrement)
                    {
                        qDebug() << "\t\tA " << A;

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

                        for (int n=0; n<=V; n++)
                        {
                            if (n > 0)
                                file<<",";
                            file<< QJsonDocument(trClass.trDistribution(0, A, n, 0).getJson()).toJson(QJsonDocument::JsonFormat::Compact).toStdString();
                        }
                        file<< "]}";
                    }
                    if (serviceStr == ModelTrClass::StreamType::Poisson)
                        break;
                }
                if (arrivalStr == ModelTrClass::StreamType::Poisson)
                    break;
            }
        }
    }
    file<<"]";

    return 0;
}
