#include <QSize>

#include "resultsXlsx.h"
#include "../utils/lag.h"

resultsXlsx::resultsXlsx(RSystem *systemResults): results(systemResults)
{
    fmtHdrQoSparOdd.setPatternBackgroundColor(Qt::darkRed);
    fmtHdrQoSparOdd.setFontColor(Qt::white);
    fmtHdrQoSparOdd.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
    fmtHdrQoSparOdd.setFontBold(true);
    fmtHdrQoSparEven.setPatternBackgroundColor(Qt::darkGreen);
    fmtHdrQoSparEven.setFontColor(Qt::white);
    fmtHdrQoSparEven.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
    fmtHdrQoSparEven.setFontBold(true);

    fmtHdrParOdd.setPatternBackgroundColor(Qt::white);
    fmtHdrParOdd.setFontColor(Qt::black);
    fmtHdrParOdd.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
    fmtHdrParOdd.setBorderColor(Qt::white);
    fmtHdrParOdd.setBorderStyle(QXlsx::Format::BorderThick);
    fmtHdrParEven.setPatternBackgroundColor(Qt::black);
    fmtHdrParEven.setFontColor(Qt::white);
    fmtHdrParEven.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
    fmtHdrParEven.setBorderColor(Qt::white);
    fmtHdrParEven.setBorderStyle(QXlsx::Format::BorderThick);

    fmtHdrEx.setPatternBackgroundColor(QColor(255, 190, 155));
    fmtHdrEx.setFontColor(Qt::white);
    fmtHdrEx.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
    fmtHdrEx.setFontBold(true);
    fmtHdrDx.setPatternBackgroundColor(QColor(155, 190, 255));
    fmtHdrDx.setFontColor(Qt::white);
    fmtHdrDx.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
    fmtHdrDx.setFontBold(true);

    fmtTrClass.setPatternBackgroundColor(Qt::yellow);
    fmtTrClass.setFontColor(Qt::black);
    fmtTrClass.setFontBold(true);

    fmtDistribution.setPatternBackgroundColor(Qt::gray);
    fmtDistribution.setFontColor(Qt::black);
    fmtDistribution.setFontBold(true);

    fmtRowAofOdd.setPatternBackgroundColor(QColor(225, 235, 215));
    fmtRowAofEven.setPatternBackgroundColor(Qt::white);

    fmtRowValOdd.setPatternBackgroundColor(QColor(225, 225, 235));
    fmtRowValEven.setPatternBackgroundColor(Qt::white);
}

void resultsXlsx::spreadshetWriteAUsAvailabilityOnlyForGroupSets(QXlsx::Worksheet *sheet, int &rowIdx, int &colIdx)
{
    int noOfApoints = results->getAvailableAperAU().length();
    int noOfAlgorithms = results->getNumberOrCalculatedAlgorithms();

    const ModelSyst& system = results->getModel();
    QVector<QXlsx::Chart *>chartAUsAvailability;
    QXlsx::Chart *chartClassServicePossibility;

    int k = system.k_s();

    int vMax = system.v_sMax();
    chartAUsAvailability.resize(noOfAlgorithms);

    //TODO zrobić nagłówek z opisem systemu
    bool firstIteration = true;
    int tmpColIdx = colIdx;

    int chartClassAvailabilityColNo = colIdx + 2 + 2*system.m() + 2*(1+system.v_sMax());
    int chartAuAvailabilityFirstCol = colIdx + 2 + 2*system.m() + 2*(1+system.v_sMax()) + 1;

    for (int noOfGroups=0; noOfGroups <= k; noOfGroups++)
    {
        int chartRowSart = rowIdx + 2;
        tmpColIdx = colIdx+2;
//Nagłówek
        QString groupsDescription = QString("Number of groups %1").arg(noOfGroups);

        int headerCombination = rowIdx++;
        int header2           = rowIdx++;
        int header3           = rowIdx++;
        int algNo =0;
        int rowParity;

        sheet->mergeCells(QXlsx::CellRange(headerCombination, 1, headerCombination, 2), fmtTrClass);
        sheet->write(headerCombination, 1, groupsDescription, fmtTrClass);

        sheet->write(header3, colIdx, "Algorithm");
        sheet->setColumnWidth(colIdx, colIdx, 25);
        sheet->write(header3, colIdx+1, "a");
        sheet->setColumnWidth(colIdx+1, colIdx+1, 5);

        sheet->mergeCells(QXlsx::CellRange(headerCombination, tmpColIdx, headerCombination, tmpColIdx+2*system.m()-1), fmtHdrQoSparOdd);
        sheet->write(headerCombination, tmpColIdx, "Service possibility for class", fmtHdrQoSparOdd);
        sheet->mergeCells(QXlsx::CellRange(header2, tmpColIdx, header2, tmpColIdx+system.m()-1), fmtHdrQoSparOdd);
        sheet->write(header2, tmpColIdx, "E(x)", fmtHdrEx);
        if (firstIteration)
            sheet->groupColumns(tmpColIdx, tmpColIdx+2*system.m()-1, false);
        for (int classNo=0; classNo<system.m(); classNo++)
        {
            sheet->write(header3, tmpColIdx, system.getClass(classNo)->shortName(), fmtTrClass);
            tmpColIdx++;
        }

        sheet->mergeCells(QXlsx::CellRange(header2, tmpColIdx, header2, tmpColIdx+system.m()-1), fmtHdrQoSparOdd);
        sheet->write(header2, tmpColIdx, "±", fmtHdrDx);
        if (firstIteration)
            sheet->groupColumns(tmpColIdx, tmpColIdx+system.m()-1);
        for (int classNo=0; classNo<system.m(); classNo++)
        {
            sheet->write(header3, tmpColIdx, system.getClass(classNo)->shortName(), fmtTrClass);
            tmpColIdx++;
        }

        sheet->mergeCells(QXlsx::CellRange(headerCombination, tmpColIdx, headerCombination, tmpColIdx+2*vMax+1), fmtHdrQoSparEven);
        sheet->write(headerCombination, tmpColIdx,
                     QString("Probability that n AUs are available in exactelly %1 %2").arg(noOfGroups).arg((noOfGroups == 1) ? "group" : "groups"),
                     fmtHdrQoSparOdd);
        sheet->mergeCells(QXlsx::CellRange(header2, tmpColIdx, header2, tmpColIdx+vMax), fmtHdrEx);
        sheet->write(header2, tmpColIdx, "E(x)", fmtHdrEx);
        if (firstIteration)
            sheet->groupColumns(tmpColIdx+4, tmpColIdx+vMax);
        for (int n=0; n <= vMax; n++)
        {
            sheet->write(header3, tmpColIdx, n, (n%2) ? fmtHdrParOdd : fmtHdrParEven);
            tmpColIdx++;
        }
        sheet->mergeCells(QXlsx::CellRange(header2, tmpColIdx, header2, tmpColIdx+vMax), fmtHdrDx);
        sheet->write(header2, tmpColIdx, "±", fmtHdrDx);
        if (firstIteration)
            sheet->groupColumns(tmpColIdx+1, tmpColIdx+vMax);
        for (int n=0; n <= vMax; n++)
        {
            sheet->write(header3, tmpColIdx, n, (n%2) ? fmtHdrParOdd : fmtHdrParEven);
            tmpColIdx++;
        }
        firstIteration = false;

        chartClassServicePossibility = sheet->insertChart(chartRowSart, chartClassAvailabilityColNo-1, QSize(500, static_cast<int>(noOfApoints*noOfAlgorithms*(sheet->rowHeight(rowIdx) + 5))));
        chartClassServicePossibility->setChartType(QXlsx::Chart::CT_Line);
        sheet->setColumnWidth(chartClassAvailabilityColNo, chartClassAvailabilityColNo, 70);
        sheet->write(headerCombination, chartClassAvailabilityColNo, "AUs availability for classes", fmtTrClass);
        sheet->write(header2, chartClassAvailabilityColNo, "X axis: traffic a, serie: class and algorithm", fmtHdrEx);

        foreach (Investigator *algorithm, results->getAvailableAlgorithms())
        {
            if (!algorithm->isSelected)
                continue;

            chartAUsAvailability[algNo] = sheet->insertChart(chartRowSart, chartAuAvailabilityFirstCol-1+algNo, QSize(500, static_cast<int>(noOfApoints*noOfAlgorithms*(sheet->rowHeight(rowIdx) + 5))));
            chartAUsAvailability[algNo]->setChartType(QXlsx::Chart::CT_Line);
            sheet->setColumnWidth(chartAuAvailabilityFirstCol+algNo, chartAuAvailabilityFirstCol+algNo, 70);
            sheet->write(headerCombination, chartAuAvailabilityFirstCol+algNo, QString("AUs availability: %1").arg(algorithm->shortName()), fmtHdrQoSparEven);
            sheet->write(header2, chartAuAvailabilityFirstCol+algNo, "X axis: no of AUs", fmtHdrEx);
            algNo++;
        }
//Wyniki
        algNo =0;
        foreach (Investigator *algorithm, results->getAvailableAlgorithms())
        {
            if (!algorithm->isSelected)
                continue;
            rowParity = 0;

            bool firstRowForNewAlgorithm = true;
            int startRowForResults = rowIdx;
            int lastRowResults = rowIdx;

            int firstClassColIdx = -1;

            foreach (decimal a, results->getAvailableAperAU())
            {
                rowParity++;

                const RInvestigator *pointResults = results->getInvestigationResults(algorithm, a);
                if (firstRowForNewAlgorithm)
                {
                    sheet->write(rowIdx, colIdx, algorithm->shortName(), ((rowParity) % 2) ?  fmtRowAofOdd : fmtRowAofEven);
                    firstRowForNewAlgorithm = false;
                }
                else
                {
                    sheet->write(rowIdx, colIdx, "", ((rowParity)%2) ? fmtRowAofOdd : fmtRowAofEven);
                }
                sheet->write(rowIdx, colIdx+1, static_cast<double>(a), ((rowParity)%2) ? fmtRowAofOdd : fmtRowAofEven);

                tmpColIdx = colIdx+2;

//Wyniki - klasy
                //E(X)
                firstClassColIdx = tmpColIdx;
                for (int classNo=0; classNo < system.m(); classNo++)
                {
                    double value;
                    int t = system.getClass(classNo)->t();
                    (*pointResults)->read(value, TypeForResourcessAndNumberOfServerGroups::AvailabilityOnlyInAllTheGroups, t, noOfGroups);
                    sheet->write(rowIdx, tmpColIdx, value, ((rowParity) % 2) ?  fmtRowValOdd : fmtRowValEven);
                    tmpColIdx++;
                }
                //±
                for (int classNo=0; classNo < system.m(); classNo++)
                {
                    if (pointResults->hasConfidencyIntervall())
                    {
                        const RSingle& tmpRS = pointResults->getConfidencyIntervall();
                        double value;
                        int t = system.getClass(classNo)->t();

                        tmpRS.read(value, TypeForResourcessAndNumberOfServerGroups::AvailabilityOnlyInAllTheGroups, t, noOfGroups);
                        sheet->write(rowIdx, tmpColIdx, value, ((rowParity) % 2) ?  fmtRowValOdd : fmtRowValEven);
                    }
                    else
                        sheet->write(rowIdx, tmpColIdx, "", ((rowParity) % 2) ?  fmtRowValOdd : fmtRowValEven);

                    tmpColIdx++;
                }
//Wyniki - zasoby
                //E(X)
                int startColIdx = tmpColIdx;
                for (int n=0; n <= vMax; n++)
                {
                    double value;
                    (*pointResults)->read(value, TypeForResourcessAndNumberOfServerGroups::AvailabilityOnlyInAllTheGroups, n, noOfGroups);
                    sheet->write(rowIdx, tmpColIdx, value, ((rowParity) % 2) ? fmtRowValOdd : fmtRowValEven);
                    tmpColIdx++;
                }
                chartAUsAvailability[algNo]->addSeries(QXlsx::CellRange(rowIdx, startColIdx+1, rowIdx, tmpColIdx-1));

                if (pointResults->hasConfidencyIntervall())
                {   //±
                    for (int n=0; n <= vMax; n++)
                    {
                        double value;
                        pointResults->getConfidencyIntervall().read(value, TypeForResourcessAndNumberOfServerGroups::AvailabilityOnlyInAllTheGroups, n, noOfGroups);
                        sheet->write(rowIdx, tmpColIdx, value, ((rowParity) % 2) ? fmtRowValOdd : fmtRowValEven);
                        tmpColIdx++;
                    }
                }
                lastRowResults = rowIdx;
                rowIdx++;
            }
//Wyniki wykresy f(a)
            for (int classNo=0; classNo < system.m(); classNo++)
            {
                chartClassServicePossibility->addSeries(QXlsx::CellRange(startRowForResults, firstClassColIdx+classNo, lastRowResults, firstClassColIdx+classNo));
            }
            algNo++;
        }
        rowIdx++;
    }
    colIdx = tmpColIdx;
}

void resultsXlsx::spreadshetWriteAUsAvailabilityForBestGroupSets(QXlsx::Worksheet *sheet, int &rowIdx, int &colIdx)
{
    int noOfApoints = results->getAvailableAperAU().length();
    int noOfAlgorithms = results->getNumberOrCalculatedAlgorithms();

    const ModelSyst& system = results->getModel();
    QVector<QXlsx::Chart *>chartAUsAvailability;
    QXlsx::Chart *chartClassServicePossibility;

    int k = system.k_s();

    int vMax = system.v_sMax();
    chartAUsAvailability.resize(noOfAlgorithms);

    //TODO zrobić nagłówek z opisem systemu
    bool firstIteration = true;
    int tmpColIdx=colIdx;

    int chartClassAvailabilityColNo = colIdx + 2 + 2*system.m() + 2*(1+system.v_sMax());
    int chartAuAvailabilityFirstCol = colIdx + 2 + 2*system.m() + 2*(1+system.v_sMax()) + 1;

    for (int noOfGroups=1; noOfGroups <= k; noOfGroups++)
    {
        int chartRowSart = rowIdx + 2;
        tmpColIdx = colIdx+2;
//Nagłówek
        QString groupsDescription = QString("Number of groups %1").arg(noOfGroups);

        int headerCombination = rowIdx++;
        int header2           = rowIdx++;
        int header3           = rowIdx++;
        int algNo =0;
        int rowParity;

        sheet->mergeCells(QXlsx::CellRange(headerCombination, 1, headerCombination, 2), fmtTrClass);
        sheet->write(headerCombination, 1, groupsDescription, fmtTrClass);

        sheet->write(header3, colIdx, "Algorithm");
        sheet->setColumnWidth(colIdx, colIdx, 25);
        sheet->write(header3, colIdx+1, "a");
        sheet->setColumnWidth(colIdx+1, colIdx+1, 5);

        sheet->mergeCells(QXlsx::CellRange(headerCombination, tmpColIdx, headerCombination, tmpColIdx+2*system.m()-1), fmtHdrQoSparOdd);
        sheet->write(headerCombination, tmpColIdx, QString("Service possibility in %1 or more groups").arg(noOfGroups), fmtHdrQoSparOdd);
        sheet->mergeCells(QXlsx::CellRange(header2, tmpColIdx, header2, tmpColIdx+system.m()-1), fmtHdrQoSparOdd);
        sheet->write(header2, tmpColIdx, "E(x)", fmtHdrEx);
        if (firstIteration)
            sheet->groupColumns(tmpColIdx, tmpColIdx+2*system.m()-1, false);
        for (int classNo=0; classNo<system.m(); classNo++)
        {
            sheet->write(header3, tmpColIdx, system.getClass(classNo)->shortName(), fmtTrClass);
            tmpColIdx++;
        }

        sheet->mergeCells(QXlsx::CellRange(header2, tmpColIdx, header2, tmpColIdx+system.m()-1), fmtHdrQoSparOdd);
        sheet->write(header2, tmpColIdx, "±", fmtHdrDx);
        if (firstIteration)
            sheet->groupColumns(tmpColIdx, tmpColIdx+system.m()-1);
        for (int classNo=0; classNo<system.m(); classNo++)
        {
            sheet->write(header3, tmpColIdx, system.getClass(classNo)->shortName(), fmtTrClass);
            tmpColIdx++;
        }

        sheet->mergeCells(QXlsx::CellRange(headerCombination, tmpColIdx, headerCombination, tmpColIdx+2*vMax+1), fmtHdrQoSparEven);
        sheet->write(headerCombination, tmpColIdx, QString("Probability that n or more AUs are available in %1 or more groups").arg(noOfGroups), fmtHdrQoSparOdd);
        sheet->mergeCells(QXlsx::CellRange(header2, tmpColIdx, header2, tmpColIdx+vMax), fmtHdrEx);
        sheet->write(header2, tmpColIdx, "E(x)", fmtHdrEx);
        if (firstIteration)
            sheet->groupColumns(tmpColIdx+4, tmpColIdx+vMax);
        for (int n=0; n <= vMax; n++)
        {
            sheet->write(header3, tmpColIdx, n, (n%2) ? fmtHdrParOdd : fmtHdrParEven);
            tmpColIdx++;
        }

        sheet->mergeCells(QXlsx::CellRange(header2, tmpColIdx, header2, tmpColIdx+vMax), fmtHdrDx);
        sheet->write(header2, tmpColIdx, "±", fmtHdrDx);
        if (firstIteration)
            sheet->groupColumns(tmpColIdx+1, tmpColIdx+vMax);
        for (int n=0; n <= vMax; n++)
        {
            sheet->write(header3, tmpColIdx, n, (n%2) ? fmtHdrParOdd : fmtHdrParEven);
            tmpColIdx++;
        }
        firstIteration = false;

        chartClassServicePossibility = sheet->insertChart(chartRowSart, chartClassAvailabilityColNo-1, QSize(500, static_cast<int>(noOfApoints*noOfAlgorithms*(sheet->rowHeight(rowIdx) + 5))));
        chartClassServicePossibility->setChartType(QXlsx::Chart::CT_Line);
        sheet->setColumnWidth(chartClassAvailabilityColNo, chartClassAvailabilityColNo, 70);
        sheet->write(headerCombination, chartClassAvailabilityColNo, "AUs availability for classes", fmtTrClass);
        sheet->write(header2, chartClassAvailabilityColNo, "X axis: traffic a, serie: class and algorithm", fmtHdrEx);

        foreach (Investigator *algorithm, results->getAvailableAlgorithms())
        {
            if (!algorithm->isSelected)
                continue;

            chartAUsAvailability[algNo] = sheet->insertChart(chartRowSart, chartAuAvailabilityFirstCol-1+algNo, QSize(500, static_cast<int>(noOfApoints*noOfAlgorithms*(sheet->rowHeight(rowIdx) + 5))));
            chartAUsAvailability[algNo]->setChartType(QXlsx::Chart::CT_Line);
            sheet->setColumnWidth(chartAuAvailabilityFirstCol+algNo, chartAuAvailabilityFirstCol+algNo, 70);
            sheet->write(headerCombination, chartAuAvailabilityFirstCol+algNo, QString("AUs availability: %1").arg(algorithm->shortName()), fmtHdrQoSparEven);
            sheet->write(header2, chartAuAvailabilityFirstCol+algNo, "X axis: no of AUs", fmtHdrEx);
            algNo++;
        }
//Wyniki
        algNo =0;
        foreach (Investigator *algorithm, results->getAvailableAlgorithms())
        {
            if (!algorithm->isSelected)
                continue;
            rowParity = 0;

            bool firstRowForNewAlgorithm = true;
            int startRowForResults = rowIdx;
            int lastRowResults = rowIdx;

            int firstClassColIdx = -1;

            foreach (decimal a, results->getAvailableAperAU())
            {
                rowParity++;

                const RInvestigator *pointResults = results->getInvestigationResults(algorithm, a);
                if (firstRowForNewAlgorithm)
                {
                    sheet->write(rowIdx, colIdx, algorithm->shortName(), ((rowParity) % 2) ?  fmtRowAofOdd : fmtRowAofEven);
                    firstRowForNewAlgorithm = false;
                }
                else
                {
                    sheet->write(rowIdx, colIdx, "", ((rowParity)%2) ? fmtRowAofOdd : fmtRowAofEven);
                }
                sheet->write(rowIdx, colIdx+1, static_cast<double>(a), ((rowParity)%2) ? fmtRowAofOdd : fmtRowAofEven);

                tmpColIdx = colIdx+2;

//Wyniki - klasy
                //E(X)
                firstClassColIdx = tmpColIdx;
                for (int classNo=0; classNo < system.m(); classNo++)
                {
                    double value;
                    (*pointResults)->read(value, TypeClassForServerBestGroupsSet::ServPossibilityOnlyInAllTheSubgroups, classNo, noOfGroups);
                    sheet->write(rowIdx, tmpColIdx, value, ((rowParity) % 2) ?  fmtRowValOdd : fmtRowValEven);
                    tmpColIdx++;
                }
                //±
                for (int classNo=0; classNo < system.m(); classNo++)
                {
                    if (pointResults->hasConfidencyIntervall())
                    {
                        const RSingle& tmpRS = pointResults->getConfidencyIntervall();
                        double value;
                        tmpRS.read(value, TypeClassForServerBestGroupsSet::ServPossibilityOnlyInAllTheSubgroups, classNo, noOfGroups);
                        sheet->write(rowIdx, tmpColIdx, value, ((rowParity) % 2) ?  fmtRowValOdd : fmtRowValEven);
                    }
                    else
                        sheet->write(rowIdx, tmpColIdx, "", ((rowParity) % 2) ?  fmtRowValOdd : fmtRowValEven);

                    tmpColIdx++;
                }
//Wyniki - zasoby
                //E(X) prawdopodobieństwo, że w x najlepszych podgrupach dostępnych jest n zasobów
                int startColIdx = tmpColIdx;
                for (int n=0; n <= vMax; n++)
                {
                    double value;
                    (*pointResults)->read(value, TypeForResourcessAndNumberOfServerGroups::AvailabilityOnlyInAllTheGroups, n, noOfGroups);
                    sheet->write(rowIdx, tmpColIdx, value, ((rowParity) % 2) ? fmtRowValOdd : fmtRowValEven);
                    tmpColIdx++;
                }
                chartAUsAvailability[algNo]->addSeries(QXlsx::CellRange(rowIdx, startColIdx+1, rowIdx, tmpColIdx-1));

                if (pointResults->hasConfidencyIntervall())
                {   //± Prawdopobieństwo, że w dokładnie *** noOfGroups *** podgrupach jest n lub więcej wolnych zasobów
                    for (int n=0; n <= vMax; n++)
                    {
                        double value;
                        pointResults->getConfidencyIntervall().read(value, TypeForResourcessAndNumberOfServerGroups::AvailabilityOnlyInAllTheGroups, n, noOfGroups);
                        sheet->write(rowIdx, tmpColIdx, value, ((rowParity) % 2) ? fmtRowValOdd : fmtRowValEven);
                        tmpColIdx++;
                    }
                }
                lastRowResults = rowIdx;
                rowIdx++;
            }
//Wyniki - wykresy f(a)
            for (int classNo=0; classNo < system.m(); classNo++)
            {
                chartClassServicePossibility->addSeries(QXlsx::CellRange(startRowForResults, firstClassColIdx+classNo, lastRowResults, firstClassColIdx+classNo));
            }
            algNo++;
        }
        rowIdx++;
    }
    colIdx = tmpColIdx;
}

void resultsXlsx::_spreadshetWriteForGroupCombinations(
        QXlsx::Worksheet *sheet, int &rowIdx, int &colIdx
      , QString descriptionTableAUs, QString descriptionChartAUs, QString descriptionTableClasses, QString descriptionChartClasses
      , TypeResourcess_VsServerGroupsCombination typeForState)
{
    const ModelSyst& system = results->getModel();
    int k = system.k_s();

    QVector<QPair<QVector<int>, QVector<int> > > combinations = Utils::UtilsLAG::getPossibleCombinations(k);
    int vMax = system.v_sMax();

    //TODO zrobić nagłówek z opisem systemu
    int combIndex = 0;
    bool firstIteration = true;
    int tmpColIdx = colIdx;

    int chart1ColNo = colIdx + 2 + 2*system.m() + 2*(1+system.v_sMax());

    int chart2ColNo = chart1ColNo+1;

    int noOfApoints = results->getAvailableAperAU().length();
    int noOfAlgorithms = results->getNumberOrCalculatedAlgorithms();

    sheet->setColumnWidth(chart1ColNo, chart1ColNo, 70);
    sheet->setColumnWidth(chart2ColNo, chart2ColNo, 70);

    foreach (auto singleComb, combinations)
    {
        int chartRowIdx = rowIdx + 2;

        tmpColIdx = colIdx+2;
//Nagłówek
        QString combinationDescr = Utils::UtilsLAG::getCombinationString(singleComb.first);

        int headerCombination = rowIdx++;
        int header2           = rowIdx++;
        int header3           = rowIdx++;

        sheet->write(headerCombination, 1, "Podgrupy", fmtTrClass);
        sheet->write(headerCombination, 2, combinationDescr, fmtTrClass);
        sheet->write(header3, colIdx, "Algorytm");
        sheet->setColumnWidth(colIdx, colIdx, 25);

        sheet->write(header3, colIdx+1, "a");
        sheet->setColumnWidth(colIdx+1, colIdx+1, 5);

        sheet->mergeCells(QXlsx::CellRange(headerCombination, tmpColIdx, headerCombination, tmpColIdx+2*system.m()-1), fmtHdrQoSparOdd);
        sheet->write(headerCombination, tmpColIdx, descriptionTableClasses, fmtHdrQoSparOdd);
        sheet->mergeCells(QXlsx::CellRange(header2, tmpColIdx, header2, tmpColIdx+system.m()-1), fmtHdrQoSparOdd);
        sheet->write(header2, tmpColIdx, "E(x)", fmtHdrEx);
        if (firstIteration)
            sheet->groupColumns(tmpColIdx, tmpColIdx+2*system.m()-1, false);
        for (int classNo=0; classNo<system.m(); classNo++)
        {
            sheet->write(header3, tmpColIdx, system.getClass(classNo)->shortName(), fmtTrClass);
            tmpColIdx++;
        }

        sheet->mergeCells(QXlsx::CellRange(header2, tmpColIdx, header2, tmpColIdx+system.m()-1), fmtHdrQoSparOdd);
        sheet->write(header2, tmpColIdx, "±", fmtHdrDx);
        if (firstIteration)
            sheet->groupColumns(tmpColIdx, tmpColIdx+system.m()-1);
        for (int classNo=0; classNo<system.m(); classNo++)
        {
            sheet->write(header3, tmpColIdx, system.getClass(classNo)->shortName(), fmtTrClass);
            tmpColIdx++;
        }

        sheet->mergeCells(QXlsx::CellRange(headerCombination, tmpColIdx, headerCombination, tmpColIdx+vMax), fmtHdrQoSparEven);
        sheet->write(headerCombination, tmpColIdx, descriptionTableAUs, fmtHdrQoSparEven);
        sheet->mergeCells(QXlsx::CellRange(header2, tmpColIdx, header2, tmpColIdx+vMax), fmtHdrEx);
        sheet->write(header2, tmpColIdx, "E(x)", fmtHdrEx);
        if (firstIteration)
            sheet->groupColumns(tmpColIdx+4, tmpColIdx+vMax);
        for (int n=0; n <= vMax; n++)
        {
            QXlsx::Format &format = (n%2) ? fmtHdrParOdd : fmtHdrParEven;
            sheet->write(header3, tmpColIdx, n, format);
            tmpColIdx++;
        }

        sheet->mergeCells(QXlsx::CellRange(headerCombination, tmpColIdx, headerCombination, tmpColIdx+vMax), fmtHdrQoSparEven);
        sheet->write(headerCombination, tmpColIdx, descriptionTableAUs, fmtHdrQoSparEven);
        sheet->mergeCells(QXlsx::CellRange(header2, tmpColIdx, header2, tmpColIdx+vMax), fmtHdrDx);
        sheet->write(header2, tmpColIdx, "±", fmtHdrDx);
        if (firstIteration)
            sheet->groupColumns(tmpColIdx+1, tmpColIdx+vMax);
        for (int n=0; n <= vMax; n++)
        {
            sheet->write(header3, tmpColIdx, n, (n%2) ? fmtHdrParOdd : fmtHdrParEven);
            tmpColIdx++;
        }

        sheet->write(headerCombination, chart1ColNo, descriptionChartAUs, fmtHdrQoSparOdd);
        sheet->write(header2, chart1ColNo, "Przebieg: a oraz algorytm", fmtHdrEx);
        sheet->write(header3, chart1ColNo, "Oś X - liczba zasobów", fmtHdrDx);

        sheet->write(headerCombination, chart2ColNo, descriptionChartClasses, fmtHdrQoSparEven);
        sheet->write(header2, chart2ColNo, "Przebieg: klasa oraz algorytm", fmtHdrEx);
        sheet->write(header3, chart2ColNo, "Oś X - ruch oferowany", fmtHdrDx);

        firstIteration = false;

//Wyniki
        int rowSubIdx=0;

        QXlsx::Chart *chartClasses = sheet->insertChart(chartRowIdx, chart2ColNo-1, QSize(500, static_cast<int>(noOfApoints*noOfAlgorithms*(sheet->rowHeight(rowIdx) + 5))));
        chartClasses->setChartType(QXlsx::Chart::CT_Line);

        foreach (Investigator *algorithm, results->getAvailableAlgorithms())
        {
            if (!algorithm->isSelected)
                continue;

            QXlsx::Chart *chartCombination = sheet->insertChart(chartRowIdx, chart1ColNo-1, QSize(500, static_cast<int>(noOfApoints*(sheet->rowHeight(rowIdx) + 5))));
            chartCombination->setChartType(QXlsx::Chart::CT_Line);
            chartRowIdx += noOfApoints;


            bool firstRowForNewA = true;
            rowSubIdx = 0;

            int firstRowWithResults = rowIdx;
            int firstClassColIdx = colIdx + 2;
            foreach (decimal a, results->getAvailableAperAU())
            {
                rowSubIdx++;
                const RInvestigator *pointResults = results->getInvestigationResults(algorithm, a);
                if (firstRowForNewA)
                {
                    sheet->write(rowIdx, colIdx, algorithm->shortName(), (rowSubIdx % 2) ?  fmtRowAofOdd : fmtRowAofEven);
                    firstRowForNewA = false;
                }
                else
                {
                    sheet->write(rowIdx, colIdx, "", (rowSubIdx%2) ? fmtRowAofOdd : fmtRowAofEven);
                }
                sheet->write(rowIdx, colIdx+1, static_cast<double>(a), (rowSubIdx%2) ? fmtRowAofOdd : fmtRowAofEven);

                tmpColIdx = colIdx+2;

                //Prawdopodobieństwo, że Klasa może być obsłużona
                //E(X)
                for (int classNo=0; classNo < system.m(); classNo++)
                {
                    double value;
                    (*pointResults)->read(value, typeForState, classNo, combIndex);
                    sheet->write(rowIdx, tmpColIdx, value, (rowSubIdx % 2) ?  fmtRowValOdd : fmtRowValEven);
                    tmpColIdx++;
                }
                //±
                for (int classNo=0; classNo < system.m(); classNo++)
                {
                    if (pointResults->hasConfidencyIntervall())
                    {
                        const RSingle& tmpRS = pointResults->getConfidencyIntervall();
                        double value;
                        tmpRS.read(value, typeForState, classNo, combIndex);
                        sheet->write(rowIdx, tmpColIdx, value, (rowSubIdx % 2) ?  fmtRowValOdd : fmtRowValEven);
                    }
                    else
                        sheet->write(rowIdx, tmpColIdx, "", (rowSubIdx % 2) ?  fmtRowValOdd : fmtRowValEven);
                    tmpColIdx++;
                }

                //E(X) Prawdopobieństwo, że w najbardziej obciążonej grupie dostępnych jest n zasobów
                int startColIdx = tmpColIdx;
                for (int n=0; n <= vMax; n++)
                {

                    double value;
                    (*pointResults)->read(value, typeForState, n, combIndex);
                    sheet->write(rowIdx, tmpColIdx, value, (rowSubIdx % 2) ? fmtRowValOdd : fmtRowValEven);
                    tmpColIdx++;
                }
                chartCombination->addSeries(QXlsx::CellRange(rowIdx, startColIdx+1, rowIdx, tmpColIdx-1));

                if (pointResults->hasConfidencyIntervall())
                {   //± Prawdopobieństwo, że w najbardziej obciążonej grupie dostępnych jest n zasobów
                    for (int n=0; n <= vMax; n++)
                    {
                        double value;
                        pointResults->getConfidencyIntervall().read(value, typeForState, n, combIndex);
                        sheet->write(rowIdx, tmpColIdx, value, (rowSubIdx % 2) ? fmtRowValOdd : fmtRowValEven);
                        tmpColIdx++;
                    }
                }
                rowIdx++;
            }
            for (int i=0; i<system.m(); i++)
                chartClasses->addSeries(QXlsx::CellRange(firstRowWithResults, firstClassColIdx+i, rowIdx-1, firstClassColIdx+i));
        }
        combIndex++;
        rowIdx++;
    }
    colIdx = tmpColIdx;
}

void resultsXlsx::spreadshetWriteAUsAvailabilityForGroupCombinations(QXlsx::Worksheet *sheet, int &rowIdx, int &colIdx)
{
    _spreadshetWriteForGroupCombinations(sheet, rowIdx, colIdx
      , "Prawdopodobieństwo, że każda z podgrup ma dostępne n zasobów (pozostałych podgrup nie rozpatrujemy)", "Dostępność n zasobów"
      , "Możliwość obsługi klasy w każdej z podgrup", "Dostępność dla klasy"
      , TypeResourcess_VsServerGroupsCombination::AvailabilityInAllTheGroups);
}

void resultsXlsx::spreadshetWriteAUsInavailabilityForGroupCombinations(QXlsx::Worksheet *sheet, int &rowIdx, int &colIdx)
{
    _spreadshetWriteForGroupCombinations(sheet, rowIdx, colIdx
      , "n niedostępnych zasobów w każdej z podgrup (pozostałych podgrup nie rozpatrujemy)", "n zasobów niedostępnych"
      , "Brak możliwości obsługi zgłoszeń klasy", "Brak dostępności dla klasy"
      , TypeResourcess_VsServerGroupsCombination::InavailabilityInAllTheGroups);
}

void resultsXlsx::spreadshetWriteAUsFreeForGroupCombinations(QXlsx::Worksheet *sheet, int &rowIdx, int &colIdx)
{
    const ModelSyst& system = results->getModel();

    int k = system.k_s();

    QVector<QPair<QVector<int>, QVector<int> > > combinations = Utils::UtilsLAG::getPossibleCombinations(k);
    int vMax = system.v_sMax();

    //TODO zrobić nagłówek z opisem systemu

    int combIndex = 0;
    bool firstIteration = true;
    int tmpColIdx = colIdx;


    int chart1ColNo = colIdx + 2 + /*2*system.m() + */4*(1+system.v_sMax()) + 1;
    int chart2ColNo = chart1ColNo+2;

    int noOfApoints = results->getAvailableAperAU().length();

    foreach ( auto singleComb, combinations)
    {
        QXlsx::Chart *chartMin = sheet->insertChart(rowIdx + 2, chart1ColNo-1, QSize(500, static_cast<int>(noOfApoints*(sheet->rowHeight(rowIdx) + 5))));
        chartMin->setChartType(QXlsx::Chart::CT_Line);
        sheet->setColumnWidth(chart1ColNo, chart1ColNo, 70);

        QXlsx::Chart *chartMax = sheet->insertChart(rowIdx + 2, chart2ColNo-1, QSize(500, static_cast<int>(noOfApoints*(sheet->rowHeight(rowIdx) + 5))));
        chartMax->setChartType(QXlsx::Chart::CT_Line);
        sheet->setColumnWidth(chart2ColNo, chart2ColNo, 70);

        tmpColIdx = colIdx+2;
//Nagłówek
        QString combinationDescr = Utils::UtilsLAG::getCombinationString(singleComb.first);

        int headerCombination = rowIdx++;
        int header2           = rowIdx++;
        int header3           = rowIdx++;

        sheet->write(headerCombination, 1, "Podgrupy", fmtTrClass);
        sheet->write(headerCombination, 2, combinationDescr, fmtTrClass);
        sheet->write(header3, colIdx, "a");
        sheet->write(header3, colIdx+1, "Algorytm");
        sheet->setColumnWidth(colIdx+1, colIdx+1, 25);

        sheet->mergeCells(QXlsx::CellRange(headerCombination, tmpColIdx, headerCombination, tmpColIdx+vMax), fmtHdrQoSparEven);
        sheet->write(headerCombination, tmpColIdx, "MIN: Prawdopodobieństwo, że najbardziej obciążona grupa ma n zasobów wolnych", fmtHdrQoSparEven);
        sheet->mergeCells(QXlsx::CellRange(header2, tmpColIdx, header2, tmpColIdx+vMax), fmtHdrEx);
        sheet->write(header2, tmpColIdx, "E(x)", fmtHdrEx);
        if (firstIteration)
            sheet->groupColumns(tmpColIdx+4, tmpColIdx+vMax);
        for (int n=0; n <= vMax; n++)
        {
            QXlsx::Format &format = (n%2) ? fmtHdrParOdd : fmtHdrParEven;
            sheet->write(header3, tmpColIdx, n, format);
            tmpColIdx++;
        }

        sheet->mergeCells(QXlsx::CellRange(headerCombination, tmpColIdx, headerCombination, tmpColIdx+vMax), fmtHdrQoSparOdd);
        sheet->write(headerCombination, tmpColIdx, "MAX: Prawdopodobieństwo, że najmniej obciążona grupa ma n zasobów wolnych", fmtHdrQoSparOdd);
        sheet->mergeCells(QXlsx::CellRange(header2, tmpColIdx, header2, tmpColIdx+vMax), fmtHdrEx);
        sheet->write(header2, tmpColIdx, "E(x)", fmtHdrEx);
        if (firstIteration)
            sheet->groupColumns(tmpColIdx+4, tmpColIdx+vMax);
        for (int n=0; n <= vMax; n++)
        {
            sheet->write(header3, tmpColIdx, n, (n%2) ? fmtHdrParOdd : fmtHdrParEven);
            tmpColIdx++;
        }

        sheet->mergeCells(QXlsx::CellRange(headerCombination, tmpColIdx, headerCombination, tmpColIdx+vMax), fmtHdrQoSparEven);
        sheet->write(headerCombination, tmpColIdx, "MIN: Prawdopodobieństwo, że najbardziej obciążona grupa ma n zasobów wolnych", fmtHdrQoSparEven);
        sheet->mergeCells(QXlsx::CellRange(header2, tmpColIdx, header2, tmpColIdx+vMax), fmtHdrDx);
        sheet->write(header2, tmpColIdx, "±", fmtHdrDx);
        if (firstIteration)
            sheet->groupColumns(tmpColIdx+1, tmpColIdx+vMax);
        for (int n=0; n <= vMax; n++)
        {
            sheet->write(header3, tmpColIdx, n, (n%2) ? fmtHdrParOdd : fmtHdrParEven);
            tmpColIdx++;
        }

        sheet->mergeCells(QXlsx::CellRange(headerCombination, tmpColIdx, headerCombination, tmpColIdx+vMax), fmtHdrQoSparOdd);
        sheet->write(headerCombination, tmpColIdx, "MAX: Prawdopodobieństwo, że najmniej obciążona grupa ma n zasobów wolnych", fmtHdrQoSparOdd);
        sheet->mergeCells(QXlsx::CellRange(header2, tmpColIdx, header2, tmpColIdx+vMax), fmtHdrDx);
        sheet->write(header2, tmpColIdx, "±", fmtHdrDx);
        if (firstIteration)
            sheet->groupColumns(tmpColIdx+1, tmpColIdx+vMax);
        for (int n=0; n <= vMax; n++)
        {
            sheet->write(header3, tmpColIdx, n, (n%2) ? fmtHdrParOdd : fmtHdrParEven);
            tmpColIdx++;
        }
        firstIteration = false;

//Wyniki
        int rowSubIdx=0;
        foreach (decimal a, results->getAvailableAperAU())
        {
            rowSubIdx++;
            bool firstRowForNewA = true;

            foreach (Investigator *algorithm, results->getAvailableAlgorithms())
            {
                if (!algorithm->isSelected)
                    continue;

                const RInvestigator *pointResults = results->getInvestigationResults(algorithm, a);
                if (firstRowForNewA)
                {
                    sheet->write(rowIdx, colIdx, static_cast<double>(a), (rowSubIdx%2) ? fmtRowAofOdd : fmtRowAofEven);
                    firstRowForNewA = false;
                }
                else
                {
                    sheet->write(rowIdx, colIdx, "", (rowSubIdx%2) ? fmtRowAofOdd : fmtRowAofEven);
                }
                sheet->write(rowIdx, colIdx+1, algorithm->shortName(), (rowSubIdx % 2) ?  fmtRowAofOdd : fmtRowAofEven);

                tmpColIdx = colIdx+2;

                //E(X) Prawdopobieństwo, że w najbardziej obciążonej grupie dostępnych jest n zasobów
                int startColIdx = tmpColIdx;
                for (int n=0; n <= vMax; n++)
                {

                    double value;
                    (*pointResults)->read(value, TypeResourcess_VsServerGroupsCombination::FreeAUsInEveryGroup, n, combIndex);
                    sheet->write(rowIdx, tmpColIdx, value, (rowSubIdx % 2) ? fmtRowValOdd : fmtRowValEven);
                    tmpColIdx++;
                }
                chartMin->addSeries(QXlsx::CellRange(rowIdx, startColIdx, rowIdx, tmpColIdx-1));

                //E(X) Prawdopobieństwo, że w najmniej obciążonej grupie dostępnych jest n zasobów
                startColIdx = tmpColIdx;
                for (int n=0; n <= vMax; n++)
                {
                    double value;
                    (*pointResults)->read(value, TypeResourcess_VsServerGroupsCombination::FreeAUsInBestGroup, n, combIndex);
                    sheet->write(rowIdx, tmpColIdx, value, (rowSubIdx % 2) ? fmtRowValOdd : fmtRowValEven);
                    tmpColIdx++;
                }
                chartMax->addSeries(QXlsx::CellRange(rowIdx, startColIdx, rowIdx, tmpColIdx-1));

                if (pointResults->hasConfidencyIntervall())
                {   //± Prawdopobieństwo, że w najbardziej obciążonej grupie dostępnych jest n zasobów
                    for (int n=0; n <= vMax; n++)
                    {
                        double value;
                        pointResults->getConfidencyIntervall().read(value, TypeResourcess_VsServerGroupsCombination::FreeAUsInEveryGroup, n, combIndex);
                        sheet->write(rowIdx, tmpColIdx, value, (rowSubIdx % 2) ? fmtRowValOdd : fmtRowValEven);
                        tmpColIdx++;
                    }
                    //± Prawdopobieństwo, że w najmniej obciążonej grupie dostępnych jest n zasobów
                    for (int n=0; n <= vMax; n++)
                    {
                        double value;
                        pointResults->getConfidencyIntervall().read(value, TypeResourcess_VsServerGroupsCombination::FreeAUsInBestGroup, n, combIndex);
                        sheet->write(rowIdx, tmpColIdx, value, (rowSubIdx % 2) ? fmtRowValOdd : fmtRowValEven);
                        tmpColIdx++;
                    }
                }
                rowIdx++;
            }
        }
        combIndex++;
        rowIdx++;
    }
    colIdx = tmpColIdx;
}

void resultsXlsx::spreadshetWriteQoS_ForClasses(QXlsx::Worksheet *sheet, int &rowIdx, int &colIdx)
{
    const ModelSyst& system = results->getModel();

    int k = system.k_s();

    QVector<QPair<QVector<int>, QVector<int> > > combinations = Utils::UtilsLAG::getPossibleCombinations(k);
    int vMax = system.v_sMax();

    //TODO zrobić nagłówek z opisem systemu

    int combIndex = 0;
    bool firstIteration = true;
    int tmpColIdx = colIdx;


    int chart1ColNo = colIdx + 2 + /*2*system.m() + */4*(1+system.v_sMax()) + 1;
    int chart2ColNo = chart1ColNo+2;

    int noOfApoints = results->getAvailableAperAU().length();

    foreach ( auto singleComb, combinations)
    {
        QXlsx::Chart *chartMin = sheet->insertChart(rowIdx + 2, chart1ColNo-1, QSize(500, static_cast<int>(noOfApoints*(sheet->rowHeight(rowIdx) + 5))));
        chartMin->setChartType(QXlsx::Chart::CT_Line);
        sheet->setColumnWidth(chart1ColNo, chart1ColNo, 70);

        QXlsx::Chart *chartMax = sheet->insertChart(rowIdx + 2, chart2ColNo-1, QSize(500, static_cast<int>(noOfApoints*(sheet->rowHeight(rowIdx) + 5))));
        chartMax->setChartType(QXlsx::Chart::CT_Line);
        sheet->setColumnWidth(chart2ColNo, chart2ColNo, 70);

        tmpColIdx = colIdx+2;
//Nagłówek
        QString combinationDescr = Utils::UtilsLAG::getCombinationString(singleComb.first);

        int headerCombination = rowIdx++;
        int header2           = rowIdx++;
        int header3           = rowIdx++;

        sheet->write(headerCombination, 1, "Podgrupy", fmtTrClass);
        sheet->write(headerCombination, 2, combinationDescr, fmtTrClass);
        sheet->write(header3, colIdx, "a");
        sheet->write(header3, colIdx+1, "Algorytm");
        sheet->setColumnWidth(colIdx+1, colIdx+1, 25);

        sheet->mergeCells(QXlsx::CellRange(headerCombination, tmpColIdx, headerCombination, tmpColIdx+vMax), fmtHdrQoSparEven);
        sheet->write(headerCombination, tmpColIdx, "MIN: Prawdopodobieństwo, że najbardziej obciążona grupa ma n zasobów wolnych", fmtHdrQoSparEven);
        sheet->mergeCells(QXlsx::CellRange(header2, tmpColIdx, header2, tmpColIdx+vMax), fmtHdrEx);
        sheet->write(header2, tmpColIdx, "E(x)", fmtHdrEx);
        if (firstIteration)
            sheet->groupColumns(tmpColIdx+4, tmpColIdx+vMax);
        for (int n=0; n <= vMax; n++)
        {
            QXlsx::Format &format = (n%2) ? fmtHdrParOdd : fmtHdrParEven;
            sheet->write(header3, tmpColIdx, n, format);
            tmpColIdx++;
        }

        sheet->mergeCells(QXlsx::CellRange(headerCombination, tmpColIdx, headerCombination, tmpColIdx+vMax), fmtHdrQoSparOdd);
        sheet->write(headerCombination, tmpColIdx, "MAX: Prawdopodobieństwo, że najmniej obciążona grupa ma n zasobów wolnych", fmtHdrQoSparOdd);
        sheet->mergeCells(QXlsx::CellRange(header2, tmpColIdx, header2, tmpColIdx+vMax), fmtHdrEx);
        sheet->write(header2, tmpColIdx, "E(x)", fmtHdrEx);
        if (firstIteration)
            sheet->groupColumns(tmpColIdx+4, tmpColIdx+vMax);
        for (int n=0; n <= vMax; n++)
        {
            sheet->write(header3, tmpColIdx, n, (n%2) ? fmtHdrParOdd : fmtHdrParEven);
            tmpColIdx++;
        }

        sheet->mergeCells(QXlsx::CellRange(headerCombination, tmpColIdx, headerCombination, tmpColIdx+vMax), fmtHdrQoSparEven);
        sheet->write(headerCombination, tmpColIdx, "MIN: Prawdopodobieństwo, że najbardziej obciążona grupa ma n zasobów wolnych", fmtHdrQoSparEven);
        sheet->mergeCells(QXlsx::CellRange(header2, tmpColIdx, header2, tmpColIdx+vMax), fmtHdrDx);
        sheet->write(header2, tmpColIdx, "±", fmtHdrDx);
        if (firstIteration)
            sheet->groupColumns(tmpColIdx+1, tmpColIdx+vMax);
        for (int n=0; n <= vMax; n++)
        {
            sheet->write(header3, tmpColIdx, n, (n%2) ? fmtHdrParOdd : fmtHdrParEven);
            tmpColIdx++;
        }

        sheet->mergeCells(QXlsx::CellRange(headerCombination, tmpColIdx, headerCombination, tmpColIdx+vMax), fmtHdrQoSparOdd);
        sheet->write(headerCombination, tmpColIdx, "MAX: Prawdopodobieństwo, że najmniej obciążona grupa ma n zasobów wolnych", fmtHdrQoSparOdd);
        sheet->mergeCells(QXlsx::CellRange(header2, tmpColIdx, header2, tmpColIdx+vMax), fmtHdrDx);
        sheet->write(header2, tmpColIdx, "±", fmtHdrDx);
        if (firstIteration)
            sheet->groupColumns(tmpColIdx+1, tmpColIdx+vMax);
        for (int n=0; n <= vMax; n++)
        {
            sheet->write(header3, tmpColIdx, n, (n%2) ? fmtHdrParOdd : fmtHdrParEven);
            tmpColIdx++;
        }
        firstIteration = false;

//Wyniki
        int rowSubIdx=0;
        foreach (decimal a, results->getAvailableAperAU())
        {
            rowSubIdx++;
            bool firstRowForNewA = true;

            foreach (Investigator *algorithm, results->getAvailableAlgorithms())
            {
                if (!algorithm->isSelected)
                    continue;

                const RInvestigator *pointResults = results->getInvestigationResults(algorithm, a);
                if (firstRowForNewA)
                {
                    sheet->write(rowIdx, colIdx, static_cast<double>(a), (rowSubIdx%2) ? fmtRowAofOdd : fmtRowAofEven);
                    firstRowForNewA = false;
                }
                else
                {
                    sheet->write(rowIdx, colIdx, "", (rowSubIdx%2) ? fmtRowAofOdd : fmtRowAofEven);
                }
                sheet->write(rowIdx, colIdx+1, algorithm->shortName(), (rowSubIdx % 2) ?  fmtRowAofOdd : fmtRowAofEven);

                tmpColIdx = colIdx+2;

                //E(X) Prawdopobieństwo, że w najbardziej obciążonej grupie dostępnych jest n zasobów
                int startColIdx = tmpColIdx;
                for (int n=0; n <= vMax; n++)
                {

                    double value;
                    (*pointResults)->read(value, TypeResourcess_VsServerGroupsCombination::FreeAUsInEveryGroup, n, combIndex);
                    sheet->write(rowIdx, tmpColIdx, value, (rowSubIdx % 2) ? fmtRowValOdd : fmtRowValEven);
                    tmpColIdx++;
                }
                chartMin->addSeries(QXlsx::CellRange(rowIdx, startColIdx, rowIdx, tmpColIdx-1));

                //E(X) Prawdopobieństwo, że w najmniej obciążonej grupie dostępnych jest n zasobów
                startColIdx = tmpColIdx;
                for (int n=0; n <= vMax; n++)
                {
                    double value;
                    (*pointResults)->read(value, TypeResourcess_VsServerGroupsCombination::FreeAUsInBestGroup, n, combIndex);
                    sheet->write(rowIdx, tmpColIdx, value, (rowSubIdx % 2) ? fmtRowValOdd : fmtRowValEven);
                    tmpColIdx++;
                }
                chartMax->addSeries(QXlsx::CellRange(rowIdx, startColIdx, rowIdx, tmpColIdx-1));

                if (pointResults->hasConfidencyIntervall())
                {   //± Prawdopobieństwo, że w najbardziej obciążonej grupie dostępnych jest n zasobów
                    for (int n=0; n <= vMax; n++)
                    {
                        double value;
                        pointResults->getConfidencyIntervall().read(value, TypeResourcess_VsServerGroupsCombination::FreeAUsInEveryGroup, n, combIndex);
                        sheet->write(rowIdx, tmpColIdx, value, (rowSubIdx % 2) ? fmtRowValOdd : fmtRowValEven);
                        tmpColIdx++;
                    }
                    //± Prawdopobieństwo, że w najmniej obciążonej grupie dostępnych jest n zasobów
                    for (int n=0; n <= vMax; n++)
                    {
                        double value;
                        pointResults->getConfidencyIntervall().read(value, TypeResourcess_VsServerGroupsCombination::FreeAUsInBestGroup, n, combIndex);
                        sheet->write(rowIdx, tmpColIdx, value, (rowSubIdx % 2) ? fmtRowValOdd : fmtRowValEven);
                        tmpColIdx++;
                    }
                }
                rowIdx++;
            }
        }
        combIndex++;
        rowIdx++;
    }
    colIdx = tmpColIdx;
}

void resultsXlsx::SaveGroupsAvailability(QString fileName)
{
    QXlsx::Document xslxResults;
    QString sheetName;
    QString choosenSheetName;
    QXlsx::Worksheet *sheet;
    int rowNo = 1;
    int colNo = 1;

    const ModelSyst& system = results->getModel();
    int k = system.k_s();
    if (k <= 1)
        return;

    sheetName = "Distr. of available subgr";
    xslxResults.addSheet(sheetName);
    xslxResults.selectSheet(sheetName);
    sheet= static_cast<QXlsx::Worksheet *>(xslxResults.sheet(sheetName));
    rowNo = 1;
    colNo = 1;
    spreadshetWriteAUsAvailabilityOnlyForGroupSets(sheet, rowNo, colNo);
    choosenSheetName = sheetName;

    sheetName = "Availability in combinations";
    xslxResults.addSheet(sheetName);
    xslxResults.selectSheet(sheetName);
    sheet= static_cast<QXlsx::Worksheet *>(xslxResults.sheet(sheetName));
    rowNo = 1;
    colNo = 1;
    spreadshetWriteAUsAvailabilityForGroupCombinations(sheet, rowNo, colNo);


    sheetName = "Inavailability in combinations";
    xslxResults.addSheet(sheetName);
    xslxResults.selectSheet(sheetName);
    sheet= static_cast<QXlsx::Worksheet *>(xslxResults.sheet(sheetName));
    rowNo = 1;
    colNo = 1;
    spreadshetWriteAUsInavailabilityForGroupCombinations(sheet, rowNo, colNo);

    sheetName = "Availability in best groups";
    xslxResults.addSheet(sheetName);
    xslxResults.selectSheet(sheetName);
    sheet= static_cast<QXlsx::Worksheet *>(xslxResults.sheet(sheetName));
    rowNo = 1;
    colNo = 1;
    spreadshetWriteAUsAvailabilityForBestGroupSets(sheet, rowNo, colNo);

    sheetName = "Free AUs in combinations";
    xslxResults.addSheet(sheetName);
    xslxResults.selectSheet(sheetName);
    sheet= static_cast<QXlsx::Worksheet *>(xslxResults.sheet(sheetName));
    rowNo = 1;
    colNo = 1;
    spreadshetWriteAUsFreeForGroupCombinations(sheet, rowNo, colNo);

    xslxResults.selectSheet(choosenSheetName);

    xslxResults.saveAs(fileName);
}

void resultsXlsx::SaveAll(QString fileName)
{
    SaveGroupsAvailability(fileName);
    SaveQoS_ForClasses(fileName);
}

void resultsXlsx::SaveQoS_ForClasses(QString fileName)
{
    QXlsx::Document xslxResults;
    QString sheetName;
    QXlsx::Worksheet *sheet;
    int rowNo = 1;
    int colNo = 1;

    sheetName = "QoS for classes";
    xslxResults.addSheet(sheetName);
    xslxResults.selectSheet(sheetName);
    sheet= static_cast<QXlsx::Worksheet *>(xslxResults.sheet(sheetName));
    rowNo = 1;
    colNo = 1;
    spreadshetWriteQoS_ForClasses(sheet, rowNo, colNo);

    xslxResults.saveAs(fileName);
}
