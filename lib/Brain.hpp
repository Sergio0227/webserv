#pragma once

#include <vector>
#include <string>
#include "Config.hpp"
#include "HttpServer.hpp"

class Brain
{
	private:
		int										_nb_servers;
		int										_max_fd;
		bool									_debug;
		std::vector<std::vector<std::string> >	_config_files;
		std::vector<Config*>					_server_conf;
		std::vector<std::string>				_locations_keys;
		std::vector<int>						_server_sockets;
		std::vector<HttpServer*>				_servers;
		std::map<int, HttpServer*>				_client_to_serv_map; //used to associate client with server
		std::map<int, HttpResponse>				_pending_responses;
		std::map<int, std::time_t>				_client_start_time;
		std::map<int, CGI>						_client_cgi;
		std::map<int, int>						_cgi_write_fd_to_client;
		std::map<int, int>						_cgi_read_fd_to_client;
		fd_set									_send_fd_set, _recv_fd_set;

		
		void	parseConfigFile(int server_index);
		void	parseLocation(size_t *i, int server_index, std::string location_name);
        void	handleHttpResponseSend(int client_fd);
        void	handleClientRequest(int client_fd);
        void	acceptAndSetupClient(int server_fd);
        void	handleCgiWrite(int cgi_stdin_fd);
        void	handleCgiRead(int fd);
        void	handleCGI(int fd, ClientInfo &info, HttpResponse &res);
		void	debugPrintState() const;
        void	closeClientConnection(int fd, int flag);
		void	addFdToSet(int fd, fd_set &fd_set);
		void	removeFdFromSet(int fd, fd_set &fd_set);
		int		isServerFd(int i);
		int		timeOutHandler();
		
	public:
		Brain(bool debug);
		~Brain();
		int		getNbServers();
		void	handleConnections();
		void	splitServers(std::vector<std::string>);
		void	initServerConfigs();
		void	setupServers();
		
};