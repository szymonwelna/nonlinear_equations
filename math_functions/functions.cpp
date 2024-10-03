/* można skompilować z użyciem: 
g++ -shared -o functions.so functions.cpp -I /home/szymon/cppProjects/rownania_nieliniowe/include/ ${Boost_LIBRARIES} ${MPFR_LIBRARIES} -fPIC
*/
#include <iostream>
#include "interval.h"
#include <functional>

using namespace interval_arithmetic;

// Funkcja pomocnicza do konwersji double na Interval<double>
Interval<double> make_interval(double value) {
    return Interval<double>(value, value);
}

// Definicje funkcji z atrybutem eksportu
extern "C" Interval<double> funkcja_kwadratowa(Interval<double> x) {
    return make_interval(4.0) * x * x + make_interval(3.0) * x - make_interval(9.0); // Funkcja f(x) = x^2 - 2
}

extern "C" Interval<double> funkcja_szescienna(Interval<double> x) {
    return make_interval(1.0) * x * x * x - make_interval(4.0) * x * x + make_interval(2.0) * x - make_interval(7.0); // Funkcja f(x) = x^3 + x^2 + x + 1
}

extern "C" Interval<double> funkcja_homograficzna(Interval<double> x) {
    return make_interval(1.0) / x; // Funkcja f(x) = 1/x
}
