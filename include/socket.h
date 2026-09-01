#pragma once

#include <stdbool.h>

#ifdef __WIN32__

typedef unsigned long long sock_t;

#endif

#ifdef __linux__

typedef int sock_t;

#endif

extern const sock_t SOCKET_INVALID;
extern const int SOCK_ERROR;

int socket_startup(void);
int socket_cleanup(void);

sock_t socket_start(const char* host, const char* port); // Socket is non-blocking. Remember that when using it
sock_t socket_try_to_accept(sock_t sock_fd); // If obtained from socket_start, sock_fd is non-blocking. Remember to consider that
sock_t socket_join(const char* host, const char* port);

bool socket_would_block(sock_t sock_fd);
