#ifndef RESULTSGNUPLOT_H
#define RESULTSGNUPLOT_H

#include <boost/tuple/tuple.hpp>

#include "results/resultsApi.h"
#include "results/resultsSystem.h"

#include "algorithms/investigator.h"
#include "gnuplot-iostream.h"

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
    Gnuplot *gp;

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

    void WriteDataAndScript(QString baseFileName, const ModelSyst *system, Settings *setting, Type qosType);
};

#endif // RESULTSGNUPLOT_H
