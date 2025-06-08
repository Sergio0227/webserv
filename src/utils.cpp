#include "webserv.hpp"

Methods	getEnumMethod(std::string &method)
{
	if (method == "GET")
		return GET;
	else if (method == "POST")
		return POST;
	else if (method == "DELETE")
		return DELETE;
	else if (method == "PUT")
		return PUT;
	return UNKNOWN_METHOD;
}

std::string	getStringMethod(Methods method)
{
	switch (method)
	{
		case GET:		return "GET";
		case POST:		return "POST";
		case DELETE:	return "DELETE";
		case PUT:		return "PUT";
		default:		return "UNKNOWN_METHOD";
	}
}

std::string	getCurrentTime()
{
	time_t				t;
	struct tm*			localTime;
	std::stringstream	ss;

	time(&t);
	localTime = localtime(&t);
	ss << localTime->tm_year + 1900
		<< "-" << std::setw(2) << std::setfill('0') << localTime->tm_mon + 1
		<< "-" << std::setw(2) << std::setfill('0') << localTime->tm_mday
		<< " " << std::setw(2) << std::setfill('0') << localTime->tm_hour
		<< ":" << std::setw(2) << std::setfill('0') << localTime->tm_min
		<< ":" << std::setw(2) << std::setfill('0') << localTime->tm_sec;
	return (ss.str());
}

void	logMessage(LogType level, int server_fd, const std::string& message, ClientInfo *ptr_info, int flag)
{
	std::ostringstream					oss;
	std::string							category, color, msg, str_fd = "";
	std::pair<std::string, std::string>	pair;

	if (server_fd > 0)
	{
		oss << server_fd;
		str_fd = oss.str();
		oss.clear();
		oss.str("");
	}
	pair = getPairLog(level);
	category = pair.first;
	color = pair.second;
	while (category.size() != 14)
		category.append(" ");
	msg = color + category + RESET;
	msg.append(getCurrentTime());
	msg.append("\t-\t");
	if (server_fd > 0)
	{
		msg.append("Server_ID: ");
		msg.append(str_fd);
		msg.append(" | ");
		if (ptr_info != NULL)
		{
			msg.append("Client_ID: ");
			oss << ptr_info->fd;
			msg.append(oss.str());
			oss.clear();
			oss.str("");
			msg.append(" | ");
		}
	}
	msg.append(message);
	if (ptr_info != NULL)
	{
		if (flag == 1)
		{
			msg.append("Request -> (Method: ");
			msg.append(getStringMethod(ptr_info->info.method));
			msg.append(" | Path: ");
			msg.append(ptr_info->info.path);
			msg.append(")  |  Server: (Code: ");
			oss << ptr_info->status_code;
			msg.append(oss.str());
			msg.append(" | Msg: ");
			msg.append(ptr_info->status_msg);
			msg.append(")");
		}
	}
	std::cout << msg << std::endl;
}

std::pair<std::string, std::string>	getPairLog(LogType level)
{
	static std::map<LogType, std::pair<std::string, std::string> >	log_map;

	if (log_map.empty())
	{
		log_map[INFO] = std::make_pair("[INFO]", BLUE);
		log_map[DEBUG] = std::make_pair("[DEBUG]", ORANGE);
		log_map[ERROR] = std::make_pair("[ERROR]", RED);
		log_map[SUCCESS] = std::make_pair("[SUCCESS]", GREEN);
	}
	return (log_map[level]);
}

std::string getStatusMessage(int code)
{
	static std::map<int, std::string>	status_map;

	if (status_map.empty())
	{
		status_map[200] = "OK";
		status_map[201] = "Created";
		status_map[204] = "No Content";
		status_map[303] = "See Other";
		status_map[400] = "Bad Request";
		status_map[401] = "Unauthorized";
		status_map[404] = "Not Found";
		status_map[405] = "Method Not Allowed";
		status_map[408] = "Request Timeout";
		status_map[409] = "Conflict";
		status_map[413] = "Payload Too Large";
		status_map[415] = "Unsupported Media Type";
		status_map[500] = "Internal Server Error";
		status_map[505] = "HTTP Version Not Supported";
	}
    if (status_map.find(code) != status_map.end())
		return (status_map[code]);
	else
		return ("Unknown Error");
}

std::string	getMimeType(const std::string &ext)
{
	static std::map<std::string, std::string>	mime_map;

	if (mime_map.empty())
	{
		mime_map[".html"] = "text/html";
		mime_map[".css"] = "text/css";
		mime_map[".png"] = "image/png";
		mime_map[".jpg"] = "image/jpeg";
		mime_map[".jpeg"] = "image/jpeg";
		mime_map[".ico"] = "image/x-icon";
	}
    if (mime_map.find(ext) != mime_map.end())
		return (mime_map[ext]);
	else
		return ("application/octet-stream");
}

bool	emailExists(ClientInfo &info)
{
	std::fstream	file("var/www/data/users.csv", std::ios::in);
	std::string		line;

	if (!file.is_open())
		return (false);
	while (getline(file, line))
	{
		std::string csv_email = line.substr(0, line.find(','));
		if (csv_email == info.info.body.email)
		{
			file.close();
			return true;
		}	
	}
	file.close();
	return (false);
}

bool	passwordCorrect(BodyInfo &body)
{
	std::fstream	file("var/www/data/users.csv", std::ios::in);
	std::string		line;

	if (!file.is_open())
		return (false);
	while (getline(file, line))
	{
		std::string csv_passw = line.substr(line.find(',') + 1);
		std::string csv_email = line.substr(0, line.find(','));
		if (csv_email == body.email)
		{
			if (csv_passw == body.passw)
				return (true);
		}
	}
	file.close();
	return (false);
}

void	storeCredential(BodyInfo &body, const char *name)
{
	std::fstream	file(name, std::ios::out | std::ios::app);

	if (!file.is_open())
		throw std::runtime_error("Couldn`t open file for storing credentials.");
	file << body.email << "," << body.passw << "\n";
	file.close();
}

bool	isDirectory(const std::string &path)
{
	struct stat	st;

	if (stat(path.c_str(), &st) != 0)
		return (false);
	return (S_ISDIR(st.st_mode));
}

bool	isRegularFile(const std::string& path)
{
	struct stat	st;

	if (stat(path.c_str(), &st) != 0)
		return false;
	return (S_ISREG(st.st_mode));
}

bool	hasReadAccess(const std::string& path)
{
	return (access(path.c_str(), R_OK | X_OK) == 0);
}

std::string	buildUploadFilename(std::string &data, const char *path)
{
	size_t				start = data.find("filename=\"") + 10;
	size_t				end = data.find("\"", start);
	std::string			filename = data.substr(start, end - start);
	std::ostringstream	oss;
	std::string			ext;
	int					count;

	start = filename.rfind(".");
	end = filename.size();
	ext = filename.substr(start, end - start);
	filename = filename.substr(0, start);
	count = countFilesInsideDir(path);
	if (count == -1)
		return ("");
	oss << count;
	filename += "_" + oss.str() + ext;
	return (filename);
}

int countFilesInsideDir(const char *path)
{
	DIR*			dir = opendir(path);
	struct dirent*	entry;
	int				i = 0;

	if (!dir)
		return -1;
	while ((entry = readdir(dir)) != NULL)
	{
		std::string name = entry->d_name;
		if (name != "." && name != "..")
			i++;
	}
	return (i);
}

bool	safeExtract(std::string& input, char delim, std::string& out)
{
	size_t	pos;
	
	pos = input.find(delim);
	if (pos == std::string::npos)
		return (false);
	out = input.substr(0, pos);
	input = input.substr(pos + 1);
	return (true);
}

std::string	retrieveContentType(ClientInfo &info)
{
	std::string	ext;
	size_t		dot_pos = info.info.path.rfind('.');

	if (dot_pos == std::string::npos)
	{
		dot_pos = info.info.absolute_path.rfind('.');
		if (dot_pos == std::string::npos)
			return "application/octet-stream";
		else
			ext = info.info.absolute_path.substr(dot_pos);
	}
	else
		ext = info.info.path.substr(dot_pos);
	return (getMimeType(ext));
}

void	setNonBlockingFD(int fd)
{
	//save all flags
	int	flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1)
		throw std::runtime_error("fcntl F_GETFL failed");
	//set the fd with NonBlock Flag and saved flags
	if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
		throw std::runtime_error("fcntl F_SETFL failed");
}
