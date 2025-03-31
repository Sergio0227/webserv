#include "Socket.hpp"

Socket::Socket()
{}
Socket::Socket(int domain, int type, int protocol, int port, std::string &ip)
{
    _socket_addr.sin_family = domain;
    int ip_res = inet_pton(domain, ip.c_str(), &_socket_addr.sin_addr);
    if (ip_res <= 0)
        throw std::runtime_error("Error: Invalid IP address format");
    _socket_addr.sin_port = htons(port);
    memset(_socket_addr.sin_zero, 0, sizeof(_socket_addr.sin_zero));
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
