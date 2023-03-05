#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <iostream>
#include <netdb.h>
#include <poll.h>
#include <unistd.h>

typedef int SOCKET;

#define handle_error(msg) \
        do { perror(msg); return(EXIT_FAILURE); } while (0)


SOCKET	createSocket()
{
	SOCKET	socketFD = socket(AF_INET, SOCK_STREAM, 0);
	int		optVal = 1;
	int		optValSize = sizeof(optVal);

	if (setsockopt(socketFD, SOL_SOCKET, SO_REUSEADDR, (const char*)&optVal, optValSize) == -1 
	||	setsockopt(socketFD, SOL_SOCKET, SO_NOSIGPIPE, (const char*)&optVal, optValSize) == -1)
	{
		handle_error("SET SOCKET OPT FAILURE");
		return (-1);
	}
	fcntl(socketFD, F_SETFL, O_NONBLOCK);
	if (socketFD == -1)
	{
		handle_error("SOCKET FAILURE");
		return (-1);
	}
	std::cout << "SOCKET CREATED ON FD: " << socketFD << std::endl;
	return (socketFD);
}

int bindSocket(SOCKET socketFD, int port)
{
	int			bindRet;
	sockaddr_in myAddr;

	myAddr.sin_family = AF_INET;
	myAddr.sin_port = htons(port);
	inet_pton(AF_INET, "0.0.0.0", &myAddr.sin_addr);

	bindRet = bind(socketFD, (sockaddr *)&myAddr, sizeof(myAddr));
	if (bindRet == -1)
	{
		handle_error("BIND FAILURE");
		return (-1);
	}
	std::cout << "BIND WAS SUCCESFULL" << std::endl;
	return (bindRet);
}

int	listenOnSocket(SOCKET socketFD)
{
	int	listenRet;

	listenRet = listen(socketFD, 128);
	if (listenRet == -1)
	{
		handle_error("LISTEN FAILURE");
		return (-1);
	}
	std::cout << "LISTEN WAS SUCCESFULL" << std::endl;
	return (listenRet);
}

int setupSocket(int port)
{
	SOCKET socketFD;

	socketFD = createSocket();
	bindSocket(socketFD, port);
	listenOnSocket(socketFD);
	return (socketFD);
}