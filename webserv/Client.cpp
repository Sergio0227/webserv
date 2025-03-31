#include "Client.hpp"

Client::Client(int domain, int type, int protocol, int port, u_long ip) : Socket(domain, type, protocol, port, ip)
{
    std::cout << "client obj + socket created" << std::endl;
    connect_to_server();
}

Client::~Client()
{}
void Client::connect_to_server()
{
    if (connect(_socket_fd, (sockaddr *)&_socket_addr, sizeof(_socket_addr)) < 0)
        throw std::runtime_error("Error: Client can't connect to server!");
    write(_socket_fd, "Hello from Client\n", 19);
    char buf[1024];
    std::cout << "Reading from server socket" << std::endl;
    read(_socket_fd, buf, 1024);
}
