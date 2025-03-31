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

std::vector<std::string> storeFile(std::string config_file_path)
{
    std::fstream file;
    std::vector<std::string> configFile;

    file.open(config_file_path.c_str());
    if (!file)
        throw std::runtime_error("Config file could not be opened");
    configFile.resize(0);
    std::string line;
    while (std::getline(file, line))
    {
        line = trim(line);
        if (!line.empty())
            configFile.push_back(line);
    }
    return configFile;
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
            std::vector<std::string> configFile = storeFile(config_file_path);
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
