#pragma once

#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

typedef int SOCKET;

SOCKET	createSocket();
SOCKET	acceptSocket(SOCKET socket_fd);
int 	bindSocket(SOCKET socketFD, int port);
int		listenOnSocket(SOCKET socketFD);
int		connectSocket(int socketFD);
int 	bindSocket(SOCKET socketFD, int port);
SOCKET	setupSocket(int port);