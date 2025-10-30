#include "../include/tscan.h"

void load_logo() {
    FILE* fptr = fopen("/usr/share/tscan/tscan-logo", "r");

    printf("\n");

    char c;
    while((c = fgetc(fptr)) != EOF) {
        printf("%c", c);
    }

    printf("\nVersion 1.0 (BETA)\n\n");

    fclose(fptr);
}

int main(int argc, char** argv) {
    if(argc < 2) {
        load_logo();
        fprintf(stderr, "USAGE: tscan [target]\n");
        return -1;
    }
    
    const char* hostname = argv[1];

    Tscan* tscan = tscan_init(hostname);

    printf("Tscan 1.0 (BETA)\n\n");

    printf("Scanning %s\n\n", hostname);

    tscan_lookup(tscan);

    tscan_portscan(tscan);

    tscan_close(tscan);

    return 0;
}
