# include "TcpServer.hpp"

TcpServer::TcpServer()
{}

TcpServer::TcpServer(int domain, int type, int protocol, int port, std::string &ip, int backlog) : Socket(domain, type, protocol, port, ip) 
{
	_backlog = backlog;
	FD_ZERO(&_cur_sockets);
	FD_SET(_socket_fd, &_cur_sockets);
	if (bind(_socket_fd, (sockaddr *)&_socket_addr, sizeof(_socket_addr)) < 0)
		errorHandler("bind");
	if (listen(_socket_fd, _backlog) < 0)
		errorHandler("listen");
	handleIncomingConnections();
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
			errorHandler("select");
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
					closeConnection(i);
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
		errorHandler("accept");
	return client_fd;
}


void TcpServer::handleRequest(int client_fd)
{
	parseRequest(client_fd);
	std::string body = "a";
	sendHttpResponse(client_fd, 200, "OK", body);
	throw std::runtime_error("debug point");
	executeRequest(); 
}

std::string TcpServer::readRequest(int client_fd)
{
	char buffer[1024];
	int bytes_read = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
	if (bytes_read < 0)
		throw std::runtime_error("No Request send.");
	buffer[bytes_read] = '\0';
	return (std::string(buffer));
}

void TcpServer::parseRequestLine(int client_fd, std::string &request)
{
	int i = request.find(' ');
	std::string method = request.substr(0, i);
	if (method != "GET" && method != "POST" && method != "PUT" && method != "DELETE")
	{
		std::string body = parseFileToString("var/www/html/error_pages/405.html");
		sendHttpResponse(client_fd, 405, "Method Not Allowed", body);
		closeConnection(client_fd);
	}
	request = request.substr(i, request.size());
	i = request.find(' ');
	std::string path = request.substr(0, i);
	if (!pathExists(path))
	{
		std::string body = parseFileToString("var/www/html/error_pages/404.html");
		sendHttpResponse(client_fd, 404, "Not Found", body);
		closeConnection(client_fd);
	}
	request = request.substr(i, request.size());
	i = request.find('\r');
	std::string http_version = request.substr(0, i);
	if (http_version != "HTTP/1.1")
	{
		std::string body = parseFileToString("var/www/html/error_pages/505.html");
		sendHttpResponse(client_fd, 505, "HTTP Version Not Supported", body);
		closeConnection(client_fd);
	}
	_client_info[client_fd].info.method = method;
	_client_info[client_fd].info.path = path;
	_client_info[client_fd].info.http_version = http_version;
	request = request.substr(i+2, request.size());
	std::cout << request << std::endl;
}
void TcpServer::parseRequest(int client_fd)
{
	std::string request;
	request = readRequest(client_fd);
	parseRequestLine(client_fd, request);
	parseRequestHeader(request);
	parseRequestBody(request);
}

void TcpServer::parseRequestHeader(std::string &request)
{
	
}
void TcpServer::parseRequestBody(std::string &request)
{
	if (request.empty() == true)
		return;
}

int TcpServer::checkRequest(std::string &msg)
{
	return 0;
}
void TcpServer::executeRequest()
{

}

void TcpServer::sendHttpResponse(int fd, int status_code, const char *msg, std::string &body)
{
	std::ostringstream oss;
	oss << status_code;
	std::string status_code_str = oss.str();
	oss.str("");
	oss.clear();
	oss << body.size();
	std::string body_len = oss.str();
	std::string response = "HTTP/1.1 " + status_code_str + " " + std::string(msg) + "\r\n";
	response += "Content-Type: text/html\r\n";
	response += "Content-Length: " + body_len + "\r\n";
	response += "Connection: close\r\n";
	response += "\r\n";
	response += body;
	std::cout << response << std::endl;
	int bytes_sent = send(fd, response.c_str(), response.size(), 0);
	if (bytes_sent < 0) {
		errorHandler("send");
	}
}

void TcpServer::closeConnection(int fd)
{
	FD_CLR(fd, &_cur_sockets);
	_client_info.erase(fd);
	close(fd);
	std::cerr << "Connection closed: " << fd << std::endl;
}
