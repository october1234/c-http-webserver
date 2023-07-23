#include "http_parser.h"
#include <stdio.h>
#include <sys/socket.h>

#define PORT 8080
#define BUFFSIZE 65535

int main() {
    // Initializing server socket
    int socket_fd;
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("failed to create socket");
        exit(EXIT_FAILURE);
    }
    struct sockaddr_in address = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = INADDR_ANY,
        .sin_port = htons(PORT)
    };
    socklen_t addresslen = sizeof(address);
    if (bind(socket_fd, &address, addresslen) < 0) {
        perror("failed to bind socket");
        exit(EXIT_FAILURE);
    }

    if (listen(socket_fd, 3) < 0) {
        perror("failed to listen");
        exit(EXIT_FAILURE);
    }

    // Create buffer for incoming connections
    char buffer[BUFFSIZE] = "";

    for (;;) {
        // Accept new socket connection
        int new_socket;
        if ((new_socket = accept(socket_fd, (struct sockaddr*)&address, (socklen_t*) &addresslen)) < 0) {
            perror("failed to accept socket");
            exit(EXIT_FAILURE);
        }

        // Read from the socket
        int valread = read(new_socket, buffer, BUFFSIZE);

        // Displaying the buffer for debugging
        printf("%s\n", buffer);

        // Initialize some variables
        http_request_t *req = malloc(sizeof(http_request_t));

        // Parsing the request
        parse(buffer, req);

        // Displaying the request for debugging
        printf("method: %s\n", req->method);
        printf("path: %s\n", req->path);
        for (int i = 0; i < req->header_count; i++) {
            header_t header = req->headers[i];
            printf("HEADER - %d:\n", i);
            printf("%s\n", header.key);
            printf("%s\n", header.value);
        }

        // Closing the connected socket
        send(new_socket, "HTTP/1.1 200 OK\r\n\r\n", strlen("HTTP/1.1 200 OK\r\n\r\n"), 0);
        close(new_socket);

        // Freeing allocated memory
        for (int i = 0; i < req->header_count; i++) {
            header_t header = req->headers[i];
            free(header.key);
            free(header.value);
        }
        free(req->headers);
        free(req->path);
        free(req->body);
        free(req);

        // Empty buffer
        memset(buffer, '\0', strlen(buffer));
    }
    return 0;
}
