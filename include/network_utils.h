#ifndef NETWORK_UTILS_H
#define NETWORK_UTILS_H

/**
 * Sends an HTTP error response based on the given status code.
 * Returns 1 on success, -1 if status code is not recognized.
 */
int sendErrorMessage(int socket, int status_code);

/**
 * Creates a socket and connects to the given remote server (host:port).
 * Returns the socket file descriptor on success, -1 on failure.
 */
int connectRemoteServer(char* host_addr, int port_num);

#endif // NETWORK_UTILS_H
