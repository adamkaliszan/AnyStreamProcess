#ifndef TRCLVECTOR2_H
#define TRCLVECTOR2_H


#include <QList>
#include <string.h>
#include <QTextStream>
#include <QVector>

//#include "algorithms/alg_FAG_AnyStream.h"

class TrClVector;

class State
{
    friend class TrClVector;
public:
    double p;
    double tIntInNew;     /// Intensywność przejścia do stanu po przyjęciu nowego zgłoszenia
    double tIntInEnd;     /// Intensywność przejścia do stanu po przyjęciu nowego zgłoszenia
    double tIntOutNew;    /// Intensywność wyjścia ze stanu po przyjęciu nowego zgłoszenia
    double tIntOutEnd;    /// Intensywność wyjścia ze stanu po zakończeniu obsługi zgłoszenia

private:
    double *intInNewSC;   /// Intensywność wejścia do stanu po przyjęciu nowego zgłoszenia określonej klasy
    double *intInEndSC;   /// Intensywność wejścia do stanu po zakończeniu obsługi zgłoszenia określonej klasy
    double *intOutNewSC;  /// Intensywność wyjścia ze stanu po przyjęciu nowego zgłoszenia określonej klasy
    double *intOutEndSC;  /// Intensywność wyjścia ze stanu po zakończeniu obsługi zgłoszenia określonej klasy

    double *y;            /// Średnia liczba obsługiwanych zgłoszeń

    int m;
public:
    State();
    State(const State &rho);
    ~State();

    State &operator=(const State &rho);

    void addClassOnTheEnd();
};

struct pairIdT
{
    int id;
    int t;

    bool operator==(const pairIdT& a) const
    {
        return (id == a.id && t == a.t);
    }
};

class TrClVector
{
public:
    TrClVector *previous; //TODO rozdzielić na osobną klasę, która dziecziczy z tej klasy. Za dużo bałaganu, a przeszukiwanie listy jest neioptymalne

    QVector<pairIdT> aggregatedClasses;
private:

    QVector<State> _states;

public:

   // friend static trClVector AlgorithmAnyStreamFAG::convFAG(trClVector Pa, trClVector Pb, int V);
    friend QDebug operator<<(QDebug stream, TrClVector &distribution);


    TrClVector();
    TrClVector(const TrClVector &rho);
    TrClVector(int LastIdx);
    TrClVector(int V, const QVector<pairIdT> &aggregatedClasses);
    ~TrClVector();

    TrClVector &operator=(const TrClVector &rho);
    double & operator[](int n);

    void addClass(int id, int t);
    int internalClassId(int classId) const;
    int classId(int internalClassId)  const               {return aggregatedClasses[internalClassId].id; }

    static void prepareResult(TrClVector &result, const TrClVector &Pa, const TrClVector &Pb, int len);

    static TrClVector convFAG(const TrClVector &Pa, const TrClVector &Pb, bool doNormalization = false, int len = 0);
    static TrClVector convFAGanyStream(const TrClVector &P_A, const TrClVector &P_B, int len = 0);

    void normalize(double sumOfAllTheStates=1);
    void denormalize(double p0);
    void denormalize(double pn, int n);

    void generateNormalizedPoissonPrevDistrib();
    void generateDeNormalizedPoissonPrevDistrib();

    const TrClVector *getTruncatedVector(int lastIndex) const;

    int    V() const                                       { return  _states.length() -1; }
    int    m() const                                       { return aggregatedClasses.length(); }

    void    setState(int n, const State &refState);
    State & getState(int n);

    void    setIntInNew(int n, int i, double value);
    void    setIntInEnd(int n, int i, double value);
    void    setIntOutNew(int n, int i, double value);
    void    setIntOutEnd(int n, int i, double value);
    void    setY(int n, int i, double value);

    double  getIntInNew(int n, int i) const;
    double  getIntInEnd(int n, int i) const;
    double  getIntOutNew(int n, int i) const;
    double  getIntOutEnd(int n, int i) const;

    double  getY(int n, int i) const;

    //double getIntInNew(int n);                              /// Intensywność przejścia do stanu po przyjęciu nowego zgłoszenia
    //double getIntInEnd(int n);                              /// Intensywność przejścia do stanu po przyjęciu nowego zgłoszenia
    //static trClVector convQUEUE(const trClVector &Pa, const trClVector &Pb, int V, bool doNormalization = false, int len=0);
    //static double FAG2QEUE(double value, int n, int V);
    //static double QEUE2FAG(double value, int n, int V);
};

QDebug operator<<(QDebug &stream, TrClVector &distribution);

#endif // TRCLVECTOR2_H
