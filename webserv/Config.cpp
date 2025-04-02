#include "Config.hpp"

#include <cstdlib>
#include <cstring>
#include <iostream>

Config::Config(std::vector<std::string> &config_section)
{
    this->_config_section = config_section;
}

Config::~Config()
{
}


