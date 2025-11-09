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
#include <pthread.h>
#include <ctype.h>

typedef struct {
    char** services;
    int* ports;
    int max_common;
} Common;

typedef struct {
    struct sockaddr_in* ipv4;
    struct sockaddr_in6* ipv6;
    char ipstr[INET6_ADDRSTRLEN];

    Common common;

    bool scan_all;
    bool silent;

    bool save_ports;
    FILE* ports_file;

    int max_threads;

    int open_ports[0xFFFF];
    int ptop;

    struct addrinfo* addr;
} Tscan;

typedef struct {
    Tscan* tscan;
    int start;
    int end;
} TscanArgs;

Tscan* tscan_init(const char* hostname);

void tscan_close(Tscan* tscan);

void tscan_lookup(Tscan* tscan);

int tscan_socket(int domain, int type, int protocol);

int tscan_connect(Tscan* tscan, int *sockfd, uint16_t port);

int tscan_getnameinfo(Tscan* tscan, char* host);

void tscan_portscan(Tscan* tscan);

void tscan_open_ports(Tscan* tscan);

void tscan_save_ports(Tscan* tscan);

#endif
