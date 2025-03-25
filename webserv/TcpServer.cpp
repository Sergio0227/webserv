# include "TcpServer.hpp"

TcpServer::TcpServer()
{}

TcpServer::TcpServer()
{
    if (connect(_socket_fd, (sockaddr *)&_socket_addr, sizeof(_socket_addr)) < 0)
        throw std::runtime_error("Error: Can't establish connection!");
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
