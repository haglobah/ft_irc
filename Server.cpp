#include "Server.hpp"

using std::string;
using std::cout;
using std::cerr;
using std::endl;
using std::pair;

namespace {
	const string hostname = "ft_irc.de";
}

bool invalid(long port)
{
	return (port <= 0 || port > 65535 || errno == ERANGE);
}

bool invalid(string password)
{
	return (password.empty() || contains(password, ":,"));
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
		loop();
	}
	close(_listeningSocket);
}


string	getCommand(string& buf)
{
	string cmd;
	size_t newLine;
	
	while (buf.find("\r\n") != std::string::npos)
		buf.replace(buf.find("\r\n"), 2, "\n");
	newLine = buf.find('\n');
	cmd = buf.substr(0, newLine);
	buf.erase(0, newLine + 1);
	return (cmd);
}

void	Server::executeCommand(User &u, Command& c)
{
	std::string cmd = c.getName();
	cout << "Command is: |" << cmd << "|";
	printsvec(c.getArgs());

	// CONNECTION
	if 		(cmd == "PASS") { pass(u, c); } // WORKS
	else if (cmd == "CAP") { cap(u, c); } // WORKS
	else if (cmd == "PING") { ping(u, c); } // WORKS
	else if (!u.getAllowConnection())	{ sendResponse("462", ":You need to register first!", u); } // WORKS
	else if (cmd == "NICK") { nick(u, c); } // WORKS
	else if (cmd == "USER") { user(u, c); } // WORKS
	else if (cmd == "OPER") { oper(u, c); } // WORKS
	else if (cmd == "QUIT") { quit(u, c); } // WORKS

	// CHANNEL
	else if (cmd == "JOIN") { join(u, c); } // WORKS
	else if (cmd == "PART") { part(u, c); } // WORKS
	else if (cmd == "TOPIC") { topic(u, c); } // WORKS
	else if (cmd == "LIST") { list(u, c); } // WORKS
	else if (cmd == "KICK") { kick(u, c); }  // DOESNT WORK -> maybe just remove

	// SERVER
	else if (cmd == "MODE") { mode(u, c); }

	// USER
	else if (cmd == "PRIVMSG") { privmsg(u, c); } //WORKS
	else if (cmd == "NOTICE") { notice(u, c); }
	else if (cmd == "WHO") { who(u, c); }
	else { sendResponse("421", cmd + " :" + cmd, u); } // WORKS
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
			removeConnection(fd);
		throw readingMsgFailed();
	}
	else
		processCommands(buf, fd);
}

void	Server::disconnectUser(User &u)
{
	u.setDisconnected();
	for (vector<Channel>::iterator it = _channels.begin(); it != _channels.end(); it++)
	{
		if (isUserIn(u, it->_name))
		{
			it->removeUser(&u);
			sendToChannel(":" + u.getNick() + "!" + u.getName() + "@" + hostname.substr(1) + " QUIT " + it->_name + "\r\n", *it, u);
			sendToChannel(getRPL_list(u), *it, u);
			sendResponseRaw(getRPL_list(u), u);
		}
		if (it->_userCount == 0)
			removeChannel(it);
	}
	removeConnection(u.getFD());
	_users.erase(u.getFD());
	close(u.getFD());
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
	addConnection(clientSocket);
	_users.insert(pair<int, User>(clientSocket, User(clientSocket, host)));
	cout << "Accept was successful!" << endl;
    cout << "Hostmask: " << host << " hostname Socket: " << clientSocket << " hostname added!" << endl;
}

void Server::loop()
{
	if (poll(_userPoll, _activePoll, 5000) == -1)
		throw activePollFull();
	for (int i = 0; i < _activePoll; i++)
	{
		try
		{
			if (_userPoll[i].revents & POLLIN)
			{
				if (_userPoll[i].fd == _listeningSocket)
					acceptUser();
				else
					receiveInput(_userPoll[i].fd);
			}
		}
		catch (std::exception &e)
		{
			cerr << e.what() << endl;
		}
	}
	return ; 
}