#ifndef TCPSERVER_HPP
#define TCPSERVER_HPP
# include "Socket.hpp"


class TcpServer : public Socket
{
    private:
        TcpServer();
		int _backlog;
    public:
		TcpServer(int domain, int type, int protocol, int port, u_long ip, int backlog);
		TcpServer(const TcpServer &other);
		TcpServer &operator=(const TcpServer &other);
		~TcpServer();
		void accept_connections();
};
#endif