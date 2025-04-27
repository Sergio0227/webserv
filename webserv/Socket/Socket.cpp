#include "Socket.hpp"

Socket::Socket()
{}
Socket::Socket(int port, std::string &ip)
{
	// Setting up socket parameters: IPv4 (AF_INET), TCP (SOCK_STREAM), default protocol (0)
	_domain = AF_INET;
	_type = SOCK_STREAM;
	_protocol = 0;

	// Create and configure the socket
	_socket_addr.sin_family = _domain;
	int ip_res = inet_pton(_domain, ip.c_str(), &_socket_addr.sin_addr);
	if (ip_res <= 0)
		errorHandler("Inet_pton");
	_socket_addr.sin_port = htons(port);
	memset(_socket_addr.sin_zero, 0, sizeof(_socket_addr.sin_zero));
	_socket_fd = socket(_domain, _type, _protocol);
	if (_socket_fd < 0)
		errorHandler("Socket");
	int optval = 1;
	if (setsockopt(_socket_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0)
		errorHandler("Setsockopt");
}

Socket::~Socket()
{
	close(_socket_fd);
}

void Socket::errorHandler(const char *function_name)
{
	std::string error_msg(function_name);
	error_msg += ": ";
	error_msg += strerror(errno);
	logMessage(ERROR, error_msg, NULL, 0);
	throw std::runtime_error("");
}