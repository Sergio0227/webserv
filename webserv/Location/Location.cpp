#include "Location.hpp"

Location::Location()
{
    this->_return_value = "";
    this->_alias = "";
    this->_root = "";
    this->_client_max_body_size = MAX_CONTENT_LENGTH;
    this->_index = "";
    this->_autoindex = false;
    initErrorPages();
}

Location::~Location()
{
}

void Location::initErrorPages()
{
    this->_error_pages[301] = ""; //"Moved Permanently"
    this->_error_pages[302] = ""; //"Found"
    this->_error_pages[400] = ""; //"Bad Request"
    this->_error_pages[401] = ""; //"Unauthorized"
    this->_error_pages[403] = ""; //"Forbidden"
    this->_error_pages[404] = ""; //"Not Found"
    this->_error_pages[405] = ""; //"Method Not Allowed"
    this->_error_pages[500] = ""; //"Internal Server Error"
    this->_error_pages[501] = ""; //"Not Implemented"
    this->_error_pages[502] = ""; //"Bad Gateway"
    this->_error_pages[503] = ""; //"Service Unavailable"
}