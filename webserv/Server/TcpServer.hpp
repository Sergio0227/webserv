#pragma once
# include "Socket.hpp"
class TcpServer : public Socket
{
    private:
        TcpServer();
		int _backlog;
		int _client_socket;
		struct sockaddr_in _client_addr;
    public:
		TcpServer(int domain, int type, int protocol, int port, std::string &ip, int backlog);
		TcpServer(const TcpServer &other);
		TcpServer &operator=(const TcpServer &other);
		~TcpServer();
		void acceptConnections();
};