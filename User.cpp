#include "Server.hpp"

User::User(int fd, std::string hostmask): _userFd(fd), _hostmask(hostmask), _name(""), _isDisconnected(0){}

int	User::getFD() const {return (this->_userFd);}

std::string	User::getFull() const {return (this->_fullName);}
void User::setFull(std::string fullName) {_fullName = fullName;}

std::string	User::getName() const {return (this->_name);}
void User::setName(std::string name) {_name = name; std::cout << "Name in setter [" << _name << "]" << std::endl;}

std::string	User::getNick() const {return (this->_nick);}
void User::setNick(std::string nick) {_nick = nick;}

std::string	User::getBuffer() const {return (this->_buf);}
void	User::setBuffer(std::string cmd) {_buf = cmd;}

bool	User::isDisconnected() const {return (_isDisconnected);}
void	User::setDisconnected() {_isDisconnected = true;}

bool	User::isRegistered() {return (_isRegistered);}
void	User::registrate() {_isRegistered = true;}