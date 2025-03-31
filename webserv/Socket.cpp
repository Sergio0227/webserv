#include "Socket.hpp"

Socket::Socket()
{}

Socket::Socket(int domain, int type, int protocol, int port, u_long ip)
{
    _socket_addr.sin_family = domain;
    _socket_addr.sin_addr.s_addr = htonl(ip);
    _socket_addr.sin_port = htons(port);
    _socket_fd = socket(domain, type, protocol);
    if (_socket_fd < 0)
        throw std::runtime_error("Error: Socket can't be created!");
}

Socket::Socket(const Socket &other)
{
    _socket_addr = other._socket_addr;
    _socket_fd = other._socket_fd;
}

Socket &Socket::operator=(const Socket &other)
{
    if (this != &other)
    {
        _socket_addr = other._socket_addr;
    }
    return *this;
}

Socket::~Socket()
{
    close(_socket_fd);
}
