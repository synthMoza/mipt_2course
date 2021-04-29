#include "network.h"

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
            // addr = inet_ntoa(sa->sin_addr);
            // printf("Interface: %s\tAddress: %s\n", ifa->ifa_name, addr);
        }
    }

    freeifaddrs(ifap);
    return ret_value;
}

int main(int argc, char* argv[]) {
    int sk_br = 0, sk_tcp = 0, sk_cl = 0;
    int ret = 0;
    int enable = 1;
    struct sockaddr_in server; // for sending messages
    struct sockaddr_in client;
    struct sockaddr_in* network = NULL; // for discovering IP address
    socklen_t socklen;

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

    // ret = bind(sk_br, (struct sockaddr*) &server, sizeof(server));
    // check_return(ret, "Failed to bind the socket!\n");

    // Discorver IP address and sent it
    network = getNetworkAddress(1);
    ret = sendto(sk_br, &network->sin_addr.s_addr, sizeof(network->sin_addr.s_addr), 0, (struct sockaddr*) &server, sizeof(server));
    check_return(ret, "Failed to send the broadcast message!\n");

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

    sk_cl = accept(sk_tcp, (struct sockaddr*) &server, &socklen);
    check_return(sk_cl, "Failed to accept the connection!\n");

    if (network != NULL)
        free(network);

    close(sk_cl);
    close(sk_tcp);
    close(sk_br);
    return 0;
}