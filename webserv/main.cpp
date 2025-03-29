#include <cstdlib>
#include <cstring>
#include <iostream>
#include "Server.hpp"

std::string trim(const std::string& str)
{
    size_t start = 0;

    while (start < str.size() && std::isspace(str[start]))
        ++start;
    size_t end = str.size();
    while (end > start && std::isspace(str[end - 1]))
        --end;
    return str.substr(start, end - start);
}

std::vector<std::vector<void *> > read_config(const std::string& config_file_path)
{
    std::fstream file;
    std::vector<std::vector<void *> > config;

    file.open(config_file_path.c_str());
    if (!file)
        throw std::runtime_error("Config file could not be opened");
    config.resize(0);
    std::string line;
    int index = -1;
    while (std::getline(file, line))
    {
        if (!line.empty())
        {
            if (strncmp(line.c_str(), "server", 6) == 0)
            {
                index++;
                config.resize(index + 1);
                while (std::getline(file, line) && line.empty())
                    ;
            }
            std::string *line_ptr = new std::string(trim(line));
            config[index].push_back((void *) line_ptr);
        }
    }
    return config;
}

int main(int argc, char *argv[])
{
    if (argc == 1|| argc == 2)
    {
        std::string config_file_path = "configs/default.conf";
        if (argc == 2)
            config_file_path = argv[1];
        try
        {
            std::vector<std::vector<void *> > config = read_config(config_file_path);
            std::vector<Server *> servers(config.size(), NULL);
            for (unsigned int i = 0; i < servers.size(); i++)
                servers[i] = new Server(i);
            for (unsigned int i = 0; i < config[0].size(); i++)
                std::cout << *((std::string*)config[0][i]) << std::endl;
        }
        catch (const std::runtime_error &e)
        {
            std::cout << "Error: " << e.what() << std::endl;
        }
    }
    else
    {
        std::cerr << "Error: Too many arguments \n" << "Usage -> ./webserv [configfile]" <<std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
