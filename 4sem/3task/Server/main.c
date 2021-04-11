#include "server.h"

// The minimum amout of processors for debug
const int MIN_PROC = 8;
// The amount of threads per each core of the CPU
const int THREADS_PER_CORE = 2;
// Structure that describes data to be sent to each computer
// @param a the start of the length to integrate on
// @param b the end of the length to integrate on
// @param nthreads the amount of threads to launch
struct comp_data {
    double a;
    double b;
    int nthreads;
};

// The length to integrate on
const int integrate_a = 1;
const int integrate_b = 10e6;

#define DEBUG

void printCompData(struct comp_data* comp_data, int ncomp) {
    // Debug comp_data
    for (int i = 0; i < ncomp; ++i) {
        printf("comp_data[%d]\n", i);
        printf("comp_data[%d].a = %g\n", i, comp_data[i].a);
        printf("comp_data[%d].b = %g\n", i, comp_data[i].b);
        printf("comp_data[%d].nthreads = %d\n", i, comp_data[i].nthreads);
    }
}

int main(int argc, char* argv[]) {
    int sk = 0;
    int ret = 0;
    int nthreads = 0;
    int nproc = 0;
    int nphys = 0;
    int ncomp = 0;
    struct comp_data* comp_data = NULL;
    struct sockaddr_in server;
    int* client_sockets = NULL;
    socklen_t socklen = 0;
    double result = 0;
    double tmp = 0;

    if (argc != 2) {
        printf("Usage: ./server <nthreads>\n");
        return EXIT_FAILURE;
    }
    nthreads = input(argc, argv);

    // Calculate the amount of needed computers
    #ifdef DEBUG
    nproc = MIN_PROC;
    #else
    nproc = get_nprocs(); // all cores
    #endif

    nphys = nproc / THREADS_PER_CORE; // physical cores
    ncomp = nthreads / nphys;
    if (nthreads % nphys != 0)
        ncomp++;

    // Init TCP socket
    sk = socket(AF_INET, SOCK_STREAM, 0);
    if (sk == -1) {
        printf("Error creating socket!\n");
        return EXIT_FAILURE;
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(8022);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    ret = bind(sk, (struct sockaddr*) &server, sizeof(server));
    if (ret == -1) {
        perror("");
        printf("Failed to bind socket!\n");
        shutdown(sk, SHUT_RDWR);
        return EXIT_FAILURE;
    }

    // Set this socket for listening
    ret = listen(sk, 256);
    if (ret == -1) {
        perror("");
        printf("Failed to configure socket to listen!\n");
        shutdown(sk, SHUT_RDWR);
        return EXIT_FAILURE;
    }

    // Allocate client sockets
    client_sockets = (int*) calloc(ncomp, sizeof(int));
    if (client_sockets == NULL) {
        perror("");
        printf("Calloc error!\n");
        shutdown(sk, SHUT_RDWR);
        return EXIT_FAILURE;
    }

    // Accept all computers (besides this one)
    for (int i = 1; i < ncomp; ++i) {
        socklen = sizeof(server);
        client_sockets[i] = accept(sk, (struct sockaddr*) &server, &socklen);
        if (client_sockets[i] == -1) {
            perror("");
            printf("Error accepting new connection!\n");
            shutdown(sk, SHUT_RDWR);
            return EXIT_FAILURE;
        }
    }

    // Calculate the data to send to each computer
    comp_data = (struct comp_data*) calloc(ncomp, sizeof(struct comp_data));
    if (comp_data == NULL) {
        perror("");
        printf("Error allocating memory!\n");
        shutdown(sk, SHUT_RDWR);
        return EXIT_FAILURE;
    }

    // The amount of computers that will create threads on each physical core
    int nfullcomp = nthreads / nphys; 
    // The difference between two threads
    double diff = (integrate_b - integrate_a) / nthreads; 
    for (int i = 0; i < nfullcomp; ++i) {
        comp_data[i].a = integrate_a + i * diff * nphys;
        comp_data[i].b = integrate_a + (i + 1) * diff * nphys;
        comp_data[i].nthreads = nphys;
    }

    // The amount of threads for the last computer
    if (nfullcomp != ncomp) {
        int left_threads = nthreads % nphys;
        comp_data[nfullcomp].a = integrate_a + nfullcomp * diff * nphys;
        comp_data[nfullcomp].b = integrate_a + nthreads * diff;
        comp_data[nfullcomp].nthreads = left_threads;
    }

    //printCompData(comp_data, ncomp);

    for (int i = 1; i < ncomp; ++i) {
        // Send data to calculate to each computer
        ret = send(client_sockets[i], &comp_data[i], sizeof(struct comp_data), 0);
        if (ret != sizeof(struct comp_data)) {
            perror("");
            printf("Failed to send data!\n");
            shutdown(sk, SHUT_RDWR);
            return EXIT_FAILURE;
        }
    }

    result = thread_integrate(comp_data[0].a, comp_data[0].b, comp_data[0].nthreads);
    for (int i = 1; i < ncomp; ++i) {
        ret = recv(client_sockets[i], &tmp, sizeof(double), 0);
        if (ret != sizeof(double)) {
            perror("");
            printf("Error receiving results from computer %d!\n", i);
            return EXIT_FAILURE;
        }

        result += tmp;
    }

    printf("Number of threads: %d\n", nthreads);
    printf("Number of computers: %d\n", ncomp);
    printf("Result: %g\n", result);

    // Free the resources
    free(comp_data);
    free(client_sockets);
    shutdown(sk, SHUT_RDWR);

    return 0;
}