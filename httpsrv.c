#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define PORT 8080
//#define BUFFSIZE 4194304
#define BUFFSIZE 65535


typedef struct {
  const char *name;
  const char *value;
} cookie;

typedef struct {
  const char *key;
  const char *value;
} header;

typedef struct {
    const char method[10];
    const char* path;
    header* headers;
    uint16_t header_count;
    cookie *cookies;
    uint16_t cookie_count;
    const char *body;
} http_request;

void crtcHandler(int socket_fd) {
    printf("\nShutting down...\n");
    // shutdown socket
    shutdown(socket_fd, SHUT_RDWR);
    exit(0);
}

int main() {
    // initialize some variables
    int socket_fd, new_socket, valread;
    char buffer[BUFFSIZE] = {0};
    char* hello = "HTTP/1.1 200 OK\r\n\r\nHello World!\r\n";

    signal(SIGINT, crtcHandler);


    // create socket address struct
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    int addrlen = sizeof(address);

    // initialise socket file descriptor
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &address, sizeof(address)) < 0) {
        exit(EXIT_FAILURE);
    }

    // bind socket to address
    if (bind(socket_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    // listen for incoming connections
    if (listen(socket_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }


    // accept incoming connections
    for (;;) {
        // accept incoming connection
        if ((new_socket = accept(socket_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        http_request req = {
            .headers = NULL,
            .header_count = 0,
            .cookies = NULL,
            .cookie_count = 0,
        };

        // read message from socket
        valread = read(new_socket, buffer, BUFFSIZE);
        printf("==> BUFFER <==\n%s\n==============\n", buffer);

        // parse method
        char strtok_save[BUFFSIZE];
        char *method = strtok(buffer, " ");
        strcpy(req.method, method);
        // parse path
        req.path = strtok(NULL, " ");
        printf("%s\n", req.path);
        // parse body
        char *payload = strstr(buffer, "\r\n\r\n");
        if (payload != NULL) {
            payload += 4;
        }

        printf("%s\n", req.method);
        printf("%s\n", req.path);
        printf("%s\n", req.body);


        /**
        // split message into parts by line
        char body[20][4096];
        char *token = strtok(buffer, "\n");
        printf("%s\n", token);
        for(int i = 0; token != NULL; i++) {
            token = strtok(NULL, "\n");
            if (token != NULL) {
                printf("%s\n", token);
                strcpy(body[i], token);
            }
        }
        if (strcmp(body[7], "shutdown") == 0) {
            close(new_socket);
            break;
        }
        **/

        send(new_socket, hello, strlen(hello), 0);
        printf("Hello message sent\n");

        // closing the connected socket
        close(new_socket);
        // empty buffer
        strcpy(&buffer[0], "\0");
    }
    return 0;
}
