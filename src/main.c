#include <stdio.h>
#include <stdlib.h>
#include "proxy_server.h"
#include "cache.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port_number>\n", argv[0]);
        return 1;
    }

    int port = atoi(argv[1]);
    init_cache();
    start_proxy_server(port);
    return 0;
}
