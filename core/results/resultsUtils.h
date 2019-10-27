#ifndef RESULTS_UTILS_H
#define RESULTS_UTILS_H

#include <QVector>

namespace Results {

class Utils
{
public:
    template<typename T> static QVector<T> &addElementsToFirst(QVector<T> &lho, const QVector<T>&rho);
    template<typename T> static QVector<T> addElements(const QVector<T> &lho, const QVector<T>&rho);

    template<typename T> static QVector<T>& subtractElementsToFirst(QVector<T> &lho, const QVector<T>&rho);
    template<typename T> static QVector<T> subtractElements(const QVector<T> &lho, const QVector<T>&rho);

    template<typename T> static QVector<T>& multiplyElementToFirst(QVector<T> &lho, const QVector<T>&rho);
    template<typename T> static QVector<T> multiplyElement(const QVector<T> &lho, const QVector<T>&rho);

    template<typename T> static QVector<T>& divideElementToFirst(QVector<T> &lho, const QVector<T>&rho);
    template<typename T> static QVector<T> divideElement(const QVector<T> &lho, const QVector<T>&rho);

    template<typename T> static QVector<T>& multiplyElementToFirst(QVector<T> &lho, double rho);
    template<typename T> static QVector<T> multiplyElement(const QVector<T> &lho, double rho);

    template<typename T> static QVector<T>& divideElementToFirst(QVector<T> &lho, double rho);
    template<typename T> static QVector<T> divideElement(const QVector<T> &lho, double rho);

    static QVector<double>& powerElementToFirst(QVector<double> &lho, double rho);
    static QVector<double> powerElement(const QVector<double> &lho, double rho);
    template<typename T> static QVector<T> powerElementTempl(const QVector<T> &lho, double rho);

    static void sqrt(QVector<double>& vector);
    template<typename T> static void sqrtTemplate(QVector<T>& vector);
};

template<typename T>
QVector<T>& Utils::addElementsToFirst(QVector<T> &lho, const QVector<T> &rho)
{
    if (lho.length() != rho.length())
        qFatal("Vector has different lengths");

    for(int index=0; index<lho.length(); index++)
        lho[index]+= rho[index];

    return lho;
}

template<typename T>
QVector<T> Utils::addElements(const QVector<T> &lho, const QVector<T> &rho)
{
    if (lho.length() != rho.length())
        qFatal("Vector has different lengths");

    QVector<T> result;
    result.resize(lho.size());

    for(int index=0; index<lho.length(); index++)
        result[index] = lho[index] + rho[index];

    return result;
}

template<typename T>
QVector<T> &Utils::subtractElementsToFirst(QVector<T> &lho, const QVector<T> &rho)
{
    if (lho.length() != rho.length())
        qFatal("Vector has different lengths");

    for(int index=0; index<lho.length(); index++)
        lho[index]-= rho[index];

    return lho;
}

template<typename T>
QVector<T> Utils::subtractElements(const QVector<T> &lho, const QVector<T> &rho)
{
    if (lho.length() != rho.length())
        qFatal("Vector has different lengths");

    QVector<T> result;
    result.resize(lho.size());

    for(int index=0; index<lho.length(); index++)
        result[index] = lho[index] - rho[index];

    return result;
}


template<typename T>
QVector<T> &Utils::multiplyElementToFirst(QVector<T> &lho, const QVector<T> &rho)
{
    if (lho.length() != rho.length())
        qFatal("Vector has different lengths");

    for(int index=0; index<lho.length(); index++)
        lho[index]*= rho[index];

    return lho;
}

template<typename T>
QVector<T> Utils::multiplyElement(const QVector<T> &lho, const QVector<T> &rho)
{
    if (lho.length() != rho.length())
        qFatal("Vector has different lengths");

    QVector<T> result;
    result.resize(lho.size());

    for(int index=0; index<lho.length(); index++)
        result[index] = lho[index] * rho[index];

    return result;
}

template<typename T>
QVector<T> &Utils::divideElementToFirst(QVector<T> &lho, const QVector<T> &rho)
{
    if (lho.length() != rho.length())
        qFatal("Vector has different lengths");

    for(int index=0; index<lho.length(); index++)
        lho[index]/= rho[index];

    return lho;
}

template<typename T>
QVector<T> Utils::divideElement(const QVector<T> &lho, const QVector<T> &rho)
{
    if (lho.length() != rho.length())
        qFatal("Vector has different lengths");

    QVector<T> result;
    result.resize(lho.size());

    for(int index=0; index<lho.length(); index++)
        result[index] = lho[index] / rho[index];

    return result;
}

template<typename T>
QVector<T> &Utils::multiplyElementToFirst(QVector<T> &lho, double rho)
{
    for(int index=0; index<lho.length(); index++)
        lho[index]*= rho;

    return lho;
}

template<typename T>
QVector<T> Utils::multiplyElement(const QVector<T> &lho, double rho)
{
    QVector<T> result;
    result.resize(lho.size());

    for(int index=0; index<lho.length(); index++)
        result = lho[index] * rho;

    return lho;
}

template<typename T>
QVector<T> &Utils::divideElementToFirst(QVector<T> &lho, double rho)
{
    for(int index=0; index<lho.length(); index++)
        lho[index]/= rho;

    return lho;
}

template<typename T>
QVector<T> Utils::divideElement(const QVector<T> &lho, double rho)
{
    QVector<T> result;
    result.resize(lho.size());

    for(int index=0; index<lho.length(); index++)
        result = lho[index] / rho;

    return lho;
}

QVector<double> &Utils::powerElementToFirst(QVector<double> &lho, double rho)
{
    for(int index=0; index<lho.length(); index++)
        lho[index] = qPow(lho[index], rho);

    return lho;
}

QVector<double> Utils::powerElement(const QVector<double> &lho, double rho)
{
    QVector<double> result;
    result.resize(lho.size());

    for(int index=0; index<lho.length(); index++)
        result[index] = qPow(lho[index], rho);

    return lho;
}

template<typename T>
QVector<T> Utils::powerElementTempl(const QVector<T> &lho, double rho)
{
    QVector<T> result;
    result.resize(lho.size());

    for(int index=0; index<lho.length(); index++)
        result[index] = lho[index].pow(rho);

    return lho;
}

void Utils::sqrt(QVector<double> &vector)
{
    for(int index=0; index<vector.length(); index++)
        vector[index] = qSqrt(vector[index]);
}

template<typename T>
void Utils::sqrtTemplate(QVector<T> &vector)
{
    for(int index=0; index<vector.length(); index++)
        vector[index].sqrt();
}

} //namespace Results

#endif // RESULTSUTILS_H
