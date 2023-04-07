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
	(_channels.end() - 1)->updatePrivileges(&user, OPERATOR);	
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
		std::stringstream userCountss;
		userCountss << chan_it->_userCount;
		string userCount = userCountss.str();
		string topic = (chan_it->_topic.empty()) ? "No topic is set" : chan_it->_topic;
		sendResponseRaw(hostname + "322 " + user.getNick() + " " + chan_it->_name + " " + userCount + " :" + topic + "\r\n", user);
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
			string msg = ":ft_irc.de 331 " + user.getNick() + " " + chan_it->_name + " :No topic is set\r\n";
			sendResponseRaw(msg, user);
			sendToChannel(msg, *chan_it , user);
			chan_it->_topic.clear();
		}
		else
		{
			chan_it->_topic = c.getArgs()[1];
			string msg = ":" + user.getNick() + "!" + user.getName() + "@" + hostname + " TOPIC " + chan_it->_name + " :" + chan_it->_topic + "\r\n";
			sendResponseRaw(msg, user);
			sendToChannel(msg, *chan_it, user);
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
		else 
		{
			string reason = (c.getArgs().size() == 1) ? "" : " :" + c.getArgs()[1];
			getChannel(channel)->removeUser(&user);
			sendResponseRaw(prefix + "PART " + channel + "\r\n", user);
			sendToChannel(prefix + "PART " + channel + reason + "\r\n", *getChannel(channel), user);
			sendResponseRaw(getRPL_list(user, true), user);
			sendToChannel(getRPL_list(user, true), *getChannel(channel), user);
		}
	}
}

void	Server::list(User &user, Command& c)
{
	std::stringstream userCountss;

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
			vector<Channel>::iterator chan_it = getChannel(it->first);
			if (chan_it == _channels.end())
				;
			else
			{
				userCountss << chan_it->_userCount;
				string userCount = userCountss.str();
				string topic = (chan_it->_topic.empty()) ? "No topic is set" : chan_it->_topic;
				sendResponseRaw(hostname + "322 " + user.getNick() + " " + chan_it->_name + " " + userCount + " :" + topic + "\r\n", user);
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
			string msg = "KICK " + channelName + " " + userToKick.getNick() + "\r\n";

			chan.removeUser(&userToKick);
			sendResponseRaw(prefix + msg + getRPL_list(userToKick, true), userToKick);
			for (std::map<User const *, Privileges>::iterator user_it = chan._users.begin(); user_it != chan._users.end(); user_it++)
			{
				User curr_user = *user_it->first; 
				sendResponseRaw(prefix + msg + getRPL_list(curr_user, true), curr_user);
			}
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
		sendResponseRaw(":" + user.getNick() + "!" + user.getName() + "@" + hostname.substr(1) + "MODE " + c.getArgs()[0] + " +o\r\n", user);
	}
	else if (modestring == "-o")
	{
		user.setOper(false);
		sendResponseRaw(":" + user.getNick() + "!" + user.getName() + "@" + hostname.substr(1) + "MODE " + c.getArgs()[0] + " -o\r\n", user);
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
	string modifier = modestring.substr(0, 1); 
	
	if (chan._users[&user] != OPERATOR && (contains(modifier, "-+")))
	{
		// "<client> <channel> :You're not channel operator"
		sendResponseServer("482", chan._name + " :You're not channel operator", user);
	}
	if (modestring == "+o")
	{
		if (c.getArgs().size() != 3)
			sendResponseServer("461", "MODE :Not enough parameters", user);
		else
		{
			string userNick = c.getArgs()[2];
			User &userToChange = getUser(userNick);
			chan.updatePrivileges(&userToChange, OPERATOR);
			sendToChannel(":" + user.getNick() + "!" + user.getName() + "@" + hostname.substr(1) + "MODE " + chan._name + " +o " + userNick + "\r\n", chan, user);
		}
	}
	else if (modestring == "-o")
	{
		if (c.getArgs().size() != 3)
			sendResponseServer("461", "MODE :Not enough parameters", user);
		else
		{
			string userNick = c.getArgs()[2];
			User &userToChange = getUser(userNick);
			chan.updatePrivileges(&userToChange, VOICE_PRIO);
			sendToChannel(":" + user.getNick() + "!" + user.getName() + "@" + hostname.substr(1) + "MODE " + chan._name + " -o " + userNick + "\r\n", chan, user);
		}
	}
	else if (modestring == "b")
	{
		sendResponseServer("368", chan._name + " :End of Channel Ban List", user);
	}
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
