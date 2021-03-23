#include "integral.h"


// Integrate the given function on the given length ()
// @param function function to be integrated
// @param a the start of the length
// @param b the end of the length
double integrate(double (*function)(double x), double a, double b) {
    while(1) {};
    unsigned int mesh = (int)(b - a) * 25; // number of lengths for numerical integration
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
    double result;
};

void* threadFunction(void* thread_data) {
    struct thread_data* thread_data_t = (struct thread_data*) thread_data;
    thread_data_t->result = integrate(thread_data_t->function, thread_data_t->a, thread_data_t->b);
    pthread_exit(EXIT_SUCCESS);
}

double thread_integrate(double (*function)(double x), double a, double b, unsigned int nthreads) {
    struct thread_data* thread_data[nthreads];
    cpu_set_t cpu_set;
    double diff = (b - a) / nthreads;
    double result = 0;
    pthread_t thread_ids[nthreads];
    int ret = 0;
    pthread_attr_t* pthread_attr = NULL; // attributes of creating threads
    int i = 0;

    // Get information about the system
    int nproc = get_nprocs(); // maximum number of threads
    int page_size = getpagesize();  // the size of memory page

    // Pin the main thread to Core 0
    CPU_ZERO(&cpu_set);
    CPU_SET(0, &cpu_set);
    pthread_setaffinity_np(pthread_self(), sizeof(cpu_set), &cpu_set);

    pthread_attr = (pthread_attr_t*) calloc(nthreads, sizeof(*pthread_attr));

    // Write data for each thread
    for (i = 0; i < nthreads; ++i) {
        thread_data[i] = (struct thread_data*) memalign(page_size, sizeof(struct thread_data));
        //thread_data[i] = (struct thread_data*) calloc(1, sizeof(struct thread_data));
        if (thread_data[i] == NULL) {
            printf("Error! Can't allocate memory for thread data number %d!\n", i);
            exit(EXIT_FAILURE);
        }
  
        thread_data[i]->a = a + i * diff;
        thread_data[i]->b = a + (i + 1) * diff;
        thread_data[i]->function = function;
    }
    
    // Create attributes for all threads
    int ncores = (nthreads - nproc / 2); // the number of cores to be linked with two or more threads
    if (ncores < 0)
        ncores = 0; // if ncores is less than zero, no threads must be put by two on one CPU
    int dnthreads = min(ncores * 2, nthreads); // threads to be put by two

    if (dnthreads != 0) {
        for (i = 0; i < dnthreads; ++i) {
            ret = pthread_attr_init(&pthread_attr[i]);
            if (ret != 0) {
                printf("Error getting default attributes for threads!\n");
                printf("return - %d\n", ret);
                exit(EXIT_FAILURE);
            }
            
            // Link threads to cores
            CPU_ZERO(&cpu_set);
            CPU_SET(i % nproc, &cpu_set);

            ret = pthread_attr_setaffinity_np(&pthread_attr[i], sizeof(cpu_set), &cpu_set);
            if (ret != 0) {
                printf("Error setting affinity attributes for threads!\n");
                printf("return - %d\n", ret);
                exit(EXIT_FAILURE);
            }
        }
    }

    // If there are any threads left, put them on other cores
    if (dnthreads < nthreads) {
        for (i = dnthreads; i < nthreads; ++i) {
            ret = pthread_attr_init(&pthread_attr[i]);
            if (ret != 0) {
                printf("Error getting default attributes for threads!\n");
                printf("return - %d\n", ret);
                exit(EXIT_FAILURE);
            }
            
            // Link threads to cores
            CPU_ZERO(&cpu_set);
            CPU_SET(dnthreads + 2 * (i - dnthreads), &cpu_set);

            ret = pthread_attr_setaffinity_np(&pthread_attr[i], sizeof(cpu_set), &cpu_set);
            if (ret != 0) {
                printf("Error setting affinity attributes for threads!\n");
                printf("return - %d\n", ret);
                exit(EXIT_FAILURE);
            }
        }
    }

    // Launch all other threads except the main one
    for (i = 1; i < nthreads; ++i) {    
            ret = pthread_create(&thread_ids[i], &pthread_attr[i], threadFunction, thread_data[i]);
            pthread_attr_destroy(&pthread_attr[i]);
            if (ret != 0) {
                printf("Error creating thread!\n");
                printf("return - %d\n", ret);
                exit(EXIT_FAILURE);
            }
    }

    // Launch the main thread
    thread_data[0]->result = integrate(thread_data[0]->function, thread_data[0]->a, thread_data[0]->b);

    // Collect results from all threads
    result += thread_data[0]->result;
    free(thread_data[0]);

    for (i = 1; i < nthreads; ++i) {
        pthread_join(thread_ids[i], NULL);
        
        result += thread_data[i]->result;
        free(thread_data[i]);
    }

    free(pthread_attr);
    return result;
}