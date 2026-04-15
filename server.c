#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 8080

int main() {
    int socketFD = socket(PF_INET, SOCK_STREAM, 0);

    if (socketFD == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = 0;

    if (bind(socketFD, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("Could not bind to the port");
        exit(EXIT_FAILURE);
    }

    while(1) {}

    return 0;
}