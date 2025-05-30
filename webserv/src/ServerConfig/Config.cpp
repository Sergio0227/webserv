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
	for (std::map<std::string, Location*>::iterator it = _locations.begin(); it != _locations.end(); ++it)
		delete it->second;
}

void Config::initErrorPages()
{

}

void Config::setPort(std::string& port)
{
	static void *set = NULL;

	if (set == this)
		throw ConfigException("Duplicate Port declaration, only one allowed per server block");
	validParamtr(port);
	for (size_t i = 0; i < port.size(); ++i)
		if (!std::isdigit(port[i]))
			throw ConfigException("Port must contain only numeric values");
	int port_int = std::atoi(port.c_str());
	if (port_int < 1 || port_int > 65535)
		throw ConfigException("Invalid Port number");
	this->_port =   port_int;
	set = this;
}


void Config::setServerName(std::string& server_name)
{
	static void *set = NULL;
	if (set == this)
		throw ConfigException("Duplicate ServerName declaration, only one allowed per server block");
	validParamtr(server_name);
	if (server_name == "localhost")
		server_name =  "127.0.0.1";
	if (inet_addr(server_name.c_str()) == INADDR_NONE) //INADDR_NON is not a valid IPV4 address
			throw ConfigException("Invalid Host address");
	this->_server_name = server_name;
	set = this;
}

void Config::setRoot(std::string& root)
{
	static void *set = NULL;
	if (set == this)
		throw ConfigException("Duplicate Root declaration, only one allowed per server block");
	validParamtr(root);
	// //  if (!isDirectory(root))
	//       throw ConfigException("Invalid Root, error while opening root folder");
	//   if (!hasReadAccess(root))
	//       throw ConfigException("Invalid Root, no permissions to open root folder");
	this->_root = root;
	set = this;
}

void Config::setClientMaxBodySize(std::string& client_max_body_size)
{
	static void *set = NULL;
	if (set == this)
		throw ConfigException("Duplicate ClientMaxBodySize declaration, only one allowed per server block");
	validParamtr(client_max_body_size);
	for (size_t i = 0; i < client_max_body_size.size(); ++i)
		if (!std::isdigit(client_max_body_size[i]))
			throw ConfigException("Body size must contain only numeric values");
	int int_body_size = std::atoi(client_max_body_size.c_str());
	this->_client_max_body_size = int_body_size;
	set = this;
}

void Config::setIndex(std::string& index)
{
	static void *set = NULL;
	if (set == this)
		throw ConfigException("Duplicate Index declaration, only one allowed per server block");
	validParamtr(index);
	// //  if (!isRegularFile(index))
	//       throw ConfigException("Invalid Index, error while opening index file");
	//   if (!hasReadAccess(index))
	//       throw ConfigException("Invalid Index, no permissions to open index file");
	//   std::string absRoot = realpath(_root.c_str(), NULL);
	this->_index = index;
	set = this;
}

void Config::setAutoindex(std::string& autoindex)
{
	static void *set = NULL;
	if (set == this)
		throw ConfigException("Duplicate Autoindex declaration, only one allowed per server block");
	validParamtr(autoindex);
	if (autoindex != "on" && autoindex != "off")
		throw ConfigException("Invalid Autoindex, Off or On expected");
	this->_autoindex = (autoindex == "on");
	set = this;
}

void Config::setLocation(std::string& location_name, Location& location)
{
	this->_locations[location_name] = new Location(location);
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

Location* Config::getLocation(std::string& location_name)
{
    std::map<std::string, Location*>::iterator it = this->_locations.find(location_name);
    if (it != this->_locations.end())
        return it->second;
    else
        return NULL;
}

void Config::validParamtr(std::string& paramtr)
{
    size_t pos;

    pos = paramtr.find(';');
    if (pos != paramtr.size() - 1)
    {
        if (pos > paramtr.size() - pos -1 && pos < paramtr.size() - 1)
            throw ConfigException("unexpected end of input near \"" + paramtr + "\"");
        throw ConfigException("semi-colon missing near \"" + paramtr + "\"");
    }
    paramtr = trim(paramtr.substr(0, pos));
}