#ifndef ALGORITHMCONVGAMMA2D_H
#define ALGORITHMCONVGAMMA2D_H

#include <QMetaType>
#include "algorithms/trclvector2.h"

/**
 * @brief The convolutionAlgorithmGamma: uogólniony algorytm splotowy
 */
/*
class convolutionAlgorithmGamma2d : public algorithm
{
protected:
    class VectQEUE
    {
    private:
        int Vs;
        int Vb;
        int VsVb;
        int m;

        int *loc2globIdx;
        modelTrClass **trClasses;

        state **states;
        double **ySYSTEM;

    public:
        VectQEUE(): Vs(0), Vb(0), VsVb(0), m(0), loc2globIdx(NULL), trClasses(NULL), states(NULL), ySYSTEM(NULL) {}
        VectQEUE(int Vs, int Vb, int m, int i, modelTrClass *trClass, double A);
        ~VectQEUE();

        double get_y(int globIdx, int n) const;
        inline double getState(int n_s, int n_q);

        void fillGamma(double **gamma, double **gammaOther, int rowNo) const;
        void agregate(VectQEUE *A, const VectQEUE *B);
        void clone(const VectQEUE *A);
    };

private:
    VectQEUE **p_single;

public:
    convolutionAlgorithmGamma2d();

    void deleteTemporaryData();

    QString shortName() {return "conv gamma"; }
    virtual int complexity() {return 1; }

    void calculateSystem(modelSyst *system, double a, algorithmResults *algRes, simulationParameters *simParameters);

    void prepareTemporaryData(modelSyst *system, double a);
};
Q_DECLARE_METATYPE(convolutionAlgorithmGamma2d*)

*/
#endif // ALGORITHMCONVGAMMA_H
