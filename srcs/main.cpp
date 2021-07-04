#include "webserv.hpp"

int main(int argc, char **argv)
{
	uint32_t max_connection = 200;
	if (argc == 3)
		max_connection = ft_atoi(argv[2]);
	if (argc < 2 || argc > 3)
		return 1;
	try
	{
		Webserver server(argv[1], max_connection);
		server.startServer();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return 1;
	}
	return 0;
}

/* 
1)logger
2)cgi
3)parser request responce
4)chuncked
5)clients
*/
