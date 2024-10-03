/*kompilować z użyciem:
gcc -fPIC -c cube_4.c -o cube_4.o
gcc -shared -o cube_4.so cube_4.o

*/
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

// Definicja funkcji matematycznej: f(x) = x^3 + x^2 - 3x - 3
double math_function(double x) {
    return x * x * x + x * x - x - 3 * x - 3;
}

#ifdef __cplusplus
}
#endif
