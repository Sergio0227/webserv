#include "Socket.hpp"

Socket::Socket()
{

}

Socket::Socket(int domain, int type, int protocol) : _domain(domain), _type(type), _protocol(protocol)
{
    _socket = socket(_domain, _type, _protocol);
    if (_socket < 0)
        throw std::exe
}

Socket::~Socket()
{
}
