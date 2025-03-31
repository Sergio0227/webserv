#ifndef CLIENT_HPP
#define CLIENT_HPP
#include "Socket.hpp"
class Client : public Socket
{
    private:
        Client();
    public:
        Client(int domain, int type, int protocol, int port, u_long ip);
        ~Client();
        void connect_to_server();
};
#endif