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

void tscan_lookup(Tscan* tscan) {
    void *addr;
    
    struct addrinfo* temp = tscan->addr;

    printf("Listing all IP addresses...\n");

    while(temp != NULL) {
        const char* ver_str;

        if(temp->ai_family == AF_INET) {
            tscan->ipv4 = (struct sockaddr_in*)temp->ai_addr;
            addr = &(tscan->ipv4->sin_addr);
            ver_str = "IPv4";
        }

        if(temp->ai_family == AF_INET6) {
            tscan->ipv6 = (struct sockaddr_in6*)temp->ai_addr;
            addr = &(tscan->ipv6->sin6_addr);
            ver_str = "IPv6";
        }

        inet_ntop(temp->ai_family, addr, tscan->ipstr, sizeof tscan->ipstr);

        printf("\e[34m%s:\e[36m %s\n\e[00m", ver_str, tscan->ipstr);

        temp = temp->ai_next;
    }

    printf("\n");
}

void tscan_close(Tscan* tscan) {
    freeaddrinfo(tscan->addr);
    free(tscan);
}
