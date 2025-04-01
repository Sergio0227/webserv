#include "Client.hpp"

Client::Client(int domain, int type, int protocol, int port, std::string &ip) : Socket(domain, type, protocol, port, ip)
{
    connectToSocket();
    sendRequest();
    readFromServer();
}

Client::~Client()
{}

void Client::connectToSocket()
{
    if (connect(_socket_fd, (sockaddr *)&_socket_addr, sizeof(_socket_addr)) < 0)
        throw std::runtime_error("Error: Client can't connect to server!");
}


void Client::readFromServer()
{
    char buf[1024];
    std::cout << "Reading from server socket" << std::endl;
    int bytes_read = read(_socket_fd, buf, 1024);
    if (bytes_read < 0)
        throw std::runtime_error("Error: Failed to read from server");
    buf[bytes_read] = '\0';
    std::cout << buf << std::endl;
}
// example of GET method
void Client::sendRequest()
{
    std::string requestLine = "GET /example.html HTTP/1.1 \r\n";
    requestLine += "Host: www.example.com\r\n";
    requestLine += "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/58.0.3029.110 Safari/537.36\r\n";
    requestLine += "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8\r\n";
    requestLine += "Accept-Language: en-US,en;q=0.9\r\n";
    requestLine += "Accept-Encoding: gzip, deflate, br\r\n";
    requestLine += "Connection: keep-alive\r\n";
    requestLine += "Upgrade-Insecure-Requests: 1\r\n";
    requestLine += "\r\n";
    write(_socket_fd, requestLine.c_str(), requestLine.size());  
}
