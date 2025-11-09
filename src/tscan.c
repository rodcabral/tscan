#include "../include/tscan.h"

Tscan* tscan_init(const char* hostname) {
    Tscan* tscan = malloc(sizeof(Tscan));

    tscan->scan_all = false;
    tscan->max_threads = 10;

    tscan->ipv4 = NULL;
    tscan->ipv6 = NULL;

    tscan->ptop = 0;

    tscan->save_ports = false;

    struct addrinfo hints;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int status = getaddrinfo(hostname, NULL, &hints, &tscan->addr);
    
    if(status != 0) {
        fprintf(stderr, "ERROR: %s\n", gai_strerror(status));
        exit(EXIT_FAILURE);
    }

    FILE* fptr = fopen("/usr/share/tscan/tscan-common-services", "r");

    if(!fptr) {
        fprintf(stderr, "ERROR: Could not open /usr/share/tscan/tscan-common-services\n");
        exit(EXIT_FAILURE);
    }

    char c;
    int max_common = 0;
    while((c = fgetc(fptr)) != EOF) {
        if(c == '\n') {
            max_common++;
        }
    }
    fseek(fptr, 0, SEEK_SET);

    tscan->common.max_common = max_common;
    tscan->common.ports = malloc(sizeof(int) * max_common);
    tscan->common.services = malloc(sizeof(char*) * max_common);

    char common_service[50];
    int common_port;

    for(int i = 0; i < max_common; ++i) {
        tscan->common.services[i] = malloc(sizeof(char) * 50);
        fscanf(fptr, "%s %d", common_service, &common_port);
        tscan->common.ports[i] = common_port;
        strncpy(tscan->common.services[i], common_service, 50);
    }

    return tscan;
}

int tscan_getnameinfo(Tscan* tscan, char* host) {
    char service[20];
    if(tscan->addr->ai_family == AF_INET) {
        tscan->ipv4 = (struct sockaddr_in*)tscan->addr->ai_addr;
        if(getnameinfo((struct sockaddr*)tscan->ipv4, sizeof(struct sockaddr_in), host, 255, service, 20, 0) == 0) {
            return 0;
        }
    }

    tscan->ipv4 = NULL;

    return -1;
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

void tscan_portscan_thread(TscanArgs *args) {
    Tscan* tscan = args->tscan;

    uint16_t current_port;

    int sockfd;

    for(int port = args->start; port < args->end; ++port) {
        int conn;

        if(tscan->scan_all) {
            conn = tscan_connect(tscan, &sockfd, port);
            current_port = port;
        }

        if(!tscan->scan_all) {
            conn = tscan_connect(tscan, &sockfd, tscan->common.ports[port]);
            current_port = tscan->common.ports[port];
        }

        if(conn == 0) {
            tscan->open_ports[tscan->ptop++] = current_port;
            printf("\e[34m%s:\e[00m PORT %d is open\n", tscan->ipstr, current_port);
        }

        close(sockfd);
    }

    for(uint16_t i = 0; i < tscan->ptop; ++i) {
        bool sorted = false;
        for(uint16_t j = 0; j < tscan->ptop - i - 1; ++j) {
            if(tscan->open_ports[j] > tscan->open_ports[j + 1]) {
                uint16_t aux = tscan->open_ports[j];
                tscan->open_ports[j] = tscan->open_ports[j + 1];
                tscan->open_ports[j + 1] = aux;
                sorted = true;
            }
        }
        if(!sorted) break;
    }
}

void tscan_portscan(Tscan* tscan) {
    printf("Looking for open ports...\n");

    int max_ports = tscan->common.max_common;

    if(tscan->scan_all) max_ports = 0xFFFF;

    pthread_t threads[tscan->max_threads];
    TscanArgs args[tscan->max_threads];

    int start = 0, end = 0;

    for(int i = 0; i < tscan->max_threads; ++i) {
        start = end;
        end = end + (max_ports/tscan->max_threads);

        args[i].tscan = tscan;
        args[i].start = start;
        args[i].end = end;

        pthread_create(&threads[i], NULL, (void*)tscan_portscan_thread, (void*)&args[i]);
    }

    for(int i = 0; i < tscan->max_threads; ++i) {
        pthread_join(threads[i], NULL);
    }
}

void tscan_open_ports(Tscan* tscan) {
    printf("\nOpen ports: \n");
    printf("\e[34mPORT\t\e[36mSERVICE\e[00m\n");
    for(int i = 0; i < tscan->ptop; ++i) {
        printf("\e[34m");
        if(tscan->open_ports[i] > 9999) {
            printf("%d   ", tscan->open_ports[i]);
        } else {
            printf("%d\t", tscan->open_ports[i]);
        }

        printf("\e[36m");
        for(int j = 0; j < tscan->common.max_common; j++) {
            if(tscan->common.ports[j] == tscan->open_ports[i]) {
                printf("%s\n", tscan->common.services[j]);
            }
        }
        printf("\e[00m");
    }
}

void tscan_save_ports(Tscan* tscan) {
    if(tscan->save_ports) {
        for(int i = 0; i < tscan->ptop; ++i) {
            fprintf(tscan->ports_file, "%d\n", tscan->open_ports[i]);
        }
        fclose(tscan->ports_file);
    }
}

void tscan_close(Tscan* tscan) {
    freeaddrinfo(tscan->addr);
    
    for(int i = 0; i < tscan->common.max_common; ++i) {
        free(tscan->common.services[i]);
    }

    free(tscan->common.ports);
    free(tscan->common.services);

    free(tscan);
}
