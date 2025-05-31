#include "CGI.hpp"

CGI::CGI(ClientInfo &info) : _info(info)
{
	_pid = -1;
	_argv = NULL;
	_envp = NULL;
	setEnv();
	executeCGI();
}

CGI::~CGI()
{
	if (_argv)
	{
		for (int i = 0; _argv[i]; ++i)
			delete[] _argv[i];
		delete[] _argv;
	}
	if (_envp)
	{
		for (int i = 0; _envp[i]; ++i)
			delete[] _envp[i];
		delete[] _envp;
	}
}

std::string CGI::getCGIResponse()
{
	return (_response);
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

	_envp = new char*[_env.size() + 1];
	int i = 0;
	std::map<std::string, std::string>::iterator it = _env.begin();
	while (it != _env.end())
	{
		std::string env_entry = it->first + "=" + it->second;
		_envp[i] = new char[env_entry.size() + 1];
		std::strcpy(_envp[i], env_entry.c_str());
		it++;
		i++;
	}
	_envp[i] = NULL;
}

void CGI::executeCGI()
{
	int		in_pipe[2];
	int		out_pipe[2];
	int		status;
	ssize_t	bytes;
	char	buffer[4096];

	if (pipe(in_pipe) == -1 || pipe(out_pipe) == -1)
	{
		setStatus(_info, 500), _info.close_connection = true;
		return ;
	}
	_pid = fork();
	if (_pid < 0)
	{
		setStatus(_info, 500), _info.close_connection = true;
		return ;
	}
	if (_pid == 0)
	{
		dup2(in_pipe[0], STDIN_FILENO);
		dup2(out_pipe[1], STDOUT_FILENO);
		close(in_pipe[1]);
		close(out_pipe[0]);
		execve(_argv[0], _argv, _envp);
		exit(1);
	}
	else
	{
		close(in_pipe[0]);
		close(out_pipe[1]);

		if (_info.info.method == POST)
			write(in_pipe[1], _info.info.body.body_str.c_str(), _info.info.body.body_str.size());
		close(in_pipe[1]);
		while ((bytes = read(out_pipe[0], buffer, sizeof(buffer))) > 0)
			_response.append(buffer, bytes);
		close(out_pipe[0]);
		waitpid(_pid, &status, 0);
	}
}