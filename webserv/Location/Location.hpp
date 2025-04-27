#pragma once

#ifndef LOCATION_HPP
#define LOCATION_HPP

#include "webserv.hpp"

class Location {
private:
    std::string                         _return_value;
    std::string                         _alias;
    unsigned long			    		_client_max_body_size;
    std::map<short, std::string>    	_error_pages;
    std::string	    					_index;
    bool	    						_autoindex;
    std::string					    	_root;


public:
    Location();
    ~Location();

    // Setters
    void setReturnValue(std::string& return_value);
    void setAlias(std::string& alias);
    void setRoot(std::string& root);
    void setClientMaxBodySize(std::string& client_max_body_size);
    void setIndex(std::string& index);
    void setAutoindex(std::string& autoindex);
    void setErrorPages(std::map<short, std::string>& error_pages);

    // Getters
    std::string getReturnValue() const;
    std::string getAlias() const;
    std::string getRoot() const;
    unsigned long getClientMaxBodySize() const;
    std::string getIndex() const;
    bool getAutoindex() const;
    const std::map<short, std::string>& getErrorPages() const;


    void initErrorPages();
};


#endif //LOCATION_HPP