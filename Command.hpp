#pragma once

enum cmds {

};

class Command
{
	private:
		std::string				_name;
		std::vector<std::string>	_args;

	public:
		Command(std::string);
		~Command();

		std::string getName() const;
		std::vector<std::string> getArgs() const;
};