#include "integral.h"

// Integrate the given function on the given length ()
// @param function function to be integrated
// @param a the start of the length
// @param b the end of the length
double integrate(double (*function)(double x), double a, double b) {
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

// Prepare data structures for each active thread
void wr_data_active_threads(struct thread_data** thread_data, int nthreads, double a, double b, double (*function)(double x)) {
    int page_size = getpagesize();  // the size of memory page
    double diff = (b - a) / nthreads;
    // Write data for each active thread
    for (int i = 0; i < nthreads; ++i) {
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
}

// Prepare data structures for each dummy thread
void wr_data_dummy_threads(struct thread_data** thread_data, int nthreads, int ndummythreads, double a, double b, double (*function)(double x)) {
    int page_size = getpagesize();  // the size of memory page
    double diff = (b - a) / nthreads;
    for (int j = nthreads; j < nthreads + ndummythreads; ++j) {
        thread_data[j] = (struct thread_data*) memalign(page_size, sizeof(struct thread_data));
        if (thread_data[j] == NULL) {
            printf("Error! Can't allocate memory for thread data number %d!\n", j);
            exit(EXIT_FAILURE);
        }
  
        thread_data[j]->a = a + (j - nthreads) * diff;
        thread_data[j]->b = a + (j - nthreads + 1) * diff;
        thread_data[j]->function = function;
    }
}

double thread_integrate(double (*function)(double x), double a, double b, unsigned int nthreads) {
    struct thread_data** thread_data = NULL;
    cpu_set_t cpu_set;
    double result = 0;
    pthread_t* thread_ids = NULL;
    int ret = 0;
    pthread_attr_t* pthread_attr = NULL; // attributes of creating threads
    int i = 0;
    int ndummythreads = 0; // number of threads to do useless calculations

    // Get information about the system
    int nproc = get_nprocs(); // maximum number of threads

    // Pin the main thread to Core 0
    CPU_ZERO(&cpu_set);
    CPU_SET(0, &cpu_set);
    pthread_setaffinity_np(pthread_self(), sizeof(cpu_set), &cpu_set);

    // Other avalible cores will be filled
    if (nthreads < nproc)
        ndummythreads = nproc - nthreads;

    // Allocate space for threads data and attributes (and dummy threads too)
    pthread_attr = (pthread_attr_t*) calloc(nthreads + ndummythreads, sizeof(*pthread_attr));
    thread_data = (struct thread_data**) calloc(nthreads + ndummythreads, sizeof(*thread_data));
    thread_ids = (pthread_t*) calloc(nthreads + ndummythreads, sizeof(*thread_ids));

    // Write data for each active and dummy thread
    wr_data_active_threads(thread_data, nthreads, a, b, function);
    wr_data_dummy_threads(thread_data, nthreads, ndummythreads, a, b, function);

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

    int dummy_index = nthreads; // the first dummy thread index
    // If there are any threads left, put them on other cores (and add dummy threads)
    if (dnthreads < nthreads) {
        for (i = dnthreads; i < nthreads; ++i) {
            // Put an active thread on the core
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
                printf("Error setting affinity attributes for active threads!\n");
                printf("return - %d\n", ret);
                exit(EXIT_FAILURE);
            }

            // Put an dummy thread on that PSYSICAL core too
            // and set its state to detachable
            ret = pthread_attr_init(&pthread_attr[dummy_index]);
            if (ret != 0) {
                printf("Error getting default attributes for threads!\n");
                printf("return - %d\n", ret);
                exit(EXIT_FAILURE);
            }
            
            CPU_ZERO(&cpu_set);
            CPU_SET(dnthreads + 2 * (i - dnthreads) + 1, &cpu_set);

            ret = pthread_attr_setaffinity_np(&pthread_attr[dummy_index], sizeof(cpu_set), &cpu_set);
            if (ret != 0) {
                printf("Error setting affinity attributes for dummy threads!\n");
                printf("return - %d\n", ret);
                exit(EXIT_FAILURE);
            }
            ret = pthread_attr_setdetachstate(&pthread_attr[dummy_index], PTHREAD_CREATE_DETACHED);
            if (ret != 0) {
                printf("Error setting detache state attributes for dummy threads!\n");
                printf("return - %d\n", ret);
                exit(EXIT_FAILURE);
            }

            dummy_index++;
        }
    }

    // If there are any dummy threads left, put them on all other cores
    if (dummy_index < nthreads + ndummythreads && ndummythreads != 0) {
        for (i = dummy_index; i < ndummythreads + nthreads; ++i) {
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

    // Launch all other threads except the main one
    for (i = 1; i < nthreads + ndummythreads; ++i) {    
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

    // Collect results from all active threads
    result += thread_data[0]->result;
    free(thread_data[0]);

    for (i = 1; i < nthreads; ++i) {
        pthread_join(thread_ids[i], NULL);
        
        result += thread_data[i]->result;
        free(thread_data[i]);
    }

    // Wait for dummy threads
    for (int i = nthreads; i < nthreads + ndummythreads; ++i) {
        //pthread_join(thread_ids[i], NULL);
        free(thread_data[i]);
    }

    free(pthread_attr);
    free(thread_data);
    free(thread_ids);

    return result;
}