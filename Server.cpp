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

void	Server::executeCommand(string cmd)
{
	cout << "Command is: " << cmd << endl;
}

void	Server::processCommand(char *buf, int fd)
{
	User	*user = &_users.find(fd)->second;
	string	cmd;

	user->setBuffer(buf);
	while(1)
	{
		cmd = user->parseCommand();
		if (cmd.empty())
			break;
		executeCommand(cmd);
	}
}

void	Server::processInput(int fd)
{
	char	buf[8192];
	int		recvBytes;

	memset(buf, 0, 8192);
	recvBytes = recv(fd, buf, 8192, 0);
	if (recvBytes == -1 || recvBytes == 0)
	{
		if (recvBytes == 0)
			removeFromPoll(fd);
		throw readingMsgFailed();
	}
	else
		processCommand(buf, fd);
}

void	Server::acceptUser()
{
	SOCKET		clientSocket;
	sockaddr_in	client;
	socklen_t	clientSize = sizeof(client);
	char		host[INET_ADDRSTRLEN];

	clientSocket = accept(_listeningSocket, (sockaddr *)&client, &clientSize);
	inet_ntop(AF_INET, &client.sin_addr, host, INET_ADDRSTRLEN);
	if (clientSocket == -1)
		throw acceptOnSocketFailed();
	addToPoll(clientSocket);
	_users.insert(pair<int, User>(clientSocket, User(clientSocket, host)));
	cout << "Accept was succesfull!" << endl;
    cout << "Hostmask: " << host << " Client Socket: " << clientSocket << " Client added!" << endl;
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
				processInput(_userPoll[i].fd);
		}
	}
	return ; 
}