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

	_env["REQUEST_METHOD"] = getStringMethod(_info.info.method);
	_env["QUERY_STRING"] = _info.info.query;
	_env["CONTENT_LENGTH"] = size_str;
	_env["PATH_INFO"] = _info.info.absolute_path;

	_argv = new char*[2];
	_argv[0] = new char[_env["PATH_INFO"].size() + 1];
	std::strcpy(_argv[0], _env["PATH_INFO"].c_str());
	_argv[1] = NULL;

	std::map<std::string, std::string>::iterator it = _env.begin();
	while (it != _env.end())
	{
		
		return;
	}
}

void CGI::executeCGI()
{
	_pid = fork();
	int in_pipe[2];
	int out_pipe[2];
	int status;

	if (pipe(in_pipe) == -1 || pipe(out_pipe) == -1 || (_pid = fork()) < 0)
	{
		setStatus(_info, 500);
		return ;
	}
	if (_pid == 0)
	{
		//execve(_interpreter.c_str(), _argv, );
	}
	else
	{
		waitpid(NULL, &status, 0);
	}

	
}