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
		sendResponse("Not the same amount of channel names and keys", u);
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

void	Server::addUser(std::vector<Channel>::iterator it, User &user)
{
	it->addUser(user);
	sendToChannel(user.getName() + " is joining the channel " + it->_name, *it);
	
	Command show_topic("TOPIC " + it->_name);
	topic(user, show_topic);
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
	std::cout << "Channel 1: " << _channels[0]._name << std::endl;
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
			std::cout << "Before join channel" << std::endl;
			joinChannel(it, user);
			Command topicCommand("TOPIC " + it->first);
			topic(user, topicCommand);
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
			sendToChannel("331 " + c.getArgs()[0] + " :No topic is set", chan);
		}
		else
		{
			chan._topic = c.getArgs()[1];
			sendToChannel("332" + c.getArgs()[0] + " :" + chan._topic, chan);
			// REMINDER: add time after getNick
			sendToChannel("333" + c.getArgs()[0] + " " + user.getNick() + " ", chan);
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
			sendToChannel(user.getNick() + " leave channel " + it->first + " because " + c.getArgs()[1], *getChannel(it->first));
		else
			sendToChannel(user.getNick() + " leave channel " + it->first, *getChannel(it->first));
		getChannel(it->first)->removeUser(user);
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
			chan.removeUser(userToKick);
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

void	Server::mode(User &user, Command c)
{
	if (c.getArgs().size() < 1)
		sendResponse("461", "MODE :Not enough parameters", user);
	else if (c.getArgs().size() == 1)
	{
		if (!isUserRegistered(c.getArgs()[0]))
			sendResponse("401", c.getArgs()[0] + " :No such nick/channel", user);
		else if (c.getArgs()[0] != user.getNick())
			sendResponse("502", " :Can't change mode for other users", user);
		else
		{

		}
	}
}
