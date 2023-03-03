#pragma once

class wrongNumberOfArgs : public std::exception { char const *what() const throw() {
			return "Wrong number of arguments. Usage:\n    ./ircserv <port> <password>";
		}};

class wrongPortNumber : public std::exception { char const *what() const throw() {
			return "Provided port number was incorrect";
		}};

class connectionFailed : public std::exception {char const *what() const throw() {
			return "Failed to accept a user trying to connect";
		}};

class socketCreationFailed : public std::exception {char const *what() const throw() {
			return "Failed to create a socket";
		}};

class bindingSocketFailed : public std::exception {char const *what() const throw() {
			return "Failed to bind the socket to IP/port";
		}};

class listenOnSocketFailed : public std::exception {char const *what() const throw() {
			return "Failed to listen on the socket";
		}};

class activePollFull : public std::exception {char const *what() const throw() {
			return "Number of connection is full, can't accept more connections";
		}};

class indexOutOfBounds : public std::exception {char const *what() const throw() {
			return "Given index is out of bounds";
		}};

class pollFailed : public std::exception {char const *what() const throw() {
			return "poll() failed";
		}};

class readingMsgFailed : public std::exception {char const *what() const throw() {
			return "Failed to read the input from the user";
		}};

class sendingMsgFailed : public std::exception {char const *what() const throw() {
			return "Failed to send a response to the user";
		}};

class incorrectPassword : public std::exception {char const *what() const throw() {
			return "Provided password is incorrect";
		}};