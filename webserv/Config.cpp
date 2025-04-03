#include "Config.hpp"

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <iostream>

Config::Config(std::vector<std::string> &config_section)
{
    for (unsigned int i = 0; i < config_section.size(); i++)
        setConfig(config_section[i]);
}

Config::~Config()
{
}

void Config::setConfig(std::string &line)
{
    if (!std::strncmp(line.c_str(), "listen", 6))
        this->_listen = trim(line.substr(6, line.find(';', 6) - 6));
    if (!std::strncmp(line.c_str(), "server_name", 11))
        this->_server_name = trim(line.substr(11, line.find(';', 11) - 11));
    if (!std::strncmp(line.c_str(), "location", 8))
    {
        this->_location[line] = {{"key1", "shit"}, {"key2", "othershit"}};
    }
}