#pragma once
#include "Socket.hpp"
#include "Config.hpp"

class HttpServer : public Socket
{
	private:
		int		_backlog;
		bool	_debug;
		// serverConfig struct
		Config *_conf;

		//monitor sockets
		std::map<int, ClientInfo> _client_info;

		HttpServer();
		void	handleIncomingConnections();
		void	parseRequestLine(ClientInfo &info);
		size_t	parseRequestHeader(ClientInfo &info);
		void	parseRequestBody(ClientInfo &info);
		void	handleErrorResponse(ClientInfo &info);
		void	executeResponse(ClientInfo &info);
		
		//void serveImage(int client_fd, const std::string &filePath);
		
		

		std::string	extractBody(ClientInfo &info);
		void uploadFile(ClientInfo &info, std::string data, const char *path);
		bool deleteEmail(std::string &path, const char *filePath);

		

		void readRequest(ClientInfo &info);
		std::string readRequestPOST(ClientInfo &info);
		void sendHttpResponse(ClientInfo &info, const char *location, const char *content_type, std::string &body);
		std::string parseFileToString(const char *filename);

		bool checkPath(ClientInfo &info, std::string &path);
		bool checkBodySize(ClientInfo &info);

	
	public:
		HttpServer(Config *conf, short port, std::string ip, int backlog, bool debug);
		~HttpServer();
		int getSocket();
        
        int acceptConnections();
        bool	handleRequest(int client_fd);
		ClientInfo &getClientInfoElem(int fd);
};


