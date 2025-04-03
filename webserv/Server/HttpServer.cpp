# include "HttpServer.hpp"

HttpServer::HttpServer()
{}

HttpServer::HttpServer(int domain, int type, int protocol, int port, std::string &ip, int backlog) : Socket(domain, type, protocol, port, ip) 
{
	root_path = "var/www/html";
	_backlog = backlog;
	FD_ZERO(&_cur_sockets);
	FD_SET(_socket_fd, &_cur_sockets);
	if (bind(_socket_fd, (sockaddr *)&_socket_addr, sizeof(_socket_addr)) < 0)
		errorHandler("bind");
	if (listen(_socket_fd, _backlog) < 0)
		errorHandler("listen");
	handleIncomingConnections();
}

HttpServer::~HttpServer()
{
	for (int i = 0; i < FD_SETSIZE; ++i)
	{
		if (FD_ISSET(i, &_cur_sockets))
		{
			close(i);
			FD_CLR(i, &_cur_sockets);
		}
	}
}

void HttpServer::handleIncomingConnections()
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
					_client_info[client_fd] = 

				}
				else
				{
					handleRequest(i);
					//closeConnection(i);
				}
			}
		}        
	}
}


int HttpServer::acceptConnections()
{
	sockaddr_in client_addr;
	size_t addr_len = sizeof(client_addr);
	int client_fd;

	if ((client_fd = accept(_socket_fd, (struct sockaddr *)&client_addr, (socklen_t *)&addr_len)) < 0)
		errorHandler("accept");
	return client_fd;
}


//parse request to methods, based on method execute response or handle error
void HttpServer::handleRequest(int client_fd)
{
	parseRequest(client_fd);
	std::string body = parseFileToString(_client_info[client_fd].info.path.c_str());
	sendHttpResponse(client_fd, 200, "OK", body);
}

//parse request to map<int client_fd, struct ClientInfo>
void HttpServer::parseRequest(int client_fd)
{
	std::string request;
	request = readRequest(client_fd);
	parseRequestLine(client_fd, request);
	parseRequestHeader(request);
	parseRequestBody(request);
}

std::string HttpServer::readRequest(int client_fd)
{
	char buffer[1024];
	int bytes_read = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
	if (bytes_read < 0)
		throw std::runtime_error("No Request send.");
	buffer[bytes_read] = '\0';
	return (std::string(buffer));
}
//extract location->root = path from config file
//todo parseFileToString(location->root + error_pages/XXX.html)
void HttpServer::parseRequestLine(int client_fd, std::string &request)
{
	int i = request.find(' ');
	std::string method = request.substr(0, i);
	if (method != "GET" && method != "POST" && method != "PUT" && method != "DELETE")
	{
		std::string body = parseFileToString("/error_pages/405.html");
		sendHttpResponse(client_fd, 405, "Method Not Allowed", body);
		closeConnection(client_fd);
	}
	request = request.substr(i+1, request.size());
	i = request.find(' ');
	std::string path = request.substr(0, i);
	if (!pathExists(path))
	{
		std::string body = parseFileToString("/error_pages/404.html");
		sendHttpResponse(client_fd, 404, "Not Found", body);
		closeConnection(client_fd);
	}
	request = request.substr(i+1, request.size());
	i = request.find('\r');
	std::string http_version = request.substr(0, i);
	if (http_version != "HTTP/1.1")
	{
		std::string body = parseFileToString("/error_pages/505.html");
		sendHttpResponse(client_fd, 505, "HTTP Version Not Supported", body);
		closeConnection(client_fd);
	}
	_client_info[client_fd].info.method = method;
	_client_info[client_fd].info.path = path;
	_client_info[client_fd].info.http_version = http_version;
	std::cout << "Method: " << _client_info[client_fd].info.method << std::endl;
	std::cout << "Path: " << _client_info[client_fd].info.path << std::endl;
	std::cout << "HTTP Version: " << _client_info[client_fd].info.http_version << std::endl;
	request = request.substr(i+2, request.size());
	std::cout << request << std::endl;
}


void HttpServer::parseRequestHeader(std::string &request)
{
	return;
}
void HttpServer::parseRequestBody(std::string &request)
{
	if (request.empty() == true)
		return;
}

int HttpServer::checkRequest(std::string &msg)
{
	return 0;
}
void HttpServer::executeRequest()
{
	return;
}

void HttpServer::sendHttpResponse(int fd, int status_code, const char *msg, std::string &body)
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
	int bytes_sent = send(fd, response.c_str(), response.size(), 0);
	if (bytes_sent < 0) {
		errorHandler("send");
	}
}

std::string HttpServer::parseFileToString(const char *filename)
{
	std::string str;
	std::string full_path(root_path);
	full_path += filename;

	int fd = open(full_path.c_str(), O_RDONLY);
	char buffer[1024];
	int b_read = 1;
	while (b_read)
	{
		b_read = read(fd, buffer, sizeof(buffer) - 1);
		buffer[b_read] = '\0';
		str.append(buffer, b_read);
	}
	close(fd);
	return str;
}

bool HttpServer::pathExists(std::string &path)
{
	std::string full_path(root_path);
	full_path += path;
	if (!access(full_path.c_str(), F_OK))
		return true;
	else return false;
}

void HttpServer::closeConnection(int fd)
{
	close(fd);
	FD_CLR(fd, &_cur_sockets);
	_client_info.erase(fd);
	std::cerr << "Connection closed: " << fd << std::endl;
}
