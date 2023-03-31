#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char test_str[100] = "test100 tust200 tist300 tost400 tast500";
    printf("%s\n", test_str);
    char* pTestStr = strstr(test_str, "tist300");
    pTestStr += 4;
    printf("%s\n", pTestStr);
}
