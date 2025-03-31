#ifndef SOCKET_HPP
#define SOCKET_HPP
#include <sys/socket.h>
#include <iostream>
#include <stdexcept>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

class Socket
{
	protected:
		sockaddr_in _socket_addr;
		int _socket_fd;
		Socket();

	public:
		Socket(int domain, int type, int protocol, int port, u_long ip);
		Socket(const Socket &other);
		Socket &operator=(const Socket &other);
		virtual ~Socket();
};


#endif
