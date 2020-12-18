#pragma once

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include <signal.h>
#include <sys/prctl.h>
#include <math.h>
#include <assert.h>
#include <sys/wait.h>

#include "Input/input.h"

#define PIPE_RD 0
#define PIPE_WR 1

#define CHBUFSIZE 1024

#define PROXYBUF 1000

#define ERROR(message) do {             \
    perror("");                         \
    printf(message);                    \
    exit(EXIT_FAILURE);                 \
} while(0)

#define MAX(a, b) ((a) > (b)) ? (a) : (b)

// Structure that is to contain two pipes
struct pipes {
    int toChild[2];
    int fromChild[2];
};

// Circle buffer for containing data 
struct circleBuf {
    char *buffer;

    int writeIndex;
    int readIndex;
    int endIndex;

    size_t empty;
    size_t full;
};

// Structure that is related to the certain connection between the child and the parent
struct connection {
    int toChild;
    int fromChild;
    struct circleBuf cBuf;
    size_t bufSize;
};

void proxyChild(const char* file_name, int i, int n, struct pipes* pipes);
void proxyParent(int nfds, struct pipes *pipes, int n);