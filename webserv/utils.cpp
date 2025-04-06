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
	if (!info.info.body.empty())
		std::cout << "Body: " << info.info.body << std::endl;
	std::cout << "----------------------------------------" << std::endl;
}

Methods getMethod(std::string &method)
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

void logMessage(LogType level, const std::string& message, ClientInfo *ptr_info)
{
	std::string s;
	std::string c;

	switch (level)
	{
		case INFO:
			s = "[INFO]";
			c = BLUE;
			break;
		case DEBUG:
			s = "[DEBUG]";
			c = ORANGE;
			break;
		case ERROR:
			s = "[ERROR]";
			c = RED;
			break;
		case SUCCESS:
			s = "[SUCCESS]";
			c = GREEN;
			break;
		default:
			break;
	}
	if (c.empty())
		return;
	while (s.size() != 14)
		s.append(" ");
	if (ptr_info == NULL)
		std::cout << c << s << RESET << getCurrentTime() << "\t-\t" << message << std::endl;
	else
	{
		struct in_addr ip_addr;
		ip_addr.s_addr = ptr_info->addr.sin_addr.s_addr;
		std::string ip_str = inet_ntoa(ip_addr);
		uint16_t port = ntohs(ptr_info->addr.sin_port);
		std::cout << c << s << RESET << getCurrentTime() << "\t-\t" << message << " -> " << "(IP: " << ip_str << " | " << "PORT: " << port << ")" << std::endl;
	}
}

