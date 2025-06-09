#include "Brain.hpp"

int		g_flag = 1;

void	handleShutdown(int signal)
{
	(void)signal;
	std::cout << std::endl;
	logMessage(INFO, -1, "Server Shutdown Initiated.", NULL, 0);
	g_flag = 0;
}

int		main(int argc, char *argv[])
{
	if (argc > 2)
	{
		std::cerr << "Error: Too many arguments \n" << "Usage -> ./webserv [configfile]" <<std::endl;
		return (EXIT_FAILURE);
	}
	std::string	config_file_path = "configs/default.conf";
	if (argc == 1 && access(config_file_path.c_str(), R_OK))
	{
		std::cerr << "Error: Missing default configuration file at configs/default.conf" <<std::endl;
		return (EXIT_FAILURE);
	}
	signal(SIGINT, handleShutdown);
	if (argc == 2)
		config_file_path = argv[1];
	try
	{
		std::vector<std::string>	configFile;
		configFile = storeFormatedFile(config_file_path);
		Brain brain(ENABLE_DEBUG);
		try
		{
			brain.splitServers(configFile);
			brain.initServerConfigs();
		}
		catch (std::exception &e)
		{
			brain.setError(true);
			std::cerr << e.what() << std::endl;
			return (EXIT_FAILURE);
		}
		brain.setupServers();
		brain.handleConnections();
	}
	catch (std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}
