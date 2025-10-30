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

int tscan_socket(int domain, int type, int protocol) {
    int sockfd = socket(domain, type, protocol);

    if(sockfd < 0) {
        fprintf(stderr, "ERROR: socket error\n");
        exit(EXIT_FAILURE);
    }

    struct timeval tv;
    tv.tv_sec = 2;
    tv.tv_usec = 0;

    if(setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
        fprintf(stderr, "ERROR: setsockopt error\n");
        exit(EXIT_FAILURE);
    }

    if(setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv)) < 0) {
        fprintf(stderr, "ERROR: setsockopt error\n");
        exit(EXIT_FAILURE);
    }

    return sockfd;
}

int tscan_connect(Tscan* tscan, int *sockfd, uint16_t port) {
    if(tscan->ipv4 != NULL) {
        *sockfd = tscan_socket(AF_INET, SOCK_STREAM, 0);

        tscan->ipv4->sin_port = htons(port);
        return connect(*sockfd, (struct sockaddr*)tscan->ipv4, sizeof(struct sockaddr_in));
    }

    if(tscan->ipv6 != NULL) {
        *sockfd = tscan_socket(AF_INET6, SOCK_STREAM, 0);

        tscan->ipv6->sin6_port = htons(port);

        return connect(*sockfd, (struct sockaddr*)tscan->ipv6, sizeof(struct sockaddr_in6));
    }

    return -1;
}

void tscan_close(Tscan* tscan) {
    freeaddrinfo(tscan->addr);
    free(tscan);
}
