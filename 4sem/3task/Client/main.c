#include "client.h"

// Structure that describes data to be sent to each computer
// @param a the start of the length to integrate on
// @param b the end of the length to integrate on
// @param nthreads the amount of threads to launch
struct comp_data {
    double a;
    double b;
    int nthreads;
};

int main(int argc, char* argv[]) {
    int sk = 0;
    int ret = 0;
    struct sockaddr_in client;
    struct comp_data data;
    socklen_t socklen;
    double result = 0;

    if (argc != 2) {
        printf("Usage: ./client <server-address>\n");
        return EXIT_FAILURE;
    }
    
    printf("Server address: %s\n", argv[1]);

    // Create a socket to communicate with the server
    sk = establishConnection(argv[1], &client);
    if (sk == EXIT_FAILURE) {
        printf("Can't connect to the server!\n");
        return EXIT_FAILURE;
    }

    // Wait for corresponding data to calculate
    socklen = sizeof(client);
    ret = recv(sk, &data, sizeof(struct comp_data), 0);
    if (ret != sizeof(struct comp_data)) {
        perror("");
        printf("Package size - %d\n", ret);
        printf("Error receiving data!\n");
        shutdown(sk, SHUT_RDWR);
        return EXIT_FAILURE;
    }
    
    // Calculate the given data
    result = thread_integrate(data.a, data.b, data.nthreads);

    // Send the result
    socklen = sizeof(client);
    ret = send(sk, &result, sizeof(result), MSG_CONFIRM);
    if (ret != sizeof(result)) {
        perror("");
        printf("Error sending results!\n");
        return EXIT_FAILURE;
    }

    shutdown(sk, SHUT_RDWR);
    return 0;
}