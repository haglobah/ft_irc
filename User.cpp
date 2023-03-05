#include "User.hpp"

User::User(int fd, std::string hostmask): _userFd(fd), _hostmask(hostmask){}

int	User::getUserFD() const {return (this->_userFd);}