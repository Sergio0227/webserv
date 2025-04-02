#pragma once
#ifndef SERVER_HPP
#define SERVER_HPP

#include <vector>
#include "Config.hpp"


class Server {
private:
    unsigned int     _server_id;
    Config  *_config;

public:
    Server(unsigned int index, std::vector<std::string> config_section);
    ~Server(void);

    //Config getConfig(int index);
};



#endif //SERVER_HPP