#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define PORT 8080
int main(int argc, char const* argv[])
{
    int server_fd, new_socket, valread;

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = { 0 };
    char* hello = "Hello World\n";
    char* shutdown_msg = "Server Shutting Down...\n";

    // Creating socketfd
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Binding socket to port
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    for (;;) {
        if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        valread = read(new_socket, buffer, 1024);
        printf("==> BUFFER <==\n%s\n==============\n", buffer);

        char body[20][512];
        char *token = strtok(buffer, "\n");
        printf("%s\n", token);
        int i = 0;
        while(token != NULL) {
            token = strtok(NULL, "\n");
            if (token != NULL) {
                printf("%s\n", token);
                strcpy(body[i], token);
            }
            i++;
        }

        if (strcmp(body[7], "shutdown") == 0) {
            send(new_socket, shutdown_msg, strlen(shutdown_msg), 0);
            printf("%s\n", shutdown_msg);
            close(new_socket);
            break;
        }

        send(new_socket, hello, strlen(hello), 0);
        printf("Hello message sent\n");
        // closing the connected socket
        close(new_socket);
    }
    // closing the listening socket
    shutdown(server_fd, SHUT_RDWR);
    printf("Shutting down...\n");
    return 0;
}
