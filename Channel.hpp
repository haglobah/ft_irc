#pragma once

#include <map>
#include <string>

#include "User.hpp"

enum Privileges
{
	OPERATOR = 1,
	VOICE_PRIO = 2,
	NO_PRIO = 3,
	INVITED = 4	
};

enum Modes
{
	NONE = 0,
	SECRET = 1,
	PRIV = 2,
	MODERATED = 3,
	INVITE_ONLY = 4,
	TOPIC_RESTRICTED = 5,
	KEY_PROTECTED = 6	
};

class Channel
{
	public:
		std::string							_name;
		std::string							_key;
		std::string							_topic;
		std::map<const User *, Privileges>	_users;
		// std::vector<int>					_modes;
		Modes					_modes;
		int									_userCount;

		Channel();
		Channel(std::string name);
		~Channel();

		void	addUser(User const *uptr);
		void	removeUser(User const *uptr);
		void	showUsers();

		std::string	getActiveModes();
		
		void	updatePrivileges(User &u);
};