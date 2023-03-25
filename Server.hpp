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

using std::string;
using std::map;
using std::vector;												;

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
		void	processCommands(string, int);
		void	receiveInput(int);
		void	acceptUser();
		void	disconnectUser(User &u);

		void	run(void);

		// SEND
		void	sendResponse(string code, string msg, User &user);
		void 	sendResponse(string message, User& user);
		void 	sendResponseRaw(string message, User& user);
		void	sendToChannel(string message, Channel c, User user);

		// COMMANDS
		// REMINDER: Shouldn't _Command c_ be _Command &c_ ? 
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

		void	applyUserModes(User &user, Command c);
		string	getUserModes(void);
		string	getChannelModes(string name);
		void	userMode(string target, User &user, Command c);
		void	channelMode(string target, User &user, Command c);
		void	mode(User &user, Command c);
		string	getRPL_list(User &u);
		string	getRPL_namelist(std::vector<Channel>::iterator chan_it, User &u);

		void	privmsg(User &user, Command c);
		void	notice(User &user, Command c);
		void	who(User &user, Command c);

		bool	alreadyInUse(string mode, string name);
		map<string, string>	parseChannels(User &u, string channelStr);
		map<string, string>	parseChannels(User &u, string channelStr, string keyStr);
		vector<string> parseUsers(string);

		string	getUsersIn(vector<Channel>::iterator chan_it);
		void	addUser(vector<Channel>::iterator it, User &user);
		void	joinChannel(map<string, string>::iterator chan_keys, User &user);
		bool	notInChannelNames(string channel);
		bool	inChannelNames(string channel);

		vector<Channel>::iterator	getChannel(string name);
		User&	getUser(string name);
		bool	isUserIn(User &u, string name);
		bool	isUserRegistered(string name);
		string	getChannelNames(User user);
		vector<string>	parseChannelPRIVMSG(User &u, string channelStr);
		
	private:
		int 			_port;
		string			_password;
		pollfd			_userPoll[SOMAXCONN];
		nfds_t 			_activeUsers;
		int				_activePoll;
		bool			_stop;
		int 			_listeningSocket;
		map<int, User>	_users;
		vector<Channel>	_channels;

		void loop();
};

bool isChannelValid(string channel);
bool contains(string &haystack, string const &needles);
void to_upper(string& str);
void printsvec(vector<string> strs);
vector<string>	split(string str, char delim);
