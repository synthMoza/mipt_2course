#pragma once

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <malloc.h>       
#include <sched.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/sysinfo.h>

#define min(a, b) ((a) > (b)) ? (b) : (a)

// Integrate the function on the given length
// @param a the start of the length
// @param b the end of the length
double integrate(double a, double b);

// Integrate the function on the given length using threads
// @param a the start of the length
// @param b the end of the length
// @param nthreads the amount of threads to be launched
double thread_integrate(double a, double b, unsigned int nthreads);