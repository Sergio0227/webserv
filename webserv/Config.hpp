#pragma once

#ifndef CONFIGPARSER_H
#define CONFIGPARSER_H

#include "utils.hpp"
#include <fstream>
#include <map>
#include <vector>

class Config {
private:
    std::string     _listen;
    std::string     _server_name;
    std::string     _root;
    std::map<std::string, std::map<std::string, std::string> > _location;

    void setConfig(std::string &line);

public:
    Config(std::vector<std::string> &config_section);
    ~Config();
};


#endif //CONFIGPARSER_H