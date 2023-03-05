#include "Server.hpp"

static bool containsChars(std::string &haystack, std::string const &needles)
{
	for (u_int32_t i = 0; i < haystack.size(); i++)
	{
		if (needles.find(haystack[i]) != std::string::npos)
			return true;
	}
	return false;
}

static bool invalid(long port)
{
	return (port <= 0 || port > 65535 || errno == ERANGE);
}

static bool invalid(std::string password)
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
	while (!_stop)
	{
		loop(_listeningSocket);
	}
	close(_listeningSocket);
}

void	executeCommand(){}

int	Server::addToPoll(int clientSocket)
{
	if (_activePoll >= SOMAXCONN)
		handle_error("ADD TO POLL FAILURE");
	_userPoll[_activePoll].fd = clientSocket;
	_userPoll[_activePoll].events = POLLIN; 
	_activePoll++;
	return (0);
}

int	Server::removeFromPoll(int clientFD)
{
	int	i = -1;

	for (int j = 0; j < _activePoll; j++)
	{
		if (_userPoll[j].fd == clientFD)
			i = j;
	}
	if (i == -1)
		handle_error("NO POLL FOUND");
	_userPoll[i].fd = _userPoll[_activePoll - 1].fd;
	_userPoll[i].events = POLLIN;
	_userPoll[_activePoll - 1].fd--;
	_activePoll--;
	return (0);
}

int	Server::acceptUser()
{
	SOCKET		clientSocket;
	sockaddr_in	client;
	socklen_t	clientSize = sizeof(client);

	clientSocket = accept(_listeningSocket, (sockaddr *)&client, &clientSize);
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
	addToPoll(clientSocket);
	_users.insert(std::pair<int, User>(clientSocket, User(clientSocket, hostmask)));
	std::cout << "CLIENT LISTENING" << std::endl;
	return(clientSocket);
}

int Server::setPoll(int socketFD)
{
	int	pollReturn;

	memset(_userPoll, 0, sizeof(_userPoll));
	_activePoll = 1;
	_userPoll[0].fd	= socketFD;
	_userPoll[0].events = POLLIN;
	pollReturn = poll(_userPoll, 1, 50000);
	if (pollReturn == -1)
	{
		handle_error("POLL FAILURE");
		return (-1);	
	}
	return (0);
}

void Server::loop(int fd)
{
	setPoll(_listeningSocket);
	std::map<int, User>::iterator it = _users.find(_listeningSocket);
	if (it != _users.end())
		executeCommand();
	else
		acceptUser();
}