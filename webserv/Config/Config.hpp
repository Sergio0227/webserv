#pragma once

#ifndef CONFIGPARSER_H
#define CONFIGPARSER_H

#include "webserv.hpp"
#include <fstream>
#include <map>
#include <vector>

typedef struct s_loc
{
    std::string root;
    std::string error;
    std::string autoindex;
    std::string index;
    std::string allow_methods;
    std::string return_path;
}               t_loc;

class Config {
private:
    std::string     _listen;
    std::string     _server_name;
    std::string     _root;
    std::map<std::string, t_loc> _location;

    void setConfig(std::vector<std::string> &config_section, unsigned int *i);

public:
    Config(std::vector<std::string> &config_section);
    ~Config();
};


#endif //CONFIGPARSER_H