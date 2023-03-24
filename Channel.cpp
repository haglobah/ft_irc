#include "Channel.hpp"
#include <iostream>

Channel::Channel(std::string name)
: _name(name), _key(""), _topic(""),  _users(), _modes(), _userCount(0)
{}

Channel::Channel()
: _name(""), _key(""), _topic(""), _users(), _modes(), _userCount(0)
{}

Channel::~Channel(){}

void	Channel::addUser(User const *uptr)
{
	showUsers();
	// std::cout << "Adding a user: " << std::endl;
	_users.insert(std::pair<User const *, Privileges>(uptr, VOICE_PRIO));
	_userCount++;
	showUsers();
}

void	Channel::removeUser(User const *uptr)
{
	showUsers();
	std::cout << "Removing a user: " << std::endl;
	_users.erase(uptr);
	_userCount--;
	showUsers();
}

void	Channel::showUsers()
{
	for (std::map<const User *, Privileges>::iterator it = _users.begin(); it != _users.end(); it++)
	{
		std::cout << "User: '" << it->first->getName() << "'" << std::endl;
	}
}

void	updatePrivileges(User &u)
{
	// REMINDER
	(void)u;
}