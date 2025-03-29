#include "Config.hpp"

#include <cstdlib>
#include <cstring>
#include <iostream>

Config::Config(std::string const filePath)
{
    loadConfig();
}

Config::~Config()
{
}

void Config::loadConfig()
{
 //   std::string line;
 // //  while (std::getline(this->_file, line))
 //   {
 //       std::string value = line.substr(line.find('=') + 1);
 //       if (line.find("host") != std::string::npos)
 //           this->_host = value.substr(value.find(' ') + 1);
 //       else if (line.find("port") != std::string::npos)
 //           this->_port = atoi(value.c_str());
 //       else if (line.find("document_root") != std::string::npos)
 //           this->_document_root =  value.substr(value.find(' ') + 1);
 //       else if (line.find("index") != std::string::npos)
 //           this->_index = value.substr(value.find(' ') + 1);
 //   }
}

std::string Config::getHost()
{
    return this->_host;
}

int Config::getPort()
{
    return this->_port;
}

std::string Config::getDocumentRoot()
{
    return this->_document_root;
}

std::string Config::getIndex()
{
    return this->_index;
}