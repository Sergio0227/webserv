#include "webserv.hpp"

std::string parseFileToString(const char *filename)
{
	std::string str;
	int fd = open(filename, O_RDONLY);
	char buffer[1024];
	int b_read = 1;
	while (b_read)
	{
		b_read = read(fd, buffer, sizeof(buffer) - 1);
		buffer[b_read] = '\0';
		str.append(buffer, b_read);
	}
	close(fd);
	return str;
}
//could be directory or file
bool pathExists(std::string path)
{
	if (!access(path.c_str(), F_OK))
		return true;
	else return false;
}
