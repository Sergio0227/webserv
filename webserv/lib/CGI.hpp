#pragma once
#include "webserv.hpp"
#include <sys/wait.h>

class CGI
{
	private:
		pid_t _pid;
		ClientInfo &_info;
		std::map<std::string, std::string> _env;
		std::string _response;
		char **_argv;
		char **_envp;
	public:
		CGI(ClientInfo &info);
		~CGI();
		std::string getCGIResponse();
        void setEnv();
        void executeCGI();
};