#pragma once

#include <string>
#include <map>
#include <ctime>
#include <netinet/in.h>


enum Methods
{
	GET,
	POST,
	DELETE,
	PUT,
	UNKNOWN_METHOD
};

struct BodyInfo
{
	std::string	body_str;
	std::string	email;
	std::string	passw;
	void reset();
};

struct RequestInfo
{
	std::string							request;
	Methods								method;
	std::string							http_version;
	std::string							path;

	BodyInfo							body;
	std::map<std::string, std::string>	headers;
	std::string boundary;
	void reset();
};

struct ClientInfo
{
	int			fd;
	int			status_code;
	std::string	status_msg;
	bool		close_connection;
	sockaddr_in	addr;
	time_t		connection_time;
    bool		keep_alive;
	RequestInfo	info;
	bool file_uploaded;
	void reset();
};