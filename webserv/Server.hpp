#pragma once
#ifndef SERVER_HPP
#define SERVER_HPP

#include <vector>
#include "Config.hpp"


class Server {
private:
    int     server_id;
    //Config  config;

public:
    Server(unsigned int index);
    ~Server(void);

    //Config getConfig(int index);
};



#endif //SERVER_HPP