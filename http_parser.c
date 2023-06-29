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
    const char* name;
    const char* value;
} cookie_t;

typedef struct {
    const char* key;
    const char* value;
} header_t;

typedef struct {
    const char* key;
    const char* value;
} param_t;

typedef struct {
    char method[10];
    char* path;
    header_t* headers;
    uint16_t header_count;
    // cookie_t* cookies;
    // uint16_t cookie_count;
    // param_t* params;
    // uint16_t params_count;
    char* body;
} http_request_t;

int parse(char* buffer, http_request_t* req) {
    // Initialise http request struct
    req->header_count = 0;
    req->headers = malloc(0 * sizeof(header_t));

    // Separate body from buffer
    char* body = strstr(buffer, "\r\n\r\n");
    if (body != NULL) {
        body += 4;
        printf("body exists\n");
    }
    printf("body: %s\n", body);
    req->body = strdup(body);
    if (req->body == NULL) {
        printf("failed to allocate memory for body\n");
    }

    // Separate header from buffer
    char* start = buffer;
    char* end = body - 4;
    char* raw_header = (char*) calloc(1, end - start + 1);
    if (raw_header == NULL) {
        printf("failed to allocate memory for header\n");
    }
    memcpy(raw_header, start, end - start + 1);
    printf("header: \n%s\n", raw_header);

    // Separate first line from header
    char* saveptr_1;
    char* first_line;
    first_line = strtok_r(raw_header, "\r\n", &saveptr_1);

    // Create a struct for each header and add them to the req struct
    char* token_tmp;
    for(int i = 0;; i++) {
        // Get a line from header
        token_tmp = strtok_r(NULL, "\r\n", &saveptr_1);
        if (token_tmp == NULL) {
            break;
        }

        // Separate key and value, using strdup to avoid bugs caused by breaking the original string
        char* tmp = strdup(token_tmp);
        char* saveptr_2;
        // using strdup so the strings will still live after we free "tmp"
        header_t header = {
            .key = strdup(strtok_r(tmp, ":", &saveptr_2)),
            // + 1 to remove leading space
            .value = strdup(strtok_r(NULL, ":", &saveptr_2) + 1)
        };
        free(tmp);

        printf("HEADER - %d:\n", i);
        printf("%s\n", header.key);
        printf("%s\n", header.value);

        req->header_count++;
        req->headers = realloc(req->headers, req->header_count * sizeof(header_t));
        req->headers[i] = header;
    }

    free(raw_header);

    return 0;
}


int main() {
    // initialize some variables
    char buffer[BUFFSIZE] = "POST /cgi-bin/process.cgi HTTP/1.1\r\nUser-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)\r\nContent-Length: length\r\n\r\nlicenseID=string&content=string&/paramsXML=string";

    // create socket address struct
    // struct sockaddr_in address;
    // address.sin_family = AF_INET;
    // address.sin_addr.s_addr = INADDR_ANY;
    // address.sin_port = htons(PORT);
    // int addrlen = sizeof(address);
    http_request_t *req = malloc(sizeof(http_request_t));

    parse(buffer, req);

    // printf("Method: %s\n", req->method);
    // printf("Path: %s\n", req->path);
    // printf("Body: %s\n", req->body);

    // printf("%s", req->body);

    free(req->path);
    free(req->body);
    free(req);


    // empty buffer
    memset(buffer, '\0', strlen(buffer));
    printf("%s\n", buffer);
    return 0;
}
