#include "ConfigParser.hpp"

#include <cstdlib>
#include <cstring>
#include <iostream>

ConfigParser::ConfigParser(std::string const filePath)
{
    this->_file.open(filePath.c_str());
    if (!this->_file.is_open())
      throw std::runtime_error("Config file could not be opened");
    loadConfig();

}

ConfigParser::~ConfigParser()
{
  this->_file.close();
}

void ConfigParser::loadConfig()
{
    std::string line;
    while (std::getline(this->_file, line))
    {
        std::string value = line.substr(line.find('=') + 1);
        if (line.find("host") != std::string::npos)
            this->_host = value.substr(value.find(' ') + 1);
        else if (line.find("port") != std::string::npos)
            this->_port = atoi(value.c_str());
        else if (line.find("document_root") != std::string::npos)
            this->_document_root =  value.substr(value.find(' ') + 1);
        else if (line.find("index") != std::string::npos)
            this->_index = value.substr(value.find(' ') + 1);
    }
}

std::string ConfigParser::getHost()
{
    return this->_host;
}

int ConfigParser::getPort()
{
    return this->_port;
}

std::string ConfigParser::getDocumentRoot()
{
    return this->_document_root;
}

std::string ConfigParser::getIndex()
{
    return this->_index;
}