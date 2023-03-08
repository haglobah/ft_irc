#include "User.hpp"

using namespace std;

User::User(int fd, string hostmask): _userFd(fd), _hostmask(hostmask){}

int	User::getUserFD() const {return (this->_userFd);}

string	User::getBuffer() const {return (this->_buf);}

void	User::setBuffer(char *str)
{
	if (_buf.empty())
		_buf = str;
	else
		_buf.append(str);
}