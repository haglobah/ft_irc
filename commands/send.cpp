#include "../Server.hpp"

using std::string;
using std::vector;
using std::map;

namespace {
	const string client = ":ft_irc.de ";
}

void	Server::sendResponse(string numeric_reply, string message, User& user)
{
	string response;

	response = client + numeric_reply + " " + user.getNick() + " :" + message + "\r\n";
	std::cout << "Response to send is|" << response << "| to: " << user.getFD() << std::endl;
	if (send(user.getFD(), response.c_str(), response.length(), 0) == -1)
		std::cout << "Couldn't send the response to FD :" << user.getFD() << std::endl;
	if (user.isDisconnected())
		removeUser(user);
}

void	Server::sendResponse(string message, User& user)
{
	string response;

	response = client + user.getNick() + " :" + message + "\r\n";
	std::cout << "Response to send is: " << response << std::endl;
	if (send(user.getFD(), response.c_str(), response.length(), 0) == -1)
		std::cout << "Couldn't send the response to FD:" << user.getFD() << std::endl;
	if (user.isDisconnected())
		removeUser(user);
}

void	Server::sendResponseJoin(string message, User& user)
{
	string response;

	response = message + "\r\n";
	std::cout << "Response to send is: " << response << std::endl;
	if (send(user.getFD(), response.c_str(), response.length(), 0) == -1)
		std::cout << "Couldn't send the response to FD:" << user.getFD() << std::endl;
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


void	Server::privmsg(User &user, Command c)
{
	if (c.getArgs().size() != 2)
	{
		sendResponse("461", "PRIVMSG :Not enough parameters", user);
		return ;
	}
	vector<string> targets = split(c.getArgs()[0], ',');
	for (vector<string>::iterator it = targets.begin(); it < targets.end(); it++)
	{
		string target = *it;
		if (isUserRegistered(target))
		{
			User&	receiver = getUser(target);
			string sender = ":" + user.getNick() + "!" + user.getName();
			sendResponseJoin(sender + " PRIVMSG " + receiver.getNick() + " :" + c.getArgs()[1], receiver);
		}
		else if (inChannelNames(target))
		{
			Channel &channel = *getChannel(target);
			if (!isUserIn(user, target))
				sendResponse("404", user.getNick() + " :Cannot send to channel", user);
			else
			{
				string sender = ":" + user.getNick() + "!" + user.getName();
				sendToChannel(sender + " PRIVMSG " + channel._name + " :" + c.getArgs()[1] + "\r\n", channel);
			}
		}
		else 
		{
			sendResponse("401", target + " :No such nick/channel", user);
		}
	}
}

void	Server::notice(User &user, Command c)
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
			sendResponse(user.getNick() + ": " + c.getArgs()[1], receiver);
		}
		else if (inChannelNames(target))
		{
			Channel &channel = *getChannel(target);
			if (!isUserIn(user, target))
				;
			else
			{
				string sender = user.getNick() + "@" + client;
				sendToChannel(sender + " #" + channel._name + " :" + c.getArgs()[1], channel);
			}
		}
		else 
		{}
	}
}
