#pragma once

#include <vector>
#include <string>
#include "Config.hpp"
#include "HttpServer.hpp"

class Brain
{
	private:
		int	_nb_servers;
		int _max_fd;
		std::vector<std::vector<std::string> >	_config_files;
		std::vector<Config*>					_server_conf;
		std::vector<HttpServer*>				_servers;
		std::map<int, HttpServer*>				_client_to_serv_map; //used to associate client with server
		std::vector<std::string>				_locations_keys;
		std::vector<int>						_server_sockets;
		fd_set									_send_fd_set, _recv_fd_set;
		std::map<int, bool> _client_write_pending;

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
        void addFdToSet(int fd, fd_set &fd_set);
        void removeFdFromSet(int fd, fd_set &fd_set);
};