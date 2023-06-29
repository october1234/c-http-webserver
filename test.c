#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {

    char* test = strdup("Hello: World!");
    char* key = strtok(test, ":");
    char* value = strtok(NULL, ":");

    printf("%s\n", key);
    printf("%s\n", value);

    free(test);
    return 0;
}
