#pragma once
#include "Socket.hpp"
#include "Config.hpp"
#include "HttpResponse.hpp"

class HttpServer : public Socket
{
	private:
		int							_backlog;
		bool						_debug;
		Config						*_conf;
		//monitor sockets
		std::map<int, ClientInfo>	_client_info;

		HttpServer();
		void	readRequest(ClientInfo &info, HttpResponse &res);
		void	parseRequestLine(ClientInfo &info, HttpResponse &res);
		void	sendErrorResponse(ClientInfo &info, HttpResponse &res);
		void	parseRequestBody(ClientInfo &info);
		void	executeResponse(ClientInfo &info, HttpResponse &res);
		size_t	parseRequestHeader(ClientInfo &info);
		
		//void serveImage(int client_fd, const std::string &filePath);

		std::string	extractBody(ClientInfo &info);
		std::string readRequestPOST(ClientInfo &info);
		std::string parseFileToString(const char *filename);

		int uploadFile(ClientInfo &info, const char *path);
		int checkPath(ClientInfo &info, std::string &path, std::string &method);
		bool deleteEmail(ClientInfo &info, const char *filePath);
		bool checkBodySize(ClientInfo &info);

        std::string constructBodyForDirList(ClientInfo &info);

        void runCGI(ClientInfo &info, HttpResponse &res);

    public:
		HttpServer(Config *conf, short port, std::string ip, int backlog, bool debug);
		~HttpServer();

		int			getSocket();
        int			acceptConnections();
        HttpResponse	handleRequest(int client_fd);
		ClientInfo &getClientInfoElem(int fd);
};


