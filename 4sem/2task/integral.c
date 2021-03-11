#include "integral.h"

// The variable that will store results of each thread work
double sums[MAX_THREADS] = {0};

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

struct thread_data {
    double a; // the start of the length
    double b; // the end of the length
    double (*function)(double x); // the function to be integrated
    size_t num;
};

void* threadFunction(void* thread_data) {
    struct thread_data* thread_data_t = (struct thread_data*) thread_data;
    unsigned int mesh = (int)(thread_data_t->b - thread_data_t->a) * 10; // number of lengths for numerical integration
    double eps = 1e-06; // the accuracy of calculations
    double h = (thread_data_t->b - thread_data_t->a) / mesh;
    double result = 0;
    double x = thread_data_t->a + h; // the sequency of arguments

    while (x - eps < thread_data_t->b) {
        result += 4 * thread_data_t->function(x);
        x += h;
        if (x + eps >= thread_data_t->b) // if the argument is out of the given range
            break;
        result += 2 * thread_data_t->function(x);
        x += h;
    }

    result = (h / 3) * (thread_data_t->function(thread_data_t->a) + result + thread_data_t->function(thread_data_t->b));
    if (result > -eps && result < eps)
        result = 0;

    sums[thread_data_t->num] = result;
    pthread_exit(EXIT_SUCCESS);
}

double thread_integrate(double (*function)(double x), double a, double b, unsigned int nthreads) {
    struct thread_data* thread_data[nthreads];
    double diff = (b - a) / nthreads;
    double result = 0;
    pthread_t thread_ids[nthreads];

    for (int i = 0; i < nthreads; ++i) {
        thread_data[i] = (struct thread_data*) memalign(PAGE_SIZE, sizeof(struct thread_data));
        if (thread_data[i] == NULL)
            return -1;
  
        thread_data[i]->a = a + i * diff;
        thread_data[i]->b = a + (i + 1) * diff;
        thread_data[i]->function = function;
        thread_data[i]->num = i;
        
        pthread_create(&thread_ids[i], NULL, threadFunction, thread_data[i]);
    }

    for (int i = 0; i < nthreads; ++i) {
        pthread_join(thread_ids[i], NULL);
        result += sums[i];
        free(thread_data[i]);
    }

    return result;
}