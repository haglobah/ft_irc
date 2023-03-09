#pragma once

#include "../User.hpp"
#include "../Command.hpp"

#define CLIENT "www.ft_irc.de"

void	pass(User &user, Command c);
void	nick(User &user, Command c);
void	user(User &user, Command c);
void	ping(User &user, Command c);
void	cap(User &user, Command c);
void	join(User &user, Command c);
void	privmsg(User &user, Command c);
void	topic(User &user, Command c);
void	who(User &user, Command c);
void	part(User &user, Command c);
void	mode(User &user, Command c);
void	quit(User &user, Command c);
void	list(User &user, Command c);
void	invite(User &user, Command c);
void	kick(User &user, Command c);