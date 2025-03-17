####  Building a fully functional HTTP server from scratch in C++. 

The webserv project requires students to implement a web server that can:
- Handle HTTP requests (GET, POST, DELETE methods)
- Serve static files
- Execute CGI scripts
- Handle configuration files (similar to Nginx)
- Implement proper error handling
- Manage multiple connections concurrently

##### <u>GENERAL INSTRUCTIONS</u>
1. **Core Server Requirements**:
    - Non-blocking server using poll() (or equivalent like select(), kqueue(), or epoll())
    - Monitor both reading and writing operations simultaneously
    - Never do read/write operations without going through poll() (or equivalent)
    - Never check errno after read/write operations
    - Handle client disconnections properly
    - Response timeouts (requests should never hang indefinitely)
    - Stress test resilience
2. **HTTP Protocol Implementation**:
    - Support HTTP methods: GET, POST, DELETE (minimum requirement)
    - Accurate HTTP response status codes
    - Compatible with standard web browsers
    - Default error pages if none provided
    - Proper header handling (compatible with HTTP 1.1)
3. **Configuration File Parsing**:
    - Port and host configuration for each server
    - Server name setup
    - Default server for host:port (first server defined)
    - Custom error pages configuration
    - Client body size limits
4. **Route Configuration**:
    - HTTP method restrictions per route
    - HTTP redirects
    - Root directory mapping
    - Directory listing control (enable/disable)
    - Default files for directory requests
    - File upload handling with custom save locations
5. **CGI Implementation**:
    - Execute CGI based on file extensions (e.g., .php)
    - Support POST and GET methods with CGI
    - Handle chunked requests (server must unchunk)
    - Pass the requested file as first argument
    - Run CGI in correct directory for relative path access
    - Support at least one CGI (php-CGI, Python, etc.)
6. **File Operations**:
    - Serve static website content
    - Upload file functionality
    - Directory listing functionality
    - Proper MIME type handling
7. **Advanced Features (for mandatory part)**:
    - Multiple port listening
    - Support for multiple virtual hosts
8. **Bonus Features** (only if mandatory part is complete):
    - Cookie and session management
    - Multiple CGI support

[SUBJECT](https://cdn.intra.42.fr/pdf/pdf/151149/en.subject.pdf)

![img1](https://trungams.github.io/assets/img/socket-programming-tcp.png)

## Needed Header files and functions

Key elements defined in **netinet/in.h** include:
1. **Socket address structures**:
    - `struct sockaddr_in` - IPv4 socket address structure
    - `struct sockaddr_in6` - IPv6 socket address structure
2. **Port and address manipulation**:
    - `htons()`, `ntohs()` - Functions to convert between host and network byte order for short integers (typically port numbers)
    - `htonl()`, `ntohl()` - Functions to convert between host and network byte order for long integers (typically IP addresses)
3. **IP protocol constants**:
    - `IPPROTO_TCP`, `IPPROTO_UDP`, etc. - Constants for different protocols
    - `INADDR_ANY` - Special address used to bind to all network interfaces
4. **Address family definitions**:
    - `AF_INET` - IPv4 address family
    - `AF_INET6` - IPv6 address family

Key elements defined in **sys/socket.h** include:
1. **Socket functions**:
    - `socket()` - Creates a new socket of a specified type
    - `bind()` - Associates a socket with an address and port
    - `listen()` - Marks a socket as passive (ready to accept connections)
    - `accept()` - Accepts a connection on a listening socket
    - `connect()` - Establishes a connection to a remote socket
    - `send()`, `recv()` - Send and receive data on connected sockets
    - `sendto()`, `recvfrom()` - Send and receive data on unconnected (datagram) sockets
    - `close()` - Closes a socket
2. **Socket types**:
    - `SOCK_STREAM` - Reliable, connection-oriented streams (TCP)
    - `SOCK_DGRAM` - Connectionless, message-oriented sockets (UDP)
    - `SOCK_RAW` - Raw network protocol access
3. **Socket options**:
    - `setsockopt()`, `getsockopt()` - Set and get socket options
    - Various option constants (SO_REUSEADDR, SO_KEEPALIVE, etc.)
4. **Address structures**:
    - `struct sockaddr` - Generic socket address structure
    - Socket address constants and macros
5. **File descriptor operations**:
    - `select()`, `poll()` - Monitor multiple file descriptors
    - FD_SET, FD_CLR, FD_ISSET, FD_ZERO - Macros for managing file descriptor sets