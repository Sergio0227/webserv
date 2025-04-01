#pragma once
# include "Socket.hpp"
class TcpServer : public Socket
{
    private:
        TcpServer();
		int _backlog;
		fd_set _cur_sockets, _rdy_sockets;
		std::map<int, ClientInfo> _client_info;
    public:
		TcpServer(int domain, int type, int protocol, int port, std::string &ip, int backlog);
		~TcpServer();
		void handleIncomingConnections();
		int acceptConnections();
		void sendHttpResponse(int fd, int status_code, std::string &msg, std::string &body);
		void closeConnection(int fd);
		void handleRequest(int client_fd);
		std::string readRequest(int client_fd);
		void parseRequestLine(int client_fd, std::string &request);
		void parseRequest(std::string &line);
		int checkRequest(std::string &msg);
        void executeRequest();
};