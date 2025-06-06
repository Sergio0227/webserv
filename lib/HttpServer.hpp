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
		std::map<int, ClientInfo>	_client_info;

		HttpServer();
		void		readRequest(ClientInfo &info, HttpResponse &res);
		void		parseRequestLine(ClientInfo &info, HttpResponse &res);
        void		parseRequestBody(ClientInfo &info, HttpResponse &res);
        void		sendErrorResponse(ClientInfo &info, HttpResponse &res);
        void		executeResponse(ClientInfo &info, HttpResponse &res);
        void		runCGI(ClientInfo &info, HttpResponse &res);
		std::string	extractBody(ClientInfo &info);
		std::string	readRequestPOST(ClientInfo &info);
		std::string	parseFileToString(const char *filename);
		int			uploadFile(ClientInfo &info, const char *path);
		int			checkPath(ClientInfo &info, std::string &path, std::string &method);
		bool		deleteEmail(ClientInfo &info, const char *filePath);
		bool		checkBodySize(ClientInfo &info);
		std::string	constructBodyForDirList(ClientInfo &info);
		size_t		parseRequestHeader(ClientInfo &info, HttpResponse &res);

	public:
		HttpServer(Config *conf, short port, std::string ip, int backlog, bool debug);
		~HttpServer();
		int				getSocket();
		int				acceptConnections();
		HttpResponse	handleRequest(int client_fd);
		ClientInfo		&getClientInfoElem(int fd);
		void			eraseClientFromMap(int fd);
		std::string		getFallBackBody(int err);
};
