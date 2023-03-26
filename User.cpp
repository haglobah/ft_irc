#include "Server.hpp"

User::User(int fd, std::string hostmask): _nick(""),
										  _name(""), 
										  _fullName("") ,
										  _hostmask(hostmask),
										  _userFd(fd), 
										  _isRegistered(false) , 
										  _allowConnection(true) , 
										  _isDisconnected(0), 
										  _isOper(false){}

int	User::getFD() const {return (this->_userFd);}

std::string	User::getFull() const {return (this->_fullName);}
void User::setFull(std::string fullName) {_fullName = fullName;}

std::string	User::getName() const {return (this->_name);}
void User::setName(std::string name) {_name = name;}

std::string	User::getNick() const {return (this->_nick);}
void User::setNick(std::string nick) {_nick = nick;}

std::string	User::getBuffer() const {return (this->_buf);}
void	User::setBuffer(std::string cmd) {_buf = cmd;}

std::string	User::getHostmask() const {return (this->_hostmask);}

bool	User::isDisconnected() const {return (_isDisconnected);}
void	User::setDisconnected() {_isDisconnected = true;}

void	User::setOper(bool isOper) {_isOper = isOper;}

bool	User::getAllowConnection() const {return (_allowConnection);}
void	User::setAllowConnection() {_allowConnection = true;}

bool	User::isRegistered() {return (_isRegistered);}
void	User::registrate() {_isRegistered = true;}