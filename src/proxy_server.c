#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <pthread.h>
#include <semaphore.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "proxy_parse.h"
#include "proxy_utils.h"
#include "network_utils.h"
#include "cache.h"
#include "proxy_server.h"

#define MAX_BYTES 4096
#define MAX_CLIENTS 400

static int proxy_socketId;
static pthread_t tid[MAX_CLIENTS];
static int connected_sockets[MAX_CLIENTS];
static sem_t semaphore;

void* thread_fn(void* arg) {
    sem_wait(&semaphore);
    int clientSocket = *(int*)arg;

    char *buffer = (char*)calloc(MAX_BYTES, sizeof(char));
    int bytes_read = recv(clientSocket, buffer, MAX_BYTES, 0);

    while (bytes_read > 0 && strstr(buffer, "\r\n\r\n") == NULL) {
        int len = strlen(buffer);
        bytes_read = recv(clientSocket, buffer + len, MAX_BYTES - len, 0);
    }

    char *tempReq = strdup(buffer);
    cache_element* cached = find(tempReq);

    if (cached) {
        int pos = 0;
        while (pos < cached->len) {
            int to_send = (cached->len - pos > MAX_BYTES) ? MAX_BYTES : (cached->len - pos);
            send(clientSocket, cached->data + pos, to_send, 0);
            pos += to_send;
        }
        printf("Cache hit for request\n");
    } else if (bytes_read > 0) {
        ParsedRequest *request = ParsedRequest_create();
        if (ParsedRequest_parse(request, buffer, strlen(buffer)) == 0) {
            if (strcmp(request->method, "GET") == 0 &&
                request->host && request->path &&
                checkHTTPversion(request->version) == 1) {

                if (handle_request(clientSocket, request, tempReq) < 0) {
                    sendErrorMessage(clientSocket, 500);
                }
            } else {
                sendErrorMessage(clientSocket, 501);
            }
        } else {
            sendErrorMessage(clientSocket, 400);
        }
        ParsedRequest_destroy(request);
    } else {
        perror("Client disconnected or recv error");
    }

    shutdown(clientSocket, SHUT_RDWR);
    close(clientSocket);
    free(buffer);
    free(tempReq);
    sem_post(&semaphore);
    return NULL;
}

void start_proxy_server(int port_number) {
    struct sockaddr_in server_addr, client_addr;
    int client_socketId, client_len;

    sem_init(&semaphore, 0, MAX_CLIENTS);

    proxy_socketId = socket(AF_INET, SOCK_STREAM, 0);
    if (proxy_socketId < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    int reuse = 1;
    setsockopt(proxy_socketId, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port_number);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(proxy_socketId, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        exit(1);
    }

    if (listen(proxy_socketId, MAX_CLIENTS) < 0) {
        perror("Listen failed");
        exit(1);
    }

    printf("Proxy server listening on port %d...\n", port_number);

    int i = 0;
    while (1) {
        client_len = sizeof(client_addr);
        client_socketId = accept(proxy_socketId, (struct sockaddr*)&client_addr, (socklen_t*)&client_len);
        if (client_socketId < 0) {
            perror("Accept failed");
            continue;
        }

        connected_sockets[i] = client_socketId;
        pthread_create(&tid[i], NULL, thread_fn, &connected_sockets[i]);
        i++;
    }

    close(proxy_socketId);
}
