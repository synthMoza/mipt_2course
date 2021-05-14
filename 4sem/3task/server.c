#include "network.h"

// The length to integrate on
const int integrate_a = 1;
const int integrate_b = 10e6;

void print_compdata(struct comp_data* comp_data, int n) {
    assert(comp_data);

    for (int i = 0; i < n; ++i) {
        printf("comp_data[%d].a = %g\n", i, comp_data[i].a);
        printf("comp_data[%d].b = %g\n", i, comp_data[i].b);
    }
}

int main(int argc, char* argv[]) {
    int sk_br = 0, sk_tcp = 0;
    int* sk_cl = NULL; // the array of all socket ids
    struct comp_data* comp_data = NULL;
    int ret = 0;
    int enable = 1;
    struct sockaddr_in server; // for sending messages
    struct sockaddr_in* network = NULL; // for discovering IP address
    socklen_t socklen;
    int ncomp = 0;
    double result = 0;
    double tmp = 0;
    fd_set fdset;
    struct timeval timeval;
    int flags = 0;
    int* comp_threads = NULL;
    int max = 0;

    if (argc != 2) {
        printf("Usage: ./server <ncomp>\n");
        return EXIT_FAILURE;
    }

    ncomp = input(argv[1]);
    printf("Computers got: %d\n", ncomp);

    // Allocate space for information
    sk_cl = (int*) calloc(ncomp, sizeof(int));
    if (sk_cl == NULL) 
        check_return(-1, "Calloc error!\n");
    
    comp_data = (struct comp_data*) calloc(ncomp, sizeof(*comp_data));
    if (comp_data == NULL) 
        check_return(-1, "Calloc error!\n");
    comp_threads = (int*) calloc(ncomp, sizeof(int));
    if (comp_threads == NULL) 
        check_return(-1, "Calloc error!\n");

    // Initialize UDP socket and send broadcast message
    sk_br = socket(AF_INET, SOCK_DGRAM, 0);
    check_return(sk_br, "Failed to create the socket!\n");

    // Set socket's options
    ret = setsockopt(sk_br, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));
    check_return(ret, "Failed to configure the socket!\n");
    ret = setsockopt(sk_br, SOL_SOCKET, SO_BROADCAST, &enable, sizeof(enable));
    check_return(ret, "Failed to configure the socket!\n");

    // Initialize sockaddr structure
    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = htonl(INADDR_BROADCAST);

    // Send a message
    ret = sendto(sk_br, BR_MSG, strlen(BR_MSG), 0, (struct sockaddr*) &server, sizeof(server));
    check_return(ret, "Failed to send the broadcast message!\n");

    // Initialize TCP socket
    sk_tcp = socket(AF_INET, SOCK_STREAM, 0);
    check_return(sk_tcp, "Failed to create TCP socket!\n");

    ret = setsockopt(sk_tcp, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));
    check_return(ret, "Failed to configure the socket!\n");

    // Set KEEP_ALIVE option
    ret = sktcp_keepalive(sk_tcp);
    check_return(ret, "Failed to set KEEPALIVE option!\n");

    // Initialize sockaddr structure
    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    ret = bind(sk_tcp, (struct sockaddr*) &server, sizeof(server));
    check_return(ret, "Failed to bind TCP socket!\n");

    ret = listen(sk_tcp, 8);
    check_return(ret, "Failed to listen to this socket!\n");

    // Set the listening socket to NON_BLOCK
    flags = fcntl(sk_tcp, F_GETFL, 0);
    check_return(flags, "Failed to get flags of TCP socket!\n");
    ret = fcntl(sk_tcp, F_SETFL, flags | O_NONBLOCK);
    check_return(ret, "Failed to set NON_BLOCKING flag!\n");

    // Accept all computers and recv their socket information
    int naccepted = 0; // accepted computers
    int nthrecv = 0; // receive threads
    int sum = 0;

    // Accept all computers
    FD_ZERO(&fdset);
    FD_SET(sk_tcp, &fdset);
    timeval.tv_usec = 0;
    timeval.tv_sec = 10;

    for (int i = 0; i < ncomp; ++i) {
        ret = select(sk_tcp + 1, &fdset, NULL, NULL, &timeval);
        check_return(ret, "Select error!\n");
        if (ret == 0) {
            printf("Accept timeout expired!\n");
            exit(EXIT_FAILURE);
        }

        if (FD_ISSET(sk_tcp, &fdset)) {
            socklen = sizeof(server);
            sk_cl[i] = accept(sk_tcp, (struct sockaddr*) &server, &socklen);
            check_return(sk_cl[i], "Failed to accept the connection!\n");
        }

        // Send a message to the console
        printf("Accepted new computer! Number: %d\n", i);
    }
    

    // Accept information about each computer threads
    for (int i = 0; i < ncomp; ++i) {
    	ret = recv(sk_cl[i], &comp_threads[i], sizeof(comp_threads[i]), 0);
    	check_return(ret, "Failed to receive nthreads!\n");
    	if (ret == 0)
    			return EXIT_FAILURE;
    	sum += comp_threads[i];
    	printf("comp_threads[%d] = %d\n", i, comp_threads[i]);
    }
    printf("sum = %d\n", sum);

    double diff = (integrate_b - integrate_a) / sum; 
    comp_data[0].a = integrate_a;
    comp_data[0].b = integrate_a + comp_threads[0] * diff;
    for (int i = 1; i < ncomp; ++i) {
        comp_data[i].a = comp_data[i-1].b;
        comp_data[i].b = comp_data[i].a + comp_threads[i] * diff;
    }

    print_compdata(comp_data, ncomp);

    // Send data to all computers
    for (int i = 0; i < ncomp; ++i) {
        ret = send(sk_cl[i], &comp_data[i], sizeof(comp_data[i]), 0);
        check_return(ret, "Failed to send data to calculate!\n");
    }

    // Accept all results
    for (int i = 0; i < ncomp; ++i) {
        ret = recv(sk_cl[i], &tmp, sizeof(tmp), 0);
        check_return(ret, "Failed to receive data from some computer!\n");
        printf("Size: %u\n", ret);
        printf("Temp: %g\n", tmp);
        if (ret == 0) {
            printf("Lost package from computer %d!\n", i);
            exit(EXIT_FAILURE);
        }

        close(sk_cl[i]);

        result += tmp;
    }

    printf("Result: %g\n", result);

    // Free resources
    if (network != NULL)
        free(network);

    free(sk_cl);
    free(comp_data);
    free(comp_threads); 

    close(sk_tcp);
    close(sk_br);

    return 0;
}