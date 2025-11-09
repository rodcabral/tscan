#include "../include/tscan.h"

void load_logo() {
    FILE* fptr = fopen("/usr/share/tscan/tscan-logo", "r");

    printf("\n");

    char c;
    while((c = fgetc(fptr)) != EOF) {
        printf("%c", c);
    }

    printf("\nVersion 1.0\n\n");

    fclose(fptr);
}

int main(int argc, char** argv) {
    if(argc < 2 || strncmp(argv[1], "--help", 7) == 0 || strncmp(argv[1], "-h", 3) == 0) {
        load_logo();
        printf("USAGE: tscan [Target] [Options]\n\n");

        printf("[Target]\n");
        printf("The target can be hostnames or IP addresses\n");
        printf("Ex: scanme.org, localhost, ::1, 192.168.0.1\n");

        printf("\n[Options]\n");
        printf("Without any options, tscan will only scan the known ports defined in\n/usr/share/tscan/tscan-common-services and the scan will only occur on the last\nIP address found, giving preference to IPv4.\n\n");
        printf("--all: Scan all ports (1 - 65535)\n");
        printf("--silent: Silences all CLI\n");
        printf("-t [number]: Number of threads to use\n");
        printf("-o [filename]: Save all open ports to a file\n");

        if(argc < 2) return -1;

        return 0;
    }
    
    const char* hostname = argv[1];

    Tscan* tscan = tscan_init(hostname);

    for(int i = 0; i < argc; ++i) {
        if(strncmp(argv[i], "--all", 6) == 0) {
            tscan->scan_all = true;
        }

        if(strncmp(argv[i], "-t", 3) == 0) {
            char* p;
            long threads_num = strtol(argv[i + 1], &p, 10);
            if(*p != '\0' || threads_num > INT_MAX || threads_num < INT_MIN) {
                fprintf(stderr, "ERROR: invalid thread number\n");
                return -1;
            }

            tscan->max_threads = threads_num;
        }

        if(strncmp(argv[i], "-o", 3) == 0) {
            tscan->save_ports = true;
            tscan->ports_file = fopen(argv[i + 1], "w");

            if(!tscan->ports_file) {
                fprintf(stderr, "ERROR: could not open %s\n", argv[i+1]);
                exit(EXIT_FAILURE);
            }
        }

        if(strncmp(argv[i], "--silent", 9) == 0) {
            tscan->silent = true;
        }
    }

    if(!tscan->silent) {
        printf("Tscan 1.0\n\n");

        char temp_hostname[255];
        if(isdigit(hostname[0]) && tscan_getnameinfo(tscan, temp_hostname) == 0) {
            printf("Scanning %s\n\n", temp_hostname);
        } else {
            printf("Scanning %s\n\n", hostname);
        }
    }

    tscan_lookup(tscan);

    tscan_portscan(tscan);

    tscan_open_ports(tscan);

    tscan_save_ports(tscan);

    tscan_close(tscan);

    return 0;
}
