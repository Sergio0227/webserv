#include "webserv.hpp"


void	printClientInfo(ClientInfo &info)
{
	std::cout << "--------Parsing-Request-----------------"<< std::endl;
	std::cout << "Client FD: " << info.fd << std::endl;
	std::cout << "Method: " << info.info.method << std::endl;
	std::cout << "Path: " << info.info.path << std::endl;
	std::cout << "HTTP Version: " << info.info.http_version << std::endl;

	struct in_addr ip_addr;
	ip_addr.s_addr = info.addr.sin_addr.s_addr;
	std::string ip_str = inet_ntoa(ip_addr);
	std::cout << "IP: " << ip_str << std::endl;
	uint16_t port = ntohs(info.addr.sin_port);
	std::cout << "Port: " << port << std::endl;
	if (!info.info.headers.empty())
	{
		std::cout << "Headers:" << std::endl;
		std::map<std::string, std::string>::iterator header_it;
		for (header_it = info.info.headers.begin(); header_it != info.info.headers.end(); ++header_it)
			std::cout << "  " << header_it->first << ": " << header_it->second << std::endl;
	}
	else
		std::cout << "No headers found." << std::endl;
	if (!info.info.body.email.empty())
	{
		std::cout << "Body email: " << info.info.body.email << std::endl;
		std::cout << "Body passw: " << info.info.body.passw << std::endl;
	}
	std::cout << "----------------------------------------" << std::endl;
}

Methods getEnumMethod(std::string &method)
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

std::string getStringMethod(Methods method)
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

std::string getCurrentTime()
{
	time_t t;
	struct tm* localTime;
	std::stringstream ss;

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

void logMessage(LogType level, const std::string& message, ClientInfo *ptr_info, int flag)
{
	std::string category, color;
	std::pair<std::string, std::string> pair = getPairLog(level);
	category = pair.first;
	color = pair.second;
	while (category.size() != 14)
		category.append(" ");
	if (ptr_info == NULL)
		std::cout << color << category << RESET << getCurrentTime() << "\t-\t" << message << std::endl;
	else
	{
		if (flag == 0)
		{
			std::cout << color << category << RESET << getCurrentTime()
				<< "\t-\t" << message << " -> "
				<< "(IP: " << inet_ntoa(ptr_info->addr.sin_addr) << " | "
				<< "PORT: " << ntohs(ptr_info->addr.sin_port) << ")"
				<< std::endl;
		}
		else if (flag == 1)
		{
			std::cout << color << category << RESET << getCurrentTime()
				<< "\t-\t" << message << " -> "
				<< "(IP: " << inet_ntoa(ptr_info->addr.sin_addr) << " | "
				<< "PORT: " << ntohs(ptr_info->addr.sin_port) << ")"
				<< "{ Method: " << ptr_info->info.method << " | Path: " << ptr_info->info.path << " }" << std::endl;
		}
		else if (flag == 2)
		{
			std::cout << color << category << RESET << getCurrentTime()
				<< "\t-\t" << message << " -> "
				<< "(IP: " << inet_ntoa(ptr_info->addr.sin_addr) << " | "
				<< "PORT: " << ntohs(ptr_info->addr.sin_port) << ")  |  "
				<< "Request -> (Method: " << getStringMethod(ptr_info->info.method) << " | Path: " << ptr_info->info.path << ")"
				<< "  |  Server: (Code: " << ptr_info->status_code << " | Msg: " << ptr_info->status_msg << ")" << std::endl;
		}
	}
}

std::pair<std::string, std::string> getPairLog(LogType level)
{
	std::map<LogType, std::pair<std::string, std::string> > log_map;
	log_map[INFO] = std::make_pair("[INFO]", BLUE);
	log_map[DEBUG] = std::make_pair("[DEBUG]", ORANGE);
	log_map[ERROR] = std::make_pair("[ERROR]", RED);
	log_map[SUCCESS] = std::make_pair("[SUCCESS]", GREEN);
	return log_map[level];
}

std::string getStatusMessage(int code)
{
	std::map<int, std::string> status_map;
	status_map[200] = "OK";
	status_map[201] = "Created";
	status_map[204] = "No Content";
	status_map[303] = "See Other";
	status_map[400] = "Bad Request";
	status_map[401] = "Unauthorized";
	status_map[404] = "Not Found";
	status_map[405] = "Method Not Allowed";
	status_map[409] = "Conflict";
	status_map[415] = "Unsupported Media Type";
	status_map[505] = "HTTP Version Not Supported";
	
    if (status_map.find(code) != status_map.end())
		return status_map[code];
	else
		return "Unknown Error";
}

bool emailExists(ClientInfo &info)
{
	std::fstream file("var/www/data/users.csv", std::ios::in);
	std::string line;

	if (!file.is_open())
		return false;
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
	return false;
}

bool passwordCorrect(BodyInfo &body)
{
	std::fstream file("var/www/data/users.csv", std::ios::in);
	std::string line;

	if (!file.is_open())
		return false;
	while (getline(file, line))
	{
		std::string csv_passw = line.substr(line.find(',') + 1);
		std::string csv_email = line.substr(0, line.find(','));
		if (csv_email == body.email)
		{
			if (csv_passw == body.passw)
				return true;
		}
	}
	file.close();
	return false;
}

void storeCredential(BodyInfo &body, const char *name)
{
	std::fstream file(name, std::ios::out | std::ios::app);
	if (!file.is_open())
	{
		std::cerr << "Error: Could not open file for writing\n";
		return ;
	}
	file << body.email << "," << body.passw << "\n";
	file.close();
}

std::string decodeUrl(const std::string &encoded)
{
	std::string decoded;
	size_t j = 0;
	size_t i = 0;
	std::cout << encoded << std::endl;
	while(i < encoded.size())
	{
		if (encoded[i] == '%' && i+2 < encoded.size())
		{
			int int_decode;
			std::stringstream ss;
			ss << std::hex << encoded.substr(i + 1, 2);
			ss >> int_decode;
			decoded[j++] = static_cast<char>(int_decode);
			i += 2;
		}
		else
			decoded[j++] = encoded[i];
		i++;
	}
	return decoded;
}

bool isDirectory(const std::string &path)
{
	struct stat st;
	if (stat(path.c_str(), &st) != 0)
		return false;
	return S_ISDIR(st.st_mode);
}

bool isRegularFile(const std::string& path)
{
	struct stat st;
	if (stat(path.c_str(), &st) != 0)
		return false;
	return S_ISREG(st.st_mode);
}

bool hasReadAccess(const std::string& path)
{
	return access(path.c_str(), R_OK | X_OK) == 0;
}
void BodyInfo::reset()
{
	body_str.clear();
	email.clear();
	passw.clear();
}

void RequestInfo::reset()
{
	request.clear();
	http_version.clear();
	path.clear();
	headers.clear();
	boundary.clear();

	body.reset();
}

void ClientInfo::reset()
{
	status_code = 0;
	status_msg.clear();
	close_connection = false;
	keep_alive = false;
	file_uploaded = false;

	info.reset();
}
void setStatus(ClientInfo &info, int code)
{
	info.status_code = code;
	info.status_msg = getStatusMessage(code);
}