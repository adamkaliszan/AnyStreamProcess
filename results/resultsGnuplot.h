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

    void WriteScript(QString scriptFileName, QString dataFileName, QString graphFileName, double minY, double maxY, Type QoS_MainParam);
    void WriteScript(QTextStream &script, QString dataFileName, QString graphFileName, Type QoS_MainParam, double minY, double maxY, bool isLogScale);
    void WriteData(QString dataFileName, double &minY, double &maxY, Type QoS_MainParam);
    void WriteData(QTextStream &scriptData, double &minY, double &maxY, Type type);

    void Show(Type QoS_par, QList<ModelTrClass *> selClasses, bool useColors);
};

QTextStream &operator<<(QTextStream &st, GnuplotScript &results);
std::ostream &operator<<(std::ostream &st, GnuplotScript &results);

#endif // RESULTSGNUPLOT_H
