#pragma once

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

#include <sys/socket.h>
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
socket() creates a new socket like a door for connections
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
struct RequestInfo
{
	std::string method;
	std::string http_version;
	std::string path;

	std::string body;
	//std::string query_string;
	std::map<std::string, std::string> headers;
};
struct ClientInfo
{
	bool closed_connection;
	std::string ip_address;
	int socket_fd;
	int port;
	time_t connection_time;
    bool keep_alive;
	RequestInfo info;
};