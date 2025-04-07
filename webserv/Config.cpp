#include "Config.hpp"

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <iostream>

Config::Config(std::vector<std::string> &config_section)
{
    unsigned int i;
    for (i = 0; i < config_section.size(); i++)
        setConfig(config_section, &i);
}

Config::~Config()
{
}

void Config::setConfig(std::vector<std::string> &config_section, unsigned int *i)
{
    if (!std::strncmp(config_section[*i].c_str(), "listen", 6))
        this->_listen = trim(config_section[*i].substr(6, config_section[*i].find(';', 6) - 6));
    if (!std::strncmp(config_section[*i].c_str(), "server_name", 11))
        this->_server_name = trim(config_section[*i].substr(11, config_section[*i].find(';', 11) - 11));
    if (!std::strncmp(config_section[*i].c_str(), "location", 8))
    {
        std::string index = trim(config_section[*i].substr(8, config_section[*i].find(';', 8) - 8));
    while (*i < config_section.size() && config_section[*i] != "}")
        {
            this->_location[index];
            if (!strncmp(config_section[*i].c_str(), "allow_methods", 13))
                this->_location[index].allow_methods = config_section[*i];
            else if (!strncmp(config_section[*i].c_str(), "autoindex", 8))
                this->_location[index].autoindex = config_section[*i];
            else if (!strncmp(config_section[*i].c_str(), "index", 5))
                this->_location[index].index = config_section[*i];
            else if (!strncmp(config_section[*i].c_str(), "return", 6))
                this->_location[index].return_path = config_section[*i];
            else if (!strncmp(config_section[*i].c_str(), "root", 4))
                this->_location[index].root = config_section[*i];
            else if (!strncmp(config_section[*i].c_str(), "error", 6))
                this->_location[index].error = config_section[*i];
           (*i)++;
        }
    }
}