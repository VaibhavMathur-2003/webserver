#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>

#include "proxy_utils.h"
#include "cache.h"
#include "network_utils.h"
#include "proxy_parse.h"

#define MAX_BYTES 4096

int checkHTTPversion(char *msg)
{
    if (strncmp(msg, "HTTP/1.1", 8) == 0 || strncmp(msg, "HTTP/1.0", 8) == 0)
    {
        return 1;
    }
    return -1;
}

int handle_request(int clientSocket, ParsedRequest *request, char *tempReq)
{
    char *buf = (char *)malloc(MAX_BYTES);
    if (!buf)
        return -1;

    snprintf(buf, MAX_BYTES, "GET %s %s\r\n", request->path, request->version);
    size_t len = strlen(buf);

    if (ParsedHeader_set(request, "Connection", "close") < 0)
    {
        printf("Failed to set Connection header\n");
    }

    if (ParsedHeader_get(request, "Host") == NULL)
    {
        if (ParsedHeader_set(request, "Host", request->host) < 0)
        {
            printf("Failed to set Host header\n");
        }
    }

    if (ParsedRequest_unparse_headers(request, buf + len, MAX_BYTES - len) < 0)
    {
        printf("Header unparse failed, continuing with minimal headers\n");
    }

    int server_port = (request->port != NULL) ? atoi(request->port) : 80;
    int remoteSocketID = connectRemoteServer(request->host, server_port);

    if (remoteSocketID < 0)
    {
        free(buf);
        return -1;
    }

    if (send(remoteSocketID, buf, strlen(buf), 0) < 0)
    {
        perror("Failed to send to remote server");
        close(remoteSocketID);
        free(buf);
        return -1;
    }

    char *temp_buffer = (char *)malloc(MAX_BYTES);
    if (!temp_buffer)
    {
        close(remoteSocketID);
        free(buf);
        return -1;
    }

    int temp_buffer_size = MAX_BYTES;
    int temp_buffer_index = 0;

    ssize_t recv_len;
    while ((recv_len = recv(remoteSocketID, buf, MAX_BYTES, 0)) > 0)
    {
        send(clientSocket, buf, recv_len, 0);

        if (temp_buffer_index + recv_len >= temp_buffer_size)
        {
            temp_buffer_size *= 2;
            temp_buffer = (char *)realloc(temp_buffer, temp_buffer_size);
            if (!temp_buffer)
            {
                close(remoteSocketID);
                free(buf);
                return -1;
            }
        }

        memcpy(temp_buffer + temp_buffer_index, buf, recv_len);
        temp_buffer_index += recv_len;
    }

    temp_buffer[temp_buffer_index] = '\0';

    add_cache_element(temp_buffer, temp_buffer_index, tempReq);

    close(remoteSocketID);
    free(buf);
    free(temp_buffer);

    return 0;
}
