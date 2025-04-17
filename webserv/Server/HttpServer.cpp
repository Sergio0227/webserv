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
					//another class should handle this function like this: 
					/*
						int client_fd = _servers[i].accept();
						if (client_fd > 0)
						{
							FD_SET(client_fd, &_fd_set);
							_clients[client_fd] = &_servers[i]; // associate client with Server!
						}
						else if (_clients.count(i)) // i is a client socket
						{
							Server* server = _clients[i];
							server->handleRequest(i);
					*/
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

	readRequest(info);
	info.info.request.clear();
	if (info.file_uploaded)
	{
		return;
	}
	if (!info.close_connection)
		executeResponse(info);
	if (info.close_connection)
	{
		if (_debug)
			logMessage(DEBUG, "Client request failed.", &info, 0);
		handleErrorResponse(info);
	}
	//info.reset();
}

//reads request and parses request to map<int client_fd, struct ClientInfo>
//todo error function for recv
void HttpServer::readRequest(ClientInfo &info)
{
	char buffer[1024];
	int bytes_read;
	size_t body_size;
	size_t total_read = 0;

	while (true)
	{
		bytes_read = recv(info.fd, buffer, sizeof(buffer) - 1, 0);
		if (bytes_read < 0)
			throw std::runtime_error("Error: recv");
		if (bytes_read == 0)
			break;
		buffer[bytes_read] = '\0';
		info.info.request.append(buffer, bytes_read);
		if (info.info.request.find("\r\n\r\n") != std::string::npos)
		{
			parseRequestLine(info);
			if (info.close_connection)
				return ;
			body_size = parseRequestHeader(info);
			if (info.close_connection || body_size == 0)
				return ;
			total_read = info.info.request.size() - 2;
			break;
		}
	}
	while (total_read < body_size)
	{
		bytes_read = recv(info.fd, buffer, sizeof(buffer) - 1, 0);
		if (bytes_read < 0)
			throw std::runtime_error("Error: recv");
		if (bytes_read == 0)
			break;
		buffer[bytes_read] = '\0';
		info.info.request.append(buffer, bytes_read);
		total_read += bytes_read;
	}
	//std::cout << "Request read: " << info.info.request << std::endl; //uncomment this to see raw request body from client
	parseRequestBody(info);
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
size_t HttpServer::parseRequestHeader(ClientInfo& info)
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
			return 0;
		}
		info.info.headers[key] = val;
	}
	if (info.info.headers["Content-Type"].find("multipart/form-data") != std::string::npos)
	{
		size_t pos = info.info.headers["Content-Type"].find("boundary=") + 9;
		info.info.boundary = info.info.headers["Content-Type"].substr(pos, info.info.headers["Content-Type"].size());
	}
	if (info.info.headers.find("Content-Length") != info.info.headers.end())
		return (std::atoi(info.info.headers["Content-Length"].c_str()));
	else
		return 0;
}

//only x-www-form-urlencoded and multidata is allowed on this webserver
//parses body into email and passw
void HttpServer::parseRequestBody(ClientInfo& info)
{
	std::string &req = info.info.request;
	std::string &body_ref = info.info.body.body_str;

	body_ref = req.substr(2, req.size());
	req.clear();
	if (info.info.headers["Content-Type"] == "application/x-www-form-urlencoded")
	{
		int start = body_ref.find("email=") + 6;
		int end = body_ref.find('&') - start;
		info.info.body.email = body_ref.substr(start, end);
		info.info.body.passw = body_ref.substr(body_ref.find("password=") + 9, body_ref.size());
	}
	else if (info.info.headers["Content-Type"].find("multipart/form-data") != std::string::npos)
	{
		uploadFile(info, body_ref, "var/www/data/images");
		info.file_uploaded = true;
	}
	else
		info.status_code = 415, info.close_connection = true;
}
//Depending on the method, the server sends a response back
//GET: if path or dir exists, if dir -> add index.html, if file doesnt exist send 404, else send html back with 200 ok
//POST: email already exists send 409 Conflict, else save credentials in csv and send 301 redirect to login
//PUT: updates a password
//DELETE: deletes an account if user is logged in
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
		if (info.info.path == "/register")
		{
			if (emailExists(info))
			{
				info.status_code = 409, info.close_connection = true;
				return ;
			}
			info.status_code = 303;
			info.status_msg = "See Other";
			std::string empty_string = "";
			sendHttpResponse(info, info.status_msg.c_str(), empty_string, "/login.html");
			storeCredential(info.info.body, "var/www/data/users.csv");
		}
		else if (info.info.path == "/login")
		{
			if (!emailExists(info) || !passwordCorrect(info.info.body))
			{
				info.status_code = 401, info.close_connection = true;
				return ;
			}
			info.status_code = 303;
			info.status_msg = "See Other";
			std::string empty_string = "";
			sendHttpResponse(info, info.status_msg.c_str(), empty_string, "/user.html");
		}
		else if (info.info.path == "/upload")
		{
			info.status_code = 201;
			info.status_msg = "Created";
			std::string empty_string = "";
			sendHttpResponse(info, info.status_msg.c_str(), empty_string, NULL);
		}
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
	//std::cout << "Response: " << response << std::endl; //uncomment this to see full raw response from server
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
	if (path == "/register" || path == "/login" || path == "/upload")
		return true;
	if (!access(full_path.c_str(), F_OK))
		return true;
	else return false;
}

void HttpServer::closeConnection(int fd)
{
	if (_debug)
		logMessage(DEBUG, "Connection closed", &_client_info[fd], 0);
	_client_info.erase(fd);
	close(fd);
	FD_CLR(fd, &_cur_sockets);
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
void HttpServer::uploadFile(ClientInfo &info, std::string data, const char *path)
{
	size_t start = data.find("filename=\"") + 10;
	size_t end = data.find("\"", start);
	std::string filename = data.substr(start, end - start);
	std::string fullPath = path + std::string("/") + filename;

	start = data.find("\r\n\r\n") + 4;
	end = data.find(info.info.boundary, start) - 4;	//-2: "--" +(-2): "\r\n" = 4
	std::string binary_data = data.substr(start, end - start);

	// todo if name exist send error
	std::fstream output(fullPath.c_str(), std::ios::binary | std::ios::out);
	
	if (!output.is_open())
		return;
	output.write(binary_data.data(), binary_data.size());
	output.close();
	executeResponse(info);
}

void serveImage(int client_fd, const std::string& filePath)
{
	std::ifstream image(filePath.c_str(), std::ios::binary);
	if (!image.is_open())
	{
		//send 404
		return;
	}
	std::ostringstream oss;
	oss << image.rdbuf();
	std::string data = oss.str();
	oss.str("");
	oss.clear();
	oss << data.size();
	std::string response = "HTTP/1.1 200 OK\r\n";
	response += "Content-Type: image/png\r\n";
	response += "Content-Length: " + oss.str() + "\r\n";
	response += "Connection: close\r\n";
	response += "\r\n";

	send(client_fd, response.c_str(), response.size(), 0);
	send(client_fd, data.c_str(), data.size(), 0);
	image.close();
}