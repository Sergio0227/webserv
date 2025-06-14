#include "Location.hpp"
#include "Config.hpp"

Location::Location()
{
	this->_path = "";
	this->_return_value = "";
	this->_alias = "";
	this->_root = "";
	this->_client_max_body_size = MAX_CONTENT_LENGTH;
	this->_index = "";
	this->_autoindex = false;
	this->_has_autoindex = false;
	this->_allowed_methods["GET"] = false;
	this->_allowed_methods["POST"] = false;
	this->_allowed_methods["DELETE"] = false;
}

Location::Location(const Location& ref)
{
	this->_path = ref._path;
	this->_return_value = ref._return_value;
	this->_alias = ref._alias;
	this->_root = ref._root;
	this->_client_max_body_size = ref._client_max_body_size;
	this->_index = ref._index;
	this->_autoindex = ref._autoindex;
	this->_error_pages = ref._error_pages;
	this->_allowed_methods = ref._allowed_methods;
}

Location::~Location()
{}

void    Location::setPath(std::string& path)
{
	this->_path = path;
}

void    Location::setAutoindex(std::string& autoindex, char server_index)
{
	static std::string	set = "";

	if (set == this->getPath() + server_index)
		throw Config::ConfigException("Duplicate Autoindex declaration, only one allowed per location block");
	Config::validParamtr(autoindex);
	if (autoindex != "on" && autoindex != "off")
		throw Config::ConfigException("Invalid Autoindex, Off or On expected");
	this->_autoindex = (autoindex == "on");
	_has_autoindex = true;
	set = this->getPath() + server_index;
}


void Location::setIndex(std::string& index, char server_index)
{
	static std::string	set = "";

	if (set == this->getPath() + server_index)
		throw Config::ConfigException("Duplicate Index declaration, only one allowed per location block");
	Config::validParamtr(index);
	this->_index = index;
	set = this->getPath() + server_index;
}

void Location::setClientMaxBodySize(std::string& client_max_body_size, char server_index)
{
	static std::string	set = "";

	if (set == this->getPath() + server_index)
		throw Config::ConfigException("Duplicate ClientMaxBodySize declaration, only one allowed per location block");
	Config::validParamtr(client_max_body_size);
	for (size_t i = 0; i < client_max_body_size.size(); ++i)
		if (!std::isdigit(client_max_body_size[i]))
			throw Config::ConfigException("Body size must contain only numeric values");
	int int_body_size = std::atoi(client_max_body_size.c_str());
	this->_client_max_body_size = int_body_size;
	set = this->getPath() + server_index;
}

void Location::setAlias(std::string& alias, char server_index)
{
	static std::string	set = "";

	if (set == this->getPath() + server_index)
		throw Config::ConfigException("Duplicate Alias declaration, only one allowed per location block");
	Config::validParamtr(alias);
	this->_alias = alias;
	set = this->getPath() + server_index;
}

void Location::setRoot(std::string& root, char server_index)
{
    static std::string	set = "";

    if (set == this->getPath() + server_index)
        throw Config::ConfigException("Duplicate Root declaration, only one allowed per location block");
    Config::validParamtr(root);
    this->_root = root;
    set = this->getPath() + server_index;
}

void Location::setReturnValue(std::string& return_value, char server_index)
{
    static std::string	set = "";

    if (set == this->getPath() + server_index)
        throw Config::ConfigException("Duplicate ReturnValue declaration, only one allowed per location block");
    Config::validParamtr(return_value);
    this->_return_value = return_value;
    set = this->getPath() + server_index;
}

void Location::setAllowedMethods(std::string& allowed_methods, char server_index)
{
    static std::string	set = "";

	if (set == this->getPath() + server_index)
		throw Config::ConfigException("Duplicate AllowedMethods declaration, only one allowed per location block");
	Config::validParamtr(allowed_methods);
	size_t j = 0;
	for (size_t i = 0; i <= allowed_methods.size(); ++i)
	{
		if (allowed_methods[i] == ' ' || !allowed_methods[i])
		{
			std::string method = trim(allowed_methods.substr(j, i - j));
			if (method == "GET" || method == "POST" || method == "DELETE")
			{
				this->_allowed_methods[method] = true;
				j = i;
			}
			else
				throw Config::ConfigException("Invalid AllowedMethods, only GET, POST and DELETE are allowed");
		}
	}
	set = this->getPath() + server_index;
}

std::string Location::getPath() const
{
	return (this->_path);
}

std::string Location::getReturnValue() const
{
	return (_return_value);
}

std::string Location::getRoot() const
{
	return (_root);
}

std::string Location::getIndex() const
{
	return (_index);
}

size_t Location::getClientMaxBodySize() const
{
    return (_client_max_body_size);
}

bool Location::getAutoindex() const
{
	return (_autoindex);
}

bool Location::hasAutoindex() const
{
	return (_has_autoindex);
}

bool Location::isMethodAllowed(std::string &meth)
{
	return (_allowed_methods[meth]);
}