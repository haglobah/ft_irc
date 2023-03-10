// #include "commands.hpp"
#include "Server.hpp"

using std::string;
using std::vector;
using std::map;

namespace {
	const string client = "www.ft_irc.de";
}

bool contains(string s, string chars)
{
	for (int i = 0; i < chars.length(); i++)
	{
		if (s.find(chars[i]) != string::npos)
			return (true);
	}
	return (false);
}

void Server::sendResponse(string numeric_reply, string message, User& user)
{
	string response;

	if (numeric_reply.empty())
		response = client + " " + message + "\r\n";
	else
		response = client + " " + numeric_reply + " " + message + "\r\n";
	std::cout << "Response to send is: " << response << std::endl;
	if (send(user.getFD(), message.c_str(), message.length(), 0) == -1)
		std::cout << "Couldn't send the response to FD :" << user.getFD() << std::endl;
	if (user.isDisconnected())
		removeUser(user);
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
		sendResponse("001", "Welcome to the " + client + " Network, " + user.getNick(), user);
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
		sendResponse("", oldNickname + "changed his nickname to " + user.getNick(), user);
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
		sendResponse("" ,"User gets registered with username '" + user.getName() + "' and real name \"" + user.getFull() + "\"", user);
	}
}

void	Server::ping(User &user, Command c)
{
	if (c.getArgs().size() != 1)
		sendResponse("461", "PING :Not enough parameters", user);
	else
		sendResponse("", "PONG message from " + client + " to " + c.getArgs()[0] , user);
}

void	Server::cap(User &user, Command c)
{
	if (c.getArgs()[0] == "LS")
		sendResponse("", "CAP * LS :End of CAP LS negotiation", user);
}

void	Server::quit(User &user, Command c)
{
	// REMINDER: message needs to get send to channel
	if (c.getArgs().size() == 0)
		sendResponse("", "Quit: ", user);
	else
	{
		string response;
		for (int i = 0; i < c.getArgs().size(); i++)
			response.append(c.getArgs()[i] + " ");
		sendResponse("", "Quit: " + response, user);
	}
	removeUser(user);
}

bool	isChannelValid(string channel)
{
	if (contains(channel, "\7"))
		return (false);
	return (true); 
}

bool	Server::notInChannelNames(string channel)
{
	return (_channels.find(channel) == _channels.end());
}

map<string, string> Server::parseChannels(User &u, string channelStr)
{
	map<string, string>		chan_keys;
	string					c;

	std::istringstream chanStream(channelStr);
    while (getline(chanStream, c, ','))
	{
		if (!isChannelValid(c) || notInChannelNames(c))
		{
			chan_keys.clear();
			sendResponse("403", c + " :No such channel found", u);
		}
        chan_keys.insert(std::pair<string, string>(c, ""));
	}
	return (chan_keys);
}

map<string, string> Server::parseChannels(User &u, string channelStr, string keyStr)
{
	map<string, string>		chan_keys;
	string					c;
	string					k;

	std::istringstream chanStream(channelStr);
	std::istringstream keyStream(keyStr);
    while (getline(chanStream, c, ',') && getline(keyStream, k, ','))
	{
		if (!isChannelValid(c) || notInChannelNames(c))
		{
			chan_keys.clear();
			sendResponse("403", c + " :No such channel found", u);
		}
        chan_keys.insert(std::pair<string, string>(c, k));
	}
	if (getline(chanStream, c, ',') || getline(keyStream, k, ','))
	{
		chan_keys.clear();
		sendResponse("", "Not the same amount of channel names and keys", u);
	}
	return (chan_keys);
}

void	Server::join(User &user, Command c)
{
	map<string, string> chan_keys;

	if (c.getArgs().size() > 2)
		sendResponse("461", "JOIN :Not enough parameters", user);
	else if (c.getArgs().size() == 1)
		chan_keys = parseChannels(user, c.getArgs()[0]);
	else if (c.getArgs().size() == 2)
		chan_keys = parseChannels(user, c.getArgs()[0], c.getArgs()[1]);
	if (chan_keys.empty())
		return ;
	else
	{
		for (std::map<string, string>::iterator it = chan_keys.begin();
			 it != chan_keys.end();
			 it++)
		{
			Channel *channel = _channels.find(it->first);
			if (channel != _channels.end())
			{
				if (channel._name == it->second)
					join;
				else
				{
					sendResponse();
					return ;
				}
			}
		}
		join
	}
}

void	Server::privmsg(User &user, Command c)
{
	
}


