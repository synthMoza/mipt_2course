#include "integral.h"

#include "input.h"
#include <math.h>
#include <unistd.h>
#include <sys/time.h>

// Elapse time that this expression takes
// Variables needed: clock_t start, end
// Result: time_used
#define ELAPSE(EXP)                                                                     \
do {                                                                                    \
    gettimeofday(&timecheck, NULL);                                                     \
    start = (long) timecheck.tv_sec * 1e06 + (long) timecheck.tv_usec;                  \
    EXP;                                                                                \
    gettimeofday(&timecheck, NULL);                                                     \
    end = (long) timecheck.tv_sec * 1e06 + (long) timecheck.tv_usec;                    \
    time_used = (end - start);                                                          \
} while (0)                                                                             \

// Function to integrate
// @param x the argument of the function
double function(double x) {
    double f = sin(x) * x + log10(x);
    return f;
}

int main(int argc, char* argv[]) {
    float result = 0;
    long start, end;
    struct timeval timecheck;
    double time_used;
    //long num_threads = input(argc, argv);

    for (int i = 1; i < 12; ++i) {
        ELAPSE(result = thread_integrate(function, 1, 5000, i));

        //printf("Number of thread: %d\n", i);
        //printf("The result: %g\n", result);
        //printf("Elapsed time: %g us\n", time_used);
        printf("%g us\n", time_used);
    
    }
    return EXIT_SUCCESS;
}