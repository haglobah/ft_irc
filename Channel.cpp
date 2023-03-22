#include "Channel.hpp"
#include <iostream>

Channel::Channel(std::string name)
: _name(name), _key(""), _topic(""),  _users(), _modes(), _userCount(0)
{}

Channel::Channel()
: _name(""), _key(""), _topic(""), _users(), _modes(), _userCount(0)
{}

Channel::~Channel(){}

void	Channel::addUser(User &u)
{
	_userCount++;
	_users.insert(std::pair<const User *, Privileges>(&u, VOICE_PRIO));
}

void	Channel::removeUser(User &u)
{
	(void)u;
}

void	updatePrivileges(User &u)
{
	(void)u;
}