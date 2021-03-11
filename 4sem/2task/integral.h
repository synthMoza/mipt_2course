#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <malloc.h>

// The size of the memory page
#define PAGE_SIZE 4096
#define MAX_THREADS 16

double integrate(double (*function)(double x), double a, double b);
double thread_integrate(double (*function)(double x), double a, double b, unsigned int nthreads);