#pragma once
#include "Socket.hpp"

class HttpServer : public Socket
{
  private:
	HttpServer();
	int _backlog;
	//in server struct
	std::string root_path;
	fd_set _cur_sockets, _rdy_sockets;
	std::map<int, ClientInfo> _client_info;

	void handleIncomingConnections();
	int acceptConnections();
	void closeConnection(int fd);
	void handleRequest(int client_fd);
	std::string readRequest(int client_fd);
	void parseRequestLine(int client_fd, std::string &request);
	void parseRequest(int client_fd);
	void parseRequestHeader(std::string &request);
	void parseRequestBody(std::string &request);
	int checkRequest(std::string &msg);
	void executeRequest();
	void sendHttpResponse(int fd, int status_code, const char *msg,
		std::string &body);

	std::string parseFileToString(const char *filename);

	bool pathExists(std::string &path);

public:
	HttpServer(int domain, int type, int protocol, int port, std::string &ip,
		int backlog);
	~HttpServer();
};