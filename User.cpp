#include "Server.hpp"

User::User(int fd, std::string hostmask): _userFd(fd), _hostmask(hostmask){}

int	User::getUserFD() const {return (this->_userFd);}

std::string	User::getBuffer() const {return (this->_buf);}

void	User::setBuffer(std::string cmd)
{
	_buf = cmd;
}
