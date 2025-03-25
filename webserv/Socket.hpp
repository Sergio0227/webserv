#ifndef SOCKET_HPP
#define SOCKET_HPP
#include <sys/socket.h>
#include <iostream>
#include <stdexcept>


class Socket
{
	private:
		int _socket;
		int _domain;
		int _type;
		int _protocol;

	public:
		Socket();
		Socket(const Socket &other);
		Socket &operator=(const Socket &other);
		~Socket();
};


#endif
