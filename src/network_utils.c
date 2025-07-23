#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>

#include "network_utils.h"

#define SERVER_NAME "VaibhavN/14785"

int sendErrorMessage(int socket, int status_code) {
    char str[1024];
    char currentTime[50];
    time_t now = time(0);
    struct tm data = *gmtime(&now);

    strftime(currentTime, sizeof(currentTime), "%a, %d %b %Y %H:%M:%S %Z", &data);

    switch (status_code) {
        case 400:
            snprintf(str, sizeof(str),
                "HTTP/1.1 400 Bad Request\r\nContent-Length: 95\r\n"
                "Connection: close\r\nContent-Type: text/html\r\n"
                "Date: %s\r\nServer: %s\r\n\r\n"
                "<HTML><HEAD><TITLE>400 Bad Request</TITLE></HEAD>"
                "<BODY><H1>400 Bad Request</H1></BODY></HTML>",
                currentTime, SERVER_NAME);
            break;

        case 403:
            snprintf(str, sizeof(str),
                "HTTP/1.1 403 Forbidden\r\nContent-Length: 112\r\n"
                "Connection: close\r\nContent-Type: text/html\r\n"
                "Date: %s\r\nServer: %s\r\n\r\n"
                "<HTML><HEAD><TITLE>403 Forbidden</TITLE></HEAD>"
                "<BODY><H1>403 Forbidden</H1><br>Permission Denied</BODY></HTML>",
                currentTime, SERVER_NAME);
            break;

        case 404:
            snprintf(str, sizeof(str),
                "HTTP/1.1 404 Not Found\r\nContent-Length: 91\r\n"
                "Connection: close\r\nContent-Type: text/html\r\n"
                "Date: %s\r\nServer: %s\r\n\r\n"
                "<HTML><HEAD><TITLE>404 Not Found</TITLE></HEAD>"
                "<BODY><H1>404 Not Found</H1></BODY></HTML>",
                currentTime, SERVER_NAME);
            break;

        case 500:
            snprintf(str, sizeof(str),
                "HTTP/1.1 500 Internal Server Error\r\nContent-Length: 115\r\n"
                "Connection: close\r\nContent-Type: text/html\r\n"
                "Date: %s\r\nServer: %s\r\n\r\n"
                "<HTML><HEAD><TITLE>500 Internal Server Error</TITLE></HEAD>"
                "<BODY><H1>500 Internal Server Error</H1></BODY></HTML>",
                currentTime, SERVER_NAME);
            break;

        case 501:
            snprintf(str, sizeof(str),
                "HTTP/1.1 501 Not Implemented\r\nContent-Length: 103\r\n"
                "Connection: close\r\nContent-Type: text/html\r\n"
                "Date: %s\r\nServer: %s\r\n\r\n"
                "<HTML><HEAD><TITLE>501 Not Implemented</TITLE></HEAD>"
                "<BODY><H1>501 Not Implemented</H1></BODY></HTML>",
                currentTime, SERVER_NAME);
            break;

        case 505:
            snprintf(str, sizeof(str),
                "HTTP/1.1 505 HTTP Version Not Supported\r\nContent-Length: 125\r\n"
                "Connection: close\r\nContent-Type: text/html\r\n"
                "Date: %s\r\nServer: %s\r\n\r\n"
                "<HTML><HEAD><TITLE>505 HTTP Version Not Supported</TITLE></HEAD>"
                "<BODY><H1>505 HTTP Version Not Supported</H1></BODY></HTML>",
                currentTime, SERVER_NAME);
            break;

        default:
            return -1;
    }

    send(socket, str, strlen(str), 0);
    return 1;
}

int connectRemoteServer(char* host_addr, int port_num) {
    int remoteSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (remoteSocket < 0) {
        perror("Error creating remote socket");
        return -1;
    }

    struct hostent *host = gethostbyname(host_addr);
    if (host == NULL) {
        fprintf(stderr, "No such host: %s\n", host_addr);
        close(remoteSocket);
        return -1;
    }

    struct sockaddr_in server_addr;
    bzero((char*)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port_num);
    bcopy((char *)host->h_addr, (char *)&server_addr.sin_addr.s_addr, host->h_length);

    if (connect(remoteSocket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error connecting to remote server");
        close(remoteSocket);
        return -1;
    }

    return remoteSocket;
}
