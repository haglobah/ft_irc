#include "Server.hpp"

void	Server::cleanupUser(int fd)
{
	map<int, User>::iterator it = getUserFD(fd);
	if (it != _users.end())
		disconnectUser(it->second);
	else
		removeConnection(fd);
}

void	cleanNewlines(string &acc)
{
	while (acc.find("\r\n") != std::string::npos)
		acc.replace(acc.find("\r\n"), 2, "\n");
}

bool	hasNewline(string &acc)
{
	size_t newline = acc.find("\n");
	return (newline != std::string::npos);
}

string	extractCmd(string &acc)
{
	size_t newline = acc.find("\n");
	string cmd = acc.substr(0, newline);
	acc.erase(0, newline + 1);
	return (cmd);
}

bool	hasEOF(string &acc)
{
	size_t eof = acc.find(EOF);
	return (eof != std::string::npos);
}

void	rmEOF(string &acc)
{
	size_t eof = acc.find(EOF);
	acc.erase(eof);
}

string	getCommand(User &u)
{
	string cmd;

	string &acc = u._cmdAcc;
	cleanNewlines(acc);
	if (hasNewline(acc))
	{
		string cmd = extractCmd(acc);
		return (cmd);
	}
	if (hasEOF(acc))
		rmEOF(acc);
	return ("");
}

void	Server::processCommands(User &u)
{
	string	cmd;

	while(1)
	{
		cmd = getCommand(u);
		if (cmd.empty())
			break;
		Command c(cmd);
		executeCommand(u, c);
	}
}

void	Server::receiveInput(int fd)
{
	char	buf[8912];
	int		received;
 
	memset(buf, 0, 8192);
	received = recv(fd, buf, 8192, 0);
	if (received == -1)
		throw readingMsgFailed();
	else if (received == 0)
		cleanupUser(fd);
	else
	{
		User &u = getUserFD(fd)->second;
		u.appendAcc(buf);
		processCommands(u);
	}
}

