#ifndef ALGORITHMCONVGAMMA_H
#define ALGORITHMCONVGAMMA_H

#include <QMetaType>
#include "algorithms/investigator.h"
#include "algorithms/trclvector2.h"

namespace Algorithms
{

/**
 * @brief The convolutionAlgorithmGamma: uogólniony algorytm splotowy
 */
class convolutionAlgorithmGamma3 : public Investigator
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
        const ModelTrClass **trClasses;

        double *states;
        double **ySYSTEM;

    public:
        VectQEUE(): Vs(0), Vb(0), VsVb(0), m(0), loc2globIdx(NULL), trClasses(NULL), states(NULL), ySYSTEM(NULL) {}
        VectQEUE(int Vs, int Vb, int m, int i, const ModelTrClass *trClass, double A);
        ~VectQEUE();

        double get_y(int globIdx, int n) const;
        inline double getState(int n);

        void fillGamma(double **gamma) const;
        void agregate(VectQEUE *A, const VectQEUE *B);
        void clone(const VectQEUE *A);
    };

private:
    VectQEUE **p_single;

public:
    convolutionAlgorithmGamma3();

    void deleteTemporaryData();

    QString shortName() {return "conv gamma 3"; }
    virtual int complexity() {return 1; }

    void calculateSystem(const ModelCreator &system, double a, Results::RInvestigator *results, SimulationParameters *simParameters);

    void prepareTemporaryData(const ModelCreator *system, double a);
    bool possible(ModelCreator *system);
};

}

Q_DECLARE_METATYPE(Algorithms::convolutionAlgorithmGamma3*)


#endif // ALGORITHMCONVGAMMA_H
