#ifndef PROXY_UTILS_H
#define PROXY_UTILS_H

struct ParsedRequest;

int checkHTTPversion(char *msg);
int handle_request(int clientSocket, struct ParsedRequest *request, char *tempReq);

#endif
