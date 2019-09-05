#include <cmath>
#include "results/resultsGnuplot.h"

void GnuplotScript::WriteDataAndScript(QString baseFileNameWithPath, const ModelCreator *system, Results::Settings *setting, Results::Type qosType)
{
    int logScale = 1;
    struct Results::ParametersSet parameters;

    QFile scriptFile(baseFileNameWithPath + ".gp");

    scriptFile.open(QFile::WriteOnly | QFile::Text);

    QTextStream scriptStream(&scriptFile);

    QTextStream *scriptStreamTrClass = new QTextStream[system->m()];
    QVector<QString> scriptStringTrClass(system->m());
    bool firstPlot = true;
    QVector<bool> firstPlotTrClass(system->m(), true);

    QList<ParameterType> noDescList;
    noDescList.append(ParameterType::TrafficClass);

    QString baseFileName = baseFileNameWithPath.mid(baseFileNameWithPath.lastIndexOf("/")+1);

    for(int i=0; i<system->m(); i++)
    {
        scriptStreamTrClass[i].setString(&scriptStringTrClass[i]);
        scriptStreamTrClass[i].setCodec("Windows-1250");
    }
    scriptStream.setCodec("Windows-1250");

    scriptStream<<"set terminal postscript enhanced 'Times' 6 color\r\n";
    scriptStream<<"set encoding cp1250\r\n";
    scriptStream<<"set lmargin 9\n";
    scriptStream<<"set xlabel \"a (traffic offered to a single BBU of the server)\"\n";
    scriptStream<<"set xtics 0.1\n";
    scriptStream<<"set xtics nomirror\n";
    scriptStream<< "set xrange ["<<setting->getXmin(*systemResults)<<":"<<setting->getXmax(*systemResults)<<"]\n";

    if (logScale == 1)
    {
        scriptStream<<"set logscale y\r\n";

        scriptStream<<"set yrange ["<<0.00000001<<":"<<1<<"]\n";
        scriptStream<<"set ytics (\\\n";
        scriptStream<<"0.00000001, 0.00000002, \"\" 0.00000003, \"\" 0.00000004, 0.00000005, \"\" 0.00000006, \"\" 0.00000008, \"\" 0.00000009, \\\n";
        scriptStream<<"0.0000001,  0.0000002,  \"\" 0.0000003,  \"\" 0.0000004,  0.0000005,  \"\" 0.0000006,  \"\" 0.0000008,  \"\" 0.0000009, \\\n";
        scriptStream<<"0.000001,   0.000002,   \"\" 0.000003,   \"\" 0.000004,   0.000005,   \"\" 0.000006,   \"\" 0.000008,   \"\" 0.000009, \\\n";
        scriptStream<<"0.00001,    0.00002,    \"\" 0.00003,    \"\" 0.00004,    0.00005,    \"\" 0.00006,    \"\" 0.00008,    \"\" 0.00009, \\\n";
        scriptStream<<"0.0001,     0.0002,     \"\" 0.0003,     \"\" 0.0004,     0.0005,     \"\" 0.0006,     \"\" 0.0008,     \"\" 0.0009, \\\n";
        scriptStream<<"0.001,      0.002,      \"\" 0.003,      \"\" 0.004,      0.005,      \"\" 0.006,      \"\" 0.008,      \"\" 0.009, \\\n";
        scriptStream<<"0.01,       0.02,       \"\" 0.03,       \"\" 0.04,       0.05,       \"\" 0.06,       \"\" 0.08,       \"\" 0.09, \\\n";
        scriptStream<<"0.1,        0.2,        \"\" 0.3,        \"\" 0.4,        0.5,        \"\" 0.6,        \"\" 0.8,        \"\" 0.9, \\\n";
        scriptStream<<"\"1\" 1)\n";
    }
    else
    {
        scriptStream<<"set yrange ["<<0<<":"<<1<<"]\n";
        scriptStream<<"unset ytics\n";
    }

    scriptStream<<"set colorsequence podo\r\n";
    scriptStream<<"set ytics nomirror\r\n";
    scriptStream<<"set border 3\r\n";
    scriptStream<<"set size 0.64, 0.60\r\n";
    scriptStream<<"set grid\r\n";
    scriptStream<<"set style line 1 lt 1 lw 0\r\n";
    scriptStream<<"set key box linestyle 1\r\n";
    scriptStream<<"set key right bottom\r\n";
    scriptStream<<"set ylabel \""<<setting->name<<"\" offset 1.5\r\n";

    scriptStream<<"set output \""<<baseFileName<<".eps\"\r\n";
    scriptStream<<"plot\\\r\n";
    for(int i=0; i<system->m(); i++)
    {
        scriptStreamTrClass[i]<<"set title \""<<system->getGnuplotDescription()<<" class "<<i+1<<"\"\r\n";
        scriptStreamTrClass[i]<<"set output \""<<baseFileName<<"trClass"<<i+1<<".eps\"\r\n";
        scriptStreamTrClass[i]<<"plot\\\r\n";
    }

    int i;

    int lc = 1;
    int dt = 1;
    int lw = 1;

    QVector<int> dtTrClass(system->m(), 1);
    QVector<int> lwTrClass(system->m(), 1);


    foreach(Investigator *algorithm, systemResults->getAvailableAlgorithms())
    {
        if (!algorithm->getQoS_Set().contains(qosType))
            continue;
        QVector<decimal> xVals = TypesAndSettings::getPlotsXorZ(*systemResults, setting->getFunctionalParameterX());
        QMap<ParametersSet, QVector<double>> yVals = TypesAndSettings::getPlotsValues(*systemResults, qosType, setting->getFunctionalParameterX(), algorithm);

        QString dataFileNameWithPath = baseFileNameWithPath + algorithm->shortName() + ".dat";
        QString dataFileName = dataFileNameWithPath.mid(dataFileNameWithPath.lastIndexOf("/")+1);

        QFile dataFile(dataFileNameWithPath);
        dataFile.open(QFile::WriteOnly | QFile::Text);
        QTextStream dataStream(&dataFile);



        dataStream<<"#"<<TypesAndSettings::parameterToString(setting->getFunctionalParameterX());
        int colNo = 2;

        dt = 1;
        lw = 1;
        dtTrClass.fill(1);
        lwTrClass.fill(1);

        foreach(ParametersSet param, yVals.keys())
        {
            dataStream<<"\t"<<setting->getParameterDescription(param, system->getConstSyst());

            if (algorithm->hasConfIntervall())
            {
                dataStream<<"\t+-";

                if (firstPlot)
                {
                    firstPlot = false;
                    scriptStream<<"    ";
                }
                else
                    scriptStream<<"  , ";

                scriptStream<<"\""<<dataFileName<<"\" using 1:"<<colNo<<" with lines dt "<<dt<<" lw "<<lw<<" lc "<<lc<<" title '"<<algorithm->shortName()<<" "<<setting->getParameterDescription(param, system->getConstSyst())<<"' \\\r\n";
                scriptStream<<"  , \""<<dataFileName<<"\" using 1:"<<colNo<<":"<<colNo+1<<" with yerrorbars pt 1 lc "<<lc<<" notitle \\\r\n";


                if (param.classIndex >= 0)
                {
                    if (firstPlotTrClass[param.classIndex])
                    {
                        firstPlotTrClass[param.classIndex] = false;
                        scriptStreamTrClass[param.classIndex]<<"    ";
                    }
                    else
                        scriptStreamTrClass[param.classIndex]<<"  , ";

                    scriptStreamTrClass[param.classIndex]
                           <<"\""<<dataFileName<<"\" using 1:"<<colNo<<" with lines dt "<<dtTrClass[param.classIndex]
                           <<" lw "<<lwTrClass[param.classIndex]<<" lc "<<lc<<" title '"
                           <<algorithm->shortName()<<" "<<setting->getParameterDescription(param, system->getConstSyst(), noDescList)<<"' \\\r\n";
                    scriptStreamTrClass[param.classIndex]
                           <<"  , \""<<dataFileName<<"\" using 1:"<<colNo<<":"<<colNo+1<<" with yerrorbars pt 1 lc "<<lc<<" notitle \\\r\n";
                }

                colNo++;
            }
            else
            {
                if (firstPlot)
                {
                    firstPlot = false;
                    scriptStream<<"    ";
                }
                else
                    scriptStream<<"  , ";
                scriptStream<<"\""<<dataFileName<<"\" using 1:"<<colNo<<" with lines dt "<<dt<<" lw "<<lw<<" lc "<<lc<<" title '"<<algorithm->shortName()<<" "
                           <<setting->getParameterDescription(param, system->getConstSyst())<<"' \\\r\n";

                if (param.classIndex >= 0)
                {
                    if (firstPlotTrClass[param.classIndex])
                    {
                        firstPlotTrClass[param.classIndex] = false;
                        scriptStreamTrClass[param.classIndex]<<"    ";
                    }
                    else
                        scriptStreamTrClass[param.classIndex]<<"  , ";
                    scriptStreamTrClass[param.classIndex]
                           <<"\""<<dataFileName<<"\" using 1:"<<colNo<<" with lines dt "
                           <<dtTrClass[param.classIndex]<<" lw "<<lwTrClass[param.classIndex]<<" lc "<<lc<<" title '"<<algorithm->shortName()<<" "
                           <<setting->getParameterDescription(param, system->getConstSyst(), noDescList)<<"' \\\r\n";
                }
            }
            colNo++;

            dt++;
            if (dt == 6)
            {
                dt = 1;
                lw++;
            }

            if (param.classIndex>=0)
            {
                dtTrClass[param.classIndex]++;
                if (dtTrClass[param.classIndex] == 6)
                {
                    dtTrClass[param.classIndex] = 1;
                    lwTrClass[param.classIndex]++;
                }
            }
        }
        dataStream<<"\n";

        for (i=0; i < xVals.length(); i++)
        {
            dataStream<< static_cast<double>(xVals[i]);

            foreach(ParametersSet param, yVals.keys())
            {
                if (std::isnan(yVals[param][i]))
                    dataStream<<"\t0";
                else
                    dataStream<<"\t"<<yVals[param][i];

                if (algorithm->hasConfIntervall())
                    dataStream<<"\t0";
            }
            dataStream<<"\n";

        }
        dataFile.close();
        lc++;
    }
    scriptStream<<"\r\n";

    if (setting->getAdditionalParameter1() == ParameterType::TrafficClass || setting->getAdditionalParameter2() == ParameterType::TrafficClass)
    {
        for (i=0; i<system->m(); i++)
        {
            scriptStream<<scriptStringTrClass[i]<<"\r\n";
        }
    }

    delete []scriptStreamTrClass;
    scriptFile.close();
}

GnuplotScript::GnuplotScript(): systemResults(nullptr)
{
    plotCi = true;
    plotWithColors = true;
    gp = new Gnuplot("tee plot.gp | gnuplot -persist");
}

