# include "HttpServer.hpp"

HttpServer::HttpServer()
{}

HttpServer::HttpServer(int domain, int type, int protocol, int port, std::string &ip, int backlog, bool debug) : Socket(domain, type, protocol, port, ip) 
{
	//init serverStruct
	root_path = "var/www/html";

	_debug = debug;
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
				}
				else
					handleRequest(i);
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
	_client_info[client_fd].addr = client_addr;
	return client_fd;
}

//parse request to methods, based on method execute response or handle error
void HttpServer::handleRequest(int client_fd)
{
	ClientInfo &info = _client_info[client_fd];
	_client_info[client_fd].fd = client_fd;
	parseRequest(info);
	std::string body = extractBody(info);
	sendHttpResponse(client_fd, 200, "OK", body);
}

//parse request to map<int client_fd, struct ClientInfo>
void HttpServer::parseRequest(ClientInfo& info)
{
	info.info.request = readRequest(info);
	if (_debug)
	{
		std::cout << "--------Reading-Request-----------------"<< std::endl;
		std::cout << info.info.request << std::endl;
		std::cout << "--------------------------"<< std::endl;
	}
	parseRequestLine(info);
	parseRequestHeader(info);
	parseRequestBody(info);
	if (_debug)
		printClientInfo(info);
}

std::string HttpServer::readRequest(ClientInfo &info)
{
	char buffer[1024];
	int bytes_read = recv(info.fd, buffer, sizeof(buffer) - 1, 0);
	if (bytes_read < 0)
		throw std::runtime_error("No Request send.");
	buffer[bytes_read] = '\0';
	return (std::string(buffer));
}

void HttpServer::parseRequestLine(ClientInfo& info)
{
	int i = info.info.request.find(' ');
	std::string method = info.info.request.substr(0, i);
	if (method != "GET" && method != "POST" && method != "PUT" && method != "DELETE")
	{
		std::string body = parseFileToString("var/www/html/error_pages/405.html");
		sendHttpResponse(info.fd, 405, "Method Not Allowed", body);
		closeConnection(info.fd);
	}
	info.info.request = info.info.request.substr(i+1, info.info.request.size());
	i = info.info.request.find(' ');
	std::string path = info.info.request.substr(0, i);
	if (!pathExists(path))
	{
		std::string body = parseFileToString("var/www/html/error_pages/404.html");
		sendHttpResponse(info.fd, 404, "Not Found", body);
		closeConnection(info.fd);
	}
	info.info.request = info.info.request.substr(i+1, info.info.request.size());
	i = info.info.request.find('\r');
	std::string http_version = info.info.request.substr(0, i);
	if (http_version != "HTTP/1.1")
	{
		std::string body = parseFileToString("var/www/html/error_pages/505.html");
		sendHttpResponse(info.fd, 505, "HTTP Version Not Supported", body);
		closeConnection(info.fd);
	}
	_client_info[info.fd].info.method = method;
	_client_info[info.fd].info.path = path;
	_client_info[info.fd].info.http_version = http_version;
	info.info.request = info.info.request.substr(i+2, info.info.request.size());
}


void HttpServer::parseRequestHeader(ClientInfo& info)
{
	int i;
	while (true)
	{
		if (info.info.request[0] == '\r')
			break;
		std::string key = info.info.request.substr(0, info.info.request.find(':'));
		info.info.request = info.info.request.substr(info.info.request.find(':') + 2, info.info.request.size());
		std::string val = info.info.request.substr(0, info.info.request.find('\r'));
		info.info.request = info.info.request.substr(info.info.request.find('\r') + 2, info.info.request.size());
		if (key.empty() == true || val.empty() == true)
		{
			//todo
			std::string body = parseFileToString("var/www/html/error_pages/400.html");
			sendHttpResponse(info.fd, 400, "Bad Request", body);
			closeConnection(info.fd);
		}
		info.info.headers[key] = val;
	}
}
void HttpServer::parseRequestBody(ClientInfo& info)
{
	if (info.info.request.size() == 2)
		return;
	info.info.body = info.info.request.substr(2, info.info.request.size());
	info.info.request.clear();
}

int HttpServer::checkRequest(std::string &msg)
{
	return 0;
}
void HttpServer::executeResponse()
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
	response += "Connection: keep-alive\r\n";
	response += "\r\n";
	response += body;
	int bytes_sent = send(fd, response.c_str(), response.size(), 0);
	if (bytes_sent < 0)
		errorHandler("send");
	if (_debug)
	{
		std::cout << "--------Sending-Response-----------------"<< std::endl;
		std::cout << response << std::endl;
		std::cout << "--------------------------"<< std::endl;
	}
}

std::string HttpServer::parseFileToString(const char *filename)
{
	std::string str;

	int fd = open(filename, O_RDONLY);
	char buffer[1024];
	int b_read;
	while ((b_read = read(fd, buffer, sizeof(buffer))) > 0)
	{
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

std::string HttpServer::extractBody(ClientInfo& info)
{
	std::string fullpath(root_path);
	if (info.info.path[info.info.path.size() - 1] == '/')
	{
		fullpath += info.info.path;
		fullpath += "index.html";
	}
	else
	{
		fullpath += info.info.path;
	}
	return (parseFileToString(fullpath.c_str()));
}