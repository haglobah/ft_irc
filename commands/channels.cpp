#include "../Server.hpp"

using std::string;
using std::vector;
using std::map;

namespace {
	const string hostname = ":ft_irc.de ";
}

void	Server::addUser(std::vector<Channel>::iterator chan_it, User &user)
{
	string	channelName = chan_it->_name;
	chan_it->addUser(&user);
	string prefix = ":" + user.getNick() + "!" + user.getName() + "@" + hostname.substr(1) + "JOIN :" + channelName + "\r\n"; 
	string response;
	if (chan_it->_topic.empty())
		response = hostname + "332 " + user.getNick() + " " + channelName + " :No topic is set\r\n";
	else
		response = hostname + "332 " + user.getNick() + " " + channelName + " :" + chan_it->_topic + "\r\n";
	string RPL_namelist = getRPL_namelist(chan_it, user);
	string RPL_list = getRPL_list(user);
	sendResponseRaw(prefix + response + RPL_namelist + RPL_list, user);
	sendToChannel(prefix + RPL_list, *chan_it ,user);
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
				sendResponseServer("475", it->_name + " :Cannot join channel (+k)", user);
				return ;
			}
		}
	}
	_channels.push_back(Channel(chan_key->first));
	addUser(_channels.end() - 1, user);
}

void	Server::join(User &user, Command& c)
{
	map<string, string> chan_keys;

	if (c.getArgs().size() > 2)
	{
		sendResponseServer("461", "JOIN :Not enough parameters", user);
		return ;
	}
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

void	Server::topic(User &user, Command& c)
{
	if (c.getArgs().size() > 2 || c.getArgs().size() < 1)
		sendResponseServer("461", "TOPIC :Not enough parameters", user);
	else if (!isUserIn(user, c.getArgs()[0]))
		sendResponseServer("442", c.getArgs()[0] + " :You're not on that channel", user);
	else if (c.getArgs().size() == 1)
	{
		vector<Channel>::iterator chan_it = getChannel(c.getArgs()[0]);
		std::stringstream userCount;
		userCount << chan_it->_userCount;
		string userStr = userCount.str();
		if (chan_it->_topic.empty())
			sendResponseRaw(":ft_irc.de 331 " + user.getNick() + " " + chan_it->_name + " :No topic is set\r\n", user);
		else
		{
			sendResponseRaw(":ft_irc.de 332 " + user.getNick() + " " + chan_it->_name + " :" + chan_it->_topic + "\r\n", user);
		}
	}
	else if (c.getArgs().size() == 2)
	{
		vector<Channel>::iterator chan_it = getChannel(c.getArgs()[0]);
		std::map<const User *, Privileges>::iterator it = chan_it->_users.find(&user);
		if (/*chan_it->_modes == TOPIC_RESTRICTED && */it->second != OPERATOR)
		{
			sendResponseServer("482", c.getArgs()[0] + " :You're not channel operator", user);
			return ;
		}
		else if (c.getArgs()[1].empty())
		{
			sendResponseRaw(":ft_irc.de 331 " + user.getNick() + " " + chan_it->_name + " :No topic is set\r\n", user);
			sendToChannel(":ft_irc.de 331 " + user.getNick() + " " + chan_it->_name + " :No topic is set\r\n", *chan_it , user);
			chan_it->_topic.clear();
		}
		else
		{
			chan_it->_topic = c.getArgs()[1];
			sendResponseRaw(":" + user.getNick() + "!" + user.getName() + "@" + hostname + " TOPIC " + chan_it->_name + " :" + chan_it->_topic + "\r\n", user);
			sendToChannel(":" + user.getNick() + "!" + user.getName() + "@" + hostname + " TOPIC " + chan_it->_name + " :" + chan_it->_topic + "\r\n", *chan_it, user);
		}
	}
}

void	Server::part(User &user, Command& c)
{
	if (c.getArgs().size() > 2 || c.getArgs().size() < 1)
	{
		sendResponseServer("461", "PART :Not enough parameters", user);
		return ;
	}

	map<string, string>		chan_keys;
	chan_keys = parseChannels(user, c.getArgs()[0]);
	for (map<string, string>::iterator it = chan_keys.begin(); it != chan_keys.end(); it++)
	{
		string prefix = ":" + user.getNick() + "!" + user.getName() + "@" + hostname;
		if (!isUserIn(user, it->first))
			sendResponseServer("442", it->first + " :You're not on that channel", user);
		else if (!c.getArgs()[1].empty()) // REMINDER: can you check it like that?
		{
			getChannel(it->first)->removeUser(&user);
			sendResponseRaw(prefix + "PART " + it->first + "\r\n", user);
			sendToChannel(prefix + "PART " + it->first + "\r\n", *getChannel(it->first), user);
			sendResponseRaw(getRPL_list(user), user);
			sendToChannel(getRPL_list(user), *getChannel(it->first), user);
		}
		else 
		{
			getChannel(it->first)->removeUser(&user);
			sendToChannel(prefix + "PART " + it->first + " :" + c.getArgs()[1] + "\r\n", *getChannel(it->first), user);
			sendResponseRaw(getRPL_list(user), user);
			sendToChannel(getRPL_list(user), *getChannel(it->first), user);
		}
	}
}

void	Server::list(User &user, Command& c)
{
	std::stringstream userCount;

	if (c.getArgs().size() > 1)
		sendResponseServer("461", "LIST :Not enough parameters", user);
	else if (c.getArgs().size() == 0)
		sendResponseRaw(getRPL_list(user), user);
	else
	{
		sendResponseRaw(hostname + "321 " + user.getNick() + " Channel :Users Name\r\n", user);
		map<string, string>		chan_keys;
		chan_keys = parseChannels(user, c.getArgs()[0]);
		for (map<string, string>::iterator it = chan_keys.begin(); it != chan_keys.end(); it++)
		{
			vector<Channel>::iterator chan_It = getChannel(it->first);
			if (chan_It == _channels.end())
				;
			else if (chan_It->_topic.empty())
			{
				userCount << chan_It->_userCount;
				string userStr = userCount.str();
				sendResponseRaw(hostname + "322 " + user.getNick() + " " + chan_It->_name + " " + userStr + " :No topic is set\r\n", user);
			}
			else
			{
				userCount << chan_It->_userCount;
				string userStr = userCount.str();
				sendResponseRaw(hostname + "322 " + user.getNick() + " " + chan_It->_name + " " + userStr + " :" + chan_It->_topic + "\r\n", user);
			}
		}
		sendResponseRaw(hostname + "323 " + user.getNick() + " End of /LIST\r\n", user);
	}
}

void	Server::kick(User &user, Command& c)
{
	if (c.getArgs().size() != 2)
	{
		sendResponseServer("461", "KICK :Not enough parameters", user);
		return ;
	}
	
	Channel& chan = *getChannel(c.getArgs()[0]);
	std::map<const User *, Privileges>::iterator it = chan._users.find(&user);
	if (notInChannelNames(c.getArgs()[0]))
		sendResponseServer("403", c.getArgs()[0] + " :No such channel", user);
	else if (it == chan._users.end())
		sendResponseServer("442", c.getArgs()[0] + " :You're not on that channel", user);
	else if (it->second != OPERATOR)
		sendResponseServer("482", c.getArgs()[0] + " :You're not channel operator", user);
	else
	{
		User& userToKick = getUser(c.getArgs()[1]);
		if (!isUserIn(userToKick, c.getArgs()[0]))
			sendResponseServer("441", userToKick.getNick() + " " + c.getArgs()[0] + " :They aren't on that channel", user);
		else
		{
			chan.removeUser(&userToKick);
			sendResponse("KICK " + user.getNick() + " has kicked " + userToKick.getNick() + " from channel " + c.getArgs()[0], user);
		}
	}
	
}

void	Server::oper(User &user, Command& c)
{
	if (c.getArgs().size() != 2)
		sendResponseServer("461", "OPER :Not enough parameters", user);
	else
	{
		if (_password != c.getArgs()[1])
			sendResponseServer("464", " :Password incorrect", user);
		else
		{
			for (vector<Channel>::iterator it = _channels.begin(); it != _channels.end(); it++)
			{
				map<const User *, Privileges>::iterator channelMap = it->_users.find(&user);
				channelMap->second = OPERATOR;
			}
			sendResponseServer("381", " :You are now an IRC operator", user);
		}
	}

}

void Server::who(User &user, Command& c)
{
	string resp;

	if (c.getArgs().size() > 1)
	{
		sendResponseServer("461", "WHO :Too many parameters", user);
		return ;
	}
	else if (c.getArgs().size() == 0)
	{
		for (map<int, User>::iterator it = _users.begin(); it != _users.end(); it++)
		{
			resp += hostname + "352 " + user.getNick() + " * " 
				+ it->second.getName() + " * " + hostname + " " + it->second.getNick() + " H :0 " + it->second.getFull() + "\r\n";
		}
		resp += hostname + "315 " + user.getNick() + "* :End of /WHO list\r\n";
	}
	sendResponseRaw(resp, user);
}

string	Server::getUserModes(void)
{
	return ("n None,\no Operator");
}

void	Server::applyUserModes(User &user, Command& c)
{
	string modestring = c.getArgs()[1];
	vector<string> v(0);
	
	if (modestring == "+o")
	{
		user.setOper(true);
		sendResponseRaw(":" + user.getNick() + "@" + user.getName() + "!" + hostname.substr(1) + " MODE " + c.getArgs()[0] + "+o", user);
	}
	else if (modestring == "-o")
	{
		user.setOper(false);
		sendResponseRaw(":" + user.getNick() + "@" + user.getName() + "!" + hostname.substr(1) + " MODE " + c.getArgs()[0] + "-o", user);
	}
	else
	{
		sendResponseServer("501", " :Unknown MODE flag", user);
	}
}

void	Server::userMode(string userNick, User &user, Command& c)
{
	if (!isUserRegistered(userNick))
		sendResponseServer("401", userNick + " :No such nick/channel", user);
	else if (userNick != user.getNick())
		sendResponseServer("502", " :Can't change mode for other users", user);
	else
	{
		if (c.getArgs().size() == 1)
		{
			string userModes = getUserModes();
			sendResponseServer("221", " :These are our modes " + userModes, user);
		}
		else 
		{
			applyUserModes(user, c);
		}
	}
}

// string	Server::getChannelModes(string name)
// {
// 	vector<Channel>::iterator it = getChannel(name);
// 	return (it->getActiveModes());

// }

void	Server::channelMode(string channelName, User &user, Command& c)
{
	if (notInChannelNames(channelName))
		sendResponseServer("403", channelName + " :No such nick/channel", user);
	else
	{
		if (c.getArgs().size() == 1)
		{
			// string userModes = getChannelModes(channelName);
			sendResponseServer("221", " :These are our modes 'userModes'", user);
		}
		else 
		{
			applyUserModes(user, c);
		}
	}
}

static bool	isChannelMode(string target)
{
	return (target[0] == '#');
}

void	Server::mode(User &user, Command& c)
{
	if (c.getArgs().size() < 1)
		sendResponseServer("461", "MODE :Not enough parameters", user);
	else
	{
		string target = c.getArgs()[0];
		if (isChannelMode(target))
			channelMode(target, user, c);
		else
			userMode(target, user, c);
	}
}
