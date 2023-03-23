#include "../Server.hpp"

using std::string;
using std::vector;
using std::map;

namespace {
	const string client = "ft_irc.de";
}

void	Server::pass(User &user, Command c)
{
	if (c.getArgs().size() != 1)
		sendResponse("461", "PASS :Not enough parameters", user);
	else if (user.isRegistered())
		sendResponse("462", " :You may not reregister", user);
	else if (c.getArgs()[0] != _password)
		sendResponse("464", " :Password incorrect", user);
	else
	{
		user.registrate();
		string	response1 = "001 " + user.getNick() + " :Welcome to the " + client + " Network!\r\n";
		string	response2 = "321 " + user.getNick() + "Channel :Users Name\r\n";
		string	response3 = "323 " + user.getNick() + ": End of /LIST";
		sendResponse(response1 + response2 + response3, user);
	}
}

bool Server::alreadyInUse(string mode, string name)
{
	std::map<int, User>::iterator it = _users.begin();
	
	for (; it != _users.end(); it ++)
	{
		if (mode == "user")
		{
			if (it->second.getName() == name)
				return (true);
		}
		else if (mode == "nick")
		{
			if (it->second.getNick() == name)
				return (true);
		}
	}
	return (false);
}

void	Server::nick(User &user, Command c)
{
	if (c.getArgs().size() != 1)
		sendResponse("461", "NICK :Not enough parameters", user);
	else if (contains(c.getArgs()[0], ":/\0"))
		sendResponse("432", c.getArgs()[0] + " :Erroneus nickname", user);
	else if (alreadyInUse("nick", c.getArgs()[0]))
		sendResponse("433", c.getArgs()[0] + " :Nickname is already in use", user);
	else
	{
		string oldNickname(user.getNick());

		user.setNick(c.getArgs()[0]);
		sendResponse(oldNickname + " changed his nickname to " + user.getNick(), user);
	}
}

void	Server::user(User &user, Command c)
{
	if (c.getArgs().size() != 4 || c.getArgs()[0].empty())
		sendResponse("461", "USER :Not enough parameters", user);
	else if (alreadyInUse("user", c.getArgs()[0]))
		sendResponse("462"," :You may not reregister", user);
	else
	{
		user.setName(c.getArgs()[0]);
		user.setFull(c.getArgs()[3]);
		sendResponse("001", "User gets registered with username '" + user.getName() + "' and real name \"" + user.getFull() + "\"", user);
	}
}

void	Server::ping(User &user, Command c)
{
	if (c.getArgs().size() < 1)
		sendResponse("461", "PING :Not enough parameters", user);
	else if (c.getArgs().size() == 1)
		sendResponse("PONG message from " + client + " to " + c.getArgs()[0] , user);
	else
	{
		string args;
		for (unsigned int i = 0; i < c.getArgs().size(); i++)
			args += c.getArgs()[i] + " ";
		sendResponse("PONG message from " + client + " " + args, user);
	}
}

void	Server::cap(User &user, Command c)
{
	if (c.getArgs()[0] == "LS")
		sendResponse("CAP * LS :End of CAP LS negotiation", user);
}

void	Server::quit(User &user, Command c)
{
	// REMINDER: message needs to get send to channel
	if (c.getArgs().size() == 0)
	{
		removeUser(user);
		sendResponse("Quit: ", user);
	}
	else
	{
		string response;
		for (unsigned int i = 0; i < c.getArgs().size(); i++)
			response.append(c.getArgs()[i] + " ");
		removeUser(user);
		sendResponse("Quit: " + response, user);
	}
}
