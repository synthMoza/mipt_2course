#include "integral.h"
#include <time.h>
#include <math.h>

// Elapse time that this expression takes
// Variables needed: clock_t start, end
// Result: time_used
#define ELAPSE(EXP)                                             \
do {                                                            \
    start = clock();                                            \
    EXP;                                                        \
    end = clock();                                              \
    time_used = ((double) (end - start)) / CLOCKS_PER_SEC;      \
} while (0)                                                     \

// Function to integrate
// @param x the argument of the function
double function(double x) {
    double f = sin(x) * x + log10(x);
    return f;
}

int main() {
    float result = 0;
    clock_t start, end;
    double time_used;

    ELAPSE(result = integrate(function, 1, 5000));

    printf("The result: %g\n", result);
    printf("Elapsed time: %g ms\n", time_used * 1000);
    
    return EXIT_SUCCESS;
}