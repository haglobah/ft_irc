#include "Server.hpp"

void	Server::setupPoll(int fd)
{
	memset(_userPoll, 0, sizeof(_userPoll));
	_activePoll = 1;
	_userPoll[0].fd	= fd;
	_userPoll[0].events = POLLIN;
}


int	Server::addToPoll(int clientSocket)
{
	if (_activePoll >= SOMAXCONN)
		handle_error("ADD TO POLL FAILURE");
	_userPoll[_activePoll].fd = clientSocket;
	_userPoll[_activePoll].events = POLLIN; 
	_activePoll++;
	return (0);
}

int	Server::removeFromPoll(int clientFD)
{
	int	i = -1;

	for (int j = 0; j < _activePoll; j++)
	{
		if (_userPoll[j].fd == clientFD)
			i = j;
	}
	if (i == -1)
		handle_error("NO POLL FOUND");
	_userPoll[i].fd = _userPoll[_activePoll - 1].fd;
	_userPoll[i].events = POLLIN;
	_userPoll[_activePoll - 1].fd--;
	_activePoll--;
	return (0);
}