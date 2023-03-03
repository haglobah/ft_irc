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

void createSocket()
{

}
void bindSocket()
{

}
void listenOnSocket()
{

}
void setPoll()
{

}

void setupSocket()
{
	createSocket();
	bindSocket();
	listenOnSocket();
}

void Server::run(void)
{
	setupSocket();
	setPoll();
	while (!_stop)
	{
		loop();
	}
	close(_listeningSocket);
}
void Server::loop(void)
{
	poll();
	for (int i = 0; i < _activeUsers)
	{
		;
	}
}