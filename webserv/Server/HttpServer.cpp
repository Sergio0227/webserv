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
	_client_info[_socket_fd].fd = _socket_fd; //first entry in _client_info map is server socked with addr saved to clean up
	_client_info[_socket_fd].addr = _socket_addr;

	if (bind(_socket_fd, (sockaddr *)&_socket_addr, sizeof(_socket_addr)) < 0)
		errorHandler("Bind");
	if (listen(_socket_fd, _backlog) < 0)
		errorHandler("Listen");
	logMessage(SUCCESS, "Server successfully set up and listening for incoming connections.", NULL);
	handleIncomingConnections();
}

HttpServer::~HttpServer()
{
	for (int i = 0; i < FD_SETSIZE; ++i)
	{
		if (FD_ISSET(i, &_cur_sockets))
		{
			if (i == _socket_fd)
				logMessage(INFO, "Server connection closed.", &_client_info[i]);
			else
				logMessage(INFO, "Client connection closed.", &_client_info[i]);
			close(i);
			FD_CLR(i, &_cur_sockets);
		}
	}
	logMessage(SUCCESS, "Server has successfully shut down. All connections closed.", NULL);
}

void HttpServer::handleIncomingConnections()
{
	while (g_flag)
	{
		_rdy_sockets = _cur_sockets;
		if (select(FD_SETSIZE, &_rdy_sockets, NULL, NULL, NULL) < 0)
		{
			if (errno == EINTR)
				continue;
			else
				errorHandler("Select");
		}	
		for (int i = 0; i < FD_SETSIZE; ++i)
		{
			if (FD_ISSET(i, &_rdy_sockets))
			{
				if (i == _socket_fd)
				{
					int client_fd = acceptConnections();
					if (client_fd < 0)
						continue;
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
	{
		logMessage(ERROR, "Error accepting client connection. Continuing...", NULL);
		return -1;
	}
	_client_info[client_fd].addr = client_addr;
	logMessage(INFO, "New client connected.", &_client_info[client_fd]);
	return client_fd;
}

//parse request to methods, based on method execute response or handle error
void HttpServer::handleRequest(int client_fd)
{
	ClientInfo &info = _client_info[client_fd];
	_client_info[client_fd].fd = client_fd;
	parseRequest(info);
	executeResponse(info);
}

//parse request to map<int client_fd, struct ClientInfo>
void HttpServer::parseRequest(ClientInfo& info)
{
	std::string &req = info.info.request;

	req = readRequest(info);
	if (_debug)
	{
		std::cout << "--------Reading-Request-----------------"<< std::endl;
		std::cout << req << std::endl;
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
	std::string &req = info.info.request;

	int i = req.find(' ');
	std::string method = req.substr(0, i);
	if (method != "GET" && method != "POST" && method != "PUT" && method != "DELETE")
	{
		std::string body = parseFileToString("var/www/html/error_pages/405.html");
		sendHttpResponse(info.fd, 405, "Method Not Allowed", body);
		closeConnection(info.fd);
	}
	req = req.substr(i+1, req.size());
	i = req.find(' ');
	std::string path = req.substr(0, i);
	if (!pathExists(path))
	{
		std::string body = parseFileToString("var/www/html/error_pages/404.html");
		sendHttpResponse(info.fd, 404, "Not Found", body);
		closeConnection(info.fd);
	}
	req = req.substr(i+1, req.size());
	i = req.find('\r');
	std::string http_version = req.substr(0, i);
	if (http_version != "HTTP/1.1")
	{
		std::string body = parseFileToString("var/www/html/error_pages/505.html");
		sendHttpResponse(info.fd, 505, "HTTP Version Not Supported", body);
		closeConnection(info.fd);
	}
	_client_info[info.fd].info.method = getMethod(method);
	_client_info[info.fd].info.path = path;
	_client_info[info.fd].info.http_version = http_version;
	req = req.substr(i+2, req.size());
}


void HttpServer::parseRequestHeader(ClientInfo& info)
{
	std::string &req = info.info.request;

	while (true)
	{
		if (req[0] == '\r')
			break;
		std::string key = req.substr(0, req.find(':'));
		req = req.substr(req.find(':') + 2, req.size());
		std::string val = req.substr(0, req.find('\r'));
		req = req.substr(req.find('\r') + 2, req.size());
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
	std::string &req = info.info.request;

	if (req.size() == 2)
		return;
	info.info.body = req.substr(2, req.size());
	req.clear();
}

void HttpServer::executeResponse(ClientInfo &info)
{
	Methods meth = info.info.method;
	if (meth == GET)
	{
		std::string body = extractBody(info);
		sendHttpResponse(info.fd, 200, "OK", body);
	}
	else if (meth == POST)
	{
		
	}
	else if (meth == DELETE)
	{
		
	}
	else if (meth == PUT)
	{
		
	}
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