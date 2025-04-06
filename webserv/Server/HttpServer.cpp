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
	logMessage(SUCCESS, "Server successfully set up and listening for incoming connections.", NULL, 0);
	handleIncomingConnections();
}

HttpServer::~HttpServer()
{
	for (int i = 0; i < FD_SETSIZE; ++i)
	{
		if (FD_ISSET(i, &_cur_sockets))
		{
			if (i == _socket_fd)
				logMessage(INFO, "Server connection closed.", &_client_info[i], 0);
			else
				logMessage(INFO, "Client connection closed.", &_client_info[i], 0);
			close(i);
			FD_CLR(i, &_cur_sockets);
		}
	}
	logMessage(SUCCESS, "Server has successfully shut down. All connections closed.", NULL, 0);
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
		logMessage(ERROR, "Error accepting client connection. Continuing...", NULL, 0);
		return -1;
	}
	_client_info[client_fd].addr = client_addr;
	logMessage(INFO, "New client connected.", &_client_info[client_fd], 0);
	return client_fd;
}

//parse request to methods, based on method execute response or handle error
void HttpServer::handleRequest(int client_fd)
{
	ClientInfo &info = _client_info[client_fd];
	info.fd = client_fd;
	info.close_connection = false;
	parseRequest(info);
	if (!info.close_connection)
		executeResponse(info);
	else if (_debug)
		logMessage(DEBUG, "Client request failed.", &info, 0);
	handleErrorResponse(info);
}

//parse request to map<int client_fd, struct ClientInfo>
void HttpServer::parseRequest(ClientInfo& info)
{
	std::string &req = info.info.request;

	req = readRequest(info);
	parseRequestLine(info);
	if (info.close_connection) return ;
	parseRequestHeader(info);
	if (info.close_connection) return ;
	parseRequestBody(info);
	if (info.close_connection) return ;
}

//reads request and return string
//todo error function for recv
std::string HttpServer::readRequest(ClientInfo &info)
{
	char buffer[1024];
	int bytes_read = recv(info.fd, buffer, sizeof(buffer) - 1, 0);
	if (bytes_read < 0)
		throw std::runtime_error("No Request send.");
	buffer[bytes_read] = '\0';
	return (std::string(buffer));
}

//parses method, path, version, and checks for errors
void HttpServer::parseRequestLine(ClientInfo& info)
{
	std::string &req = info.info.request;

	int i = req.find(' ');
	std::string method = req.substr(0, i);
	if (method != "GET" && method != "POST" && method != "PUT" && method != "DELETE")
	{
		info.status_code = 405, info.close_connection = true;
		return;
	}
	req = req.substr(i+1, req.size());
	i = req.find(' ');
	std::string path = req.substr(0, i);
	if (!pathExists(path))
	{
		info.status_code = 404, info.close_connection = true;
		return ;
		
		closeConnection(info.fd);
	}
	req = req.substr(i+1, req.size());
	i = req.find('\r');
	std::string http_version = req.substr(0, i);
	if (http_version != "HTTP/1.1")
	{
		info.status_code = 505, info.close_connection = true;
		return ;
	}
	_client_info[info.fd].info.method = getEnumMethod(method);
	_client_info[info.fd].info.path = path;
	_client_info[info.fd].info.http_version = http_version;
	req = req.substr(i+2, req.size());
}

//if an Http error occurs an appropriate Response is send and the connection is closed
void HttpServer::handleErrorResponse(ClientInfo &info)
{
	if (!info.close_connection)
		return ;
	int code = info.status_code;
	std::ostringstream oss;
	oss << code;
	std::string body_path("var/www/html/error_pages/");
	body_path += oss.str();
	body_path += ".html";
	std::string body = parseFileToString(body_path.c_str());
	std::string msg = getStatusMessage(code);
	sendHttpResponse(info, msg.c_str(), body, NULL);
	closeConnection(info.fd);
}

//parses the Client-Request-Header to a map<string, string>
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
			info.status_code = 400, info.close_connection = true;
			return ;
		}
		info.info.headers[key] = val;
	}
}

//only x-www-form-urlencoded is allowed on this webserver
//parses body into email and passw
void HttpServer::parseRequestBody(ClientInfo& info)
{
	std::string &req = info.info.request;
	std::string &body_ref = info.info.body.body_str;

	if (req.size() == 2)
		return;
	body_ref = req.substr(2, req.size());
	req.clear();
	if (info.info.headers["Content-Type"] == "application/x-www-form-urlencoded")
	{
		info.info.body.email = body_ref.substr(body_ref.find("email=") + 6, body_ref.find('&'));
		info.info.body.passw = body_ref.substr(body_ref.find("password=") + 9, body_ref.size());
	}
	else
		info.status_code = 415, info.close_connection = true;
}

//Depending on the method, the server sends a response back
void HttpServer::executeResponse(ClientInfo &info)
{
	Methods meth = info.info.method;

	if (meth == GET)
	{
		info.status_code = 200;
		info.status_msg = "OK";
		std::string body = extractBody(info);
		sendHttpResponse(info, info.status_msg.c_str(), body, NULL);
	}
	else if (meth == POST)
	{
		//if email already exists send 409 Conflict
		if (emailExists(info))
		{
			info.status_code = 409, info.close_connection = true;
			return ;
		}
		info.status_code = 201;
		info.status_msg = "Created";
		std::string body = parseFileToString("var/www/html/accountCreated.html");
		sendHttpResponse(info, info.status_msg.c_str(), body, NULL);
		storeCredential(info.info.body, "var/www/data/users.csv");
	}
	else if (meth == DELETE)
	{
		//todo
	}
	else if (meth == PUT)
	{
		//todo
	}
	if (_debug)
		logMessage(DEBUG, "Client", &info, 2);
}

//todo error function for send
//constructs and sends HttpResponse
void HttpServer::sendHttpResponse(ClientInfo &info, const char *msg, std::string &body, const char *location)
{
	std::ostringstream oss;
	oss << info.status_code;
	std::string status_code_str = oss.str();
	oss.str("");
	oss.clear();
	oss << body.size();
	std::string body_len = oss.str();
	std::string response = "HTTP/1.1 " + status_code_str + " " + std::string(msg) + "\r\n";
	response += "Content-Type: text/html\r\n";
	response += "Content-Length: " + body_len + "\r\n";
	response += "Connection: keep-alive\r\n";
	if (location != NULL)
		response += "Location: " + std::string(location) + "\r\n";
	response += "\r\n";
	response += body;
	int bytes_sent = send(info.fd, response.c_str(), response.size(), 0);
	if (bytes_sent < 0)
		std::runtime_error("send error");
}

//change open to fstream
//reads a file, parses it into a string and returns it
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
	if (_debug)
		logMessage(DEBUG, "Connection closed", &_client_info[fd], 0);
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
		fullpath += info.info.path;
	return (parseFileToString(fullpath.c_str()));
}