#include "../Server.hpp"

using std::string;
using std::vector;
using std::map;

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

bool	Server::inChannelNames(string channel)
{
	return (!notInChannelNames(channel));
}

bool	Server::isUserRegistered(string name)
{
	for (map<int, User>::iterator it = _users.begin(); it != _users.end(); it++)
	{
		if (it->second.getNick() == name)
			return (true);
	}
	return (false);
}

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

