#pragma once

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

#include "Thread_Integrate/integral.h"
#include "Thread_Integrate/input.h"

#define check_return(ret, message)              \
do {                                            \
    if (ret < 0) {                              \
        perror("");                             \
        printf(message);                        \
        exit(EXIT_FAILURE);                     \
    }                                           \
} while (0)

#define BR_MSG "BROADCAST"
#define MAX_MSG_SIZE 128

#define PORT 3456

int init_udp_socket(struct sockaddr_in* server);