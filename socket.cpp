#include "Server.hpp"

using namespace std;

SOCKET	createSocket()
{
	SOCKET	socketFD = socket(AF_INET, SOCK_STREAM, 0);
	int		optVal = 1;
	int		optValSize = sizeof(optVal);

	if (setsockopt(socketFD, SOL_SOCKET, SO_REUSEADDR, (const char*)&optVal, optValSize) == -1 
	||	setsockopt(socketFD, SOL_SOCKET, SO_NOSIGPIPE, (const char*)&optVal, optValSize) == -1)
		throw socketCreationFailed();
	fcntl(socketFD, F_SETFL, O_NONBLOCK);
	if (socketFD == -1)
		throw socketCreationFailed();
	cout << "Socket created with fd: " << socketFD << endl;
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
		throw bindingSocketFailed();
	cout << "Bind was successful!" << endl;
	return (bindRet);
}

int	listenOnSocket(SOCKET socketFD)
{
	int	listenRet;

	listenRet = listen(socketFD, 128);
	if (listenRet == -1)
		throw listenOnSocketFailed();
	cout << "Listen was successful!" << endl;
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