#pragma once

#include <string>
#include <map>

class Config;

class Location
{
    private:
        std::string							_path;
        std::string							_return_value;
        std::string							_alias;
        std::map<std::string, bool>			_allowed_methods;
        unsigned long						_client_max_body_size;
        std::map<short, std::string>		_error_pages;
        std::string							_index;
        bool								_autoindex;
		bool								_has_autoindex;
        std::string							_root;

    public:
		Location();
		Location(const Location& ref);
		~Location();

		// Setters
		void		setAlias(std::string& alias, int server_index);
		void		setRoot(std::string& root, int server_index);
		void		setClientMaxBodySize(std::string& client_max_body_size, int server_index);
		void		setReturnValue(std::string& return_value, int server_index);
		void		setIndex(std::string& index, int server_index);
		void		setAutoindex(std::string& autoindex, int server_index);
		void		setAllowedMethods(std::string& allowed_methods, int server_index);
		void		setErrorPages(std::map<short, std::string> &error_pages);
		std::string getPath() const;

		// Getters
		std::string							getReturnValue() const;
		std::string							getAlias() const;
		std::string							getRoot() const;
		unsigned long						getClientMaxBodySize() const;
		std::string							getIndex() const;
		bool								getAutoindex() const;
        bool								hasAutoindex() const;
        const std::map<short, std::string> &getErrorPages() const;
        bool								isMethodAllowed(std::string &meth);
		void								setPath(std::string &path);
};