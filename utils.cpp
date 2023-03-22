#include "Server.hpp"

using std::string;
using std::vector;
using std::map;

bool contains(string &haystack, string const &needles)
{
	for (u_int32_t i = 0; i < needles.length(); i++)
	{
		if (haystack.find(needles[i]) != string::npos)
			return (true);
	}
	return (false);
}

void printsvec(std::vector<std::string> strs)
{
	std::cout << "[ ";
	for (u_int32_t i = 0; i < strs.size(); i++)
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

void to_upper(std::string& str)
{
	for (std::string::iterator p = str.begin(); str.end() != p; ++p)
       *p = toupper(*p);
}