# Multithreaded Server

This project implements a multithreaded HTTP proxy server in C with an in-memory LRU cache. The proxy accepts HTTP GET requests from clients, forwards them to remote servers, caches responses, and serves cached content for repeated requests to improve performance.

## Features

* **Multithreaded Design:** Handles up to 400 concurrent client connections using POSIX threads.

* **LRU Caching:** Caches responses in memory. Evicts least recently used items when full.
* **Error Handling:** Returns HTTP error responses like 400, 403, 404, 500, 501, and 505.
* **Thread-Safe Cache:** Uses mutexes to ensure safe concurrent access.
* **Connection Management:** Limits concurrent clients using semaphores.


## Project Structure

```
.
├── Makefile
├── proxy                      

├── include/
│   ├── cache.h
│   ├── network_utils.h
│   ├── proxy_parse.h
│   ├── proxy_server.h
│   └── proxy_utils.h

├── src/
│   ├── cache.c
│   ├── main.c
│   ├── network_utils.c
│   ├── proxy_parse.c
│   ├── proxy_server.c
│   └── proxy_utils.c

```

## Build Instructions
To clean build artifacts:
```bash
make clean
```
To build the project:

```bash
make
```

## Running the Proxy

Run the server by passing the port number:

```bash
./proxy <port_number>
```

Example:

```bash
./proxy 8080
```


## Example requests


```
curl -x http://localhost:8080 http://example.com
curl -x http://localhost:8080 http://neverssl.com
curl -x http://localhost:8080 http://example.org
curl -x http://localhost:8080 http://info.cern.ch
curl -x http://localhost:8080 http://httpbin.org/html
```

## Components
* **main.c:** Initializes cache and starts proxy server.
* **proxy\_server.c:** Accepts connections, manages threads.
* **proxy\_utils.c:** Handles forwarding, response management, caching.
* **cache.c:** Thread-safe in-memory LRU cache.
* **network\_utils.c:** Error message generation, remote connections.
* **proxy\_parse.c:** HTTP request parsing library.

