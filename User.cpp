#include "Server.hpp"

using namespace std;

User::User(int fd, string hostmask): _userFd(fd), _hostmask(hostmask){}

int	User::getUserFD() const {return (this->_userFd);}

string	User::getBuffer() const {return (this->_buf);}

string	User::parseCommand()
{
	string cmd;
	size_t newLine;
	
	while (_buf.find("\r\n") != std::string::npos)
		_buf.replace(_buf.find("\r\n"), 2, "\n");
	// might be a bug
	newLine = _buf.find('\n');
	cmd = _buf.substr(0, newLine);
	_buf.erase(0, newLine + 1);
	return (cmd);
}

void	User::writeToBuffer(char *str)
{
	if (_buf.empty())
		_buf = str;
	else
		_buf.append(str);
}