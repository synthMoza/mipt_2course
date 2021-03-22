#include "integral.h"

// The variable that will store results of each thread work
double sums[MAX_THREADS] = {0};

// Integrate the given function on the given length ()
// @param function function to be integrated
// @param a the start of the length
// @param b the end of the length
double integrate(double (*function)(double x), double a, double b) {
    unsigned int mesh = (int)(b - a) * 20; // number of lengths for numerical integration
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
    double* result;
};

void* threadFunction(void* thread_data) {
    struct thread_data* thread_data_t = (struct thread_data*) thread_data;
    *(thread_data_t->result) = integrate(thread_data_t->function, thread_data_t->a, thread_data_t->b);
    pthread_exit(EXIT_SUCCESS);
}

double thread_integrate(double (*function)(double x), double a, double b, unsigned int nthreads) {
    struct thread_data* thread_data[nthreads];
    cpu_set_t cpu_set;
    double diff = (b - a) / nthreads;
    double result = 0;
    pthread_t thread_ids[nthreads];
    int ret = 0;
    int nproc = get_nprocs();
    int page_size = getpagesize();
    pthread_attr_t pthread_attr; // attributes of creating threads

    for (int i = 0; i < nthreads; ++i) {
        thread_data[i] = (struct thread_data*) memalign(page_size * 2, sizeof(struct thread_data));
        if (thread_data[i] == NULL) {
            printf("Error! Can't allocate memory for thread data number %d!\n", i);
            exit(EXIT_FAILURE);
        }
  
        thread_data[i]->a = a + i * diff;
        thread_data[i]->b = a + (i + 1) * diff;
        thread_data[i]->function = function;
        thread_data[i]->result = sums + i;
    }

    for (int i = 0; i < nthreads; ++i) {    
        if (i < nproc) {
            // If the number of threads is less than the amount of processors
            CPU_ZERO(&cpu_set);
            CPU_SET((i * 2) % nproc , &cpu_set);
            ret = pthread_attr_init(&pthread_attr);
            if (ret != 0) {
                printf("Error getting default attributes for threads!\n");
                printf("return - %d\n", ret);
                exit(EXIT_FAILURE);
            }

            // Link threads to cores
            ret = pthread_attr_setaffinity_np(&pthread_attr, sizeof(cpu_set), &cpu_set);
            if (ret != 0) {
                printf("Error setting affinity attributes for threads!\n");
                printf("return - %d\n", ret);
                exit(EXIT_FAILURE);
            }

            ret = pthread_create(&thread_ids[i], &pthread_attr, threadFunction, thread_data[i]);
            if (ret != 0) {
                printf("Error creating thread!\n");
                printf("return - %d\n", ret);
                exit(EXIT_FAILURE);
            }
            
            pthread_attr_destroy(&pthread_attr);
        } else {
            // Other threads which we do not link to certain core
            ret = pthread_create(&thread_ids[i], NULL, threadFunction, thread_data[i]);
            if (ret != 0) {
                printf("Error creating thread!\n");
                printf("return - %d\n", ret);
                exit(EXIT_FAILURE);
            }
        }
    }

    for (int i = 0; i < nthreads; ++i) {
        pthread_join(thread_ids[i], NULL);
        result += sums[i];
        free(thread_data[i]);
    }

    return result;
}