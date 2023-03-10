// #include "commands.hpp"
#include "Server.hpp"

using std::string;
using std::vector;
using std::map;

namespace {
	const string client = "www.ft_irc.de";
}

void	Server::sendResponse(string numeric_reply, string message, User& user)
{
	string response;

	response = client + " " + numeric_reply + " " + message + "\r\n";
	std::cout << "Response to send is: " << response << std::endl;
	if (send(user.getFD(), response.c_str(), response.length(), 0) == -1)
		std::cout << "Couldn't send the response to FD :" << user.getFD() << std::endl;
	if (user.isDisconnected())
		removeUser(user);
}

void	Server::sendResponse(string message, User& user)
{
	string response;

	response = client + " " + message + "\r\n";
	std::cout << "Response to send is: " << response << std::endl;
	if (send(user.getFD(), response.c_str(), response.length(), 0) == -1)
		std::cout << "Couldn't send the response to FD :" << user.getFD() << std::endl;
	if (user.isDisconnected())
		removeUser(user);
}

void	Server::sendToChannel(string message, Channel c)
{ 
	std::cout << "Message to the channel is: " << message << std::endl;
	for(map<const User *, Privileges>::iterator it = c._users.begin();
		it != c._users.end();
		it++)
	{
		if (send(it->first->getFD(), message.c_str(), message.length(), 0) == -1)
			std::cout << "Couldn't send the message to FD in the channel:" << it->first->getFD() << std::endl;
	}
}

bool	isChannelValid(string channel)
{
	if (contains(channel, "\7"))
		return (false);
	return (true); 
}

bool	Server::notInChannelNames(string channel)
{
	for (std::vector<Channel>::iterator it = _channels.begin();
		 it != _channels.end();
		 it++)
		{
			if (it->_name == channel)
				return (false);
		}
	return (true);
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
		if (!isChannelValid(c))
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
	addUser(_channels.end()--, user);
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

void	part(User &user, Command c);

bool Server::isUserIn(User &u, string name)
{
	for (vector<Channel>::iterator it = _channels.begin(); it != _channels.end(); it++)
	{
		if (it->_name == name)
			if (it->_users.find(&u) != it->_users.end())
				return (true);
	}
	return (false);
}

Channel& Server::getChannel(string name)
{
	for (vector<Channel>::iterator it = _channels.begin(); it != _channels.end(); it++)
	{
		if (it->_name == name)
			return (*it);
	}
}

void	Server::topic(User &user, Command c)
{
	std::cout << "TEST" << std::endl;
	if (c.getArgs().size() > 2)
		sendResponse("461", "TOPIC :Not enough parameters", user);
	
	Channel& chan = getChannel(c.getArgs()[0]);
	std::cout << chan._name << std::endl;
	if (!isUserIn(user, c.getArgs()[0]))
		sendResponse("442", c.getArgs()[0] + " :You're not on that channel", user);
	// else if (c.getArgs().size() == 1)
		
	// 	sendResponse();
	// else if (c.getArgs().size() == 2)
	// 	chan_keys = parseChannels(user, c.getArgs()[0], c.getArgs()[1]);
}

void	list(User &user, Command c);
void	kick(User &user, Command c);
void	invite(User &user, Command c);

void	Server::privmsg(User &user, Command c)
{
	
}


