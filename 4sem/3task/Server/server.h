#pragma once

#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/sysinfo.h>
#include <time.h>
#include <assert.h>

#include "../Thread_Integrate/integral.h"
#include "../Thread_Integrate/input.h"

// Initializies the socket for getting information
int initSocket(struct sockaddr_in* server);