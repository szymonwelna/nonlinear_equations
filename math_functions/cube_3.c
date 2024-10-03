/*kompilować z użyciem:
gcc -fPIC -c cube_3.c -o cube_3.o
gcc -shared -o cube_3.so cube_3.o

*/
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

// Definicja funkcji matematycznej: f(x) = x^3 - x - 1
double math_function(double x) {
    return x * x * x - x - 1;
}

#ifdef __cplusplus
}
#endif
