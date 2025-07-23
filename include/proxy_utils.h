#ifndef PROXY_UTILS_H
#define PROXY_UTILS_H

// Forward declare ParsedRequest
struct ParsedRequest;

int checkHTTPversion(char *msg);
int handle_request(int clientSocket, struct ParsedRequest *request, char *tempReq);

#endif // PROXY_UTILS_H
