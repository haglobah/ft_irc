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
	// Command& c("PART test");
	// User u(12, "127.0.0.1");
	// Server ircserv(argc, argv);
	// ircserv.executeCommand(u ,c);
	// system("leaks ircserv");
	return (0);
}