#pragma once

#include "webserv.hpp"
#include <sys/wait.h>
#include <vector>


class CGI
{
	private:
		pid_t								_pid;
		int									_in_pipe[2];
		int									_out_pipe[2];
		int									_write_fd;
		int									_read_fd;
		std::vector<std::string>			_argv_storage;
		std::vector<std::string>			_env_storage;
		std::map<std::string, std::string>	_env;
		std::vector<char*>					_argv;
		std::vector<char*>					_envp;
		std::string							_response;
		Methods								_method;

	public:
		CGI();
		~CGI();
		std::string	getCGIResponse();
		void		initCGI(ClientInfo &info);
		bool		runCGI(int *has_send_data, int *has_read_data);
		bool		writePostBody(ClientInfo &info);
		bool		readCGIOutput();
		int			getWriteFd() const;
		int			getReadFd() const;
		int			getReadPipeFromChild();
		int			getWritePipeFromChild();
};