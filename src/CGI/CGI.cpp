#include "CGI.hpp"

CGI::CGI()
{
	_pid = -1;
}

CGI::~CGI()
{}

std::string CGI::getCGIResponse()
{
	return (_response);
}

void CGI::initCGI(ClientInfo &info)
{
	_method = info.info.method;
	std::ostringstream oss;
	oss << info.info.body.body_size;
	std::string size_str = oss.str();

	_env["REQUEST_METHOD"] = getStringMethod(info.info.method);
	_env["QUERY_STRING"] = info.info.query;
	_env["CONTENT_LENGTH"] = size_str;
	_env["PATH_INFO"] = info.info.absolute_path;

	_argv_storage.clear();
	_argv.clear();
	_argv_storage.push_back(_env["PATH_INFO"]);
	for (size_t i = 0; i < _argv_storage.size(); ++i)
		_argv.push_back(const_cast<char*>(_argv_storage[i].c_str()));
	_argv.push_back(NULL);
	_env_storage.clear();
	_envp.clear();
	for (std::map<std::string, std::string>::iterator it = _env.begin(); it != _env.end(); ++it)
		_env_storage.push_back(it->first + "=" + it->second);
	for (size_t i = 0; i < _env_storage.size(); ++i)
		_envp.push_back(const_cast<char*>(_env_storage[i].c_str()));
	_envp.push_back(NULL);

}

bool	CGI::runCGI(int *has_send_data, int *has_read_data)
{
	if (pipe(_in_pipe) == -1 || pipe(_out_pipe) == -1)
	{
		*has_send_data = false;
		*has_read_data = false;
		return (false);
	}
	_pid = fork();
	if (_pid < 0)
	{
		*has_send_data = false;
		*has_read_data = false;
		return (false);
	}
	if (_pid == 0)
	{
		dup2(_in_pipe[0], STDIN_FILENO);
		dup2(_out_pipe[1], STDOUT_FILENO);
		close(_in_pipe[1]);
		close(_out_pipe[0]);
		execve(_argv[0], _argv.data(), _envp.data());
		exit(1);
	}
	else
	{
		close(_in_pipe[0]);
		close(_out_pipe[1]);
		_write_fd = _in_pipe[1];
		_read_fd = _out_pipe[0];
		setNonBlockingFD(_write_fd);
		setNonBlockingFD(_read_fd);
		if (_method == POST) *has_send_data = true;
		else *has_send_data = false;
		*has_read_data = true;
	}
	return true;
}



bool	CGI::writePostBody(ClientInfo &info)
{
	ssize_t	sent = write(_write_fd, info.info.body.body_str.c_str(), info.info.body.body_str.size());

	close(_write_fd);
	if (sent <= 0)
		return false;
	return true;
}

bool	CGI::readCGIOutput()
{
	char	buffer[4096];
	ssize_t	bytes;

	while ((bytes = read(_read_fd, buffer, sizeof(buffer))) > 0)
		_response.append(buffer, bytes);
	close(_read_fd);
	waitpid(_pid, NULL, 0);
	return true;
}

int		CGI::getWriteFd() const
{
	return (_write_fd);
}

int		CGI::getReadFd() const
{
	return (_read_fd);
}


