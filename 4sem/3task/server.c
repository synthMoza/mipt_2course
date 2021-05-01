#include "network.h"

// The length to integrate on
const int integrate_a = 1;
const int integrate_b = 10e6;

// Get the address of the network interface with this number
// If it doesn't exist, returns NULL
struct sockaddr_in* getNetworkAddress(int index) {
    int ret = 0;
    struct sockaddr_in* ret_value = NULL;
    int counter = 0;
    struct ifaddrs *ifap, *ifa;
    struct sockaddr_in *sa;
    char *addr;

    // Discover this server's IP address
    ret = getifaddrs(&ifap);
    check_return(ret, "Failed to get network interfaces!\n");

#ifdef DEBUG
    printf("List of availible IPv4 interfaces:\n");
#endif

    for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET) {
            sa = (struct sockaddr_in *) ifa->ifa_addr;
            if (counter == index) {
                ret_value = (struct sockaddr_in*) calloc(1, sizeof(*ret_value));
                ret_value = memcpy(ret_value, sa, sizeof(*sa));
                break;
            }

            counter++;
            // For printing all addresses
            #ifdef DEBUG
                addr = inet_ntoa(sa->sin_addr);
                printf("Interface: %s\tAddress: %s\n", ifa->ifa_name, addr);
            #endif
        }
    }

    freeifaddrs(ifap);
    return ret_value;
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
    int nthreads = 0;
    int nproc = 0;
    int nphys = 0;
    int ncomp = 0;
    double result = 0;
    double tmp = 0;
    fd_set fdset;
    struct timeval timeval;
    int flags = 0;

    if (argc != 2) {
        printf("Usage: ./server <nthreads>\n");
        return EXIT_FAILURE;
    }

    nthreads = input(argv[1]);

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

    printf("Computers needed: %d\n", ncomp);

    // Allocate space for information
    sk_cl = (int*) calloc(ncomp, sizeof(int));
    if (sk_cl == NULL) 
        check_return(-1, "Calloc error!\n");
    
    comp_data = (struct comp_data*) calloc(ncomp, sizeof(*comp_data));
    if (comp_data == NULL) 
        check_return(-1, "Calloc error!\n");

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

    // Discorver IP address and send it
    if (ncomp > 1) {
        network = getNetworkAddress(INTERFACE_NUMBER);
        ret = sendto(sk_br, &network->sin_addr.s_addr, sizeof(network->sin_addr.s_addr), 0, (struct sockaddr*) &server, sizeof(server));
        check_return(ret, "Failed to send the broadcast message!\n");
    }

    // Initialize TCP socket
    sk_tcp = socket(AF_INET, SOCK_STREAM, 0);
    check_return(sk_tcp, "Failed to create TCP socket!\n");

    ret = setsockopt(sk_tcp, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));
    check_return(ret, "Failed to configure the socket!\n");

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

    // Accept all computers
    FD_ZERO(&fdset);
    FD_SET(sk_tcp, &fdset);
    timeval.tv_usec = 0;
    timeval.tv_sec = 10;

    for (int i = 1; i < ncomp; ++i) {
        ret = select(sk_tcp + 1, &fdset, NULL, NULL, &timeval);
        check_return(ret, "Select error!\n");
        if (ret == 0) {
            printf("Accept timeout expired!\n");
            exit(EXIT_FAILURE);
        }

        socklen = sizeof(server);
        sk_cl[i] = accept(sk_tcp, (struct sockaddr*) &server, &socklen);
        check_return(sk_cl[i], "Failed to accept the connection!\n");
        printf("Accepted new computer! Number: %d\n", i);
    }

    // Send data to all computers
    for (int i = 1; i < ncomp; ++i) {
        ret = send(sk_cl[i], &comp_data[i], sizeof(comp_data[i]), 0);
        check_return(ret, "Failed to send data to calculate!\n");
    }

    result = thread_integrate(comp_data[0].a, comp_data[0].b, comp_data[0].nthreads);

    // Accept all results
    for (int i = 1; i < ncomp; ++i) {
        ret = recv(sk_cl[i], &tmp, sizeof(tmp), 0);
        check_return(ret, "Failed to receive data from some computer!\n");
        if (ret == 0) {
            printf("Lost package from computer %d!\n", i);
            exit(EXIT_FAILURE);
        }

        close(sk_cl[i]);

        result += tmp;
    }

    printf("Number of threads: %d\n", nthreads);
    printf("Result: %g\n", result);

    // Free resources
    if (network != NULL)
        free(network);

    free(sk_cl);
    free(comp_data);

    close(sk_tcp);
    close(sk_br);
    return 0;
}