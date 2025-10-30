#ifndef _TSCAN_H_
#define _TSCAN_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

typedef struct {
    struct sockaddr_in* ipv4;
    struct sockaddr_in6* ipv6;
    char ipstr[INET6_ADDRSTRLEN];

    struct addrinfo* addr;
} Tscan;

Tscan* tscan_init(const char* hostname);

void tscan_close(Tscan* tscan);

void tscan_lookup(Tscan* tscan);

int tscan_socket(int domain, int type, int protocol);

int tscan_connect(Tscan* tscan, int *sockfd, uint16_t port);

#endif
