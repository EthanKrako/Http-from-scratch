#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 8080
#define WAITLINE_SIZE 5

void handle_error(const char* msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

int main() {
    int socketfd = socket(PF_INET, SOCK_STREAM, 0);

    if (socketfd == -1) {
        handle_error("Socket creation failed");
    }

    struct sockaddr_in socketAddr, clientAddr = { 0 };
    socketAddr.sin_family = AF_INET;
    socketAddr.sin_port = htons(PORT);
    socketAddr.sin_addr.s_addr = 0;

    if (bind(socketfd, (struct sockaddr*)&socketAddr, sizeof(socketAddr)) == -1) {
        handle_error("Could not bind to the port");
    }

    if (listen(socketfd, WAITLINE_SIZE) == -1) {
        handle_error("Could not listen for connections");
    }

    socklen_t clientSize = sizeof(clientAddr);

    int clientfd = accept(socketfd, (struct sockaddr*)&clientAddr, &clientSize);

    if (clientfd == -1) {
        handle_error("Could not accept connection from client");
    }

    printf("Client connected, fd=%d\n", clientfd);

    while(1) {}

    return 0;
}