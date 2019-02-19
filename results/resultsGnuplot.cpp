#include "results/resultsGnuplot.h"

void GnuplotScript::WriteDataAndScript(QString baseFileName, const ModelSyst *system, Results::Settings *setting, Results::Type qosType)
{
    int logScale = 1;
    struct Results::ParametersSet parameters;

    QFile scriptFile(baseFileName + ".gp");

    scriptFile.open(QFile::WriteOnly | QFile::Text);

    QTextStream scriptStream(&scriptFile);

    scriptStream<<"set terminal postscript enhanced font 'Times,6' persist\r\n";
    scriptStream<<"set encoding iso_8859_2\r\n";
    scriptStream<<"set lmargin 9\n";
    scriptStream<<"set xlabel \"a (traffic offered to a single BBU of the server)\"\n";
    scriptStream<<"set xtics 0.1\n";
    scriptStream<<"set xtics nomirror\n";
    scriptStream<< "set xrange ["<<setting->getXmin(*systemResults)<<":"<<setting->getXmax(*systemResults)<<"]\n";

    if (logScale == 1)
    {
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

    scriptStream<<"set ytics nomirror\n";
    scriptStream<<"set border 3\n";
    scriptStream<<"set size 0.32, 0.30\n";
    scriptStream<<"set grid\n";
    scriptStream<<"set style line 1 lt 1 lw 0\n";
    scriptStream<<"set key box linestyle 1\n";
    scriptStream<<"set key right bottom\n";
    scriptStream<<"set ylabel \""<<setting->name<<"\" offset 1.5\n";


    scriptStream<<"plot\\\r\n";
    int i;
    foreach(Investigator *algorithm, systemResults->getAvailableAlgorithms())
    {
        if (!algorithm->getQoS_Set().contains(qosType))
            continue;
        QVector<decimal> xVals = TypesAndSettings::getPlotsX(*systemResults, setting->getFunctionalParameter());
        QMap<ParametersSet, QVector<double>> yVals = TypesAndSettings::getPlotsY(*systemResults, qosType, setting->getFunctionalParameter(), algorithm);

        QString dataFileName = baseFileName + algorithm->shortName() + ".dat";

        QFile dataFile(dataFileName);
        dataFile.open(QFile::WriteOnly | QFile::Text);
        QTextStream dataStream(&dataFile);



        dataStream<<"#"<<TypesAndSettings::parameterToString(setting->getFunctionalParameter());
        int colNo = 2;
        foreach(ParametersSet param, yVals.keys())
        {
            dataStream<<"\t"<<setting->getParameterDescription(param, system);

            if (algorithm->hasConfIntervall())
            {
                dataStream<<"\t+-";

                scriptStream<<"\t"<<dataFileName<<" 1:"<<colNo<<" with lines label '"<<setting->getParameterDescription(param, system)<<"'\\\r\n";
                colNo++;
            }
            else
            {
                scriptStream<<"\t"<<dataFileName<<" 1:"<<colNo<<":"<<colNo+1<<" with errors label '"<<setting->getParameterDescription(param, system)<<"'\\\r\n";
            }
            colNo++;

        }
        dataStream<<"\n";


        for (i=0; i < xVals.length(); i++)
        {
            dataStream<< static_cast<double>(xVals[i]);

            foreach(ParametersSet param, yVals.keys())
            {
                if (isnan(yVals[param][i]))
                    dataStream<<"\t0";
                else
                    dataStream<<"\t"<<yVals[param][i];

                if (algorithm->hasConfIntervall())
                    dataStream<<"\t0";
            }
            dataStream<<"\n";
        }
        dataFile.close();
    }
    scriptStream<<"\r\n";

    scriptFile.close();
}

GnuplotScript::GnuplotScript(): systemResults(nullptr)
{
    plotCi = true;
    plotWithColors = true;
    gp = new Gnuplot("tee plot.gp | gnuplot -persist");
}

