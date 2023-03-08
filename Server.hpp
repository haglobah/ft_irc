#pragma once

#include <iostream>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <map>
#include <string>

#include "errors.hpp"
#include "Channel.hpp"
#include "User.hpp"
#include "Socket.hpp"

class Server
{
	public:
		Server(int argc, char **argv);
		~Server();

		void	setupPoll(int);
		int		addToPoll(int);
		int		removeFromPoll(int);
		void	executeCommand(std::string);
		void	processCommand(char *, int);
		void	processInput(int);
		void	acceptUser();

		void	run(void);

	private:
		int 					_port;
		std::string				_password;
		pollfd					_userPoll[SOMAXCONN];
		nfds_t 					_activeUsers;
		unsigned int			_activePoll;
		bool					_stop;
		int 					_listeningSocket;
		std::map<int, User>		_users;
		std::map<std::string, Channel>	_channels;

		void loop(int fd);
};