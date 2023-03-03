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


SOCKET	create_socket()
{
	SOCKET	fd = socket(AF_INET, SOCK_STREAM, 0);
	int		optVal = 1;
	int		optValSize = sizeof(optVal);

	// if (setsockopt(fd, SOL_SOCKET, (SO_REUSEADDR | SO_NOSIGPIPE), (const char*)&optVal, optValSize) == -1)
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const char*)&optVal, optValSize) == -1 || setsockopt(fd, SOL_SOCKET, SO_NOSIGPIPE, (const char*)&optVal, optValSize) == -1)
	{
		handle_error("SET SOCKET OPT FAILURE");
		return (-1);
	}
	fcntl(fd, F_SETFL, O_NONBLOCK);
	if (fd == -1)
	{
		handle_error("SOCKET FAILURE");
		return (-1);
	}
	std::cout << "SOCKET LISTENS ON: " << fd << std::endl;
	return (fd);
}

int bind_socket(SOCKET socket_fd)
{
	int			bind_ret;
	sockaddr_in my_addr;

	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(10000);
	inet_pton(AF_INET, "0.0.0.0", &my_addr.sin_addr);

	bind_ret = bind(socket_fd, (sockaddr *)&my_addr, sizeof(my_addr));
	if (bind_ret == -1)
	{
		handle_error("BIND FAILURE");
		return (-1);
	}
	std::cout << "BIND WAS SUCCESFULL" << std::endl;
	return (bind_ret);
}

int	listen_socket(SOCKET socket_fd)
{
	int	listen_ret;

	listen_ret = listen(socket_fd, 128);
	if (listen_ret == -1)
	{
		handle_error("LISTEN FAILURE");
		return (-1);
	}
	std::cout << "LISTEN WAS SUCCESFULL" << std::endl;
	return (listen_ret);
}

SOCKET accept_socket(SOCKET socket_fd)
{
	SOCKET		clientSocket;
	sockaddr_in	client;
	socklen_t	clientSize = sizeof(client);

	clientSocket = accept(socket_fd, (sockaddr *)&client, &clientSize);
	std::cout << "ACCEPT WAS SUCCESFULL" << std::endl;
	char	host[INET_ADDRSTRLEN];	// get user's hostmask
	
	inet_ntop(AF_INET, &client.sin_addr, host, INET_ADDRSTRLEN);
	std::string	hostmask(host);
    std::cout << "Client hostmask: " << hostmask << std::endl;
	std::cout << "User connected on fd: " << clientSocket << std::endl;
	if (clientSocket == -1)
	{
		handle_error("ACCEPT FAILURE");
		return (-1);
	}
	std::cout << "Client connected" << std::endl;
	// addToPoll(clientSocket);
	return(clientSocket);
}

// int	connect_socket(int user_fd)
// {

// }

int main()
{
	SOCKET socket_fd;
	struct pollfd fds[200];
	int bind_fd;
	int pollReturn;
	int listen_fd;
	SOCKET clientSocket;

	socket_fd = create_socket();
	bind_fd = bind_socket(socket_fd);
	listen_fd = listen_socket(socket_fd);
	memset(fds, 0, sizeof(fds));
	fds[0].fd = listen_fd;
	fds[0].events = POLLIN;
	pollReturn = poll(fds, 1, 5000);
	if (pollReturn == -1)
	{
		handle_error("POLL FAILURE");
		return (-1);	
	}
	clientSocket = accept_socket(socket_fd);

	std::cout << "SOCKET FD IS: " << socket_fd << " ACCEPT FD IS:" << clientSocket << std::endl;
	close(clientSocket);
}