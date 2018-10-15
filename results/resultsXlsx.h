#ifndef RESULTSXLS_H
#define RESULTSXLS_H


#include <QString>

#include <xlsxdocument.h>
#include <xlsxcellrange.h>
#include <xlsxchart.h>

#include "results/resultsSystem.h"

class resultsXlsx
{
private:
    Results::RSystem *results;

    QXlsx::Format fmtHdrQoSparOdd;
    QXlsx::Format fmtHdrQoSparEven;

    QXlsx::Format fmtHdrEx;
    QXlsx::Format fmtHdrDx;

    QXlsx::Format fmtHdrParOdd;
    QXlsx::Format fmtHdrParEven;

    QXlsx::Format fmtTrClass;
    QXlsx::Format fmtDistribution;



    QXlsx::Format fmtRowAofOdd;
    QXlsx::Format fmtRowAofEven;

    QXlsx::Format fmtRowValOdd;
    QXlsx::Format fmtRowValEven;



    //Wyniki dla klas oraz zasobów
    void _spreadshetWriteForGroupCombinations(QXlsx::Worksheet *sheet, int &rowIdx, int &colIdx
          , QString descriptionTableAUs, QString descriptionChartAUs, QString descriptionTableClasses, QString descriptionChartClasses, TypeStateForServerGroupsCombination typeForState, TypeClassForServerGroupsCombination typeForClass);
    /**
     * @brief Rozkład dostęþnych podgrup
     * @param sheet arkusz
     * @param rowIdx pierwszy wiersz
     * @param colIdx pierwsza kolumna
     */
    void spreadshetWriteAUsAvailabilityOnlyForGroupSets(QXlsx::Worksheet *sheet, int &rowIdx, int &colIdx);

    //Wyniki dla klas oraz zasobów
    void spreadshetWriteAUsAvailabilityForBestGroupSets(QXlsx::Worksheet *sheet, int &rowIdx, int &colIdx);

    //Wyniki dla klas oraz zasobów
    void spreadshetWriteAUsAvailabilityForGroupCombinations(QXlsx::Worksheet *sheet, int &rowIdx, int &colIdx);

    //Wyniki dla klas oraz zasobów
    void spreadshetWriteAUsInavailabilityForGroupCombinations(QXlsx::Worksheet *sheet, int &rowIdx, int &colIdx);

    //Wyniki dla zasobów
    void spreadshetWriteAUsFreeForGroupCombinations(QXlsx::Worksheet *sheet, int &rowIdx, int &colIdx);

    void spreadshetWriteQoS_ForClasses(QXlsx::Worksheet *sheet, int &rowIdx, int &colIdx);

public:
    resultsXlsx(RSystem *systemResults);

    void SaveAll(QString fileName);
    void SaveQoS_ForClasses(QString fileName);

    void SaveGroupsAvailability(QString fileName);
};

#endif // RESULTSXLS_H
