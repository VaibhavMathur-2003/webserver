#ifndef NETWORK_UTILS_H
#define NETWORK_UTILS_H

int sendErrorMessage(int socket, int status_code);

int connectRemoteServer(char *host_addr, int port_num);

#endif
