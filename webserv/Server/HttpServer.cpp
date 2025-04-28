# include "HttpServer.hpp"

HttpServer::HttpServer()
{}

HttpServer::HttpServer(Config *conf, short port, std::string ip, int backlog, bool debug) : Socket(port, ip) 
{

	//init serverStruct
	_conf = conf;

	_debug = debug;
	_backlog = backlog;

	_client_info[_socket_fd].fd = _socket_fd; //first entry in _client_info map is server socked with addr saved to clean up
	_client_info[_socket_fd].addr = _socket_addr;

	if (bind(_socket_fd, (sockaddr *)&_socket_addr, sizeof(_socket_addr)) < 0)
		errorHandler("Bind");
	if (listen(_socket_fd, _backlog) < 0)
		errorHandler("Listen");
	logMessage(SUCCESS, "Server successfully set up and listening for incoming connections.", NULL, 0);
}

HttpServer::~HttpServer()
{

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

bool HttpServer::handleRequest(int client_fd)
{
	ClientInfo &info = _client_info[client_fd];
	info.fd = client_fd;
	info.close_connection = false;

	readRequest(info);
	if (info.file_uploaded)
	{
		return true;
	}
	if (!info.close_connection)
		executeResponse(info);
	if (info.close_connection)
	{
		if (_debug)
			logMessage(DEBUG, "Client request failed.", &info, 2);
		handleErrorResponse(info);
		return false;
	}
	// info.reset();
	//_client_info.erase(client_fd);
	return true;
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
			if (body_size > _conf->getClientMaxBodySize())
			{
				setStatus(info, 413), info.close_connection = true;
				return ;
			}
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
		setStatus(info, 405), info.close_connection = true;
		return;
	}
	req = req.substr(i+1, req.size());
	i = req.find(' ');
	std::string path = req.substr(0, i);
	
	if (method != "DELETE" && !pathExists(path))
	{
		setStatus(info, 404), info.close_connection = true;
		return ;
	}
	req = req.substr(i+1, req.size());
	i = req.find('\r');
	std::string http_version = req.substr(0, i);
	if (http_version != "HTTP/1.1")
	{
		setStatus(info, 505), info.close_connection = true;
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
	sendHttpResponse(info, NULL, "text/html", body);
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
//POST: email already exists send 409 Conflict, else save credentials in csv and send 303 redirect to login
//PUT: updates a password
//DELETE: deletes an account if user is logged in

void HttpServer::executeResponse(ClientInfo &info)
{
	Methods meth = info.info.method;

	if (meth == GET)
	{
		setStatus(info, 200);
		std::string body = extractBody(info);
		sendHttpResponse(info, NULL, "text/html", body);
	}
	else if (meth == POST)
	{
		if (info.info.path == "/register")
		{
			if (emailExists(info))
			{
				setStatus(info, 409), info.close_connection = true;
				return ;
			}
			setStatus(info, 303);
			std::string empty_string = "";
			sendHttpResponse(info, "/login", NULL, empty_string);
			
			storeCredential(info.info.body, "var/www/data/users.csv");
		}
		else if (info.info.path == "/login")
		{
			if (!emailExists(info) || !passwordCorrect(info.info.body))
			{
				setStatus(info, 401), info.close_connection = true;
				return ;
			}
			setStatus(info, 303);
			std::string empty_string = "";
			sendHttpResponse(info, "/user", NULL, empty_string);
		}
		else if (info.info.path == "/upload")
		{
			setStatus(info, 201);
			std::string empty_string = "";
			sendHttpResponse(info, NULL, NULL, empty_string);
		}
	}
	else if (meth == DELETE)
	{
		if (info.info.path.find("/delete_email") == std::string::npos)
		{
			setStatus(info, 404), info.close_connection = true;
			return ;
		}
		if (deleteEmail(info.info.path, "var/www/data/users.csv") == false)
		{
			setStatus(info, 400);
			std::string error_msg = "Invalid email";
			sendHttpResponse(info, NULL, "text/plain", error_msg);
			return;
		}
		setStatus(info, 200);
		std::string empty_string = "";
		sendHttpResponse(info, NULL, NULL, empty_string);
	}
	if (_debug)
		logMessage(DEBUG, "Client", &info, 2);
}

//todo error function for send
//constructs and sends HttpResponse
void HttpServer::sendHttpResponse(ClientInfo &info, const char *location, const char *content_type, std::string &body)
{
	std::ostringstream oss;
	oss << info.status_code;
	std::string status_code_str = oss.str();
	oss.str("");
	oss.clear();
	std::string response = "HTTP/1.1 " + status_code_str + " " + getStatusMessage(info.status_code) + "\r\n";
	if (content_type != NULL)
	{
		oss << body.size();
		std::string body_len = oss.str();
		response += "Content-Type: " + std::string(content_type) + "\r\n";
		response += "Content-Length: " + body_len + "\r\n";
	}
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

//reads a file, parses it into a string and returns it
std::string HttpServer::parseFileToString(const char *filename)
{
	std::string str;

	int fd = open(filename, O_RDONLY);
	if (fd < 0)
		std::runtime_error("Open");
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
	std::string full_path(_conf->getRoot());

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
}
//examples of path requests:
//ex: / -> root/index.html
//ex: /register -> root/register/index.html
//ex: /register/index.html -> root/register/index.html
std::string HttpServer::extractBody(ClientInfo& info)
{
	std::string fullpath(_conf->getRoot());

	fullpath += info.info.path;
	if (isDirectory(fullpath))
	{
		if (info.info.path == "/")
			fullpath += _conf->getIndex();
		else
			fullpath += "/" + _conf->getIndex();
	}
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

bool HttpServer::deleteEmail(std::string &path, const char *filePath)
{
	std::fstream ffile(filePath, std::ios::out | std::ios::in);
	std::string line;
	std::ostringstream oss_buff;
	bool found = false;
	
	int i = path.find("email=") + 6;
	std::string todel = path.substr(i, path.size());
	while (getline(ffile, line))
	{
		int idx = line.find(',');
		std::string email = line.substr(0, idx);
		if (email == todel)
			found = true;
		else
			oss_buff << line << std::endl;
	}
	if (!found)
	{
		ffile.close();
		return false;
	}
	ffile.close();
	std::ofstream ofile(filePath, std::ios::trunc);
	ofile << oss_buff.str();
	ofile.close();
	return true;
}
//serving images currently not used

// void HttpServer::serveImage(int client_fd, const std::string& filePath)
// {
// 	std::ifstream image(filePath.c_str(), std::ios::binary);
// 	if (!image.is_open())
// 		return;
// 	std::ostringstream oss;
// 	oss << image.rdbuf();
// 	std::string data = oss.str();
// 	oss.str("");
// 	oss.clear();
// 	oss << data.size();
// 	std::string response = "HTTP/1.1 200 OK\r\n";
// 	response += "Content-Type: image/png\r\n";
// 	response += "Content-Length: " + oss.str() + "\r\n";
// 	response += "Connection: close\r\n";
// 	response += "\r\n";

// 	send(client_fd, response.c_str(), response.size(), 0);
// 	send(client_fd, data.c_str(), data.size(), 0);
// 	image.close();
// }

int HttpServer::getSocket()
{
	return _socket_fd;
}
