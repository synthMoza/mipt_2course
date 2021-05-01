#include "network.h"

int main(int argc, char* argv[]) {
    int sk_br = 0, sk_tcp = 0;
    int enable = 1; // for setsockopt
    int ret = 0;
    struct sockaddr_in client;
    struct sockaddr_in network;
    socklen_t socklen = 0;
    struct comp_data data;
    double result = 0;

    bzero(&network, sizeof(network));

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
    bzero(&network, sizeof(network));
    ret = recvfrom(sk_br, &network.sin_addr.s_addr, sizeof(network.sin_addr.s_addr), 0, (struct sockaddr*) &client, &socklen);
    check_return(ret, "Failed to receive a message from the server!\n");

    // Now we have server's address, connect to it through TCP
    bzero(&client, sizeof(client));
    client.sin_family = AF_INET;
    client.sin_port = htons(PORT);
    client.sin_addr.s_addr = network.sin_addr.s_addr;

    // Initialize TCP socket
    sk_tcp = socket(AF_INET, SOCK_STREAM, 0);
    check_return(sk_tcp, "Failed to create TCP socket!\n");

    ret = setsockopt(sk_tcp, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));
    check_return(ret, "Failed to configure the socket!\n");

    ret = connect(sk_tcp, (struct sockaddr*) &client, sizeof(client));
    check_return(ret, "Failed to connect to the server!\n");

    printf("Connection established!\n");

    // Accept data on threads
    ret = recv(sk_tcp, &data, sizeof(data), 0);
    check_return(ret, "Failed to receive comp_data!\n");

    // Calculate the result
    result = thread_integrate(data.a, data.b, data.nthreads);

    // Send the result
    ret = send(sk_tcp, &result, sizeof(result), 0);
    check_return(ret, "Failed to send the result to the server!\n");

    close(sk_tcp);
    close(sk_br);
    return 0;
}