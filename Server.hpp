#pragma once

#include <iostream>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <map>
#include <vector>
#include <string>
#include <sstream>
#include "errors.hpp"
#include "Channel.hpp"
#include "Command.hpp"
#include "User.hpp"
#include "socket.hpp"

class Server
{
	public:
		Server(int argc, char **argv);
		~Server();

		void	setupPoll(int);
		int		addToPoll(int);
		int		removeUser(int);
		void	executeCommand(User *, Command);
		void	processCommands(char *, int);
		void	receiveInput(int);
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