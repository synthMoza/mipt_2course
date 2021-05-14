#include "network.h"

int main(int argc, char* argv[]) {
    int sk_br = 0, sk_tcp = 0;
    int enable = 1; // for setsockopt
    int ret = 0;
    struct sockaddr_in client;
    char buf[MAX_MSG_SIZE];
    socklen_t socklen = 0;
    struct comp_data data;
    double result = 0;
    fd_set fdset;
    struct timeval timeval;
    int nthreads = 0;

    if (argc != 2) {
        printf("Usage: ./client <nthreads>\n");
        return EXIT_FAILURE;
    }

    nthreads = input(argv[1]);
    check_return(nthreads, "Wrong number of threads!\n");

    // Wait for the message from the router (UDP)
    sk_br = socket(AF_INET, SOCK_DGRAM, 0);
    check_return(sk_br, "Failed to create socket!\n");

    // Set socket's options
    ret = setsockopt(sk_br, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));
    check_return(ret, "Failed to configure the socket!\n");
    ret = setsockopt(sk_br, SOL_SOCKET, SO_BROADCAST, &enable, sizeof(enable));
    check_return(ret, "Failed to configure the socket!\n");

    // Set up the struct for receiving any message
    bzero(&client, sizeof(client));
    client.sin_family = AF_INET;
    client.sin_port = htons(PORT);
    client.sin_addr.s_addr = htonl(INADDR_ANY);

    ret = bind(sk_br, (struct sockaddr*) &client, sizeof(client));
    check_return(ret, "Failed to bind the socket!\n");

    // Wait for the server's message
    printf("Waiting for the server...\n");
    socklen = sizeof(client);
    ret = recvfrom(sk_br, buf, sizeof(client.sin_addr.s_addr), 0, (struct sockaddr*) &client, &socklen);
    check_return(ret, "Failed to receive a message from the server!\n");

    // Now we have server's address, connect to it through TCP
    client.sin_family = AF_INET;
    client.sin_port = htons(PORT);

    // Initialize TCP socket
    sk_tcp = socket(AF_INET, SOCK_STREAM, 0);
    check_return(sk_tcp, "Failed to create TCP socket!\n");

    ret = setsockopt(sk_tcp, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));
    check_return(ret, "Failed to configure the socket!\n");

    // Enable keep alive option
    ret = sktcp_keepalive(sk_tcp);
    check_return(ret, "Failed to set KEEPALIVE option!\n");

    // Connect to the server
    ret = connect(sk_tcp, (struct sockaddr*) &client, sizeof(client));
    check_return(ret, "Failed to connect to the server!\n");

    printf("Connection established!\n");
    close(sk_br);

    // Send avalible amount of sockets
    ret = send(sk_tcp, &nthreads, sizeof(nthreads), 0);
    check_return(ret, "Threads sending failed!\n");

    // Accept data on threads
    // FD_ZERO(&fdset);
    // FD_SET(sk_tcp, &fdset);
    // timeval.tv_usec = 0;
    // timeval.tv_sec = 10;

    // ret = select(sk_tcp + 1, &fdset, NULL, NULL, &timeval);
    // check_return(ret, "Select error!\n");
    // if (ret == 0) {
    //     printf("Receive timeout expired!\n");
    //     exit(EXIT_FAILURE);
    // }

    ret = recv(sk_tcp, &data, sizeof(data), 0);
    check_return(ret, "Failed to receive comp_data!\n");

    // Calculate the result
    result = thread_integrate(data.a, data.b, nthreads);
    printf("This computer result = %g\n", result);

    // Send the result
    ret = send(sk_tcp, &result, sizeof(result), 0);
    check_return(ret, "Failed to send the result to the server!\n");

    close(sk_tcp);
    return 0;
}