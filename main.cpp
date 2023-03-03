#include "Server.hpp"

using std::cout;
using std::cerr;
using std::endl;

int main(int argc, char **argv)
{
	try
	{
		Server ircserv(argc, argv);
		ircserv.run();
	}
	catch (std::exception &e)
	{
		cerr << e.what() << endl;
	}
	return (0);
}