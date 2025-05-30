#pragma once
#include "Socket.hpp"
#include "Config.hpp"

class HttpServer : public Socket
{
	private:
		int		_backlog;
		bool	_debug;
		fd_set _cur_sockets, _rdy_sockets;

		// serverConfig struct
		Config *_conf;
		std::string root_path;

		//monitor sockets
		std::map<int, ClientInfo> _client_info;

		HttpServer();
		void	handleIncomingConnections();
		void	parseRequestLine(ClientInfo &info);
		size_t	parseRequestHeader(ClientInfo &info);
		void	parseRequestBody(ClientInfo &info);
		void	handleErrorResponse(ClientInfo &info);
		void	executeResponse(ClientInfo &info);
		void sendHttpResponse(ClientInfo &info, std::string &body, const char *location);
		void	handleRequest(int client_fd);
		void	closeConnection(int fd);
		void serveImage(int client_fd, const std::string &filePath);
		
		int		acceptConnections();

		std::string	extractBody(ClientInfo &info);
		void uploadFile(ClientInfo &info, std::string data, const char *path);
		int deleteEmail(std::string &path, const char *filePath);
		void readRequest(ClientInfo &info);
		std::string readRequestPOST(ClientInfo &info);
		std::string parseFileToString(const char *filename);

		bool pathExists(std::string &path);
	
	public:
		HttpServer(Config *conf, int port, std::string ip, int backlog, bool debug);
		~HttpServer();
};


