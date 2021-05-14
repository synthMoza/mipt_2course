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
#include <netinet/in.h>
#include <netinet/tcp.h>

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
// Broadcast message
#define BR_MSG "BRMSG"

// Structure that describes data to be sent to each computer
// @param a the start of the length to integrate on
// @param b the end of the length to integrate on
// @param nthreads the amount of threads to launch
struct comp_data {
    double a;
    double b;
};

int sktcp_keepalive(int sk_tcp) {
    int enable = 1;
    int ret = 0;

    if (sk_tcp <= 0)
        return -1;
    
    // Enable keep alive option
    ret = setsockopt(sk_tcp, SOL_SOCKET, SO_KEEPALIVE, &enable, sizeof(enable));
    check_return(ret, "Failed to configure the socket!\n");
    int idle = 9;
    ret = setsockopt(sk_tcp, IPPROTO_TCP, TCP_KEEPIDLE, &idle, sizeof(idle));
    check_return(ret, "Failed to configure the socket!\n");
    int interval = 1;
    ret = setsockopt(sk_tcp, IPPROTO_TCP, TCP_KEEPINTVL, &interval, sizeof(interval));
    check_return(ret, "Failed to configure the socket!\n");
    int maxpkt = 3;
    ret = setsockopt(sk_tcp, IPPROTO_TCP, TCP_KEEPCNT, &maxpkt, sizeof(maxpkt));
    check_return(ret, "Failed to configure the socket!\n");

    return 0;
}