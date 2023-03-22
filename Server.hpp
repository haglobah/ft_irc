#pragma once

#include <iostream>
#include <ctime>
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

		// SEND
		void	sendResponse(std::string, std::string, User &);
		void 	sendResponse(std::string message, User& user);
		void	sendToChannel(std::string message, Channel c);

		// COMMANDS
		void	pass(User &user, Command c);
		void	nick(User &user, Command c);
		void	user(User &user, Command c);
		void	ping(User &user, Command c);
		void	cap(User &user, Command c);
		void	oper(User &user, Command c);
		void	quit(User &user, Command c);
		
		void	join(User &user, Command c);
		void	part(User &user, Command c);
		void	topic(User &user, Command c);
		void	list(User &user, Command c);
		void	kick(User &user, Command c);

		// void	mode(User &user, Command c); // I don't think we need that.

		void	privmsg(User &user, Command c);
		void	notice(User &user, Command c);
		void	who(User &user, Command c);

		bool	alreadyInUse(std::string mode, std::string name);
		std::map<std::string, std::string>	parseChannels(User &u, std::string channelStr);
		std::map<std::string, std::string>	parseChannels(User &u, std::string channelStr, std::string keyStr);
		std::vector<std::string> parseUsers(std::string);

		void	addUser(std::vector<Channel>::iterator it, User &user);
		void	joinChannel(std::map<std::string, std::string>::iterator chan_keys, User &user);
		bool	notInChannelNames(std::string channel);
		bool	inChannelNames(std::string channel);

		std::vector<Channel>::iterator	getChannel(std::string name);
		User&	getUser(std::string name);
		bool	isUserIn(User &u, std::string name);
		bool	isUserRegistered(std::string name);
		std::vector<std::string>	parseChannelPRIVMSG(User &u, std::string channelStr);
		
	private:
		int 					_port;
		std::string				_password;
		pollfd					_userPoll[SOMAXCONN];
		nfds_t 					_activeUsers;
		int			_activePoll;
		bool					_stop;
		int 					_listeningSocket;
		std::map<int, User>		_users;
		std::vector<Channel>	_channels;

		void loop();
};

bool isChannelValid(std::string channel);
bool contains(std::string s, std::string chars);
void to_upper(std::string& str);
void printsvec(std::vector<std::string> strs);
std::vector<std::string>	split(std::string str, char delim);