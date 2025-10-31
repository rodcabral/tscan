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
    if(argc < 2 || strncmp(argv[1], "--help", 7) == 0 || strncmp(argv[1], "-h", 3) == 0) {
        load_logo();
        printf("USAGE: tscan [Target] [Options]\n\n");

        printf("[Target]\n");
        printf("The target can be hostnames or IP addresses\n");
        printf("Ex: scanme.org, localhost, ::1, 192.168.0.1\n");

        if(argc < 2) return -1;

        return 0;
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
