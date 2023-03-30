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
		string channel = it->first;

		if (!isUserIn(user, channel))
			sendResponseServer("442", channel + " :You're not on that channel", user);
		else if (c.getArgs().size() == 1)
		{
			getChannel(channel)->removeUser(&user);
			sendResponseRaw(prefix + "PART " + channel + "\r\n", user);
			sendToChannel(prefix + "PART " + channel + "\r\n", *getChannel(channel), user);
			sendResponseRaw(getRPL_list(user), user);
			sendToChannel(getRPL_list(user), *getChannel(channel), user);
		}
		else 
		{
			string reason = c.getArgs()[1];
			getChannel(channel)->removeUser(&user);
			sendToChannel(prefix + "PART " + channel + " :" + reason + "\r\n", *getChannel(channel), user);
			sendResponseRaw(getRPL_list(user), user);
			sendToChannel(getRPL_list(user), *getChannel(channel), user);
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

	string channelName = c.getArgs()[0];
	Channel& chan = *getChannel(channelName);
	std::map<const User *, Privileges>::iterator it = chan._users.find(&user);
	if (notInChannelNames(channelName))
		sendResponseServer("403", channelName + " :No such channel", user);
	else if (it == chan._users.end())
		sendResponseServer("442", channelName + " :You're not on that channel", user);
	else if (it->second != OPERATOR)
		sendResponseServer("482", channelName + " :You're not channel operator", user);
	else
	{
		string userName = c.getArgs()[1];
		User& userToKick = getUser(userName);
		if (!isUserIn(userToKick, channelName))
			sendResponseServer("441", userToKick.getNick() + " " + channelName + " :They aren't on that channel", user);
		else
		{
			string prefix = ":" + user.getNick() + "!" + user.getName() + "@" + hostname;

			chan.removeUser(&userToKick);
			sendResponse("KICK " + user.getNick() + " has kicked " + userToKick.getNick() + " from channel " + channelName, user);
			sendResponseRaw(prefix + "KICK " + channelName + "\r\n", userToKick);
			sendToChannel(prefix + "KICK " + channelName + "\r\n", *getChannel(channelName), user);
			sendResponseRaw(getRPL_list(userToKick), userToKick);
			sendToChannel(getRPL_list(user), *getChannel(channelName), user);
			// REMINDER: Send something so that the user is not shown anymore.
		}
	}
	
}

void	Server::oper(User &user, Command& c)
{
	if (c.getArgs().size() != 2)
		sendResponseServer("461", "OPER :Not enough parameters", user);
	else
	{
		// if (user.getNick() != c.getArgs()[0]) -> the specification is unclear here. It seems like we don't have to check the name in our case.
		if (_operpass != c.getArgs()[1])
			sendResponseServer("464", " :Password incorrect", user);
		else
		{
			user.setOper(true);
			sendResponseServer("381", " :You are now a Server operator", user);
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
			resp += hostname + "352 " + user.getNick() + " "
				+ it->second.getName() + " * " + hostname + " " + it->second.getNick() + " :0 " + it->second.getFull() + "\r\n";
		}
		resp += hostname + "315 " + user.getNick() + " * :End of /WHO list\r\n";
	}
	else 
	{
		string mask = c.getArgs()[0];
		// std::cout << "The mask: '" << mask << "'" << std::endl << std::endl;
		if (mask[0] == '#')
		{
			string channelName = mask;
			vector<Channel>::iterator chan_it = getChannel(channelName);
			if (chan_it == _channels.end())
			{
				resp += hostname + "315 " + user.getNick() + " * :End of /WHO list\r\n";
			}
			else
			{
				map<User const *, Privileges> users = chan_it->_users;
				for (map<User const *, Privileges>::iterator it = users.begin(); it != users.end(); it++)
				{
					resp += hostname + "352 " + user.getNick() + " " + channelName + " "
					+ it->first->getName() + " * " + hostname + " " + it->first->getNick() + " :0 " + it->first->getFull() + "\r\n";
				}
				resp += hostname + "315 " + user.getNick() + " * :End of /WHO list\r\n";
			}
		}
		else
		{
			string otherName = mask;
			User &other = getUser(otherName);
			resp += hostname + "352 " + user.getNick() + " " + otherName + " "
			+ other.getName() + " * " + hostname + " " + other.getNick() + " :0 " + other.getFull() + "\r\n";
			resp += hostname + "315 " + user.getNick() + " * :End of /WHO list\r\n";
		}
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
	
	if (modestring == "+o")
	{
		user.setOper(true);
		sendResponseRaw(":" + user.getNick() + "!" + user.getName() + "@" + hostname.substr(1) + " MODE " + c.getArgs()[0] + " +o", user);
	}
	else if (modestring == "-o")
	{
		user.setOper(false);
		sendResponseRaw(":" + user.getNick() + "!" + user.getName() + "@" + hostname.substr(1) + " MODE " + c.getArgs()[0] + " -o", user);
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

void	Server::changeUserMode(Channel &chan, User &user, Command& c)
{
	string modestring = c.getArgs()[1];
	
	if (modestring == "+o")
	{
		chan.updatePrivileges(&user, OPERATOR);
		sendResponseRaw(":" + user.getNick() + "!" + user.getName() + "@" + hostname.substr(1) + " MODE " + c.getArgs()[0] + " +o", user);
	}
	else if (modestring == "-o")
	{
		chan.updatePrivileges(&user, VOICE_PRIO);
		sendResponseRaw(":" + user.getNick() + "!" + user.getName() + "@" + hostname.substr(1) + " MODE " + c.getArgs()[0] + " -o", user);
	}
	// else if (modestring == "+b")
	// {
	// 	chan.updatePrivileges(&user, VOICE_PRIO);
	// 	sendResponseRaw(":" + user.getNick() + "!" + user.getName() + "@" + hostname.substr(1) + " MODE " + c.getArgs()[0] + "-o", user);
	// }
	// else if (modestring == "-o")
	// {
	// 	chan.updatePrivileges(&user, VOICE_PRIO);
	// 	sendResponseRaw(":" + user.getNick() + "!" + user.getName() + "@" + hostname.substr(1) + " MODE " + c.getArgs()[0] + "-o", user);
	// }
	else
	{
		sendResponseServer("501", " :Unknown MODE flag", user);
	}
}

void	Server::channelMode(string channelName, User &user, Command& c)
{
	if (notInChannelNames(channelName))
		sendResponseServer("403", channelName + " :No such nick/channel", user);
	else
	{
		vector<Channel>::iterator chan_it = getChannel(channelName);
		if (c.getArgs().size() == 1)
		{
			string channelModes = chan_it->getActiveModes();
			sendResponseServer("221", " :These are our modes " + channelModes, user);
		}
		else 
		{
			changeUserMode(*chan_it, user, c);
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
