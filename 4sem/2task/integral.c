#include "integral.h"

// Integrate the given function on the given length ()
// @param function function to be integrated
// @param a the start of the length
// @param b the end of the length
double integrate(double (*function)(double x), double a, double b) {
    unsigned int mesh = (int)(b - a) * 10; // number of lengths for numerical integration
    double eps = 1e-06; // the accuracy of calculations
    double h = (b - a) / mesh;
    double result = 0;
    double x = a + h; // the sequency of arguments

    while (x - eps < b) {
        result += 4 * function(x);
        x += h;
        if (x + eps >= b) // if the argument is out of the given range
            break;
        result += 2 * function(x);
        x += h;
    }

    result = (h / 3) * (function(a) + result + function(b));
    if (result > -eps && result < eps)
        result = 0;

    return result; 
}

float thread_integrate(double (*function)(double x), double a, double b) {

    return 0;
}