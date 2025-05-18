#pragma once

#include <vector>
#include <string>
#include "Config.hpp"
#include "HttpServer.hpp"

class Brain
{
	private:
		int	_nb_servers;
		std::vector<std::vector<std::string> >	_config_files;
		std::vector<Config*>					_server_conf;
		std::vector<HttpServer*>				_servers;
		std::map<int, HttpServer*>				_client_to_serv_map; //used to associate client with server
		std::vector<std::string>				_locations_keys;
		std::vector<int>						_server_sockets;
		fd_set									_cur_sockets, _rdy_sockets;

		void	splitServers(std::vector<std::string>);
		void	initServerConfigs();
		void	parseConfigFile(int server_index);
		void	parseLocation(size_t *i, int server_index, std::string location_name);
		void	handleConnections();
		void	setupServers();
		int		isServerFd(int i);

	public:
		Brain(std::vector<std::string>& config_files);
		~Brain();
		int	getNbServers();
};