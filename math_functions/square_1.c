/* kompilować z użyciem: 
gcc -fPIC -c square_1.c -o square_1.o
gcc -shared -o square_1.so square_1.o
*/

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

// Definicja funkcji matematycznej: f(x) = x^2 - 2
double math_function(double x) {
    return x * x - 2;
}

#ifdef __cplusplus
}
#endif
