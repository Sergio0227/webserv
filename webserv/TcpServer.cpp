# include "TcpServer.hpp"

TcpServer::TcpServer()
{}

TcpServer::TcpServer(int domain, int type, int protocol, int port, u_long ip, int backlog) : Socket(domain, type, protocol, port, ip) 
{
    _backlog = backlog;
    if (bind(_socket_fd, (sockaddr *)&_socket_addr, sizeof(_socket_addr)) < 0)
        throw std::runtime_error("Error: Can't bind socket!");
    if (listen(_socket_fd, _backlog) < 0)
        throw std::runtime_error("Error: Can't listen to ongoing connection!");
    accept_connections();
}
TcpServer::TcpServer(const TcpServer &other)
{

}
TcpServer &TcpServer::operator=(const TcpServer &other)
{

}
TcpServer::~TcpServer()
{
}
void TcpServer::accept_connections()
{
    int new_socket;
    while (1)
    {
        if (new_socket = accept(_socket_fd, (sockaddr *)&_socket_addr, (socklen_t *)&_socket_addr) < 0)
            throw std::runtime_error("Error: Can't accept the connection!");
        char buffer[1024];
        std::cout << "Reading" << std::endl;
        read(new_socket, buffer, 1024);
        std::cout << buffer << std::endl;
        std::cout << "Writing" << std::endl;
        write(new_socket, "Test", 4);
        close(new_socket);
    }
}
