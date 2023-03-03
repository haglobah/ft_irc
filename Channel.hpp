#pragma once

#include <map>
#include <string>

class Channel
{
	private:
		std::string							_name;
		std::string							_channel_key;
		std::string							_topic;
		std::map<const User *, privilege>	_users;
		int									_modes;
		int									_userCount;
};