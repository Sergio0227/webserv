#pragma once
#include "webserv.hpp"

class Socket
{
	protected:
		sockaddr_in _socket_addr;
		int _domain;
		int _type;
		int _protocol;
		int _socket_fd;
		Socket();
		void errorHandler(const char *function_name);
	public:
		Socket(int port, std::string &ip);
		virtual ~Socket();
};
