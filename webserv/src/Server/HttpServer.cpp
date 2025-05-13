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
	logMessage(SUCCESS, _socket_fd ,"Server successfully set up and listening for incoming connections.", NULL, 0);
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
		logMessage(ERROR, _socket_fd, "Error accepting client connection. Continuing...", NULL, 0);
		return -1;
	}
	_client_info[client_fd].addr = client_addr;
	_client_info[client_fd].fd = client_fd;

	logMessage(INFO, _socket_fd, "New client connected.", &_client_info[client_fd], 0);
	return client_fd;
}

//parse request to methods, based on method execute response or handle error

bool HttpServer::handleRequest(int client_fd)
{
	ClientInfo &info = _client_info[client_fd];
	info.close_connection = false;
	info.run_cgi = false;

	readRequest(info);
	if (!info.close_connection)
		executeResponse(info);
	if (info.close_connection)
	{
		if (_debug)
			logMessage(DEBUG, _socket_fd,"Client request failed.", &info, 1);
		handleErrorResponse(info);
		close(client_fd);
		if (_debug)
			logMessage(DEBUG,  _socket_fd, "Connection closed", &_client_info[client_fd], 0);
		_client_info.erase(client_fd);
		return false;
	}
	_client_info[client_fd].reset();
	return true;
}

//reads request and parses request to map<int client_fd, struct ClientInfo>
//todo error function for recv
void HttpServer::readRequest(ClientInfo &info)
{
	char buffer[1024];
	int bytes_read = 0;
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
			info.info.body.body_size = parseRequestHeader(info);
			if (!checkBodySize(info))
			{
				setStatus(info, 413), info.close_connection = true;
				return ;
			}
			if (info.close_connection)
				return ;
			total_read = info.info.request.size() - 2;
			break;
		}
	}
	if (total_read < info.info.body.body_size)
		info.info.body.body_str = info.info.request.substr(2, info.info.request.size());
	while (total_read < info.info.body.body_size)
	{
		bytes_read = recv(info.fd, buffer, sizeof(buffer) - 1, 0);
		if (bytes_read < 0)
			throw std::runtime_error("Error: recv");
		if (bytes_read == 0)
			break;
		buffer[bytes_read] = '\0';
		info.info.body.body_str.append(buffer, bytes_read);
		total_read += bytes_read;
	}
	if (info.info.body.body_size != 0)
		parseRequestBody(info);
}

//parses method, path, version, and checks for errors
void HttpServer::parseRequestLine(ClientInfo& info)
{
	std::string &req = info.info.request;

	int i = req.find(' ');
	std::string method = req.substr(0, i);
	if (method != "GET" && method != "POST" && method != "DELETE")
	{
		setStatus(info, 405), info.close_connection = true;
		return;
	}
	req = req.substr(i+1, req.size());
	i = req.find(' ');
	std::string path = req.substr(0, i);
	if (checkPath(info, path) == false)
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

void HttpServer::parseRequestBody(ClientInfo& info)
{
	std::string &req = info.info.request;
	std::string &body_ref = info.info.body.body_str;

	body_ref = req.substr(2, req.size());
	std::cout << "info.info.headers[\"Content-Type\"] :" << info.info.headers["Content-Type"] << std::endl;
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

//if an Http error occurs an appropriate Response is send and the connection is closed
//also generic based on error pages or loc (talk)
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
		
		if (info.run_cgi)
		{
			//CGI cgi(info);
		}
		else
		{
			setStatus(info, 200);
			std::string body = parseFileToString(info.info.absolute_path.c_str());
			sendHttpResponse(info, NULL, "text/html", body);
		}
		
	}
	else if (meth == POST)
	{
		if (info.run_cgi)
		{
			//CGI cgi(info);
		}

		if (info.info.path == "/register")
		{
			if (emailExists(info))
			{
				setStatus(info, 409), info.close_connection = true;
				return ;
			}
			setStatus(info, 303);
			std::string empty_string = "";
			storeCredential(info.info.body, "var/www/data/users.csv");
			sendHttpResponse(info, "/login.html", NULL, empty_string);
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
			sendHttpResponse(info, "/user.html", NULL, empty_string);
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
		logMessage(DEBUG, _socket_fd, "", &info, 1);
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
	oss << body.size();
	std::string body_len = oss.str();
	std::string response = "HTTP/1.1 " + status_code_str + " " + getStatusMessage(info.status_code) + "\r\n";
	if (content_type != NULL)
		response += "Content-Type: " + std::string(content_type) + "\r\n";
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

//more generic attempt
bool HttpServer::checkPath(ClientInfo &info, std::string &path)
{
	std::string full_path;
	bool flag = false;

	//handle built-in functionalities, register, upload, login -> webserver handles this
	std::cout << "path: " << path << std::endl;
	
	if ((path == "/register" || path == "/upload" || path == "/login") && _conf->getLocation(path) != NULL)
		return 1;
	//filter out unnecessary requests
	if (path == "/.well-known/appspecific/com.chrome.devtools.json" || path.find("favicon") != std::string::npos)
		return 0;
	//handle path based on query
	if (path.find("?") != std::string::npos)
	{
		info.info.query = path.substr(path.find("?"), path.size());
		path = path.substr(0, path.find("?"));
	}
	Location *loc = _conf->getLocation(path);
	if (loc != NULL)
	{
		if (loc->getReturnValue() != "")
		{
			std::istringstream iss(loc->getReturnValue());
			std::string test_path;
			iss >> info.status_code >> test_path;
			Location *test_loc = _conf->getLocation(test_path);
			if (test_loc != NULL)
				loc = test_loc;
			else
				flag = true;
		}
		if (!flag)
		{
			if (loc->getRoot() != "")
				full_path = loc->getRoot();
			else
				full_path = _conf->getRoot();
			if (isDirectory(full_path) && loc->getIndex() != "")
			{
				if (full_path[full_path.size() -1] == '/')
					full_path += loc->getIndex();
				else
					full_path += "/" + loc->getIndex();
			}
			//if (loc->getCGIExt() != "")
			// {
			// 	//check if ext is valid
			// }
		}
	}
	else
	{
		full_path = _conf->getRoot();
		full_path += path;
		if (isDirectory(full_path))
		{
			if (full_path[full_path.size() -1] == '/')
				full_path += _conf->getIndex();
			else
				full_path += "/" + _conf->getIndex();
		}
	}
	std::cout << full_path << std::endl;
	if (!access(full_path.c_str(), F_OK))
	{
		info.info.absolute_path = full_path;
		return 1;
	}
	else return 0;
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

ClientInfo &HttpServer::getClientInfoElem(int fd)
{
	return (_client_info[fd]);
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

bool HttpServer::checkBodySize(ClientInfo &info)
{
	Location *loc = _conf->getLocation(info.info.path);
	size_t size;
	if (loc)
		 size = loc->getClientMaxBodySize();
	else
		size = _conf->getClientMaxBodySize();
	if (info.info.body.body_size > size) 
		return false;
	return true;
}
