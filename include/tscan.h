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

} Tscan;

Tscan* tscan_init(const char* hostname);

void tscan_close(Tscan* tscan);

#endif
