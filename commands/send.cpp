#include "../Server.hpp"

using std::string;
using std::vector;
using std::map;
using std::endl;

namespace {
	const string hostname = ":ft_irc.de";
}

void	Server::sendResponseServer(string numeric_reply, string message, User& user)
{
	string response;

	response = hostname + " " + numeric_reply + " " + user.getNick() + " " + message + "\r\n";
	std::cout << "Response to send is \n|" << response << "| to: " << user.getNick() << endl << endl;
	if (send(user.getFD(), response.c_str(), response.length(), 0) == -1)
		std::cout << "Couldn't send the response to FD :" << user.getFD() << std::endl;
	if (user.isDisconnected())
		disconnectUser(user);
}

void	Server::sendResponse(string message, User& user)
{
	string response;

	response = ":" + user.getNick() + "!" + user.getName() + "@" + hostname.substr(1) + " " + message + "\r\n";
	std::cout << "Response to send is \n|" << response << "| to: " << user.getNick() << std::endl << endl;
	if (send(user.getFD(), response.c_str(), response.length(), 0) == -1)
		std::cout << "Couldn't send the response to FD:" << user.getFD() << std::endl;
	if (user.isDisconnected())
		disconnectUser(user);
}

void	Server::sendResponseRaw(string message, User& user)
{
	std::cout << "Response to send is \n|" << message << "| to: " << user.getNick() << std::endl << endl;
	if (send(user.getFD(), message.c_str(), message.length(), 0) == -1)
		std::cout << "Couldn't send the response to FD:" << user.getFD() << std::endl;
	if (user.isDisconnected())
		disconnectUser(user);
}

void	Server::sendToChannel(string message, Channel c, User user)
{ 
	std::cout << "Message to the channel is \n|" << message << "| to: " << c._name << std::endl << endl;
	for(map<const User *, Privileges>::iterator it = c._users.begin();
		it != c._users.end();
		it++)
	{
		if (it->first->getNick() == user.getNick())
			;
		else if (send(it->first->getFD(), message.c_str(), message.length(), 0) == -1)
			std::cout << "Couldn't send the message to FD in the channel:" << it->first->getFD() << std::endl;
	}
}


void	Server::privmsg(User &user, Command& c)
{
	if (c.getArgs().size() != 2)
	{
		sendResponseServer("461", "PRIVMSG :Not enough parameters", user);
		return ;
	}
	vector<string> targets = split(c.getArgs()[0], ',');
	for (vector<string>::iterator it = targets.begin(); it < targets.end(); it++)
	{
		string target = *it;
		if (isUserRegistered(target))
		{
			User&	receiver = getUser(target);
			string sender = ":" + user.getNick() + "!" + user.getName() + "@" + hostname;
			sendResponseRaw(sender + " PRIVMSG " + receiver.getNick() + " :" + c.getArgs()[1] + "\r\n", receiver);
		}
		else if (inChannelNames(target))
		{
			Channel &channel = *getChannel(target);
			if (!isUserIn(user, target))
				sendResponseServer("404", user.getNick() + " :Cannot send to channel", user);
			else
			{
				string sender = ":" + user.getNick() + "!" + user.getName() + "@" + hostname;
				sendToChannel(sender + " PRIVMSG " + channel._name + " :" + c.getArgs()[1] + "\r\n", channel, user);
			}
		}
		else
			sendResponseServer("401", target + " :No such nick/channel", user);
	}
}

void	Server::notice(User &user, Command& c)
{
	if (c.getArgs().size() != 2)
	{
		return ;
	}
	vector<string> targets = split(c.getArgs()[0], ',');
	for (vector<string>::iterator it = targets.begin(); it < targets.end(); it++)
	{
		string target = *it;
		if (isUserRegistered(target))
		{
			User&	receiver = getUser(target);
			string sender = ":" + user.getNick() + "!" + user.getName() + "@" + hostname;
			sendResponseRaw(sender + " NOTICE " + receiver.getNick() + " :" + c.getArgs()[1] + "\r\n", receiver);
		}
		else if (inChannelNames(target))
		{
			Channel &channel = *getChannel(target);
			if (!isUserIn(user, target))
				;
			else
			{
				string sender = ":" + user.getNick() + "!" + user.getName() + "@" + hostname;
				sendToChannel(sender + " NOTICE " + channel._name + " :" + c.getArgs()[1] + "\r\n", channel, user);
			}
		}
		else 
		{}
	}

}