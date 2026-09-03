#include "socket.h"

#include <stdio.h>

#define BACKLOG 1

#ifdef __WIN32__

#include <winsock2.h>
#include <ws2tcpip.h>

const sock_t SOCKET_INVALID = INVALID_SOCKET;
const int SOCK_ERROR = SOCKET_ERROR;

int socket_startup(void) {
    WSADATA wsa_data;

    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
        fprintf(stderr, "Version 2.2 of Winsock not available\n");
        return -1;
    }

    if (LOBYTE(wsa_data.wVersion) != 2 || HIBYTE(wsa_data.wVersion) != 2) {
        fprintf(stderr, "Version 2.2 of Winsock not available\n");
        WSACleanup();
        return -1;
    }

    return 0;
}

int socket_cleanup(void) {
    return WSACleanup();
}

static int set_non_blocking(sock_t sock_fd) {
    u_long mode = 1; // Non-blocking
    return ioctlsocket(sock_fd, FIONBIO, &mode);
}

bool socket_would_block(sock_t sock_fd) {
    return sock_fd == WSAEWOULDBLOCK;
}

#endif

#ifdef __linux__

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#define closesocket close

const sock_t SOCKET_INVALID = -1;
const int SOCK_ERROR = -1;

int socket_startup(void) {
    return 0;
}

int socket_cleanup(void) {
    return 0;
}

static int set_non_blocking(sock_t sock_fd) {
    return fcntl(sock_fd, F_SETFL, O_NONBLOCK);
}

bool socket_would_block(sock_t sock_fd) {
    return sock_fd == EAGAIN || sock_fd == EWOULDBLOCK;
}

#endif

sock_t socket_start(const char* host, const char* port) {
    struct addrinfo hints = {
        .ai_family = AF_UNSPEC,
        .ai_socktype = SOCK_STREAM,
    };

    struct addrinfo* res;
    int status = getaddrinfo(host, port, &hints, &res);

    if (status != 0) {
        fprintf(stderr, "gai error: %s\n", gai_strerror(status));
        return SOCKET_INVALID;
    }

    sock_t sock_fd;
    struct addrinfo* p;
    for (p = res; p != NULL; p = p->ai_next) {
        sock_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);

        if (sock_fd == SOCKET_INVALID) {
            perror("socket error");
            continue;
        }

#ifdef __WIN32__
        char yes = 1;
#endif

#ifdef __linux__
        int yes = 1;
#endif

        if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == SOCK_ERROR) {
            perror("setsockopt error");
            return SOCKET_INVALID;
        }

        if (bind(sock_fd, p->ai_addr, p->ai_addrlen) == SOCK_ERROR) {
            closesocket(sock_fd);
            perror("bind error");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "Could not bind to socket\n");
        return SOCKET_INVALID;
    }

    if (listen(sock_fd, BACKLOG) == SOCK_ERROR) {
        perror("listen error");
        return SOCKET_INVALID;
    }

    set_non_blocking(sock_fd);

    return sock_fd;
}

sock_t socket_try_to_accept(sock_t sock_fd) {
    struct sockaddr_storage addr = { 0 };
    int addrlen = sizeof(addr);

    sock_t new_sock_fd = accept(sock_fd, (struct sockaddr*) &addr, &addrlen);

    if (new_sock_fd == (sock_t) SOCK_ERROR) {
        perror("accept error");
        return SOCKET_INVALID;
    }

    return new_sock_fd;
}

sock_t socket_join(const char* host, const char* port) {
    struct addrinfo hints = {
        .ai_family = AF_UNSPEC,
        .ai_socktype = SOCK_STREAM,
    };

    struct addrinfo* res;
    int status = getaddrinfo(host, port, &hints, &res);

    if (status != 0) {
        fprintf(stderr, "gai error: %s\n", gai_strerror(status));
        return SOCKET_INVALID;
    }

    sock_t sock_fd;
    struct addrinfo* p;
    for (p = res; p != NULL; p = p->ai_next) {
        sock_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);

        if (sock_fd == SOCKET_INVALID) {
            perror("socket error");
            continue;
        }

        if (connect(sock_fd, p->ai_addr, p->ai_addrlen) == SOCK_ERROR) {
            closesocket(sock_fd);
            perror("connect error");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "Could not connect to socket\n");
        return SOCKET_INVALID;
    }

    set_non_blocking(sock_fd);

    return sock_fd;
}

int socket_send(sock_t s, const char *buf, int len, int flags) {
    int total_sent = 0;

    while (total_sent < len) {
        int sent = send(s, buf, len, flags);
        if (sent == SOCK_ERROR) return sent;
        total_sent += sent;
    }

    return total_sent;
}

int socket_recv(sock_t s, char *buf, int len, int flags) {
    int total_received = 0;

    while (total_received < len) {
        int received = recv(s, buf, len, flags);
        if (received == SOCK_ERROR) return received;
        total_received += received;
    }

    return total_received;
}
