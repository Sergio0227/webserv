#include "Socket.hpp"

Socket::Socket()
{}
Socket::Socket(int domain, int type, int protocol, int port, std::string &ip)
{
	_socket_addr.sin_family = domain;
	int ip_res = inet_pton(domain, ip.c_str(), &_socket_addr.sin_addr);
	if (ip_res <= 0)
		errorHandler("inet_pton");
	_socket_addr.sin_port = htons(port);
	memset(_socket_addr.sin_zero, 0, sizeof(_socket_addr.sin_zero));
	_socket_fd = socket(domain, type, protocol);
	if (_socket_fd < 0)
		errorHandler("socket");
}

Socket::~Socket()
{
	close(_socket_fd);
}

void Socket::errorHandler(const char *function_name)
{
	throw std::runtime_error("Error: " + std::string(function_name) + " " + std::string(strerror(errno)));
}