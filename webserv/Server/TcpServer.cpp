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
        std::string body = "<html><body><h1>200 OK</h1><p>Hello, world!</p></body></html>";
        std::string msg = "OK";
        sendHttpResponse(200, msg, body);
        close(_client_socket);
    }
}
void TcpServer::sendHttpResponse(int status_code, std::string &msg, std::string &body)
{
    std::ostringstream oss;
    oss << status_code;
    std::string status_code_str = oss.str();
    oss.str("");
    oss.clear();
    oss << body.size();
    std::string body_len = oss.str();
    std::string response = "HTTP/1.1 " + status_code_str + " " + msg + "\r\n";
    response += "Content-Type: text/html\r\n";
    response += "Content-Length: " + body_len + "\r\n";
    response += "Connection: close\r\n";
    response += "\r\n";
    response += body;
    send(_client_socket, response.c_str(), response.size(), 0);
}
