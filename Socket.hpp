#pragma once

#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <iostream>
#include <netdb.h>
#include <unistd.h>

typedef int SOCKET;

#define handle_error(msg) \
        do { perror(msg); return(EXIT_FAILURE); } while (0)

SOCKET	createSocket();
SOCKET	acceptSocket(SOCKET socket_fd);
int 	bindSocket(SOCKET socketFD, int port);
int		listenOnSocket(SOCKET socketFD);
int		connectSocket(int socketFD);
int 	bindSocket(SOCKET socketFD, int port);
SOCKET	setupSocket(int port);