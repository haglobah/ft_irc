#include "Server.hpp"

using std::string;
using std::vector;
using std::map;

namespace {
	const string client = "www.ft_irc.de";
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
		sendResponse("001", "Welcome to the " + client + " Network!" + user.getNick(), user);
	}
}

bool Server::alreadyInUse(string mode, string name, User user)
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
	else if (contains(c.getArgs()[1], ":/\0"))
		sendResponse("432", c.getArgs()[0] + " :Erroneus nickname", user);
	else if (alreadyInUse("nick", c.getArgs()[0], user))
		sendResponse("433", c.getArgs()[0] + " :Nickname is already in use", user);
	else
	{
		string oldNickname(user.getNick());

		user.setNick(c.getArgs()[0]);
		sendResponse(oldNickname + "changed his nickname to " + user.getNick(), user);
	}

}

void	Server::user(User &user, Command c)
{
	if (c.getArgs().size() != 4 || c.getArgs()[0].empty())
		sendResponse("461", "USER :Not enough parameters", user);
	else if (alreadyInUse("user", c.getArgs()[0], user))
		sendResponse("462"," :You may not reregister", user);
	else
	{
		user.setName(c.getArgs()[0]);
		user.setFull(c.getArgs()[3]);
		sendResponse("User gets registered with username '" + user.getName() + "' and real name \"" + user.getFull() + "\"", user);
	}
}

void	Server::ping(User &user, Command c)
{
	if (c.getArgs().size() != 1)
		sendResponse("461", "PING :Not enough parameters", user);
	else
		sendResponse("PONG message from " + client + " to " + c.getArgs()[0] , user);
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
		sendResponse("Quit: ", user);
	else
	{
		string response;
		for (int i = 0; i < c.getArgs().size(); i++)
			response.append(c.getArgs()[i] + " ");
		sendResponse("Quit: " + response, user);
	}
	removeUser(user);
}
