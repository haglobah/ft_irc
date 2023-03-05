#pragma once

#include <string>

class User 
{
	private:
		std::string 	_buf;
		std::string 	_nick;
		std::string 	_name;
		std::string 	_fullName;
		std::string		_hostmask;
		int 			_userFd;
		bool 			_isRegistered;
		bool 			_allowConnection;
		bool			_disconnect;

	public:
		User(int fd, std::string hostmask);		

		int	getUserFD() const;
};