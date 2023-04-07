#include "../Server.hpp"

using std::string;
using std::vector;
using std::map;

namespace {
	const string hostname = "ft_irc.de";
}

string parsePassword(string password)
{
	string ret;

	if (password[0] == ':')
		password = password.substr(1);
	return (password);
}

bool Server::alreadyInUse(string mode, string name)
{
	std::map<int, User>::iterator it = _users.begin();
	
	for (; it != _users.end(); it ++)
	{
		if (mode == "user")
		{
			if (it->second.getName() == name)
				return (true);
		}
		else if (mode == "nick")
		{
			if (it->second.getNick() == name)
				return (true);
		}
	}
	return (false);
}

void	Server::registrate(User &user)
{
	if (!user.getNick().empty() 
		&& !user.getName().empty()
		&& !user.getFull().empty() 
		&& user.getAllowConnection()
		&& !user.isRegistered())
	{
		sendResponseServer("001", " :Welcome to the " + hostname + " Network " + user.getNick() + "!" , user);
		sendResponseRaw(getRPL_list(user), user);
		user.registrate();
	}
}

