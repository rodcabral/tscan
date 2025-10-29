#include "../include/tscan.h"

Tscan* tscan_init(const char* hostname [[maybe_unused]]) {
    Tscan* tscan = malloc(sizeof(Tscan));

    return tscan;
}

void tscan_close(Tscan* tscan) {
    free(tscan);
}
