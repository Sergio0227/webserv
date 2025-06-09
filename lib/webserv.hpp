#pragma once

#include "ClientInfo.hpp"
#include "CGI.hpp"
#include <iostream>
#include <stdexcept>
#include <unistd.h>
#include <algorithm>
#include <cstring>
#include <cerrno>
#include <sstream>
#include <map>
#include <fstream>
#include <unistd.h>
#include <fcntl.h>
#include <vector>
#include <csignal>
#include <sys/socket.h>
#include <ctime>
#include <iomanip>
#include <sys/stat.h>
#include <climits>
#include <dirent.h>
#include <fcntl.h>

#define ENABLE_DEBUG true
#define TIMEOUT_SEC 25
#define BACKLOG 5
//1mb maxbody if maxbody doesnt exist in config
#define MAX_CONTENT_LENGTH 1000000

#define BLUE "\033[34m"
#define GREEN "\033[32m"
#define RED "\033[31m"
#define ORANGE "\033[38;5;214m"
#define RESET "\033[0m"
#define ERROR_PATH_404 404
#define ERROR_METHOD_405 405

/*
---Function Prototypes in Header----
int socket(int domain, int type, int protocol)
int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
int listen(int sockfd, int backlog)
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
ssize_t send(int sockfd, const void *buf, size_t len, int flags)
ssize_t recv(int sockfd, void *buf, size_t len, int flags)
ssize_t sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen)
ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen)
int shutdown(int sockfd, int how)
int close(int sockfd)

---Info------------
socket() creates a new socket like a door for connections, a socket is an endpoint that you can configure (ipv4, tcp, ...)
bind() to bind an ip address and a port to a socket
listen(), listens on a socket and tells the OS to queue up to backlog clients before calling accept()
accept() blocks until a client connects
 */

#include <netinet/in.h>
/*
----Structure for storing an Internet socket address---

	struct sockaddr_in
	{
		short            sin_family;   // e.g. AF_INET for ipv4 --> done
		unsigned short   sin_port;     // e.g. htons(3490) --> done
		struct in_addr   sin_addr;     // see struct in_addr, below
		char             sin_zero[8];  // zero this if you want to -> done with memset
	};

	struct in_addr {
		unsigned long s_addr;  // load with inet_aton() --> done with inet_pton() 
	};

---Function Prototypes in Header----
uint16_t htons(uint16_t hostshort)
uint32_t htonl(uint32_t hostlong)

---Info------------
htons() converts 16 bit host byte order to network byte order, network endianess is always Big-endian, used for ports
htonl() converts 32 bits...used for ip

*/

#include <arpa/inet.h>
/*
---Function Prototypes in Header----

int inet_aton(const char *cp, struct in_addr *inp)
int inet_pton(int af, const char *src, void *dst)

---Info------------
inet_aton() converts an ipv4 char * like "127.0.0.1" to an int, can handle ipv4
inet_pton() converts ipv4 or ipv6 char * to an int and is more modern than aton

*/

extern int	g_flag;

enum LogType
{
	INFO,
	DEBUG,
	ERROR,
	SUCCESS
};

void								logMessage(LogType level, int server_fd, const std::string& message, ClientInfo *ptr_info, int flag);
void								printClientInfo(ClientInfo &info);
void								storeCredential(BodyInfo &body, const char *name);
void								setNonBlockingFD(int fd);
Methods								getEnumMethod(std::string &method);
bool								emailExists(ClientInfo &info);
bool								passwordCorrect(BodyInfo &body);
bool								safeExtract(std::string &input, char delim, std::string &out);
bool								isDirectory(const std::string &path);
bool								isRegularFile(const std::string &path);
bool								hasReadAccess(const std::string &path);
std::string							proper_path_cat(const std::string &path_a, const std::string &path_b);
std::string							getStringMethod(Methods method);
std::string							getCurrentTime();
std::string							getStatusMessage(int code);
std::string							getMimeType(const std::string &ext);
std::string							trim(const std::string &str);
std::string							retrieveContentType(ClientInfo &info);
std::string							buildUploadFilename(std::string &data, const char *path);
std::pair<std::string, std::string>	getPairLog(LogType level);
std::vector<std::string>			storeFormatedFile(std::string config_file_path);
int									countFilesInsideDir(const char *path);
