#include "Server.hpp"

using namespace std;

static bool containsChars(string &haystack, string const &needles)
{
	for (u_int32_t i = 0; i < haystack.size(); i++)
	{
		if (needles.find(haystack[i]) != string::npos)
			return true;
	}
	return false;
}

static bool invalid(long port)
{
	return (port <= 0 || port > 65535 || errno == ERANGE);
}

static bool invalid(string password)
{
	return (password.empty() || containsChars(password, ":,"));
}

Server::Server(int argc, char **argv){
	if (argc != 3)
		throw wrongNumberOfArgs();
	long port = strtol(argv[1], NULL, 10);
	if (invalid(port)) throw wrongPortNumber();
	_port = int(port);
	_password = argv[2];
	if (invalid(_password)) throw incorrectPassword();
	//REMINDER: Do we need this?
	_stop = false;
}

Server::~Server(){}

void Server::run(void)
{
	_listeningSocket = setupSocket(_port);
	setupPoll(_listeningSocket);
	while (!_stop)
	{ 
		loop(_listeningSocket);
	}
	close(_listeningSocket);
}

void	Server::getMessage(char *buf, int fd)
{
	User	*user = &_users.find(fd)->second;

	user->setBuffer(buf);
	// user->setCommand;
	cout << "[" << user->getBuffer() << "]" << endl;
}

int	Server::executeCommand(int fd)
{
	char	buf[4096];
	string	test;
	int		recvBytes;

	memset(buf, 0, 4096);
	recvBytes = recv(fd, buf, 4096, 0);
	if (recvBytes == -1)
		handle_error("ERROR READING BYTES");
	else if (recvBytes == 0)
		removeFromPoll(fd);
	else
	{
		getMessage(buf, fd);
	}
	return(0);
}

void	Server::acceptUser()
{
	SOCKET		clientSocket;
	sockaddr_in	client;
	socklen_t	clientSize = sizeof(client);
	char	host[INET_ADDRSTRLEN];

	clientSocket = accept(_listeningSocket, (sockaddr *)&client, &clientSize);
	inet_ntop(AF_INET, &client.sin_addr, host, INET_ADDRSTRLEN);
    cout << "Hostmask: " << host << endl;
	cout << "Client Socket: " << clientSocket << endl;
	if (clientSocket == -1)
		throw acceptOnSocketFailed();
	cout << "Accept was succesfull" << endl;
	addToPoll(clientSocket);
	_users.insert(pair<int, User>(clientSocket, User(clientSocket, host)));
	cout << "Client added" << endl;
}

void Server::loop(int fd)
{
	static int counter;

	if (poll(_userPoll, _activePoll, 5000) == -1)
		throw activePollFull();
	for (int i = 0; i < _activePoll; i++)
	{
		if (_userPoll[i].revents & POLLIN)
		{
			if (_userPoll[i].fd == _listeningSocket)
				acceptUser();
			else
				executeCommand(_userPoll[i].fd);
		}
	}
	return ; 
}