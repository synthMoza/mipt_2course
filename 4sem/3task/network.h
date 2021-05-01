#pragma once

#define _GNU_SOURCE

#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/sysinfo.h>
#include <time.h>
#include <assert.h>
#include <string.h>
#include <ifaddrs.h>
#include <sys/select.h>
#include <fcntl.h>

#include "Thread_Integrate/integral.h"
#include "input.h"

#define check_return(ret, message)              \
do {                                            \
    if (ret < 0) {                              \
        perror("");                             \
        printf(message);                        \
        exit(EXIT_FAILURE);                     \
    }                                           \
} while (0)

#define MAX_MSG_SIZE 128
#define PORT 3456

// The minimum amout of processors for debug
#define MIN_PROC 4
// The amount of threads per each core of the CPU
#define THREADS_PER_CORE 2
// The number of the network interface
#define INTERFACE_NUMBER 1

// Structure that describes data to be sent to each computer
// @param a the start of the length to integrate on
// @param b the end of the length to integrate on
// @param nthreads the amount of threads to launch
struct comp_data {
    double a;
    double b;
    int nthreads;
};