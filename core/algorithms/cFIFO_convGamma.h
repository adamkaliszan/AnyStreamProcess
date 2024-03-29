#ifndef ALGORITHMCONVGAMMA2_H
#define ALGORITHMCONVGAMMA2_H

#include <QMetaType>
#include "algorithms/investigator.h"
#include "algorithms/trclvector2.h"

namespace Algorithms
{

/**
 * @brief The convolutionAlgorithmGamma: uogólniony algorytm splotowy
 */
class cFIFO_convGamma : public Investigator
{
public:
    cFIFO_convGamma();


    QString shortName()      const {return "conv gamma"; }
    virtual int complexity() const {return 1; }
    void calculateSystem(const ModelSystem &system
          , double a
          , Results::RInvestigator *results, SimulationParameters *simParameters
          );
    bool possible(const ModelSystem &system) const;

protected:
    class VectQEUE
    {
    private:
        int Vs;
        int Vb;
        int VsVb;
        int m;

        QVector<int> loc2globIdx;

        QVector<ModelTrClass> trClasses2;

        QVector<double> states;
        QVector<QVector<double>> ySYSTEM;

    public:
        VectQEUE();
        VectQEUE(int Vs, int Vb, int m, int i, const ModelTrClass &trClass);
        ~VectQEUE();

        void setStates(TrClVector &src);

        double get_y(int globIdx, int n) const;
        inline double getState(int n);

        void fillGamma(double **gamma, double **gammaOther, int rowNo) const;
        void agregate(const VectQEUE *A, const VectQEUE *B);
        void clone(const VectQEUE *A);
    };

private:
    QVector<VectQEUE *> p_single;

public:
    void deleteTemporaryData();
    void prepareTemporaryData(const ModelSystem &system, double a);
};

} // namespace Algorithms

Q_DECLARE_METATYPE(Algorithms::cFIFO_convGamma*)


#endif // ALGORITHMCONVGAMMA_H
