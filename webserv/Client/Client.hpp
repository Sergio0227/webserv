#pragma once
#include "Socket.hpp"
class Client : public Socket
{
    private:
        Client();
    public:
        Client(int domain, int type, int protocol, int port, std::string &ip);
        ~Client();
        void connectToSocket();
        void    sendRequest();
        void    readFromServer();
};