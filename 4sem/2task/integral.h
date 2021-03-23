#pragma once

#define _GNU_SOURCE      

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <malloc.h>       
#include <sched.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/sysinfo.h>

// The maximum number of threads
#define MAX_THREADS 256

#define min(a, b) ((a) > (b)) ? (b) : (a)

double integrate(double (*function)(double x), double a, double b);
double thread_integrate(double (*function)(double x), double a, double b, unsigned int nthreads);