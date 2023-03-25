#include "../Server.hpp"

using std::string;
using std::vector;
using std::map;

namespace {
	const string hostname = ":ft_irc.de";
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

std::string	Server::getRPL_list(User &u)
{
	std::string RPL_list;

	RPL_list = ":" + u.getNick() + "@" + u.getName() + "!" + hostname.substr(1) + " 321 " + u.getNick() + " Channel: Users Name\r\n";
	RPL_list += getChannelNames(u);
	RPL_list += ":" + u.getNick() + "@" + u.getName() + "!" + hostname.substr(1) + " 323 " + u.getNick() + " End of /LIST\r\n";
	return (RPL_list);
}

std::string	Server::getRPL_namelist(std::vector<Channel>::iterator chan_it, User &u)
{
	std::string RPL_namelist;

	RPL_namelist = hostname + " 353 " + u.getNick() + " = " + chan_it->_name + " :@" + getUsersIn(chan_it) + "\r\n";
	RPL_namelist += hostname + " 366 " + u.getNick() + " " + chan_it->_name + " :End of /NAMES list\r\n";
	return (RPL_namelist);
}