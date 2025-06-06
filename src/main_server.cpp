#include "Brain.hpp"

int	g_flag = 1;

void handleShutdown(int signal)
{
	(void)signal;
	std::cout << std::endl;
	logMessage(INFO, -1, "Server Shutdown Initiated.", NULL, 0);
	g_flag = 0;
}

int main(int argc, char *argv[])
{
	if (argc > 2)
	{
		std::cerr << "Error: Too many arguments \n" << "Usage -> ./webserv [configfile]" <<std::endl;
		return (EXIT_FAILURE);
	}
	signal(SIGINT, handleShutdown);
	std::string config_file_path = "configs/default.conf";
	if (argc == 2)
		config_file_path = argv[1];
	try
	{
		std::vector<std::string> configFile;
		configFile = storeFormatedFile(config_file_path);
		Brain brain(configFile);
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}
