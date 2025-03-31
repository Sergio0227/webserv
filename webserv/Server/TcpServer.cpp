# include "TcpServer.hpp"

TcpServer::TcpServer()
{}

TcpServer::TcpServer(int domain, int type, int protocol, int port, std::string &ip, int backlog) : Socket(domain, type, protocol, port, ip) 
{
    _backlog = backlog;
    if (bind(_socket_fd, (sockaddr *)&_socket_addr, sizeof(_socket_addr)) < 0)
        throw std::runtime_error("Error: Can't bind socket!");
    if (listen(_socket_fd, _backlog) < 0)
        throw std::runtime_error("Error: Can't listen to ongoing connection!");
    acceptConnections();
}
TcpServer::TcpServer(const TcpServer &other)
{

}
TcpServer &TcpServer::operator=(const TcpServer &other)
{
    return *this;
}
TcpServer::~TcpServer()
{
}
void TcpServer::acceptConnections()
{
    socklen_t client_addr_len = sizeof(_client_addr);

    while (1)
    {
        if ((_client_socket = accept(_socket_fd, (struct sockaddr *)&_client_addr, &client_addr_len)) < 0)
            throw std::runtime_error("Error: Can't accept the connection!");
        char buffer[1024];
        std::cout << "Reading" << std::endl;
        read(_client_socket, buffer, 1024);
        std::cout << buffer << std::endl;
        std::cout << "Writing" << std::endl;
        write(_client_socket, "Test", 4);
        close(_client_socket);
    }
}
