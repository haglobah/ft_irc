#include "../Server.hpp"

using std::string;
using std::vector;
using std::map;

namespace {
	const string hostname = "ft_irc.de";
}

void	Server::pass(User &user, Command& c)
{
	string password;

	if (c.getArgs().size() != 1)
		sendResponseServer("461", "PASS :Not enough parameters", user);
	else if (user.isRegistered())
		sendResponseServer("462", ":You may not reregister", user);
	password = parsePassword(c.getArgs()[0]);
	if (password != _password)
		sendResponseServer("464", ":Password incorrect", user);
	else
		user.setAllowConnection();
}

void	Server::nick(User &user, Command& c)
{
	if (c.getArgs().size() != 1)
		sendResponseServer("461", "NICK :Not enough parameters", user);
	else if (contains(c.getArgs()[0], ":/\0"))
		sendResponseServer("432", c.getArgs()[0] + " :Erroneus nickname", user);
	else if (alreadyInUse("nick", c.getArgs()[0]))
		sendResponseServer("433", c.getArgs()[0] + " :Nickname is already in use", user);
	else
	{
		if (user.getNick() == "*")
			sendResponse("NICK " + c.getArgs()[0], user);
		else
			sendResponseRaw(":" + user.getNick() + "!" + user.getName() + " NICK " + c.getArgs()[0] + "\r\n", user);
		user.setNick(c.getArgs()[0]);
		registrate(user);
	}
}

void	Server::user(User &user, Command& c)
{
	if (c.getArgs().size() != 4 || c.getArgs()[0].empty())
		sendResponseServer("461", "USER :Not enough parameters", user);
	else if (alreadyInUse("user", c.getArgs()[0]))
		sendResponseServer("462",":You may not reregister", user);
	else
	{
		user.setName(c.getArgs()[0]);
		user.setFull(c.getArgs()[3]);
		registrate(user);
	}
}

void	Server::ping(User &user, Command& c)
{
	if (c.getArgs().size() < 1)
		sendResponseServer("461", "PING :Not enough parameters", user);
	else if (c.getArgs().size() == 1)
		sendResponse("PONG :message from " + hostname + " to " + c.getArgs()[0] , user);
	else
	{
		string args;
		for (unsigned int i = 0; i < c.getArgs().size(); i++)
			args += c.getArgs()[i] + " ";
		sendResponse("PONG :message from " + hostname + " " + args, user);
	}
}

void	Server::cap(User &user, Command& c)
{
	if (c.getArgs()[0] == "LS")
		sendResponse("CAP * LS :End of CAP LS negotiation", user);
}

void	Server::quit(User &user, Command& c)
{
	if (c.getArgs().size() == 0)
	{
		user.setDisconnected();
		sendResponseRaw(":" + user.getNick() + "!" + user.getName() + "@" + hostname.substr(1) + " QUIT\r\n", user);

	}
	else
	{
		string response;
		for (unsigned int i = 0; i < c.getArgs().size(); i++)
			response.append(c.getArgs()[i] + " ");
		user.setDisconnected();
		sendResponseRaw(":" + user.getNick() + "!" + user.getName() + " QUIT " + response + "\r\n", user);
	}
}
