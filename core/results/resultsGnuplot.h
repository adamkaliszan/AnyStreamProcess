#ifndef RESULTSGNUPLOT_H
#define RESULTSGNUPLOT_H

#include <boost/tuple/tuple.hpp>

#include <QList>


#include "results/resultsApi.h"
#include "results/resultsSystem.h"

#include "algorithms/investigator.h"

enum class terminalType
{
    wxt,
    postscript,
    epslatex
};

class GnuplotScript
{
    friend std::ostream& operator<<(std::ostream& out, const GnuplotScript& gnuScript);
    friend QTextStream& operator<<(QTextStream& out, const GnuplotScript& gnuScript);

private:
    //Gnuplot *gp;

    QString fontName;
    int     fontSize;


public:
    Results::RSystem *systemResults;


    GnuplotScript();

    bool    plotCi;
    bool    plotWithColors;

    void setFontName(QString fontName)   { this->fontName = fontName; }
    const char *getFontName() const      { return fontName.toUtf8().constData(); }
    void setFontSize(int size)           { this->fontSize = size; }
    int getFontSize() const              { return fontSize; }

    void WriteDataAndScript(QString baseFileNameWithPath, const ModelCreator *system, Settings *setting, Type qosType);
    void WriteDataAndScript3d(QString baseFileNameWithPath, const ModelCreator *system, Settings *setting, Type qosType, QList<ParametersSet> parameters, QList<Investigator *> algorithms, int logScale=1, int showZeros = 0);
};

#endif // RESULTSGNUPLOT_H
