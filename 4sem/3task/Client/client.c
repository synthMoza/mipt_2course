#include "client.h"

const int PORT = 8022;
const int THREADS_PER_CORE = 2;
const int timeout = 10e5;

int establishConnection(const char* address, struct sockaddr_in* client) {
    int sk = 0;
    int ret = 0;

    // Get the socket
    sk = socket(AF_INET, SOCK_STREAM, 0);
    if (sk == -1) {
        printf("Error creating socket!\n");
        return EXIT_FAILURE;
    }

    // Initialize client socket addres input struct
    client->sin_family = AF_INET;
    client->sin_port = htons(PORT);

    // Convert IP adress from text to binary format
    ret = inet_pton(AF_INET, address, &client->sin_addr.s_addr);
    if (ret < 0) {
        perror("");
        printf("Wrong IP address!\n");
        return EXIT_FAILURE;
    }

    for (int i = 0; i < timeout; ++i) {
        ret = connect(sk, (struct sockaddr*) client, sizeof(*client));
        if (ret == 0)
            break;
    }

    if (ret == -1) {
        perror("");
        printf("Failed to connect to the server!\n");
        return EXIT_FAILURE;
    }


    return sk;
}