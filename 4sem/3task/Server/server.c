#include "server.h"

const int latency_ms = 2000;
const int PORT = 8022;

const int MAXLINE = 1024;

int initSocket(struct sockaddr_in* server) {
    int ret = 0;
    socklen_t socklen;
    long time_start = 0;
    long time_delta = 0;
    int base_sk = 0;

    // Get the main socket
    base_sk = socket(AF_INET, SOCK_DGRAM, 0);
    if (base_sk == -1) {
        printf("Error creating socket!\n");
        return EXIT_FAILURE;
    }

    // Initialize server socket addres input struct
    server->sin_family = AF_INET;
    server->sin_port = htons(PORT);
    server->sin_addr.s_addr = htonl(INADDR_ANY);

    // Bind the address to socket
    bind(base_sk, (struct sockaddr*) server, sizeof(*server));

    return base_sk;
}