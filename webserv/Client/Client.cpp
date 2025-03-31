#include "Client.hpp"

Client::Client(int domain, int type, int protocol, int port, std::string &ip) : Socket(domain, type, protocol, port, ip)
{
    std::cout << "client obj + socket created" << std::endl;
    connectToSocket();
}

Client::~Client()
{}
void Client::connectToSocket()
{
    if (connect(_socket_fd, (sockaddr *)&_socket_addr, sizeof(_socket_addr)) < 0)
        throw std::runtime_error("Error: Client can't connect to server!");
    write(_socket_fd, "Hello from Client\n", 19);
    char buf[1024];
    std::cout << "Reading from server socket" << std::endl;
    read(_socket_fd, buf, 1024);
    std::cout << buf << std::endl;
}
