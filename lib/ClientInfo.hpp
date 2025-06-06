#pragma once

#include <string>
#include <map>
#include <ctime>
#include <netinet/in.h>

enum	Methods
{
	GET,
	POST,
	DELETE,
	PUT,
	UNKNOWN_METHOD
};

struct	BodyInfo
{
	size_t		body_size;
	std::string	body_str;
	std::string	email;
	std::string	passw;

	void		reset();
};

struct	RequestInfo
{
	std::string							request;
	std::string							boundary;
	std::string							http_version;
	std::string							path;
	std::string							absolute_path;
	std::string							query;
	Methods								method;
	std::map<std::string, std::string>	headers;
	BodyInfo							body;
	
	void								reset();
};

struct	ClientInfo
{
	int			fd;
	int			status_code;
	bool		run_cgi;
	bool		dir_listening;
	bool		keep_alive;
	bool		close_connection;
	bool		file_uploaded;
	std::string	status_msg;
	sockaddr_in	addr;
	time_t		connection_time;
	RequestInfo	info;
	
	void		reset();
};