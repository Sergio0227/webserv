#include "HttpResponse.hpp"

HttpResponse::HttpResponse(ClientInfo &info) : _info(info)
{
	_response = "";
	_body = "";
	_content_type = "";
	_location = "";
	_con = "";
	_err = -1;
}

HttpResponse&	HttpResponse::operator=(const HttpResponse& other)
{
	if (this != &other)
	{
		_response = other._response;
		_body = other._body;
		_content_type = other._content_type;
		_location = other._location;
		_con = other._con;
		_err = other._err;
	}
	return *this;
}
void	HttpResponse::setStatus(int code)
{
	_info.status_code = code;
	_info.status_msg = getStatusMessage(code);
}

void	HttpResponse::buildResponse()
{
	std::ostringstream	oss;
	std::string			status_code_str;
	std::string			body_len;

	oss << _info.status_code;
	status_code_str = oss.str();
	oss.str("");
	oss.clear();
	oss << _body.size();
	body_len = oss.str();
	_response = "HTTP/1.1 " + status_code_str + " " + getStatusMessage(_info.status_code) + "\r\n";
	if (!_content_type.empty())
		_response += "Content-Type: " + _content_type + "\r\n";
	_response += "Content-Length: " + body_len + "\r\n";
	_response += "Connection: " + _con + "\r\n";
	if (!_location.empty())
		_response += "Location: " + _location + "\r\n";
	_response += "\r\n";
	_response += _body;
}

void	HttpResponse::setContentType(const std::string &ct)
{
	_content_type = ct;
}

void	HttpResponse::setBody(const std::string &body)
{
	_body = body;
}

void	HttpResponse::setLocation(const std::string &loc)
{
	_location = loc;
}

void	HttpResponse::sendResponse()
{
	buildResponse();
	int bytes_sent = send(_info.fd, _response.c_str(), _response.size(), 0);
	if (bytes_sent < 0)
		throw std::runtime_error("send error");
}

void	HttpResponse::setConnection(const std::string &con)
{
	_con = con;
}

void	HttpResponse::setError(int err)
{
	_err = err;
}

int		HttpResponse::getError()
{
	return _err;
}

int		HttpResponse::getClientStatus()
{
	return _info.status_code;
}