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
	exit(1);
	// setPoll(_listeningSocket);
	while (!_stop)
	{
		loop(_listeningSocket);
	}
	close(_listeningSocket);
}

void	executeCommand(){}

void	acceptUser(){}

void Server::loop(int fd)
{
	// poll();
	std::map<int, User>::iterator it = _users.find(_listeningSocket);
	if (it != _users.end())
		executeCommand();
	else
		acceptUser();
}