#include "Config.hpp"
#include "webserv.hpp"

Config::Config()
{
    this->_port = 0;
    this->_server_name = "";
    this->_root = "";
    this->_client_max_body_size = MAX_CONTENT_LENGTH;
    this->_index = "";
    this->_autoindex = false;
    initErrorPages();
}

Config::~Config()
{
}

void Config::initErrorPages()
{
    return;
}

void Config::setPort(std::string& port)
{
    validParamtr(port);
    for (size_t i = 0; i < port.size(); ++i)
        if (!std::isdigit(port[i]))
            throw ConfigException("Port must contain only numeric values");
    int port_int = std::atoi(port.c_str());
    if (port_int < 1 || port_int > 65535)
        throw ConfigException("Invalid Port number");
    this->_port =   port_int;
}


void Config::setServerName(std::string& server_name)
{
    validParamtr(server_name);
    if (server_name == "localhost")
        server_name =  "127.0.0.1";
    // if (inet_addr(server_name.c_str()) == INADDR_NONE) //INADDR_NON is not a valid IPV4 address
    //     throw ConfigException("Invalid Host address");
    this->_server_name = server_name;
}

void Config::setRoot(std::string& root)
{
    validParamtr(root);
    if (!isDirectory(root))
        throw ConfigException("Invalid Root, error while opening root folder");
    if (!hasReadAccess(root))
        throw ConfigException("Invalid Root, no permissions to open root folder");
    this->_root = root;
}

void Config::setClientMaxBodySize(std::string& client_max_body_size)
{
    validParamtr(client_max_body_size);
    for (size_t i = 0; i < client_max_body_size.size(); ++i)
        if (!std::isdigit(client_max_body_size[i]))
            throw ConfigException("Body size must contain only numeric values");
    client_max_body_size = std::atoi(client_max_body_size.c_str());
}

void Config::setIndex(std::string& index)
{
    validParamtr(index);
    if (!isRegularFile(index))
        throw ConfigException("Invalid Index, error while opening index file");
    if (!hasReadAccess(index))
        throw ConfigException("Invalid Index, no permissions to open index file");
    std::string absRoot = realpath(_root.c_str(), NULL);
    this->_index = index;
}

void Config::setAutoindex(std::string& autoindex)
{
    validParamtr(autoindex);
    if (autoindex != "on" && autoindex != "off")
        throw ConfigException("Invalid Autoindex, Off or On expected");
    this->_autoindex = (autoindex == "on");
}


uint16_t Config::getPort() const
{
    return _port;
}


std::string Config::getServerName() const
{
    return _server_name;
}

std::string Config::getRoot() const
{
    return _root;
}

unsigned long Config::getClientMaxBodySize() const
{
    return _client_max_body_size;
}

std::string Config::getIndex() const
{
    return _index;
}

bool Config::getAutoindex() const
{
    return _autoindex;
}

const std::map<short, std::string>& Config::getErrorPages() const
{
    return _error_pages;
}

struct sockaddr_in Config::getServerAddress() const
{
    return _server_address;
}

void Config::validParamtr(std::string& paramtr)
{
    size_t pos;

    pos = paramtr.find(';');
    if (pos != paramtr.size() - 1)
        throw ConfigException("semi-colon missing near \"" + paramtr + "\"");
    paramtr = paramtr.substr(0, pos);
}