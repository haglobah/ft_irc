#pragma once

#include <iostream>
#include <sys/socket.h>
#include <sys/poll.h>
#include <map>

#include "Channel.hpp"
#include "User.hpp"

class Server
{
	private:
		int 					_port;
		std::string				_password;
		pollfd					_userPoll[SOMAXCONN];
		nfds_t 					_activePoll;
		bool					_stop;
		int 					_listeningSocket;
		std::map<int, User>		_users;
		std::map<std::string, Channel>	_channels;

	public:
		Server(int argc, char **argv);
		~Server();
};