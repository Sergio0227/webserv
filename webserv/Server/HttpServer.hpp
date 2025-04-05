#pragma once
#include "Socket.hpp"

class HttpServer : public Socket
{
  private:
	HttpServer();
	int _backlog;
	bool _debug;
	// serverConfig struct
	std::string root_path;
	//
	fd_set _cur_sockets, _rdy_sockets;
	std::map<int, ClientInfo> _client_info;

	void handleIncomingConnections();
	int acceptConnections();
	void closeConnection(int fd);
	std::string extractBody(ClientInfo &info);
	void handleRequest(int client_fd);
	void parseRequest(ClientInfo &info);
	std::string readRequest(ClientInfo &info);
	void parseRequestLine(ClientInfo &info);
	void parseRequestHeader(ClientInfo &info);
	void parseRequestBody(ClientInfo &info);
	int checkRequest(std::string &msg);
	void executeResponse();
	void executeRequest();
	void sendHttpResponse(int fd, int status_code, const char *msg,
		std::string &body);
	std::string parseFileToString(const char *filename);

	bool pathExists(std::string &path);

public:
	HttpServer(int domain, int type, int protocol, int port, std::string &ip,
		int backlog, bool debug);
	~HttpServer();
};