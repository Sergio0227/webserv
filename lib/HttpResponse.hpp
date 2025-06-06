#pragma once

#include "webserv.hpp"
#include "ClientInfo.hpp"

class HttpResponse
{
    private:
		ClientInfo		&_info;
		std::string		_response;
		std::string		_body;
		std::string		_content_type;
		std::string		_location;
		std::string		_con;
		int				_err;

		void			buildResponse();
		HttpResponse();
		
	public:
		HttpResponse(ClientInfo &info);
		HttpResponse&	operator=(const HttpResponse& other);
		void			setStatus(int code);
		void			setContentType(const std::string &ct);
		void			setBody(const std::string &body);
		void			setLocation(const std::string &loc);
		void			sendResponse();
		void			setConnection(const std::string &con);
		void			setError(int err);
		int				getError();
		int				getClientStatus();
};
