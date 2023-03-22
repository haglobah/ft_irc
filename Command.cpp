#include "Server.hpp"

using std::string;
using std::cout;
using std::cerr;
using std::endl;

Command::~Command(){}

Command::Command(string cmd)
{
    string		s;    

	_name = cmd.substr(0, cmd.find(' '));
	to_upper(_name);
	cmd.erase(0, cmd.find(' ') + 1);

	std::istringstream cmd_istr(cmd);
    while (getline(cmd_istr, s, ' '))
	{
        _args.push_back(s);
		if (cmd_istr.peek() == ':')
		{
			cmd_istr.get();
			std::istreambuf_iterator<char> eos;
			std::string s(std::istreambuf_iterator<char>(cmd_istr), eos);
			_args.push_back(s);
			break;
		}
	}
}

std::string Command::getName() const
{
	return (_name);
}

std::vector<std::string> Command::getArgs() const
{
	return (_args);
}