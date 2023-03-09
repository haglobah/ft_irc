#pragma once

class Command
{
	private:

	public:
		std::string					_name;
		std::vector<std::string>	_args;
		Command(std::string);
		~Command();
};