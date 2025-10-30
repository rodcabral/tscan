#include "../include/tscan.h"

int main(int argc, char** argv) {
    if(argc < 2) {
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
