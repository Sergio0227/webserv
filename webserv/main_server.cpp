#include "Client.hpp"
#include "HttpServer.hpp"

int	g_flag = 1;

void handleShutdown(int signal)
{
	(void)signal;
	std::cout << std::endl;
	logMessage(INFO, "Server Shutdown Initiated.", NULL);
	g_flag = 0;
}

int main(int argc, char *argv[])
{
	if (argc > 2)
	{
		std::cerr << "Error: Too many arguments \n" << "Usage -> ./webserv [configfile]" <<std::endl;
		return EXIT_FAILURE;
	}
	(void)argv;
	signal(SIGINT, handleShutdown);
	std::string ip = "127.0.0.1";
	int port = 8080;
	int backlog = 5;
	try
	{
		HttpServer(AF_INET, SOCK_STREAM, 0, port, ip, backlog, false);
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}
	return 0;
}
