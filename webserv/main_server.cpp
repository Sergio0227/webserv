#include "Client.hpp"
#include "HttpServer.hpp"

int main(int argc, char *argv[])
{
	if (argc > 2)
	{
		std::cerr << "Error: Too many arguments \n" << "Usage -> ./webserv [configfile]" <<std::endl;
		return EXIT_FAILURE;
	}
	(void)argv;
	std::string ip = "127.0.0.1";
	int port = 8080;
	int backlog = 5;
	try
	{
		HttpServer(AF_INET, SOCK_STREAM, 0, port, ip, backlog, true);
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}
	return 0;
}
