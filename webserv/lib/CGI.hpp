#include "webserv.hpp"

class CGI
{
	private:
		pid_t _pid;
		ClientInfo &_info;
		std::map<std::string, std::string> _env;
	public:
		CGI(ClientInfo &info);
		void setEnv();
		void executeCGI();
};