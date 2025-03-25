#ifndef TCPSERVER_HPP
#define TCPSERVER_HPP
# include "Socket.hpp"
#include <netinet/in.h>

class TcpServer
{
    private:
        TcpServer();
        
    public:
		TcpServer();
		TcpServer(const TcpServer &other);
		TcpServer &operator=(const TcpServer &other);
		~TcpServer();
};
#endif