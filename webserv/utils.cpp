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

