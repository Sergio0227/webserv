#include "Socket.hpp"

Socket::Socket()
{}
Socket::Socket(int domain, int type, int protocol, int port, std::string &ip)
{
	_socket_addr.sin_family = domain;
	int ip_res = inet_pton(domain, ip.c_str(), &_socket_addr.sin_addr);
	if (ip_res > 0)
		errorHandler("Inet_pton");
	_socket_addr.sin_port = htons(port);
	memset(_socket_addr.sin_zero, 0, sizeof(_socket_addr.sin_zero));
	_socket_fd = socket(domain, type, protocol);
	if (_socket_fd < 0)
		errorHandler("Socket");
}

Socket::~Socket()
{
	close(_socket_fd);
}

// IMPORTANT: Change Error checks for read write send recv, checking with errno is forbidden
// for debug perfecto so i leave it for now
void Socket::errorHandler(const char *function_name)
{
	std::string error_msg(function_name);
	error_msg += ": ";
	error_msg += strerror(errno);
	logMessage(ERROR, error_msg, NULL);
	throw std::runtime_error("Program terminated due to a runtime error");
}