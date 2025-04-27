#include "Brain.hpp"
#include <stdexcept>

Brain::Brain(std::vector<std::string>& config_file)
{
    this->_nb_servers = 0;
    this->_server_conf.resize(0);
    this->_config_files.resize(0);
    splitServers(config_file);
	initServerConfigs();
	HttpServer(_server_conf[0], static_cast<int>(_server_conf[0]->getPort()),_server_conf[0]->getServerName(),  BACKLOG, true);
}

Brain::~Brain()
{
    for (int i = 0; i < this->_nb_servers; i++)
        delete _server_conf[i];
    _server_conf.resize(0);
    _config_files.resize(0);
}

void Brain::splitServers(std::vector<std::string> config_file)
{
    size_t i = 0;
    int nested;
    bool in_server_scope;

    while (i < config_file.size())
    {
        nested = 0;
        in_server_scope = false;
        this->_nb_servers++;
        this->_config_files.resize(this->_nb_servers);
        while (i < config_file.size())
        {
            if (config_file[i] == "server")
            {
                in_server_scope = true;
                i++;
            }
            if ((in_server_scope && config_file[i] == "server")
                || (!in_server_scope && config_file[i] != "server")
                || (config_file[i] == "{" && config_file[i + 1] == "{")
                || (config_file[i] == "{" && config_file[i + 1] == "}"))
                throw std::runtime_error("Error: Wrong character out of server scope");
            if (config_file[i] == "{")
                nested++;
            if ((config_file[i] != "}" && config_file[i] != "{") || nested > 1)
                this->_config_files[this->_nb_servers - 1].push_back(config_file[i]);
            if (config_file[i] == "}")
                nested--;
            i++;
            if (nested == 0)
                break;
        }
        if (nested)
            throw std::runtime_error("Error: Unclosed bracket");
    }
}

void Brain::initServerConfigs()
{
    this->_server_conf.resize(this->_nb_servers);
    for (int i = 0; i < this->_nb_servers; i++)
        this->_server_conf[i] = new Config();
}

void Brain::parseConfigFile()
{
    this->_server_conf.resize(this->_nb_servers);
}

int Brain::getNbServers()
{
    return this->_nb_servers;
}