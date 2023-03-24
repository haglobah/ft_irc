#include "../Server.hpp"

using std::string;
using std::vector;
using std::map;

namespace {
	const string hostname = "ft_irc.de";
}

string parsePassword(string password)
{
	string ret;

	if (password.front() == ':')
		password = password.substr(1);
	return (password);
}

void	Server::pass(User &user, Command c)
{
	string password;

	if (c.getArgs().size() != 1)
		sendResponse("461", "PASS :Not enough parameters", user);
	else if (user.isRegistered())
		sendResponse("462", " :You may not reregister", user);
	password = parsePassword(c.getArgs()[0]);
	if (password != _password)
		sendResponse("464", " :Password incorrect", user);
	else
		user.registrate();
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
		sendResponse("NICK :" + oldNickname + " changed his nickname to " + user.getNick(), user);
	}
}

string	Server::getChannelNames(User user)
{
	string channels;

	for (vector<Channel>::iterator it = _channels.begin(); it != _channels.end(); it++)
	{
		std::stringstream userCount;
		userCount << it->_userCount;
		string userStr = userCount.str();
		if (it->_topic.empty())
			channels += ":ft_irc.de 322 " + user.getNick() + " " + it->_name + " " + userStr + " :No topic is set\r\n";
		else
			channels += ":ft_irc.de 322 " + user.getNick() + " " + it->_name + " " + userStr + " :" + it->_topic + "\r\n";
	}
	return (channels);
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
		sendResponse("001", user.getNick() + " Welcome to the " + hostname + " Network!" , user);
		sendResponse("321", user.getNick() + " Channel: Users Name ", user);
		sendResponseRaw(getChannelNames(user), user);
		sendResponse("323", user.getNick() + " End of /LIST" , user);
	}
}

void	Server::ping(User &user, Command c)
{
	if (c.getArgs().size() < 1)
		sendResponse("461", "PING :Not enough parameters", user);
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
		disconnectUser(user);
		sendResponse("QUIT: ", user);
	}
	else
	{
		string response;
		for (unsigned int i = 0; i < c.getArgs().size(); i++)
			response.append(c.getArgs()[i] + " ");
		disconnectUser(user);
		sendResponse("QUIT: " + response, user);
	}
}
