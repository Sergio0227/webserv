#include "Server.hpp"
#include "utils.hpp"

std::vector<std::string> storeFormatedFile(std::string config_file_path)
{
    std::vector<std::string> configFile;

    std::ifstream file(config_file_path.c_str());
    if (!file.is_open()) {
        throw std::runtime_error("Error: Could not open config file: " + config_file_path);
    }
    configFile.resize(0);
    std::string line;
    while (std::getline(file, line))
    {
        line = trim(line);
        if (!line.empty())
        {
            size_t pos;
            if (line.size() > 1 && ((pos = line.find('{')) != std::string::npos || (pos = line.find('}')) != std::string::npos))
            {
                char bracket_type = line[pos];
                if (line[0] != bracket_type)
                    configFile.push_back(trim(line.substr(0, pos)));
                configFile.push_back(std::string(1, bracket_type));
                if (line[pos + 1] != 0)
                    configFile.push_back(trim(line.substr(pos + 1)));
            }
            else
                configFile.push_back(line);
        }
    }
    return configFile;
}

static int getNumServers(std::vector<std::string> configFile)
{
    int numServers = 0;

    for (unsigned int i = 0; i < configFile.size(); i++)
        if (configFile[i] == "server")
            numServers++;
    return numServers;
}

std::vector<std::string> getOwnConfig(std::vector<std::string> &configFile)
{
    size_t i = 0;
    int nested = 0;

    while (i < configFile.size())
    {
        if (configFile[i] == "{")
            nested++;
        if (configFile[i] == "}")
            nested--;
        if (nested == 0 && i > 1)
            break;
        i++;
    }
    std::vector<std::string> new_vector(configFile.begin(), configFile.begin() + (i + 1));
    configFile.erase(configFile.begin(), configFile.begin() + (i + 1));
    return new_vector;
}

int main(int argc, char *argv[])
{
    if (argc == 1 || argc == 2)
    {
        std::string config_file_path = "configs/default.conf";
        if (argc == 2)
            config_file_path = argv[1];
        try
        {
            std::vector<std::string> configFile = storeFormatedFile(config_file_path);
            std::vector<Server *> servers;
            servers.resize(getNumServers(configFile));
            for (unsigned int i = 0; i < servers.size(); i++)
                servers[i] = new Server(i + 1, getOwnConfig(configFile));
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
