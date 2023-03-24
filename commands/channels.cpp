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
	string start = ":" + user.getNick() + "!" + user.getName() + "@" + hostname.substr(1) + " JOIN :" + channelName + "\r\n"; 
	string response;
	if (chan_it->_topic.empty())
		response = hostname + "332 " + user.getNick() + " " + channelName + " :No topic is set\r\n";
	else
		response = hostname + "332 " + user.getNick() + " " + channelName + " :" + chan_it->_topic + "\r\n";
	string response1 = hostname + "353 " + user.getNick() + " = " + channelName + " :@" + getUsersIn(chan_it) + "\r\n";
	string response2 = hostname + "366 " + user.getNick() + " " + channelName + " :End of /NAMES list\r\n";
	string response3 = hostname + "321 " + user.getNick() + " Channel :Users Name \r\n";
	string response4 = getChannelNames(user);
	string response5 = hostname + "323 " + user.getNick() + " :End of /LIST";
	sendResponseRaw(start + response + response1 + response2 + response3 + response4 + response5, user);
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

void	Server::topic(User &user, Command c)
{
	if (c.getArgs().size() > 2 || c.getArgs().size() < 1)
		sendResponse("461", "TOPIC :Not enough parameters", user);
	else if (!isUserIn(user, c.getArgs()[0]))
		sendResponse("442", c.getArgs()[0] + " :You're not on that channel", user);
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
			sendResponseRaw(":ft_irc.de 331 " + user.getNick() + " " + chan_it->_name + " :" + chan_it->_topic + "\r\n", user);
			// REMINDER: add message to channel
			sendResponse("333", c.getArgs()[0] + " " + user.getNick() + " " , user);
		}
	}
	else if (c.getArgs().size() == 2)
	{
		vector<Channel>::iterator chan_it = getChannel(c.getArgs()[0]);
		std::map<const User *, Privileges>::iterator it = chan_it->_users.find(&user);
		if (/*chan_it->_modes == TOPIC_RESTRICTED && */it->second != OPERATOR)
		{
			sendResponse("482", c.getArgs()[0] + " :You're not channel operator", user);
			return ;
		}
		else if (c.getArgs()[1].empty())
		{
			chan_it->_topic.clear();
			sendResponseRaw(":ft_irc.de 331 " + user.getNick() + " " + chan_it->_name + " :No topic is set\r\n", user);
		}
		else
		{
			chan_it->_topic = c.getArgs()[1];
			sendResponseRaw(":" + user.getNick() + "!" + user.getName() + "@ TOPIC " + chan_it->_name + " :" + chan_it->_topic + "\r\n", user);
			// REMINDER: add message to channel
			sendToChannel("333 " + c.getArgs()[0] + " " + user.getNick() + " ", *chan_it, user);
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
		string prefix = ":" + user.getNick() + "!" + user.getName() + "@" + hostname;
		if (!c.getArgs()[1].empty()) // REMINDER: can you check it like that?
			sendToChannel(prefix + " PART :" + user.getNick() + " leave channel " + it->first + " because " + c.getArgs()[1] + "\r\n", *getChannel(it->first), user);
		else
			sendToChannel(prefix + "PART :" + user.getNick() + " leave channel " + it->first + "\r\n", *getChannel(it->first), user);
		getChannel(it->first)->removeUser(&user);
	}
}

void	Server::list(User &user, Command c)
{
	if (c.getArgs().size() > 1)
		sendResponse("461", "LIST :Not enough parameters", user);
	else if (c.getArgs().size() == 0)
	{
		for (vector<Channel>::iterator it = _channels.begin(); it != _channels.end(); it++)
		{
			std::stringstream userCount;
			userCount << it->_userCount;
			string userStr = userCount.str();
			if (it->_topic.empty())
				sendResponseRaw(":ft_irc.de 322 " + user.getNick() + " " + it->_name + " " + userStr + " :No topic is set\r\n", user);				
			else
				sendResponseRaw(":ft_irc.de 322 " + user.getNick() + " " + it->_name + " " + userStr + it->_topic + "\r\n", user);
		}
	}
	else
	{
		map<string, string>		chan_keys;
		chan_keys = parseChannels(user, c.getArgs()[0]);
		for (map<string, string>::iterator it = chan_keys.begin(); it != chan_keys.end(); it++)
		{
			vector<Channel>::iterator chan_It = getChannel(it->first);
			if (chan_It == _channels.end())
				;
			else if (chan_It->_topic.empty())
			{
				std::stringstream userCount;
				userCount << chan_It->_userCount;
				string userStr = userCount.str();
				sendResponseRaw(":ft_irc.de 322 " + user.getNick() + " " + chan_It->_name + " " + userStr + " :No topic is set\r\n", user);
			}
			else
			{
				std::stringstream userCount;
				userCount << chan_It->_userCount;
				string userStr = userCount.str();
				sendResponseRaw(":ft_irc.de 322 " + user.getNick() + " " + chan_It->_name + " " + userStr + chan_It->_topic + "\r\n", user);
			}
		}
		sendResponse("323", " :End of /LIST", user);
	}
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
			sendResponse("KICK :" + user.getNick() + " has kicked " + userToKick.getNick() + " from channel " + c.getArgs()[0], user);
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
			sendResponse("464", " :Password incorrect", user);
		else
		{
			for (vector<Channel>::iterator it = _channels.begin(); it != _channels.end(); it++)
			{
				map<const User *, Privileges>::iterator channelMap = it->_users.find(&user);
				channelMap->second = OPERATOR;
			}
			sendResponse("381", " :You are now an IRC operator", user);
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

string	Server::getUserModes(void)
{
	return ("n None,\no Operator");
}

void	Server::applyUserModes(User &user, Command c)
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
		sendResponse("501", " :Unknown MODE flag", user);
	}
}

void	Server::userMode(string userNick, User &user, Command c)
{
	if (!isUserRegistered(userNick))
		sendResponse("401", userNick + " :No such nick/channel", user);
	else if (userNick != user.getNick())
		sendResponse("502", " :Can't change mode for other users", user);
	else
	{
		if (c.getArgs().size() == 1)
		{
			string userModes = getUserModes();
			sendResponse("221", " :These are our modes " + userModes, user);
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

void	Server::channelMode(string channelName, User &user, Command c)
{
	if (notInChannelNames(channelName))
		sendResponse("403", channelName + " :No such nick/channel", user);
	else
	{
		if (c.getArgs().size() == 1)
		{
			// string userModes = getChannelModes(channelName);
			sendResponse("221", " :These are our modes 'userModes'", user);
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

void	Server::mode(User &user, Command c)
{
	if (c.getArgs().size() < 1)
		sendResponse("461", "MODE :Not enough parameters", user);
	else
	{
		string target = c.getArgs()[0];
		if (isChannelMode(target))
			channelMode(target, user, c);
		else
			userMode(target, user, c);
	}
}
