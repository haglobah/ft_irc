#pragma once

#include <string>

class User 
{
	private:
		std::string 	_buf;
		std::string 	_nick;
		std::string 	_name;
		std::string 	_fullName;
		std::string		_hostmask;
		int 			_userFd;
		bool 			_isRegistered;
		bool 			_allowConnection; // REMINDER: check if used
		bool			_isDisconnected;
		bool			_isOper;

	public:
		User(int fd, std::string hostmask);		

		void		setBuffer(std::string);

		int			getFD() const;
		std::string	getName() const;
		void		setName(std::string);

		std::string	getFull() const;
		void		setFull(std::string);

		std::string	getNick() const;
		void		setNick(std::string);
		std::string	getBuffer() const;

		std::string	getHostmask() const;

		bool		isDisconnected() const;
		void		setDisconnected();

		void		setOper(bool isOper);

		bool		getAllowConnection() const;
		void		setAllowConnection();

		bool		isRegistered();
		void		registrate();
};