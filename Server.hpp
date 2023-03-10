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

		// CONNECTION
		void	setupPoll(int);
		int		addConnection(int);
		int		removeConnection(int);
		void	executeCommand(User &, Command);
		void	processCommands(std::string, int);
		void	receiveInput(int);
		void	acceptUser();
		void	removeUser(User &u);

		void	run(void);

		void	sendResponse(std::string, std::string, User &);

		void	pass(User &user, Command c);
		void	nick(User &user, Command c);
		void	user(User &user, Command c);
		void	ping(User &user, Command c);
		void	cap(User &user, Command c);
		// void	oper(User &user, Command c);
		void	quit(User &user, Command c);
		
		void	join(User &user, Command c);
		void	part(User &user, Command c);
		void	topic(User &user, Command c);
		void	list(User &user, Command c);
		void	kick(User &user, Command c);
		void	invite(User &user, Command c);

		void	mode(User &user, Command c);

		void	privmsg(User &user, Command c);
		void	who(User &user, Command c);

		bool	alreadyInUse(std::string mode, std::string name, User user);
		std::map<std::string, std::string>	parseChannels(User &u, string channelStr);
		std::map<std::string, std::string>	parseChannels(User &u, string channelStr, string keyStr);
		bool	notInChannelNames(string channel);

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