#include "webserv.hpp"

std::string removeComment(std::string &line)
{
    size_t  pos = line.find('#');

    if (pos != std::string::npos)
        line = line.substr(0, pos);
    return (line);
}

std::string trim(const std::string& str)
{
    size_t  start = 0;

    while (start < str.size() && std::isspace(str[start]))
        ++start;
    size_t end = str.size();
    while (end > start && std::isspace(str[end - 1]))
        --end;
    return (str.substr(start, end - start));
}


std::vector<std::string>    storeFormatedFile(std::string config_file_path)
{
    std::vector<std::string>    configFile;
    std::ifstream               file(config_file_path.c_str());

    if (!file.is_open())
        throw std::runtime_error("Error: Could not open config file: " + config_file_path);
    configFile.resize(0);
    std::string line;
    while (std::getline(file, line))
    {
        if (line.find('#') != std::string::npos)
            removeComment(line);
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
    return (configFile);
}