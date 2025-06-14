# include "HttpServer.hpp"

HttpServer::HttpServer()
{}

HttpServer::HttpServer(Config *conf, short port, std::string ip, int backlog, bool debug) : Socket(port, ip) 
{
	std::ostringstream	oss;
	std::string			msg;

	_conf = conf;
	_debug = debug;
	_backlog = backlog;
	_client_info[_socket_fd].fd = _socket_fd;
	_client_info[_socket_fd].addr = _socket_addr;
	if (bind(_socket_fd, (sockaddr *)&_socket_addr, sizeof(_socket_addr)) < 0)
		errorHandler("Bind");
	if (listen(_socket_fd, _backlog) < 0)
		errorHandler("Listen");
	oss << port;
	msg = "Server successfully set up and listens on port " + oss.str() + ".";
	logMessage(SUCCESS, _socket_fd ,msg, NULL, 0);
}

HttpServer::~HttpServer()
{}

int		HttpServer::acceptConnections()
{
	sockaddr_in		client_addr;
	size_t			addr_len = sizeof(client_addr);
	int				client_fd;

	if ((client_fd = accept(_socket_fd, (struct sockaddr *)&client_addr, (socklen_t *)&addr_len)) < 0)
	{
		logMessage(ERROR, _socket_fd, "Error accepting client connection. Continuing...", NULL, 0);
		return (-1);
	}
	_client_info[client_fd].addr = client_addr;
	_client_info[client_fd].fd = client_fd;
	logMessage(INFO, _socket_fd, "New client connected.", &_client_info[client_fd], 0);
	return (client_fd);
}

//parse request to methods, based on method execute response or handle error
HttpResponse	HttpServer::handleRequest(int client_fd)
{
	ClientInfo		&info = _client_info[client_fd];
	HttpResponse	res(info);

	info.close_connection = false;
	info.run_cgi = false;
	info.fd = client_fd;
	readRequest(info, res);
	if (res.getError() == 2)
		return (res);
	if (!info.close_connection && !info.run_cgi)
		executeResponse(info, res);
	else if (!info.close_connection && info.run_cgi)
		return (res);
	if (info.close_connection)
	{
		if (_debug)
			logMessage(DEBUG, _socket_fd,"Failed ", &info, 1);
		res.setError(1);
		buildErrorResponse(info, res);
	}
	else
		res.setError(0);
	return (res);
}

//reads request and parses request to map<int client_fd, struct ClientInfo>
void	HttpServer::readRequest(ClientInfo &info, HttpResponse &res)
{
	char	buffer[1024];
	int		bytes_read = 0;
	size_t	total_read = 0;

	info.reset();
	while (true)
	{
		bytes_read = recv(info.fd, buffer, sizeof(buffer) - 1, 0);
		if (bytes_read < 0)
		{
			res.setStatus(500);
			info.close_connection = true;
			return ;
		}
		if (bytes_read == 0)
		{
			res.setError(2);
			return;
		}
		buffer[bytes_read] = '\0';
		info.info.request.append(buffer, bytes_read);
		if (info.info.request.find("\r\n\r\n") != std::string::npos)
		{
			parseRequestLine(info, res);
			if (info.close_connection)
				return ;
			info.info.body.body_size = parseRequestHeader(info, res);
			if (!info.close_connection && !checkBodySize(info))
			{
				res.setStatus(413), info.close_connection = true;
				return ;
			}
			if (info.close_connection)
				return ;
			total_read = info.info.request.size() - 2;
			break;
		}
	}
	if (info.info.request.size() > 1)
		info.info.body.body_str = info.info.request.substr(2);
	while (total_read < info.info.body.body_size)
	{
		bytes_read = recv(info.fd, buffer, sizeof(buffer) - 1, 0);
		if (bytes_read < 0)
		{
			res.setStatus(500);
			info.close_connection = true;
			return ;
		}
		if (bytes_read == 0)
			break;
		buffer[bytes_read] = '\0';
		info.info.body.body_str.append(buffer, bytes_read);
		total_read += bytes_read;
	}
	if (info.info.body.body_size != 0 && !info.run_cgi)
		parseRequestBody(info, res);
}

//parses method, path, version, and checks for errors
void	HttpServer::parseRequestLine(ClientInfo& info, HttpResponse &res)
{
	std::string	&req = info.info.request;
	std::string	method, path, http_version;

	if (!safeExtract(req, ' ', method))
	{
		res.setStatus(400), info.close_connection = true;
		return;
	}
	if (method != "GET" && method != "POST" && method != "DELETE")
	{
		res.setStatus(405), info.close_connection = true;
		return;
	}
	if (!safeExtract(req, ' ', path))
	{
		res.setStatus(400);
		info.close_connection = true;
		return;
	}
	int error = checkPath(info, path, method);
	_client_info[info.fd].info.path = path;
	if (error == ERROR_PATH_404)
	{
		res.setStatus(404), info.close_connection = true;
		return ;
	}
	else if (error == ERROR_METHOD_405)
	{
		res.setStatus(405), info.close_connection = true;
		return ;
	}
	if (!safeExtract(req, '\r', http_version))
	{
		res.setStatus(400);
		info.close_connection = true;
		return;
	}
	if (http_version != "HTTP/1.1")
	{
		res.setStatus(505), info.close_connection = true;
		return ;
	}
	_client_info[info.fd].info.method = getEnumMethod(method);
	_client_info[info.fd].info.http_version = http_version;
	if (req.size() > 0 && req[0] == '\n')
		req = req.substr(1);
}

void	HttpServer::parseRequestBody(ClientInfo& info, HttpResponse &res)
{
	std::string	&body_ref = info.info.body.body_str;

	if (info.info.headers["Content-Type"] == "application/x-www-form-urlencoded")
	{
		int start = body_ref.find("email=") + 6;
		int end = body_ref.find('&') - start;
		info.info.body.email = body_ref.substr(start, end);
		info.info.body.passw = body_ref.substr(body_ref.find("password=") + 9);
	}
	else if (info.info.headers["Content-Type"].find("multipart/form-data") != std::string::npos)
		info.file_uploaded = true;
	else
		res.setStatus(415), info.close_connection = true;
}

void HttpServer::buildErrorResponse(ClientInfo &info, HttpResponse &res)
{
	std::string	path;
	std::string	test_page = _conf->getErrorPage(info.status_code);
	std::string	body;

	if (test_page != "")
	{
		if (_conf->getRoot()[_conf->getRoot().size() -1] == '/' && test_page[0] == '/')
			path = _conf->getRoot() + test_page.substr(1);
		else if (_conf->getRoot()[_conf->getRoot().size() -1] != '/' && test_page[0] != '/')
			path = _conf->getRoot() + "/" + test_page;
		else
			path = _conf->getRoot() + test_page;
	}
	else
	{
		std::ostringstream oss;
		oss << info.status_code;
		path = _conf->getRoot() + "/error_pages/";
		path += oss.str();
		path += ".html";
	}
	body = parseFileToString(path.c_str());
	if (body == "" && info.status_code < 400)
		;
	else if (body == "")
	{
		res.setBody(getFallBackBody(info.status_code));
		res.setContentType("text/html");
	}
	else
	{
		res.setBody(body);
		res.setContentType("text/html");
	}
	res.setConnection("close");
}

//parses the Client-Request-Header to a map<string, string> and returns content-length
size_t	HttpServer::parseRequestHeader(ClientInfo& info, HttpResponse &res)
{
	std::string &req = info.info.request;

	while (true)
	{
		if (req[0] == '\r')
			break;
		std::string key = req.substr(0, req.find(':'));
		req = req.substr(req.find(':') + 2);
		std::string val = req.substr(0, req.find('\r'));
		req = req.substr(req.find('\r') + 2);
		if (key.empty() == true || val.empty() == true)
		{
			res.setStatus(400), info.close_connection = true;
			return (0);
		}
		info.info.headers[key] = val;
	}
	if (info.info.headers["Content-Type"].find("multipart/form-data") != std::string::npos)
	{
		size_t pos = info.info.headers["Content-Type"].find("boundary=") + 9;
		info.info.boundary = info.info.headers["Content-Type"].substr(pos);
	}
	if (info.info.headers.find("Content-Length") != info.info.headers.end())
		return (std::atoi(info.info.headers["Content-Length"].c_str()));
	else
		return (0);
}

//Depending on the method, the server sends a response back
void	HttpServer::executeResponse(ClientInfo &info, HttpResponse &res)
{
	if (!info.redirect_location.empty())
	{
		res.setStatus(info.status_code);
		res.setLocation(info.redirect_location);
		res.setConnection("keep-alive");
		if (_debug)
			logMessage(DEBUG, _socket_fd, "", &info, 1);
		return;
	}
	switch (info.info.method)
	{
		case GET:
		{
			if (!handleGet(info, res))
				return ;
			break;	
		}
		case POST:
		{
			if (!handlePost(info, res))
				return ;
			break;
		}
		case DELETE:
		{
			if (!handleDelete(info, res))
				return ;
			break;
		}
		default:
		{
			res.setStatus(405);
			info.close_connection = true;
			break;
		}
	}
	res.setConnection("keep-alive");
	if (_debug)
		logMessage(DEBUG, _socket_fd, "", &info, 1);
}

bool HttpServer::handleDelete(ClientInfo &info, HttpResponse &res)
{
	if (info.info.path.find("/delete_email") == std::string::npos)
	{
		res.setStatus(404), info.close_connection = true;
		return (false);
	}
	if (deleteEmail(info, "var/www/data/users.csv") == false)
	{
		res.setStatus(400);
		res.setBody("Invalid email");
		res.setContentType("text/plain");
		return (false);
	}
	res.setStatus(200);
	res.setBody("Account successfully deleted");
	res.setContentType("text/plain");
	return (true);
}

bool HttpServer::handleGet(ClientInfo &info, HttpResponse &res)
{
	res.setStatus(200);
	if (info.dir_listening)
	{
		res.setContentType("text/html");
		res.setBody(constructBodyForDirList(info));
	}
	else
	{
		std::string	body = parseFileToString(info.info.absolute_path.c_str());
		if (body == "")
		{
			res.setStatus(404), info.close_connection = true;
			return (false);
		}
		res.setContentType(retrieveContentType(info));
		res.setBody(body);
	}
	return (true);
}

bool HttpServer::handlePost(ClientInfo &info, HttpResponse &res)
{
	res.setStatus(303);
	if (info.info.path == "/register")
	{
		if (emailExists(info))
		{
			res.setStatus(409), info.close_connection = true;
			return (false);
		}
		res.setLocation("/login.html");
		storeCredential(info.info.body, "var/www/data/users.csv");
	}
	else if (info.info.path == "/login")
	{
		if (!emailExists(info) || !passwordCorrect(info.info.body))
		{
			res.setStatus(401), info.close_connection = true;
			return (false);
		}
		res.setLocation("/user.html");
	}
	else if (info.info.path == "/upload" && info.file_uploaded == true)
	{
		if (uploadFile(info, "var/www/data/images/"))
		{
			res.setStatus(500), info.close_connection = true;
			return (false);
		}
		res.setStatus(201);
	}
	return (true);
}

//reads a file, parses it into a string and returns it
std::string	HttpServer::parseFileToString(const char *filename)
{
	std::string str;

	int fd = open(filename, O_RDONLY);
	if (fd < 0)
		return "";
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

int		HttpServer::checkPath(ClientInfo &info, std::string &path, std::string &method)
{
	std::string	full_path;
	bool		autoindex_enabled = false;
	bool		is_dir = false;

	//handle path based on query
	if (path.find("?") != std::string::npos)
	{
		info.info.query = path.substr(path.find("?"));
		path = path.substr(0, path.find("?"));
	}
	//handle redirect location
	Location *loc = _conf->getLocation(path);
	if (loc && !loc->getReturnValue().empty())
	{
		std::istringstream iss(loc->getReturnValue());
		iss >> info.status_code >> info.redirect_location;
		return (true);
	}
	//handle location
	if (loc)
	{
		if (!loc->getRoot().empty())
			full_path = loc->getRoot();
		else
			full_path = _conf->getRoot();
		
		if (loc->hasAutoindex())
			autoindex_enabled = loc->getAutoindex();
		else
			autoindex_enabled = _conf->getAutoindex();
		if (!loc->getIndex().empty())
				autoindex_enabled = false;
		is_dir = isDirectory(full_path);
		if (is_dir && !autoindex_enabled)
		{
			if (!loc->getIndex().empty())
				full_path = proper_path_cat(full_path, loc->getIndex());
			else
				full_path = proper_path_cat(full_path, _conf->getIndex());
		}
		if (!loc->isMethodAllowed(method))
			return (ERROR_METHOD_405);
	}
	else
	{
		full_path = proper_path_cat(_conf->getRoot(), path);
		is_dir = isDirectory(full_path);
		if (is_dir)
			full_path = proper_path_cat(full_path, _conf->getIndex());
		autoindex_enabled = _conf->getAutoindex();
		if (!_conf->getIndex().empty())
				autoindex_enabled = false;
	}
	if (loc && (method == "POST" || method == "DELETE") && full_path.find(".py") == std::string::npos)
		return (true);
	if (!access(full_path.c_str(), F_OK))
	{
		//check for cgi script
		if (full_path.find(".py") != std::string::npos)
		{
			info.info.absolute_path = full_path;
			info.run_cgi = true;
			return (true);
		}
		//check for dir listening
		if ((loc && loc->getIndex().empty() && autoindex_enabled && is_dir)
			|| (autoindex_enabled && is_dir && _conf->getIndex().empty()))
				info.dir_listening = true;
		info.info.absolute_path = full_path;
		return (true);
	}
	else
		return (ERROR_PATH_404);
}



int		HttpServer::uploadFile(ClientInfo &info, const char *path)
{
	std::string		data = info.info.body.body_str; 
	std::string		filename = buildUploadFilename(data, path);
	std::string		fullPath = path + std::string("/") + filename;
	size_t			start = data.find("\r\n\r\n") + 4;
	size_t			end = data.find(info.info.boundary, start) - 4;	//-2: "--" +(-2): "\r\n" = 4
	std::string		binary_data = data.substr(start, end - start);
	std::fstream	output(fullPath.c_str(), std::ios::binary | std::ios::out);

	if (!output.is_open())
		return 1;
	output.write(binary_data.data(), binary_data.size());
	output.close();
	return 0;
}

bool HttpServer::deleteEmail(ClientInfo &info, const char *filePath)
{
	std::fstream		ffile(filePath, std::ios::out | std::ios::in);
	std::string			line;
	std::ostringstream 	oss_buff;
	bool				found = false;
	int					i = info.info.query.find("?email=") + 7;
	std::string			todel = info.info.query.substr(i);

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
		return (false);
	}
	ffile.close();
	std::ofstream ofile(filePath, std::ios::trunc);
	ofile << oss_buff.str();
	ofile.close();
	return (true);
}

ClientInfo	&HttpServer::getClientInfoElem(int fd)
{
	return (_client_info[fd]);
}

int HttpServer::getSocket()
{
	return (_socket_fd);
}

bool	HttpServer::checkBodySize(ClientInfo &info)
{
	Location	*loc = _conf->getLocation(info.info.path);
	size_t		size;

	if (loc && loc->getClientMaxBodySize() != 0)
			size = loc->getClientMaxBodySize();
	else if (_conf->getClientMaxBodySize() != 0)
		size = _conf->getClientMaxBodySize();
	else
		size = MAX_CONTENT_LENGTH;
	if (info.info.body.body_size > size) 
		return (false);
	return (true);
}

std::string	HttpServer::constructBodyForDirList(ClientInfo &info)
{
	std::string		name;
	struct dirent	*entry;
	std::string		body;
	std::string		directory = info.info.path;
	DIR				*dir;
	Location		*loc = _conf->getLocation(info.info.path);
	std::string		path;

	if (loc && !loc->getRoot().empty())
		path = loc->getRoot() + "/";
	else
		path = info.info.absolute_path;
	if (info.info.path != "/")
		directory += "/";
	body += "<!DOCTYPE html>\n";
	body += "<html>\n<head>\n<title>Dir Listening</title>\n</head>\n<body>\n";
	body += "<h1>Directory Listing</h1>\n";
	body += "<ul>\n";
	dir = opendir(path.c_str());
	if (!dir)
		return (body + "</ul></body></html>");
	while ((entry = readdir(dir)) != NULL)
	{
		name = entry->d_name;
		if (name == "." || name == "..")
			continue;
		body += "<li><a href=\"" + directory + name + "\">" + name + "</a></li>";
	}
	closedir(dir);
	body += "</ul></body></html>";
	return (body);
}

void	HttpServer::eraseClientFromMap(int fd)
{
	_client_info.erase(fd);
}
std::string	HttpServer::getFallBackBody(int err)
{
	std::string			msg = getStatusMessage(err);
	std::ostringstream	oss;
	std::string			code;
	std::string			body;

	oss << err;
	code = oss.str();
	body = "<html><body><h1>";
	body += code + " ";
	body += msg;
	body += "</h1></body></html>";
	return (body);
}