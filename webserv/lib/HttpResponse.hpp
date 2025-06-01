#pragma once
#include "webserv.hpp"
#include "ClientInfo.hpp"

class HttpResponse
{
    private:
		HttpResponse();
		ClientInfo &_info;
		std::string _response;
		std::string _body;
		std::string _content_type;
		std::string _location;

		void buildResponse(const std::string &c_status);
		

    public:
		HttpResponse(ClientInfo &info);
		void setStatus(int code);
		void setContentType(const std::string &ct);
		void setBody(const std::string &body);
		void setLocation(const std::string &loc);
		void sendResponse(const std::string &c_status);
};
