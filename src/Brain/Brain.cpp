#include "Brain.hpp"
#include <stdexcept>

Brain::Brain(std::vector<std::string>& config_file)
{
	_nb_servers = 0;
    _max_fd = 0;
	_server_conf.resize(0);
	_config_files.resize(0);
	FD_ZERO(&_recv_fd_set);
    FD_ZERO(&_send_fd_set);

	splitServers(config_file);
	initServerConfigs();
	setupServers();
	handleConnections();
}

void Brain::handleConnections()
{
	int j;
	fd_set  recv_fd_set_cpy;
	fd_set  send_fd_set_cpy;
	struct timeval time;
	int ready;

	while (g_flag)
	{
		time.tv_sec = 1;
		time.tv_usec = 0;
		recv_fd_set_cpy = _recv_fd_set;
		send_fd_set_cpy = _send_fd_set;

		ready = select(_max_fd + 1, &recv_fd_set_cpy, &send_fd_set_cpy, NULL, &time);
		if (ready == 0)
			continue;
		if (ready < 0)
		{
			if (errno == EINTR)
				continue;
			else
				throw std::runtime_error(std::string("select() failed: ") + strerror(errno));
		}	
		for (int i = 0; i <= _max_fd; ++i)
		{

			if (FD_ISSET(i, &recv_fd_set_cpy) && ((j = isServerFd(i)) != INT_MAX))
			{
				int client_fd = _servers[j]->acceptConnections();
				if (client_fd < 0)
					continue;
				setNonBlockingFD(client_fd);
				addFdToSet(client_fd, _recv_fd_set);
				_client_to_serv_map[client_fd] = _servers[j];
			}
			else if (FD_ISSET(i, &recv_fd_set_cpy))
			{
				HttpServer* server = _client_to_serv_map[i];
				HttpResponse res = server->handleRequest(i);
				_pending_responses.insert(std::make_pair(i, res));
				
				addFdToSet(i, _send_fd_set);
				removeFdFromSet(i, _recv_fd_set);
			}
			else if (FD_ISSET(i, &send_fd_set_cpy))
			{
				HttpServer* server = _client_to_serv_map[i];
				std::map<int, HttpResponse>::iterator it = _pending_responses.find(i);
				if (it != _pending_responses.end())
				{
					HttpResponse &res = it->second;
					res.sendResponse();
					if (res.getError())
					{
						removeFdFromSet(i, _recv_fd_set);
						close(i);
						_client_to_serv_map.erase(i);
						_pending_responses.erase(i);
						server->eraseClientFromMap(i);
						continue;
					}
					else
					{
						_pending_responses.erase(i);
						addFdToSet(i, _recv_fd_set);
					}
					removeFdFromSet(i, _send_fd_set);
				}
				else
				{
					std::cerr << "[ERROR] FD " << i << " not found in _pending_responses." << std::endl;
				}
				
			}
		}
	}
}

// void Brain::handleConnections()
// {
// 	int j;

// 	while (g_flag)
// 	{
// 		_rdy_sockets = _cur_sockets;
// 		if (select(FD_SETSIZE, &_rdy_sockets, NULL, NULL, NULL) < 0)
// 		{
// 			if (errno == EINTR)
// 				continue;
// 			else
// 				throw std::runtime_error("Error: select");
// 		}	
// 		for (int i = 0; i < FD_SETSIZE; ++i)
// 		{
// 			if (FD_ISSET(i, &_rdy_sockets))
// 			{
// 				if ((j = isServerFd(i)) != INT_MAX)
// 				{
// 					int client_fd = _servers[j]->acceptConnections();
// 					if (client_fd < 0)
// 						continue;
// 					FD_SET(client_fd, &_cur_sockets);
// 					_client_to_serv_map[client_fd] = _servers[j];
// 				}
// 				else
// 				{
// 					HttpServer* server = _client_to_serv_map[i];
// 					if (!server->handleRequest(i))
// 					{
// 						FD_CLR(i, &_cur_sockets);
// 						_client_to_serv_map.erase(i);
// 					}
// 				}
// 			}
// 		}
// 	}
// } 

int Brain::isServerFd(int fd)
{
	for(size_t i = 0; i < _server_sockets.size(); i++)
	{
		if (fd == _server_sockets[i])
			return (i);
	}
	return (INT_MAX);
}

void Brain::setupServers()
{
	for(int i = 0; i < _nb_servers; i++)
	{
		HttpServer *server = new HttpServer(_server_conf[i], _server_conf[i]->getPort(),_server_conf[i]->getServerName(),  BACKLOG, true);
		_servers.push_back(server);
		int server_fd = _servers[i]->getSocket();
		_server_sockets.push_back(server_fd);
        setNonBlockingFD(server_fd);
        addFdToSet(server_fd, _recv_fd_set);
	}
}

Brain::~Brain()
{
	int k;
	for (int i = 0; i < _nb_servers; i++)
		delete _server_conf[i];

	for (int i = 0; i < FD_SETSIZE; ++i)
	{
		if (FD_ISSET(i, &_recv_fd_set) || FD_ISSET(i, &_send_fd_set))
		{
			if ((k = isServerFd(i)) != INT_MAX)
				logMessage(INFO, i, "Server connection closed.", NULL, 0);
			else
			{
				HttpServer *server = _client_to_serv_map[i];
				std::ostringstream oss;
				oss << i;
				const std::string msg =  "Client_ID: " + oss.str() + " | Connection closed.";
				logMessage(INFO, server->getSocket(), msg, NULL, 0);
			}
			close(i);
			removeFdFromSet(i, _recv_fd_set);
			removeFdFromSet(i, _send_fd_set);
		}
	}
	logMessage(SUCCESS, -1 , "Server(s) have been successfully shut down. All connections closed.", NULL, 0);
	for (int i = 0; i < _nb_servers; i++)
		delete _servers[i];
}

void Brain::splitServers(std::vector<std::string> config_file)
{
    size_t i = 0;
    int nested;
    bool in_server_scope;

    while (i < config_file.size())
    {
        nested = 0;
        in_server_scope = false;
        this->_nb_servers++;
        this->_config_files.resize(this->_nb_servers);
        while (i < config_file.size())
        {
            if (config_file[i] == "server")
            {
                in_server_scope = true;
                i++;
            }
            if ((in_server_scope && config_file[i] == "server")
                || (!in_server_scope && config_file[i] != "server")
                || (config_file[i] == "{" && config_file[i + 1] == "{")
                || (config_file[i] == "{" && config_file[i + 1] == "}"))
                throw std::runtime_error("Error: Wrong character out of server scope");
            if (config_file[i] == "{")
                nested++;
            if ((config_file[i] != "}" && config_file[i] != "{") || nested > 1)
                this->_config_files[this->_nb_servers - 1].push_back(config_file[i]);
            if (config_file[i] == "}")
                nested--;
            i++;
            if (nested == 0)
                break;
        }
        if (nested)
            throw std::runtime_error("Error: Unclosed bracket");
    }
}

void Brain::initServerConfigs()
{
	this->_server_conf.resize(this->_nb_servers);
	for (int i = 0; i < this->_nb_servers; i++)
	{
		this->_server_conf[i] = new Config();
		parseConfigFile(i);
	}
}

void Brain::parseConfigFile(int server_index)
{
     std::string valid_params[] = {"server_name", "listen", "root", "client_max_body_size",
        "index", "autoindex", "error_page", "location", "allow_methods"};
    for (size_t i = 0; i < this->_config_files[server_index].size() ; i++)
    {
        size_t j;
        std::string param = trim(this->_config_files[server_index][i].substr(0, this->_config_files[server_index][i].find(' ')));
        if (this->_config_files[server_index][i].size() == param.size())
            throw Config::ConfigException("Error: Wrong parameter near: " + param);
        std::string value = trim(this->_config_files[server_index][i].substr(param.size() + 1, this->_config_files[server_index][i].size()));
        if (param == "location")
            parseLocation(&(++i), server_index, value);
        for (j = 0; valid_params[j] != param; j++)
            if (j > param.size() || value == ";")
               throw Config::ConfigException("Error: Wrong parameter near: " + param);
        if (param == "server_name")
            this->_server_conf[server_index]->setServerName(value);
        else if (param == "listen")
            this->_server_conf[server_index]->setPort(value);
        else if (param == "root")
            this->_server_conf[server_index]->setRoot(value);
        else if (param == "client_max_body_size")
            this->_server_conf[server_index]->setClientMaxBodySize(value);
        else if (param == "index")
            this->_server_conf[server_index]->setIndex(value);
        else if (param == "autoindex")
            this->_server_conf[server_index]->setAutoindex(value);
        }
}

void Brain::parseLocation(size_t *i, int server_index, std::string location_name)
{
    std::string valid_params[] = {"root","index", "autoindex", "error_page", "alias", "allow_methods",
        "client_max_body_size", "return", "cgi_path", "cgi_ext"};

    this->_locations_keys.push_back(location_name);
    Location ref_loc;
    ref_loc.setPath(location_name);
    while (this->_config_files[server_index][++(*i)] != "}")
    {
        size_t j;
        std::string param = trim(this->_config_files[server_index][*i].substr(0, this->_config_files[server_index][*i].find(' ')));
        std::string value= trim(this->_config_files[server_index][*i].substr(param.size() + 1, this->_config_files[server_index][*i].size()));
        for (j = 0; valid_params[j] != param; j++)
            if (j > param.size())
               throw Config::ConfigException("Error: Wrong parameter near: " + param);
        if (param == "allow_methods")
            ref_loc.setAllowedMethods(value);
        else if (param == "root")
            ref_loc.setRoot(value);
        else if (param == "index")
            ref_loc.setIndex(value);
        else if (param == "autoindex")
            ref_loc.setAutoindex(value);
        else if (param == "alias")
            ref_loc.setAlias(value);
        else if (param == "return")
            ref_loc.setReturnValue(value);
        else if (param == "client_max_body_size")
            ref_loc.setClientMaxBodySize(value);
    }
    this->_server_conf[server_index]->setLocation(location_name, ref_loc);
}

int Brain::getNbServers()
{
    return this->_nb_servers;
}

void	Brain::addFdToSet(int fd, fd_set &fd_set)
{
	FD_SET(fd, &fd_set);
	if (fd > _max_fd)
		_max_fd = fd;
}

void	Brain::removeFdFromSet(int fd, fd_set &fd_set)
{
	FD_CLR(fd, &fd_set);
	for (int i = FD_SETSIZE - 1; i >= 0; --i)
	{
			if (FD_ISSET(i, &_recv_fd_set) || FD_ISSET(i, &_send_fd_set))
			{
				_max_fd = i;
				break;
			}
	}
}