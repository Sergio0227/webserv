#pragma once

#ifndef CONFIGPARSER_H
#define CONFIGPARSER_H

#include <string>
#include <fstream>
#include <vector>

class Config {
private:
    std::string     _listen;
    std::string     _server_name;
    std::vector<std::vector<std::string> > _location;


public:
    Config();
    ~Config();
};


#endif //CONFIGPARSER_H