#pragma once

#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <string.h>
#include <sys/sysinfo.h>

#include "../Thread_Integrate/integral.h"
#include "../Thread_Integrate/input.h"

// Method for establishing connection with the server
// Returns the corresponding socket
int establishConnection(const char* address, struct sockaddr_in* client);