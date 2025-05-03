#include "Brain.hpp"
#include <stdexcept>

Brain::Brain(std::vector<std::string>& config_file)
{
    this->_nb_servers = 0;
    this->_server_conf.resize(0);
    this->_config_files.resize(0);
    this->_locations_keys.resize(0);
    splitServers(config_file);
	initServerConfigs();
    Location *loc = this->_server_conf[0]->getLocation(this->_locations_keys[0]);
    std::cout << loc->getPath();
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
                throw Config::ConfigException("Error: Wrong character out of server scope");
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
            throw Config::ConfigException("Error: Unclosed bracket");
    }
}

void Brain::initServerConfigs()
{
    this->_server_conf.resize(this->_nb_servers);
    for (int i = 0; i < this->_nb_servers; i++)
    {
        this->_server_conf[i] = new Config();
        parseConfigFile(i);
    }
}

void Brain::parseConfigFile(int server_index)
{
     std::string valid_params[] = {"server_name", "listen", "root", "client_max_body_size",
        "index", "autoindex", "error_page", "location", "allow_methods"};
    for (size_t i = 0; i < this->_config_files[server_index].size() ; i++)
    {
        size_t j;
        std::string param = trim(this->_config_files[server_index][i].substr(0, this->_config_files[server_index][i].find(' ')));
        if (this->_config_files[server_index][i].size() == param.size())
            throw Config::ConfigException("Error: Wrong parameter near: " + param);
        std::string value = trim(this->_config_files[server_index][i].substr(param.size() + 1, this->_config_files[server_index][i].size()));
        if (param == "location")
            parseLocation(&(++i), server_index, value);
        for (j = 0; valid_params[j] != param; j++)
            if (j > param.size() || value == ";")
               throw Config::ConfigException("Error: Wrong parameter near: " + param);
        if (param == "server_name")
            this->_server_conf[server_index]->setServerName(value);
        else if (param == "listen")
            this->_server_conf[server_index]->setPort(value);
        else if (param == "root")
            this->_server_conf[server_index]->setRoot(value);
        else if (param == "client_max_body_size")
            this->_server_conf[server_index]->setClientMaxBodySize(value);
        else if (param == "index")
            this->_server_conf[server_index]->setIndex(value);
        else if (param == "autoindex")
            this->_server_conf[server_index]->setAutoindex(value);
        }
}

void Brain::parseLocation(size_t *i, int server_index, std::string location_name)
{
    std::string valid_params[] = {"root","index", "autoindex", "error_page", "alias", "allow_methods",
        "client_max_body_size", "return", "cgi_path", "cgi_ext"};

    this->_locations_keys.push_back(location_name);
    Location ref_loc;
    ref_loc.setPath(location_name);
    while (this->_config_files[server_index][++(*i)] != "}")
    {
        size_t j;
        std::string param = trim(this->_config_files[server_index][*i].substr(0, this->_config_files[server_index][*i].find(' ')));
        std::string value= trim(this->_config_files[server_index][*i].substr(param.size() + 1, this->_config_files[server_index][*i].size()));
        for (j = 0; valid_params[j] != param; j++)
            if (j > param.size())
               throw Config::ConfigException("Error: Wrong parameter near: " + param);
        if (param == "allow_methods")
            ref_loc.setAllowedMethods(value);
        else if (param == "root")
            ref_loc.setRoot(value);
        else if (param == "index")
            ref_loc.setIndex(value);
        else if (param == "autoindex")
            ref_loc.setAutoindex(value);
        else if (param == "alias")
            ref_loc.setAlias(value);
        else if (param == "return")
            ref_loc.setReturnValue(value);
        else if (param == "client_max_body_size")
            ref_loc.setClientMaxBodySize(value);
    }
    this->_server_conf[server_index]->setLocation(location_name, ref_loc);
}


int Brain::getNbServers()
{
    return this->_nb_servers;
}