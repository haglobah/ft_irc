#include "../Server.hpp"

using std::string;
using std::vector;
using std::map;

namespace {
	const string hostname = ":ft_irc.de ";
}

map<string, string> Server::parseChannels(User &u, string channelStr)
{
	map<string, string>		channels;
	string					c;

	std::istringstream chanStream(channelStr);
    while (getline(chanStream, c, ','))
	{
		if (!isChannelValid(c))
		{
			channels.clear();
			sendResponse("403", c + " :No such channel found", u);
		}
        channels.insert(std::pair<string, string>(c, ""));
	}
	return (channels);
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
	if (getline(keyStream, k, ','))
	{
		chan_keys.clear();
		sendResponse("JOIN :Not the same amount of channel names and keys", u);
	}
	while (getline(chanStream, c, ','))
	{
		if (!isChannelValid(c))
		{
			chan_keys.clear();
			sendResponse("403", c + " :No such channel found", u);
		}
        chan_keys.insert(std::pair<string, string>(c, ""));
	}
	return (chan_keys);
}

string	getUsersIn(vector<Channel>::iterator chan_it)
{
	string	usersInChannel;

	for (map<const User *, Privileges>::iterator it = chan_it->_users.begin(); it != chan_it->_users.end(); it++)
	{
		usersInChannel += it->first->getNick();
		if (it != chan_it->_users.end())
			usersInChannel += " ";
	}
	return (usersInChannel);
}

void	Server::addUser(std::vector<Channel>::iterator chan_it, User &user)
{
	string	channelName = chan_it->_name;
	string	userName = user.getName();
	chan_it->addUser(&user);
	// sendToChannel(userName + "@localhost JOIN :" + channelName, *chan_it);
	string start = ":" + user.getNick() + "!" + user.getName() + "@" + hostname.substr(1) + " JOIN :" + channelName + "\r\n"; 
	string response;
	if (chan_it->_topic.empty())
		response = ":ft_irc.de 332 " + user.getNick() + " " + channelName + " :No topic is set\r\n";
	else
		response = ":ft_irc.de 332 " + user.getNick() + " " + channelName + " :" + chan_it->_topic + "\r\n";
	string response1 = ":ft_irc.de 353 " + user.getNick() + " = " + channelName + " :@" + getUsersIn(chan_it) + "\r\n";
	string response2 = ":ft_irc.de 366 " + user.getNick() + " " + channelName + " :End of /NAMES list\r\n";
	string response3 = ":ft_irc.de 321 " + user.getNick() + " Channel :Users Name " + getChannelNames() +  "\r\n";
	string response4;
	if (chan_it->_topic.empty())
		response4 = ":ft_irc.de 322 " + user.getNick() + " " + channelName + " :No topic is set\r\n";
	else
		response4 = ":ft_irc.de 322 " + user.getNick() + " " + channelName + " :" + chan_it->_topic + "\r\n";
	string response5 = ":ft_irc.de 323 " + user.getNick() + " :End of /LIST";
	sendResponseJoin(start + response + response1 + response2 + response3 + response4 + response5, user);
	chan_it->showUsers();
}

void	Server::joinChannel(map<string, string>::iterator chan_key, User &user)
{
	for (std::vector<Channel>::iterator it = _channels.begin();
		it != _channels.end();
		it++)
	{
		if (chan_key->first == it->_name)
		{
			if (chan_key->second == it->_key || it->_key.empty())
			{
				addUser(it, user);
				return ;
			}
			else
			{
				sendResponse("475", it->_name + " :Cannot join channel (+k)", user);
				return ;
			}
		}
	}
	_channels.push_back(Channel(chan_key->first));
	addUser(_channels.end() - 1, user);
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
		for (map<string, string>::iterator it = chan_keys.begin();
			 it != chan_keys.end();
			 it++)
		{
			joinChannel(it, user);
		}
	}
}

vector<Channel>::iterator Server::getChannel(string name)
{
	for (vector<Channel>::iterator it = _channels.begin(); it != _channels.end(); it++)
	{
		if (it->_name == name)
			return (it);
	}
	return (_channels.end());
}

void	Server::topic(User &user, Command c)
{
	if (c.getArgs().size() > 2 || c.getArgs().size() < 1)
		sendResponse("461", "TOPIC :Not enough parameters", user);
	else if (!isUserIn(user, c.getArgs()[0]))
		sendResponse("442", c.getArgs()[0] + " :You're not on that channel", user);
	else if (c.getArgs().size() == 1)
	{
		vector<Channel>::iterator chan_it = getChannel(c.getArgs()[0]);
		if (chan_it->_topic.empty())
			sendResponse("331", c.getArgs()[0] + " :No topic is set", user);
		else
		{
			sendResponse("332", c.getArgs()[0] + " :" + chan_it->_topic, user);
			// REMINDER: add time after getNick
			sendResponse("333", c.getArgs()[0] + " " + user.getNick() + " " , user);
		}
	}
	else if (c.getArgs().size() == 2)
	{
		Channel& chan = *getChannel(c.getArgs()[0]);
		std::map<const User *, Privileges>::iterator it = chan._users.find(&user);
		if (chan._modes == TOPIC_RESTRICTED && it->second != OPERATOR)
		{
			sendResponse("482", c.getArgs()[0] + " :You're not channel operator", user);
			return ;
		}
		else if (c.getArgs()[1].empty())
		{
			chan._topic.clear();
			sendToChannel("331 " + c.getArgs()[0] + " :No topic is set", chan, user);
		}
		else
		{
			chan._topic = c.getArgs()[1];
			sendToChannel("332" + c.getArgs()[0] + " :" + chan._topic, chan, user);
			// REMINDER: add time after getNick
			sendToChannel("333" + c.getArgs()[0] + " " + user.getNick() + " ", chan, user);
		}
	}
}

void	Server::part(User &user, Command c)
{
	if (c.getArgs().size() > 2 || c.getArgs().size() < 1)
		sendResponse("461", "PART :Not enough parameters", user);

	map<string, string>		chan_keys;
	chan_keys = parseChannels(user, c.getArgs()[0]);
	for (map<string, string>::iterator it = chan_keys.begin(); it != chan_keys.end(); it++)
	{
		if (!c.getArgs()[1].empty()) // REMINDER: can you check it like that?
			sendToChannel("PART :" + user.getNick() + " leave channel " + it->first + " because " + c.getArgs()[1], *getChannel(it->first), user);
		else
			sendToChannel("PART :" + user.getNick() + " leave channel " + it->first, *getChannel(it->first), user);
		getChannel(it->first)->removeUser(&user);
	}
}

void	Server::list(User &user, Command c)
{
	if (c.getArgs().size() != 1)
		sendResponse("461", "LIST :Not enough parameters", user);
	else
	{
		map<string, string>		chan_keys;
		chan_keys = parseChannels(user, c.getArgs()[0]);
		for (map<string, string>::iterator it = chan_keys.begin(); it != chan_keys.end(); it++)
		{
			Channel chan = *getChannel(it->first);
			sendResponse("322", chan._name + " " + std::to_string(chan._userCount) + " :" + chan._topic ,user);
		}
		sendResponse("323", " :End of /LIST", user);
	}
}

User& Server::getUser(string name)
{
	for (map<int, User>::iterator it = _users.begin(); it != _users.end(); it++)
	{
		if (it->second.getNick() == name)
			return (it->second);
	}
	return (_users.end()->second);	
}

void	Server::kick(User &user, Command c)
{
	if (c.getArgs().size() != 2)
	{
		sendResponse("461", "KICK :Not enough parameters", user);
		return ;
	}
	
	Channel& chan = *getChannel(c.getArgs()[0]);
	std::map<const User *, Privileges>::iterator it = chan._users.find(&user);
	if (notInChannelNames(c.getArgs()[0]))
		sendResponse("403", c.getArgs()[0] + " :No such channel", user);
	else if (it == chan._users.end())
		sendResponse("442", c.getArgs()[0] + " :You're not on that channel", user);
	else if (it->second != OPERATOR)
		sendResponse("482", c.getArgs()[0] + " :You're not channel operator", user);
	else
	{
		User& userToKick = getUser(c.getArgs()[1]);
		if (!isUserIn(userToKick, c.getArgs()[0]))
			sendResponse("441", userToKick.getNick() + " " + c.getArgs()[0] + " :They aren't on that channel", user);
		else
		{
			chan.removeUser(&userToKick);
			sendResponse(user.getNick() + " has kicked " + userToKick.getNick() + " from channel " + c.getArgs()[0], user);
		}
	}
	
}

void	Server::oper(User &user, Command c)
{
	if (c.getArgs().size() != 2)
		sendResponse("461", "OPER :Not enough parameters", user);
	else
	{
		if (_password != c.getArgs()[1])
			sendResponse("464", ": Password incorrect", user);
		else
		{
			for (vector<Channel>::iterator it = _channels.begin(); it != _channels.end(); it++)
			{
				map<const User *, Privileges>::iterator channelMap = it->_users.find(&user);
				channelMap->second = OPERATOR;
			}
			sendResponse("381", "You are now an IRC operator", user);
		}
	}

}

void Server::who(User &user, Command c)
{
	string resp;

	if (c.getArgs().size() > 1)
		sendResponse("461", "WHO :Too many parameters", user);
	else
	{
		for (map<int, User>::iterator it = _users.begin(); it != _users.end(); it++)
		{
			resp += hostname + "352 " + user.getNick() + " * " 
				+ it->second.getName() + " * " + hostname + it->second.getNick() + " H :0 " + it->second.getFull() + "\r\n";
		}
		resp += hostname + "315 " + user.getNick() + "* :End of /WHO list";
	}
	sendResponse(resp, user);
}

void	Server::mode(User &user, Command c)
{
	if (c.getArgs().size() < 1)
		sendResponse("461", "MODE :Not enough parameters", user);
	else if (c.getArgs().size() == 1)
	{
		string target = c.getArgs()[0];
		if (!isUserRegistered(target))
			sendResponse("401", target + " :No such nick/channel", user);
		else if (target != user.getNick())
			sendResponse("502", " :Can't change mode for other users", user);
		else
		{

		}
	}
}
