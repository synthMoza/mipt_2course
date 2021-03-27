#include "integral.h"

#include "input.h"
#include <math.h>
#include <unistd.h>
#include <sys/time.h>

// Function to integrate
// @param x the argument of the function
double function(double x) {
    double f = sin(log10(x)) + 3 * cos(x) - x;
    return f;
}

int main(int argc, char* argv[]) {
    float result = 0;
    long num_threads = input(argc, argv);

    if (num_threads < 1) {
        printf("Wrong threads number!\n");
        return EXIT_FAILURE;
    }

    result = thread_integrate(function, 1, 10e6, num_threads);

    printf("================================\n");
    printf("Number of threads: %ld\n", num_threads);
    printf("The result: %g\n", result);
    
    return EXIT_SUCCESS;
}