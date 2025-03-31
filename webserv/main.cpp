#include "Server.hpp"
#include "utils.hpp"

int numServers = 0;

std::vector<std::string> storeFormatedFile(std::string config_file_path)
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
        {
            size_t pos;
            if (line.size() > 1 && (pos = line.find('{')) != std::string::npos || (pos = line.find('}')) != std::string::npos)
            {
                char braket_type = line[pos];
                if (line[0] != braket_type)
                    configFile.push_back(line.substr(0, pos));
                configFile.push_back(std::string(1, braket_type));
                if (line[pos + 1] != 0)
                    configFile.push_back(trim(line.substr(pos + 1)));
            }
            else
                configFile.push_back(line);
        }
        if (configFile[configFile.size() - 1] == "server")
            numServers++;
    }
    std::cout << numServers << std::endl;
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
            std::vector<std::string> configFile = storeFormatedFile(config_file_path);
            //std::vector<Server> servers;
            //servers.resize(numServers);
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
