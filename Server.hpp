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
		void	executeCommand(User &, Command&);
		void	processCommands(string, int);
		void	receiveInput(int);
		void	acceptUser();
		void	disconnectUser(User &u);

		void	run(void);

		// SEND
		void	sendResponseServer(string, string, User&);
		void 	sendResponse(string, User&);
		void 	sendResponseRaw(string, User&);
		void	sendToChannel(string, Channel, User);

		// COMMANDS
		void	pass(User&, Command&);
		void	nick(User&, Command&);
		void	user(User&, Command&);
		void	ping(User&, Command&);
		void	cap(User&, Command&);
		void	oper(User&, Command&);
		void	quit(User&, Command&);
		
		void	join(User&, Command&);
		void	part(User&, Command&);
		void	topic(User&, Command&);
		void	list(User&, Command&);
		void	kick(User&, Command&);

		void	registrate(User&);
		void	removeChannel(vector<Channel>::iterator);
		void	applyUserModes(User&, Command&);
		string	getUserModes(void);
		string	getChannelModes(string);
		void	userMode(string, User&, Command&);
		void	channelMode(string, User&, Command&);
		void	mode(User&, Command&);
		string	getRPL_list(User&);
		string	getRPL_namelist(std::vector<Channel>::iterator, User&);

		void	privmsg(User&, Command&);
		void	notice(User&, Command&);
		void	who(User&, Command&);

		bool	alreadyInUse(string, string);
		map<string, string>	parseChannels(User&, string);
		map<string, string>	parseChannels(User&, string, string);
		vector<string> parseUsers(string);

		string	getUsersIn(vector<Channel>::iterator);
		void	addUser(vector<Channel>::iterator, User&);
		void	joinChannel(map<string, string>::iterator, User&);
		bool	notInChannelNames(string);
		bool	inChannelNames(string);

		vector<Channel>::iterator	getChannel(string);
		User&	getUser(string);
		bool	isUserIn(User&, string);
		bool	isUserRegistered(string);
		string	getChannelNames(User);
		vector<string>	parseChannelPRIVMSG(User&, string);
		
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


string			parsePassword(string);
bool			isChannelValid(string);
bool			contains(string&, string const &);
void			to_upper(string&);
void			printsvec(vector<string>);
vector<string>	split(string, char);
