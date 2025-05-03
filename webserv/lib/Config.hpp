#pragma once
#include "webserv.hpp"
#include "Location.hpp"

class Config
{
	private:
		uint16_t					    	_port;
		std::string					    	_server_name;
		struct sockaddr_in 		    		_server_address;
		unsigned long			    		_client_max_body_size;
		std::map<short, std::string>    	_error_pages; //short: error code, string: path to error html
		std::string	    					_index;
		bool	    						_autoindex;
		std::string					    	_root;
		std::map<std::string, Location*>     _locations;

	public:
		Config();
		~Config();

		// Getters

		uint16_t getPort() const;
		std::string getServerName() const;
		struct sockaddr_in getServerAddress() const;
		unsigned long getClientMaxBodySize() const;
		std::string getIndex() const;
		bool getAutoindex() const;
		const std::map<short, std::string>& getErrorPages() const;
		std::string getRoot() const;

		// Setters
		void setPort(std::string& port);
		void setServerName(std::string& server_name);
		void setServerAddress(struct sockaddr_in& address);
		void setRoot(std::string& root);
		void setClientMaxBodySize(std::string& client_max_body_size);
		void setIndex(std::string& index);
		void setAutoindex(std::string& autoindex);
		void setErrorPages(std::map<short, std::string>& error_pages);
		void setLocation(std::string& location_name, Location& location);

		// Helpers
		static void validParamtr(std::string &paramtr);
		void initErrorPages();


		class ConfigException : public std::runtime_error {
		public:
			ConfigException(const std::string& msg)
			: std::runtime_error(msg) {}
		};
};