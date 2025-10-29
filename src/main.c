#include <stdio.h>

int main(int argc, char** argv) {
    if(argc < 2) {
        fprintf(stderr, "USAGE: tscan [target]\n");
        return -1;
    }
    
    const char* hostname = argv[1];

    printf("Tscan 1.0 (BETA)\n\n");

    printf("Scanning %s\n", hostname);

    return 0;
}
