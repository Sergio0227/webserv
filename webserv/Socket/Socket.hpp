#pragma once
#include "webserv.hpp"
class Socket
{
	protected:
		sockaddr_in _socket_addr;
		int _socket_fd;
		Socket();

	public:
		Socket(int domain, int type, int protocol, int port, std::string &ip);
		Socket(const Socket &other);
		Socket &operator=(const Socket &other);
		virtual ~Socket();
};
