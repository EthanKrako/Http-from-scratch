#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 8080
#define WAITLINE_SIZE 5
#define BUFFER_SIZE 256

void handle_error(const char* msg, int socketfd) {
    perror(msg);
    if (socketfd != -1) {
        close(socketfd);
    }
    exit(EXIT_FAILURE);
}

int main() {
    int socketfd = socket(PF_INET, SOCK_STREAM, 0);

    if (socketfd == -1) {
        handle_error("Socket creation failed", socketfd);
    }

    struct sockaddr_in socketAddr, clientAddr = { 0 };
    socketAddr.sin_family = AF_INET;
    socketAddr.sin_port = htons(PORT);
    socketAddr.sin_addr.s_addr = 0;

    if (bind(socketfd, (struct sockaddr*)&socketAddr, sizeof(socketAddr)) == -1) {
        handle_error("Could not bind to the port", socketfd);
    }

    if (listen(socketfd, WAITLINE_SIZE) == -1) {
        handle_error("Could not listen for connections", socketfd);
    }

    socklen_t clientSize = sizeof(clientAddr);

    int clientfd = accept(socketfd, (struct sockaddr*)&clientAddr, &clientSize);

    if (clientfd == -1) {
        handle_error("Could not accept connection from client", socketfd);
    }

    char socketbuffer[BUFFER_SIZE] = { 0 };
    
    recv(clientfd, socketbuffer, BUFFER_SIZE, 0);

    printf("Received message: %s\n", socketbuffer);

    char* response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 13\r\n\r\nHello, World!";
    send(clientfd, response, strlen(response), 0);

    while(1) {}

    return 0;
}