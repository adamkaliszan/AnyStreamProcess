#include "results/resultsGnuplot.h"


void GnuplotScript::WriteScript(QTextStream &script, QString dataFileName, QString graphFileName, Results::Type qosParam, double minY, double maxY, bool isLogScale)
{
    double logMinY = 0.0000001;
    double logMaxY = 1;

    while (logMinY < minY)
        logMinY *=10;
    logMinY /=10;

    minY = floor(minY);
    maxY = ceil(maxY);

    if (graphFileName.length()>0)
        script<<"set terminal postscript enhanced \"Times\" 6\n";

    script<<"set encoding cp1250\n";
    script<<"set xrange ["<<systemResults->getMinAperAU()<<":"<<systemResults->getMaxAperAU()<<"]\n";

    if (isLogScale)
        script<<"set yrange ["<<logMinY<<":"<<logMaxY<<"]\n";
    else
        script<<"set yrange ["<<minY<<":"<<maxY<<"]\n";

    script<<"set lmargin 9\n";
    script<<"set xlabel \"a (traffic offered to a single BBU of the primary group)\"\n";
    script<<"set xtics 0.1\n";
    script<<"set xtics nomirror\n";

    if (isLogScale)
    {
        script<<"set ytics (\\\n";
        script<<"0.00000001, 0.00000002, \"\" 0.00000003, \"\" 0.00000004, 0.00000005, \"\" 0.00000006, \"\" 0.00000008, \"\" 0.00000009, \\\n";
        script<<"0.0000001,  0.0000002,  \"\" 0.0000003,  \"\" 0.0000004,  0.0000005,  \"\" 0.0000006,  \"\" 0.0000008,  \"\" 0.0000009, \\\n";
        script<<"0.000001,   0.000002,   \"\" 0.000003,   \"\" 0.000004,   0.000005,   \"\" 0.000006,   \"\" 0.000008,   \"\" 0.000009, \\\n";
        script<<"0.00001,    0.00002,    \"\" 0.00003,    \"\" 0.00004,    0.00005,    \"\" 0.00006,    \"\" 0.00008,    \"\" 0.00009, \\\n";
        script<<"0.0001,     0.0002,     \"\" 0.0003,     \"\" 0.0004,     0.0005,     \"\" 0.0006,     \"\" 0.0008,     \"\" 0.0009, \\\n";
        script<<"0.001,      0.002,      \"\" 0.003,      \"\" 0.004,      0.005,      \"\" 0.006,      \"\" 0.008,      \"\" 0.009, \\\n";
        script<<"0.01,       0.02,       \"\" 0.03,       \"\" 0.04,       0.05,       \"\" 0.06,       \"\" 0.08,       \"\" 0.09, \\\n";
        script<<"0.1,        0.2,        \"\" 0.3,        \"\" 0.4,        0.5,        \"\" 0.6,        \"\" 0.8,        \"\" 0.9, \\\n";
        script<<"\"1\" 1)\n";
    }
    else
    {
        script<<"unset ytics\n";
    }

    script<<"set "<<Results::TypesAndSettings::typeToString(qosParam)<<" y\n";

    script<<"set ytics nomirror\n";
    script<<"set border 3\n";
    script<<"set size 0.32, 0.30\n";
    script<<"set grid\n";
    script<<"set style line 1 lt 1 lw 0\n";
    script<<"set key box linestyle 1\n";


#if 0
    switch (QoS_MainParam)
    {
    case resultsType::y:
    case resultsType::yQ:
    case resultsType::ytQ:
    case resultsType::tWait:
    case resultsType::tQeue:
    case resultsType::tServer:
    case resultsType::tService:
        script<<"set key left top\n";
        break;
    default:
        script<<"set key right bottom\n";
        break;
    }

    script<<"set ylabel \""<<ResultsTmp::resType2string(QoS_MainParam)<<"\" offset 1.5\n";


    //TODO add system (modelSystem) to Results class
    AlgorithmResults *algRes = results->getAlgResult(results->algorithms().at(0));
    int m = algRes->system->m();

    int colIdx = 2;


    if (graphFileName.length()>0)
        script<<"\nset output \""<<graphFileName<<".eps\"\n";

    script<<"plot \\\n";

    int pointIdx = 1;

    bool first = true;
    for (int classIdx=0; classIdx<m; classIdx++)
    {
        const ModelTrClass *trClass = algRes->system->getClass(classIdx);

        int lineIdx = 1;

        foreach (const Investigator *alg, results->algorithms())
        {
            if (alg->hasConfIntervall() == false)
            {
                if (ResultsTmp::isTrClassDependent(QoS_MainParam))
                {
                    if (!first)
                        script<<", \\\n";                    
                    first = false;
                    script<<"    \""<<dataFileName<<"\" using 1:"<<colIdx<<" title ' "<<alg->shortName()<<": "<<*trClass<<"' with lines lt "<<lineIdx;
                }
                else
                {
                    if (classIdx > 0)
                        continue;

                    if (!first)
                        script<<", \\\n";
                    first = false;
                    script<<"    \""<<dataFileName<<"\" using 1:"<<colIdx<<" title ' "<<alg->shortName()<<"' with lines lt "<<lineIdx;
                }

                lineIdx++;
                colIdx++;
            }
            else
            {
                if (ResultsTmp::isTrClassDependent(QoS_MainParam))
                {
                    if (!first)
                        script<<", \\\n";
                    first = false;
                    script<<"    \""<<dataFileName<<"\" using 1:"<<colIdx<<":"<<colIdx+1<<" notitle with yerrorbars lt 1 pt "<<pointIdx<<", \\\n";
                    script<<"    \""<<dataFileName<<"\" using 1:"<<colIdx<<" title ' "<<alg->shortName()<<": "<<*trClass<<"' with points pt "<<pointIdx;
                }
                else
                {
                    if (classIdx > 0)
                        continue;

                    if (!first)
                        script<<", \\\n";
                    first = false;
                    script<<"    \""<<dataFileName<<"\" using 1:"<<colIdx<<":"<<colIdx+1<<" notitle with yerrorbars lt 1 pt "<<pointIdx<<", \\\n";
                    script<<"    \""<<dataFileName<<"\" using 1:"<<colIdx<<" title ' "<<alg->shortName()<<"' with points pt "<<pointIdx;
                }
                pointIdx++;
                colIdx += 2;
            }
        }
    }
#endif
    script<<"\n";
    script<<"\n";
}

void GnuplotScript::WriteScript(QString scriptFileName, QString dataFileName, QString graphFileName, double minY, double maxY, Results::Type QoS_MainParam)
{
    QFile file(scriptFileName);
    file.open(QFile::WriteOnly | QFile::Text);
    QTextStream script(&file);
    //WriteScript(script, dataFileName, graphFileName, minY, maxY, QoS_MainParam);
    file.close();
}

void GnuplotScript::WriteDataAndScript(QString baseFileName, const ModelSyst *system, Results::Settings *setting, Results::Type qosType)
{
    struct Results::ParametersSet parameters;
    QFile dataFile(baseFileName + ".dat");
    QFile scriptFile(baseFileName + ".gp");

    dataFile.open(QFile::WriteOnly | QFile::Text);
    scriptFile.open(QFile::WriteOnly | QFile::Text);

    QTextStream dataStream(&dataFile);
    QTextStream scriptStream(&scriptFile);


    QPair<double, double> minMax(DBL_MAX, -DBL_MAX);


    int i;
    foreach(Investigator *algorithm, systemResults->getAvailableAlgorithms())
    {
        QVariant tmpVariant;
        QList<QVariant> listPar1;
        QList<QVariant> listPar2;
        QString name1;
        QString name2;

        Settings::fillListWithParameters(listPar1, setting->additionalParameter1, system, systemResults->getAvailableAperAU());
        Settings::fillListWithParameters(listPar2, setting->additionalParameter2, system, systemResults->getAvailableAperAU());

        QLineSeries singlePlot;

        if (listPar1.length()>0)
        {
            foreach(QVariant tmpVariant, listPar1)
            {
                name1 = Settings::updateParameters(parameters, tmpVariant, setting->additionalParameter1, system, systemResults);
                if (listPar2.length()>0)
                {
                    foreach(QVariant tmpVariant2, listPar2)
                    {
                        name2 = Settings::updateParameters(parameters, tmpVariant, setting->additionalParameter1, system, systemResults);
                        setting->getSinglePlot(&singlePlot, minMax, *systemResults, algorithm, parameters);
                    }
                }
                else
                {
                    setting->getSinglePlot(&singlePlot, minMax, *systemResults, algorithm, parameters);
                }
            }
        }
        else
        {
            setting->getSinglePlot(&singlePlot, minMax, *systemResults, algorithm, parameters);
        }
    }


//    foreach(decimal a, systemResults->getAvailableAperAU())
//    {
//        foreach(Investigator *algorithm,  algorithms)
//        {
//            RInvestigator *res = systemResults->getInvestigationResults(algorithm, a);

//            (*res)->read()
//        }

//    }

    int clColumns = 0;

//   systemResults->getInvestigationResults()

#if 0
    AlgorithmResults *algRes = NULL;

    int noOfQoS_Col = 0;

    scriptData<<"#\t"<<ResultsTmp::resType2string(type);
    foreach(const Investigator *alg, results->algorithms())
    {
        algRes = results->getAlgResult(alg);
        int m = algRes->system->m();
        for (int i=0; i<m; i++)
        {
            if (alg->hasConfIntervall())
            {
                noOfQoS_Col +=2;
                scriptData<<"\t\t";
            }
            else
            {
                noOfQoS_Col +=1;
                scriptData<<"\t";
            }
        }
        clColumns += (alg->hasConfIntervall()) ? 2 : 1;
    }
    if (algRes == nullptr)
        return;


    scriptData<<"\n#\t";                   //# Cl1 .. Cln
    int m = algRes->system->m();
    for (int i=0; i<m; i++)
    {
        scriptData<<*(algRes->system->getClass(i));
        for (int sep=0; sep<clColumns; sep++)
            scriptData<<"\t";
    }

    scriptData<<"\n#\t";                  //# alg1 .. algn

    for (int i=0; i<m; i++)
    {
        foreach(const Investigator *alg, results->algorithms())
        {
            if (alg->hasConfIntervall())
                scriptData<<alg->shortName()<<"\t+/-\t";
            else
                scriptData<<alg->shortName()<<"\t";
        }
    }
    scriptData<<"\n";

    for (int x=0; x< results->numberOfXsamples(); x++)
    {
        double tmp_a = results->getXsample(x);
        scriptData<<QString::number(tmp_a, 'f', 5);

        if (algRes == NULL)
            continue;
        int m = algRes->system->m();
        for (int i=0; i<m; i++)
        {
            const ModelTrClass *tmpTrClass = algRes->system->getClass(i);

            foreach(const Investigator *alg, results->algorithms())
            {
                AlgorithmResults *algRes = results->getAlgResult(alg);
                double value, ci;

                bool found = algRes->getVal(value, ci,type, tmpTrClass, tmp_a);
                if (found)
                {
                    minY = qMin(minY, value);
                    maxY = qMax(maxY, value);

                    scriptData<<"\t"<<QString::number(value, 'e', 5);

                    if (alg->hasConfIntervall())
                        scriptData<<"\t"<<QString::number(ci, 'e', 5);;
                }
                else
                {
                    scriptData<<"\t";
                    if (alg->hasConfIntervall())
                        scriptData<<"\t";
                }
            }
        }
        scriptData<<"\n";
    }
#endif


    dataFile.close();
    scriptFile.close();
}

void GnuplotScript::Show(Results::Type QoS_par, QList<ModelTrClass*> selClasses, bool useColors)
{
    (void) useColors;
    double minY =  1.7976931348623158e+308;
    double maxY = -1.7976931348623158e+308;

    double logMinY = 0.0000001;
    double logMaxY = 1;

    std::vector<boost::tuple<double, double> > values;
    std::vector<boost::tuple<double, double, double> > errors;


    std::string plotStr;

    bool firstGraph = true;
    bool firstTrClass = true;

    int pointIdx = 0;

    foreach (ModelTrClass *tmpClass, selClasses)
    {
        pointIdx++;

        int lineIdx   = 0;

        int colourIdx = 0;

        int pointSize = 1;
#if 0
        foreach (const Investigator *tmpAlg, results->algorithms())
        {
           if (!tmpAlg->isSelected)
                continue;

            AlgorithmResults *algRes = results->getAlgResult(tmpAlg);
            if (algRes == nullptr)
                continue;

            double value, ci;
            bool found = false;

            foreach(double a, *(algRes->a))
            {
                if (algRes->getVal(value, ci,  QoS_par, tmpClass, a))
                {
                    minY = qMin(minY, value);
                    maxY = qMax(maxY, value);
                    found = true;
                }
            }
            if (found)
            {
                if (!ResultsTmp::isTrClassDependent(QoS_par) && !firstTrClass)
                    continue;

                if (firstGraph)
                    plotStr+="plot";
                else
                    plotStr+=", \\\n";


                std::ostringstream lcStr;  lcStr<<colourIdx;
                std::ostringstream ptStr;  ptStr<<pointIdx;
                std::ostringstream ltStr;  ltStr<<lineIdx;
                std::ostringstream psStr;  psStr<<pointSize;

                firstGraph = false;
                if (tmpAlg->hasConfIntervall())
                {
                    if (plotCi)
                    {
                        plotStr+=" '-' with errorbars notitle pt 0 ps 1 lt 1 lw 0.5 lc ";
                        if (plotWithColors)
                            plotStr.append(lcStr.str());
                        else
                            plotStr+="-1";
                        plotStr+=", \\\n";
                    }

                    plotStr+=" '-' with points ";
                    if (plotWithColors)
                    {
                        plotStr.append("lc ");
                        plotStr.append(lcStr.str());
                        plotStr.append(" pt ");
                        plotStr.append(ptStr.str());
                        plotStr.append(" ps 1");
                    }
                    else
                    {
                        plotStr+=" lc -1 pt ";
                        plotStr.append(ptStr.str());
                        plotStr+=" ps ";
                        plotStr.append(psStr.str());
                    }
                    plotStr+=" title '";
                    plotStr+=tmpAlg->shortName().toStdString();
                    if (ResultsTmp::isTrClassDependent(QoS_par))
                    {
                        plotStr+=":";
                        plotStr+=tmpClass->shortName().toStdString();
                    }
                    plotStr+="'";

                    if (plotWithColors)
                        colourIdx++;
                    else
                        pointSize++;
                }
                else
                {
                    plotStr.append(" '-' with lines ");
                    if (plotWithColors)
                    {
                        plotStr.append("lc ");
                        plotStr.append(lcStr.str());
                        plotStr.append(" lt 1");
                    }
                    else
                    {
                        plotStr.append("lc -1 lt ");
                        plotStr.append(ltStr.str());
                    }

                    plotStr.append(" title '");
                    plotStr+=tmpAlg->shortName().toStdString();
                    if (ResultsTmp::isTrClassDependent(QoS_par))
                    {
                        plotStr+=":";
                        plotStr+=tmpClass->shortName().toStdString();
                    }
                    plotStr+="'";

                    if (plotWithColors)
                        colourIdx++;
                    else
                        lineIdx++;
                }
            }

        }
#endif
        firstTrClass = false;
    }
    plotStr+="\n";
    logMaxY = 1;
    while (logMinY < minY)
        logMinY *=10;
    logMinY /=10;

    minY = floor(minY);
    maxY = floor(maxY);

    //gp<<"set terminal postscript enhanced font 'Times,6' persist\n";
    *gp<<"set encoding iso_8859_2\n";
    *gp<<"set lmargin 9\n";
    *gp<<"set xlabel \"a (traffic offered to a single BBU of the server)\"\n";
    *gp<<"set xtics 0.1\n";
    *gp<<"set xtics nomirror\n";
//    *gp << "set xrange ["<<results->min_a()<<":"<<results->max_a()<<"]\n";

//    *gp<<"set "<<results->resType2yScale(QoS_par).toStdString()<<" y\n";

#if 0
    if (results->resTypeHasLogScale(QoS_par))
    {
        *gp<<"set yrange ["<<logMinY<<":"<<logMaxY<<"]\n";
        *gp<<"set ytics (\\\n";
        *gp<<"0.00000001, 0.00000002, \"\" 0.00000003, \"\" 0.00000004, 0.00000005, \"\" 0.00000006, \"\" 0.00000008, \"\" 0.00000009, \\\n";
        *gp<<"0.0000001,  0.0000002,  \"\" 0.0000003,  \"\" 0.0000004,  0.0000005,  \"\" 0.0000006,  \"\" 0.0000008,  \"\" 0.0000009, \\\n";
        *gp<<"0.000001,   0.000002,   \"\" 0.000003,   \"\" 0.000004,   0.000005,   \"\" 0.000006,   \"\" 0.000008,   \"\" 0.000009, \\\n";
        *gp<<"0.00001,    0.00002,    \"\" 0.00003,    \"\" 0.00004,    0.00005,    \"\" 0.00006,    \"\" 0.00008,    \"\" 0.00009, \\\n";
        *gp<<"0.0001,     0.0002,     \"\" 0.0003,     \"\" 0.0004,     0.0005,     \"\" 0.0006,     \"\" 0.0008,     \"\" 0.0009, \\\n";
        *gp<<"0.001,      0.002,      \"\" 0.003,      \"\" 0.004,      0.005,      \"\" 0.006,      \"\" 0.008,      \"\" 0.009, \\\n";
        *gp<<"0.01,       0.02,       \"\" 0.03,       \"\" 0.04,       0.05,       \"\" 0.06,       \"\" 0.08,       \"\" 0.09, \\\n";
        *gp<<"0.1,        0.2,        \"\" 0.3,        \"\" 0.4,        0.5,        \"\" 0.6,        \"\" 0.8,        \"\" 0.9, \\\n";
        *gp<<"\"1\" 1)\n";
    }
    else
    {
        *gp<<"set yrange ["<<minY<<":"<<maxY<<"]\n";
        *gp<<"unset ytics\n";
    }

    *gp<<"set ytics nomirror\n";
    *gp<<"set border 3\n";
    //*gp<<"set size 0.32, 0.30\n";
    *gp<<"set grid\n";
    *gp<<"set style line 1 lt 1 lw 0\n";
    *gp<<"set key box linestyle 1\n";
    *gp<<"set key right bottom\n";
    *gp<<"set ylabel \""<<results->resType2string(QoS_par).toStdString()<<"\" offset 1.5\n";

    *gp << plotStr; //plot ....

    firstTrClass = true;
    foreach (ModelTrClass *tmpClass, selClasses)
    {
        foreach (const Investigator *tmpAlg, results->algorithms())
        {
            if (!tmpAlg->isSelected)
                continue;

            AlgorithmResults *algRes = results->getAlgResult(tmpAlg);
            if (algRes == nullptr)
                continue;

            if (!ResultsTmp::isTrClassDependent(QoS_par) && !firstTrClass)
                continue;

            values.clear();
            errors.clear();
            double value, ci;
            bool found = false;

            foreach(double a, *(algRes->a))
            {
                if (algRes->getVal(value, ci, QoS_par, tmpClass, a))
                {
                    values.push_back(boost::make_tuple(a, value));
                    found = true;

                    if (tmpAlg->hasConfIntervall())
                        errors.push_back(boost::make_tuple(a, value, ci));
                }
            }
            if (found)
            {
                if (tmpAlg->hasConfIntervall())
                {
                    if (plotCi)
                        gp->send1d(errors);
                    gp->send1d(values);
                }
                else
                    gp->send1d(values);
            }
        }
        firstTrClass = false;
    }
#endif
}

std::ostream &operator<<(std::ostream &st, GnuplotScript &results)
{
    st<<"set font"<<results.getFontName()<<" "<<results.getFontSize()<<"\n";
    return st;
}

QTextStream &operator<<(QTextStream &st, GnuplotScript &results)
{
//    st<<results;
    return st;
}

GnuplotScript::GnuplotScript(): systemResults(nullptr)
{
    plotCi = true;
    plotWithColors = true;
    gp = new Gnuplot("tee plot.gp | gnuplot -persist");
}

