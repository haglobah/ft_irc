#include "Server.hpp"

#include "commands/commands.hpp"

using std::string;
using std::cout;
using std::cerr;
using std::endl;
using std::pair;

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


string	getCommand(string& buf)
{
	string cmd;
	size_t newLine;
	
	while (buf.find("\r\n") != std::string::npos)
		buf.replace(buf.find("\r\n"), 2, "\n");
	// REMINDER: might be a bug
	newLine = buf.find('\n');
	cmd = buf.substr(0, newLine);
	buf.erase(0, newLine + 1);
	return (cmd);
}


void	Server::executeCommand(User &u, Command c)
{
	std::string cmd = c.getName();
	cout << "Command is: " << cmd << endl;

	if 		(cmd == "PASS") { pass(u, c); }
	else if (cmd == "NICK") { nick(u, c); }
	else if (cmd == "USER") { user(u, c); }
	else if (cmd == "PING") { ping(u, c); }
	else if (cmd == "CAP") { cap(u, c); }
	// else if (cmd == "JOIN") { join(u, c); }
	// else if (cmd == "PRIVMSG") { privmsg(u, c); }
	// else if (cmd == "TOPIC") { topic(u, c); }
	// else if (cmd == "WHO") { who(u, c); }
	// else if (cmd == "PART") { part(u, c); }
	// else if (cmd == "MODE") { mode(u, c); }
	// else if (cmd == "QUIT") { quit(u, c); }
	// else if (cmd == "LIST") { list(u, c); }
	// else if (cmd == "INVITE") { invite(u, c); }
	// else if (cmd == "KICK") { kick(u, c); }
	else	{}		
}

void	Server::processCommands(string buf, int fd)
{
	User	&user = _users.find(fd)->second;
	string	cmd;

	while(1)
	{
		cmd = getCommand(buf);
		if (cmd.empty())
			break;
		Command c(cmd);
		executeCommand(user, c);
	}
}

void	Server::receiveInput(int fd)
{
	char	buf[8192];
	int		received;

	memset(buf, 0, 8192);
	received = recv(fd, buf, 8192, 0);
	if (received == -1 || received == 0)
	{
		if (received == 0)
			removeUser(fd);
		throw readingMsgFailed();
	}
	else
		processCommands(buf, fd);
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
				receiveInput(_userPoll[i].fd);
		}
	}
	return ; 
}