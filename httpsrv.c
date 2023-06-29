#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define PORT 8080
// #define BUFFSIZE 4194304
#define BUFFSIZE 65535


typedef struct {
  const char *name;
  const char *value;
} cookie_t;

typedef struct {
  const char *key;
  const char *value;
} header_t;

typedef struct {
  const char *key;
  const char *value;
} param_t;

typedef struct {
    char method[10];
    char* path;
    header_t* headers;
    uint16_t header_count;
    cookie_t *cookies;
    uint16_t cookie_count;
    param_t* params;
    uint16_t params_count;
    char *body;
} http_request_t;

void crtcHandler(int socket_fd) {
    printf("\nShutting down...\n");
    // shutdown socket
    shutdown(socket_fd, SHUT_RDWR);
    exit(0);
}

int parse(char* buffer, http_request_t* req) {
    char* payload = strstr(buffer, "\r\n\r\n");
    if (payload != NULL) {
        payload += 4;
        printf("notnull\n");
    }
    // separate body from buffer
    printf("payload: %s\n", payload);
    req->body = strdup(payload);
    if (req->body == NULL) {
        printf("failed to allocate memory for body\n");
    }

    // separate header from buffer
    char *start = &buffer[0];
    char *end = payload;
    char *header = (char *)calloc(1, end - start + 1);
    if (header == NULL) {
        printf("failed to allocate memory for header\n");
    }
    memcpy(header, start, end - start);
    printf("header: \n%s\n", header);


    free(header);


    // // parse method
    // char* method = strtok(buffer, " ");
    // strcpy(req->method, method);
    // // parse path
    // char* path = strtok(NULL, " ");
    // req->path = strdup(path);
    // if (req->path == NULL) {
    //     printf("failed to allocate memory for path\n");
    // }

    return 0;
}


int main() {
    signal(SIGINT, crtcHandler);

    // initialize some variables
    int socket_fd, new_socket, valread;
    char buffer[BUFFSIZE] = {0};
    // char* hello = "HTTP/1.1 200 OK\r\n\r\nHello World!\r\n";
    char* hello = "HTTP/1.0 200 OK\r\n\r\n<h1>Hello<h1>\r\n";

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
        printf("Accepting connection\n");
        // accept incoming connection
        if ((new_socket = accept(socket_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        printf("Found connection\n");
        // read message from socket
        valread = read(new_socket, buffer, BUFFSIZE);
        printf("==> BUFFER <==\n%s\n==============\n", buffer);

        http_request_t *req = malloc(sizeof(http_request_t));

        parse(buffer, req);


        printf("Method: %s\n", req->method);
        printf("Path: %s\n", req->path);
        printf("Body: %s\n", req->body);


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
        **/

        send(new_socket, hello, strlen(hello), 0);
        printf("Hello message sent\n");

        printf("%s", req->body);

        free(req->path);
        free(req->body);
        free(req);


        // closing the connected socket
        close(new_socket);
        // empty buffer
        memset(buffer, '\0', strlen(buffer));
        printf("%s\n", buffer);
    }
    return 0;
}
