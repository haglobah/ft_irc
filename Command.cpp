#include "Server.hpp"

using std::string;
using std::cout;
using std::cerr;
using std::endl;

Command::Command(string cmd)
{
    string		s;    

	_name = cmd.substr(0, cmd.find(' '));
	cmd.erase(0, cmd.find(' ') + 1);
	
	std::istringstream cmd_istr(cmd);
    while (getline(cmd_istr, s, ' '))
	{
        _args.push_back(s);
		if (cmd_istr.peek() == ':')
		{
			std::istreambuf_iterator<char> eos;
			std::string s(std::istreambuf_iterator<char>(cmd_istr), eos);
			_args.push_back(s);
			break;
		}
	}
	cout << "Name: " << _name << endl;
	cout << "Vector: " << endl;
	for (int i = 0; i < _args.size(); i++)
	{
		cout << _args[i] << endl;
	}
}

Command::~Command(){}