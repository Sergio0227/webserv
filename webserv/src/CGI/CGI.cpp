#include "CGI.hpp"

CGI::CGI(ClientInfo &info) : _info(info)
{
	_pid = -1;
	setEnv();
	executeCGI();
}
void CGI::setEnv()
{
	std::ostringstream oss;
	oss << _info.info.body.body_size;
	std::string size_str = oss.str();

	_info.info.body.body_size;
	_env["REQUEST_METHOD"] = getStringMethod(_info.info.method);
	_env["QUERY_STRING"] = _info.info.query;
	_env["CONTENT_LENGTH"] = size_str;
	_env["PATH_INFO"] = _info.info.absolute_path;
}

void CGI::executeCGI()
{
	_pid = fork();
	if (fork < 0)
	{
		
		setStatus(_info, 500);
		return ;
	}
}