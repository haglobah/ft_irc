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

	if (setsockopt(socketFD, SOL_SOCKET, SO_REUSEADDR, (const char*)&optVal, optValSize) == -1 || setsockopt(socketFD, SOL_SOCKET, SO_NOSIGPIPE, (const char*)&optVal, optValSize) == -1)
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

SOCKET acceptSocket(SOCKET socketFD)
{
	SOCKET		clientSocket;
	sockaddr_in	client;
	socklen_t	clientSize = sizeof(client);

	clientSocket = accept(socketFD, (sockaddr *)&client, &clientSize);
	std::cout << "ACCEPT WAS SUCCESFULL" << std::endl;
	char	host[INET_ADDRSTRLEN];
	
	inet_ntop(AF_INET, &client.sin_addr, host, INET_ADDRSTRLEN);
	std::string	hostmask(host);
    std::cout << "CLIENT HOSTMASK: " << hostmask << std::endl;
	std::cout << "USER LISTENING ON FD: " << clientSocket << std::endl;
	if (clientSocket == -1)
	{
		handle_error("ACCEPT FAILURE");
		return (-1);
	}
	std::cout << "CLIENT LISTENING" << std::endl;
	return(clientSocket);
}



int	connectSocket(int socketFD)
{
	struct pollfd fds[200];
	SOCKET clientSocketFD;
	int pollReturn;

	memset(fds, 0, sizeof(fds));
	fds[0].fd = socketFD;
	fds[0].events = POLLIN;
	pollReturn = poll(fds, 1, 50000);
	if (pollReturn == -1)
	{
		handle_error("POLL FAILURE");
		return (-1);	
	}
	clientSocketFD = acceptSocket(socketFD);
	std::cout << "SOCKET FD IS: " << socketFD << " ACCEPT FD IS:" << clientSocketFD << std::endl;
	return(clientSocketFD);
}

int setupSocket(int port)
{
	SOCKET socketFd;
	int listenFD;

	socketFd = createSocket();
	bindSocket(socketFd, port);
	listenOnSocket(socketFd);

	int userFD = connectSocket(socketFd);
	return userFD;
}