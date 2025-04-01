# include "TcpServer.hpp"

TcpServer::TcpServer()
{}

TcpServer::TcpServer(int domain, int type, int protocol, int port, std::string &ip, int backlog) : Socket(domain, type, protocol, port, ip) 
{
    _backlog = backlog;
    FD_ZERO(&_cur_sockets);
    FD_SET(_socket_fd, &_cur_sockets);
    if (bind(_socket_fd, (sockaddr *)&_socket_addr, sizeof(_socket_addr)) < 0)
        throw std::runtime_error("Error: Can't bind socket!");
    if (listen(_socket_fd, _backlog) < 0)
        throw std::runtime_error("Error: Can't listen to ongoing connection!");
    acceptConnections();
}

TcpServer::~TcpServer()
{
}

//want to use select for now, select can handle 1024 fd and checks if one has readable data in it, when yes read it
void TcpServer::handleIncomingConnections()
{
    while (1)
    {
        _rdy_sockets = _cur_sockets;
        if (select(FD_SETSIZE, &_rdy_sockets, NULL, NULL, NULL) < 0)
        {
            throw std::runtime_error("Error: Select");
        }
        for (int i = 0; i < FD_SETSIZE; ++i)
        {
            if (FD_ISSET(i, &_rdy_sockets))
            {
                if (i == _socket_fd)
                {
                    int client_fd = acceptConnections();
                    FD_SET(client_fd, &_cur_sockets);
                }
                else
                {
                    handleRequest(i);
                    FD_CLR(i, &_cur_sockets);
                }
            }
        }        
    }
}
//parse rquest to methods, based on method execute response or handle error
//my idea, server makes a map/dict out of the fd and saves a struct to that fd, example std::map<int, struct ClientInfo> --> map[client_fd] = struct
//ClientInfo{int ip, std::string method, std::string path}
//must fix: no read/write without going trough poll() or sim.

int TcpServer::acceptConnections()
{
    sockaddr_in client_addr;
	size_t addr_len = sizeof(client_addr);
    int client_fd;

    if ((client_fd = accept(_socket_fd, (struct sockaddr *)&client_addr, (socklen_t *)&addr_len)) < 0)
            throw std::runtime_error("Error: Can't accept the connection!");
    return client_fd;
}

void TcpServer::closeConnection(int fd)
{
    close(fd);
    _client_info.erase(fd);
}
void TcpServer::handleRequest(int client_fd)
{
    std::string msg;
    //int status_code;
    std::string request;
    request = readRequest(client_fd);
    parseRequestLine(client_fd, request);
	throw std::runtime_error("debug point");
    //status_code = checkRequest(msg);
	//if client==get read html and write it to body
    //sendHttpResponse(status_code, msg, body);
    executeRequest(); 
}

std::string TcpServer::readRequest(int client_fd)
{
    std::stringstream sst;
	char buffer[1024];
	int bytes_read = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
	buffer[bytes_read] = '\0';
    std::string request = buffer;
    return request;
}
void TcpServer::parseRequestLine(int client_fd, std::string &request)
{
	int i = request.find(' ');
	std::string method = request.substr(0, i);
	if (method != "GET" && method != "POST" && method != "PUT" && method != "DELETE")
	{
		/*
			send:
			HTTP/1.1 405 Method Not Allowed
			Content-Type: text/html
			Content-Length: 58
			Allow: GET, POST

			<html><body><h1>405 Method Not Allowed</h1><p>The method is not allowed for the requested resource.</p></body></html>
		*/
		closeConnection(client_fd);
	}
	request = request.substr(i, request.size());
	i = request.find(' ');
	std::string path = request.substr(0, i);
	//if (!pathExists(path))
	{
		/*
			send:
			HTTP/1.1 404 Not Found
			Content-Type: text/html
			Content-Length: 52

			<html><body><h1>404 Not Found</h1><p>The requested resource could not be found.</p></body></html>
		*/
		//closeConnection(client_fd);
	}
	request = request.substr(i, request.size());
	i = request.find('\r');
	std::string http_version = request.substr(0, i);
	if (http_version != "HTTP/1.1")
	{
		/*
			send:
			HTTP/1.1 505 HTTP Version Not Supported
			Content-Type: text/html
			Content-Length: 46

			<html><body><h1>505 HTTP Version Not Supported</h1></body></html>

		*/
		closeConnection(client_fd);
	}
	_client_info[client_fd].info.method = method;
	_client_info[client_fd].info.path = path;
	_client_info[client_fd].info.http_version = http_version;
	request = request.substr(i+2, request.size());
	std::cout << request << std::endl;
}
void TcpServer::parseRequest(std::string &line)
{

}
int TcpServer::checkRequest(std::string &msg)
{
	return 0;
}
void TcpServer::executeRequest()
{

}

void TcpServer::sendHttpResponse(int fd, int status_code, std::string &msg, std::string &body)
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
    send(fd, response.c_str(), response.size(), 0);
}
