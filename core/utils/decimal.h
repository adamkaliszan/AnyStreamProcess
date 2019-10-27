#ifndef UTILS_DECIMAL_H
#define UTILS_DECIMAL_H

#include <boost/multiprecision/cpp_dec_float.hpp>

#include <QMetaType>

typedef boost::multiprecision::number<boost::multiprecision::cpp_dec_float<10>> decimal;

Q_DECLARE_METATYPE(decimal)

#endif // UTILS_DECIMAL_H
