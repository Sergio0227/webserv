#pragma once

#ifndef SOCKET_H
#define SOCKET_H

#include <netinet/in.h>
#include <sys/socket.h>

class Socket {
private:
    sockaddr_in _address;

public:
    Socket();
    ~Socket();
};


#endif //SOCKET_H