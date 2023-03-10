#include "Channel.hpp"

Channel::Channel(std::string name)
: _name(name), _key(""), _topic(""), _modes(), _userCount(0)
{}

Channel::~Channel(){}

void	Channel::addUser(User u)
{
	_userCount++;
	_users.insert(std::pair<User *, Privileges>(&u, VOICE_PRIO));
}

void	Channel::removeUser(User &u)
{

}

void	updatePrivileges(User &u)
{

}