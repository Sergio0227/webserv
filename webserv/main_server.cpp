#include <cstdlib>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include "Socket.hpp"
#include "Client.hpp"
#include "TcpServer.hpp"


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
	// int domain, int type, int protocol, int port, u_long ip, int backlog
	try
	{
		TcpServer(AF_INET, SOCK_STREAM, 0, port, ip, backlog);
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}
	return 0;
}
