#pragma once

#include <vector>
#include <string>
#include "Config.hpp"
#include "HttpServer.hpp"

class Brain
{
private:
    int                                      _nb_servers;
    std::vector<std::vector<std::string> >   _config_files;
    std::vector<Config*>                     _server_conf;
    std::vector<std::string>                 _locations_keys;

    void splitServers(std::vector<std::string>);
    void parseConfigFile(int server_index);
    void parseLocation(size_t *i, int server_index, std::string location_name);
	void initServerConfigs();

public:
    Brain(std::vector<std::string>& config_files);
    ~Brain();
    int getNbServers();


};