#include "Server.hpp"

using std::string;
using std::vector;
using std::map;

bool contains(string s, string chars)
{
	for (int i = 0; i < chars.length(); i++)
	{
		if (s.find(chars[i]) != string::npos)
			return (true);
	}
	return (false);
}