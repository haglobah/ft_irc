#include "../Server.hpp"

using std::string;
using std::vector;
using std::map;

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
			sendResponseServer("403", c + " :No such channel found", u);
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
			sendResponseServer("403", c + " :No such channel found", u);
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
			sendResponseServer("403", c + " :No such channel found", u);
		}
        chan_keys.insert(std::pair<string, string>(c, ""));
	}
	return (chan_keys);
}

string	Server::getUsersIn(vector<Channel>::iterator chan_it)
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

vector<Channel>::iterator Server::getChannel(string name)
{
	for (vector<Channel>::iterator it = _channels.begin(); it != _channels.end(); it++)
	{
		if (it->_name == name)
			return (it);
	}
	return (_channels.end());
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


string	Server::getChannelNames(User &user, bool letUserSeeThemselves)
{
	string channels;

	for (vector<Channel>::iterator it = _channels.begin(); it != _channels.end(); it++)
	{
		if (letUserSeeThemselves || isUserIn(user, it->_name))
		{
			std::stringstream userCount;
			userCount << it->_userCount;
			string userStr = userCount.str();
			if (it->_topic.empty())
				channels += ":ft_irc.de 322 " + user.getNick() + " " + it->_name + " " + userStr + " :No topic is set\r\n";
			else
				channels += ":ft_irc.de 322 " + user.getNick() + " " + it->_name + " " + userStr + " :" + it->_topic + "\r\n";
		}
	}
	return (channels);
}

void	Server::removeChannel(vector<Channel>::iterator channel)
{
	_channels.erase(channel);
}