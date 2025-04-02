#include "Server.hpp"

Server::Server(unsigned int index, std::vector<std::string> config_section)
{
    this->_server_id = index;
    this->_config = new Config(config_section);
}

Server::~Server(void)
{
}

