#include "Channel.hpp"
#include <iostream>

using std::string;

Channel::Channel(string name): _name(name),
							   _key(""), 
							   _topic(""),  
							   _users(), 
							   _modes(), 
							   _userCount(0){}

Channel::Channel(): _name(""),
				    _key(""), 
					_topic(""), 
					_users(), 
					_modes(), 
					_userCount(0){}

Channel::~Channel(){}

void	Channel::addUser(User const *uptr)
{
	// showUsers(); // REMINDER: remove before submitting
	_users.insert(std::pair<User const *, Privileges>(uptr, VOICE_PRIO));
	_userCount++;
	// showUsers(); // REMINDER: remove before submitting
}

void	Channel::removeUser(User const *uptr)
{
	// std::cout << "Show users before removing: " << uptr->getNick() << std::endl; // REMINDER: remove before submitting
	// showUsers(); // REMINDER: remove before submitting
	_users.erase(uptr);
	_userCount--;
	// std::cout << "Show users after removing: " << uptr->getNick() << std::endl; // REMINDER: remove before submitting
	// showUsers(); // REMINDER: remove before submitting
}

void	Channel::showUsers()
{
	for (std::map<const User *, Privileges>::iterator it = _users.begin(); it != _users.end(); it++)
	{
		std::cout << "User: '" << it->first->getNick() << "'" << "[ " << it->second << " ]" << std::endl;
	}
}

string	Channel::getActiveModes(void)
{
	return ("n None");
}

void	Channel::updatePrivileges(User const *uptr, Privileges priv)
{
	// std::cout << std::endl << std::endl;
	// showUsers();
	_users.at(uptr) = priv;
	// showUsers();
	// std::cout << std::endl << std::endl;
}