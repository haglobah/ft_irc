#include "Server.hpp"

using std::string;
using std::vector;
using std::map;

bool contains(string s, string chars)
{
	for (unsigned int i = 0; i < chars.length(); i++)
	{
		if (s.find(chars[i]) != string::npos)
			return (true);
	}
	return (false);
}

void printsvec(std::vector<std::string> strs)
{
	std::cout << "[ ";
	for (unsigned int i = 0; i < strs.size(); i++)
	{
		std::cout << strs[i] << " ";
	}
	std::cout << "]" << std::endl;
}

vector<string>	split(string str, char delim)
{
	vector<string>	strVec;
	string			u;

	std::istringstream sstream(str);
    while (getline(sstream, u, delim))
		strVec.push_back(u);
	return (strVec);
}