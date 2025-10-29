#include "../include/tscan.h"

Tscan* tscan_init(const char* hostname [[maybe_unused]]) {
    Tscan* tscan = malloc(sizeof(Tscan));

    tscan->ipv4 = NULL;
    tscan->ipv6 = NULL;

    struct addrinfo hints;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int status = getaddrinfo(hostname, NULL, &hints, &tscan->addr);
    
    if(status != 0) {
        fprintf(stderr, "ERROR: %s\n", gai_strerror(status));
        exit(EXIT_FAILURE);
    }

    return tscan;
}

void tscan_close(Tscan* tscan) {
    free(tscan);
}
