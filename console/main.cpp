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
    QCoreApplication::setApplicationVersion("0.9.3");

    QCommandLineParser parser;
    parser.setApplicationDescription("Tool for investigating any stream processes");
    parser.addHelpOption();
    parser.addVersionOption();


    QCommandLineOption optSservUnits(QStringList() << "V" << "volume", QCoreApplication::translate("main", "set number of service units") , QCoreApplication::translate("main","V")   , "120");
    QCommandLineOption optAmin      ("Amin"                          , QCoreApplication::translate("main", "Minimum traffic offered")     , QCoreApplication::translate("main","Amin"), "1.0");
    QCommandLineOption optAmax      ("Amax"                          , QCoreApplication::translate("main", "Maximum traffic offered")     , QCoreApplication::translate("main","Amax"), "5.0");
    QCommandLineOption optAinc      ("Ainc"                          , QCoreApplication::translate("main", "Increment of offered traffic"), QCoreApplication::translate("main","Ainc"), "0.1");
    QCommandLineOption optEaDaMin   ("EaDaMin", QCoreApplication::translate("main", "Minimum ratio of square Expected value to Variance in arrival stream")     , QCoreApplication::translate("main","EaDaMin"), "3.0");
    QCommandLineOption optEaDaMax   ("EaDaMax", QCoreApplication::translate("main", "Maximum ratio of square Expected value to Variance in arrival stream")     , QCoreApplication::translate("main","EaDaMax"), "5.0");
    QCommandLineOption optEaDaInc   ("EaDaInc", QCoreApplication::translate("main", "Increment of ratio of square Expected value to Variance in arrival stream"), QCoreApplication::translate("main","EaDaInc"), "0.1");

    QCommandLineOption optEsDsMin   ("EsDsMin", QCoreApplication::translate("main", "Minimum ratio of square Expected value to Variance in service stream")     , QCoreApplication::translate("main","EsDsMin"), "3.0");
    QCommandLineOption optEsDsMax   ("EsDsMax", QCoreApplication::translate("main", "Maximum ratio of square Expected value to Variance in service stream")     , QCoreApplication::translate("main","EsDsMax"), "5.0");
    QCommandLineOption optEsDsInc   ("EsDsInc", QCoreApplication::translate("main", "Increment of ratio of square Expected value to Variance in service stream"), QCoreApplication::translate("main","EsDsInc"), "0.1");

    QCommandLineOption optOutput    ("Output",  QCoreApplication::translate("main", "Ouptut filename (without extension)")                                      , QCoreApplication::translate("main","Output"), "results");
    QCommandLineOption optNoOfSer   ("NoOfSer", QCoreApplication::translate("main", "Number of simulation series")                                              , QCoreApplication::translate("main","Output"), "12");
    QCommandLineOption optSimLen    ("NoOfEv",  QCoreApplication::translate("main", "No of events per state unit")                                              , QCoreApplication::translate("main","Output"), "100000000");


    QList<QCommandLineOption> optArrivalStreams;
    QList<QCommandLineOption> optServiceStreams;

    QVector<ModelTrClass::StreamType> supportedStreams = {ModelTrClass::StreamType::Poisson, ModelTrClass::StreamType::Uniform, ModelTrClass::StreamType::Gamma, ModelTrClass::StreamType::Pareto, ModelTrClass::StreamType::Normal};

    for (ModelTrClass::StreamType tmp: supportedStreams)
    {
        optArrivalStreams.append(QCommandLineOption("as"+ModelTrClass::streamTypeToString(tmp), "add arrival " + ModelTrClass::streamTypeToString(tmp) + " stream"));
        optServiceStreams.append(QCommandLineOption("ss"+ModelTrClass::streamTypeToString(tmp), "add service " + ModelTrClass::streamTypeToString(tmp) + " stream"));
    }

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
    parser.addOption(optOutput);
    parser.addOption(optNoOfSer);
    parser.addOption(optSimLen);

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

    long int noOfEventsPerUnit;
    int noOfSimSeries;

    std::string filename;

    QList<ModelTrClass::StreamType> arrivalStrType;
    QList<ModelTrClass::StreamType> serviceStrType;

    bool conversionResult = false;
    if (parser.isSet(optSservUnits))
        V = parser.value(optSservUnits).toDouble(&conversionResult);
    if (!conversionResult)
        V = 120;

    conversionResult = false;
    if (parser.isSet(optAmin))
        AMin = parser.value(optAmin).toDouble(&conversionResult);
    if (!conversionResult)
        AMin = 1;

    conversionResult = false;
    if (parser.isSet(optAmax))
        AMax = parser.value(optAmax).toDouble(&conversionResult);
    if (!conversionResult)
        AMax = 60;

    conversionResult = false;
    if (parser.isSet(optAinc))
        AIncrement = parser.value(optAinc).toDouble(&conversionResult);
    if (!conversionResult)
        AIncrement = 1;

    conversionResult = false;
    if (parser.isSet(optEaDaMin))
        EaDaMin = parser.value(optEaDaMin).toDouble(&conversionResult);
    if (!conversionResult)
        EaDaMin = 1;

    conversionResult = false;
    if (parser.isSet(optEaDaMax))
        EaDaMax = parser.value(optEaDaMax).toDouble(&conversionResult);
    if (!conversionResult)
        EaDaMax = 3;

    conversionResult = false;
    if (parser.isSet(optEaDaInc))
        EaDaIncrement = parser.value(optEaDaInc).toDouble(&conversionResult);
    if (!conversionResult)
        EaDaIncrement = 1;

    conversionResult = false;
    if (parser.isSet(optEsDsMin))
        EsDsMin = parser.value(optEsDsMin).toDouble(&conversionResult);
    if (!conversionResult)
        EsDsMin = 1;

    conversionResult = false;
    if (parser.isSet(optEsDsMax))
        EsDsMax = parser.value(optEsDsMax).toDouble(&conversionResult);
    if (!conversionResult)
        EsDsMax = 3;

    conversionResult = false;
    if (parser.isSet(optEaDaInc))
        EsDsIncrement = parser.value(optEsDsInc).toDouble(&conversionResult);
    if (!conversionResult)
        EsDsIncrement = 1;

    filename          = parser.value(optOutput).toStdString();
    noOfEventsPerUnit = parser.value(optSimLen).toLong(&conversionResult);
    noOfSimSeries     = parser.value(optNoOfSer).toInt(&conversionResult);

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

    std::ofstream fileJson;
    fileJson.open(filename + ".js");

    std::ofstream fileCvs;
    fileCvs.open(filename + ".csv");

    QJsonObject jsonTrClass;


    bool firstObject = true;
    fileJson<< "[";


    char cvsSeparator = '\t';
    fileCvs << "A" << cvsSeparator << "Astr_ID" << cvsSeparator << "Astr_Desc" << cvsSeparator << "EaDa" << cvsSeparator << "Sstr_ID" << cvsSeparator<< "Sstr_Desc" << cvsSeparator << "EsDs" << cvsSeparator;

    for (int v=1; v<=V; v++)
    {
        for (int n=0; n<=v; n++)
        {
            fileCvs<<"p("<<n<<")_"<<v<<cvsSeparator<<"a("<<n<<")_"<<v<<cvsSeparator<<"s("<<n<<")_"<<v;
            if (n < V)
                fileCvs<<cvsSeparator;
        }
    }
    fileCvs<<std::endl;

    int noOfLines = 0;

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
                        qDebug() << "Call arrival stream: "<< arrivalStr <<" (" << EaDa <<") call service stream: "<< serviceStr << " (" << EsDs << ")\t\tA " << A<<"";
                        if (!firstObject)
                            fileJson<<",";
                        firstObject = false;
                        fileJson<< "{";

                        fileJson<<"\"arrivalStr\": \""<<ModelTrClass::streamTypeToString(arrivalStr).toStdString()<<"\",";
                        fileJson<<"\"Ea2Da\":"<<EaDa<<",";
                        fileJson<<"\"serviceStr\": \""<<ModelTrClass::streamTypeToString(serviceStr).toStdString()<<"\",";
                        fileJson<<"\"Es2Ds\":"<<EsDs<<",";
                        fileJson<<"\"A\":"<<A;
                        fileJson<< ",\"dta\": [";

                        QString strNameArrival = ModelTrClass::streamTypeToString(arrivalStr);
                        QString strNameService = ModelTrClass::streamTypeToString(serviceStr);

                        fileCvs << A << cvsSeparator;
                        fileCvs << ((int) arrivalStr) << cvsSeparator << strNameArrival.constData() << cvsSeparator << EaDa << cvsSeparator;
                        fileCvs << ((int) serviceStr) << cvsSeparator << strNameService.constData() << cvsSeparator << EsDs << cvsSeparator;
                        for (int n=1; n<=V; n++)
                        {
                            TrClVector tmpTrDitrib = trClass.trDistribution(0, A, n, 0, noOfSimSeries, noOfEventsPerUnit);

                            if (n > 1)
                            {
                                fileJson<<",";
                                fileCvs<<cvsSeparator;
                            }

                            fileJson<< QJsonDocument(tmpTrDitrib.getJson()).toJson(QJsonDocument::JsonFormat::Compact).toStdString();
                            fileCvs<<tmpTrDitrib.getCvs(cvsSeparator).toStdString();
                        }
                        fileJson<< "]}";
                        fileCvs<<std::endl;
                        noOfLines++;
                    }
                    if (serviceStr == ModelTrClass::StreamType::Poisson)
                        break;
                }
                if (arrivalStr == ModelTrClass::StreamType::Poisson)
                    break;
            }
        }
    }
    fileJson<<"]";

    qDebug() <<"Total no of lines" << noOfLines;


    return 0;
}
