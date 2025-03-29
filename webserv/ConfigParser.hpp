#pragma once

#ifndef CONFIGPARSER_H
#define CONFIGPARSER_H

#include <string>
#include <fstream>

class ConfigParser {
private:
    std::fstream    _file;
    std::string     _host;
    int             _port;
    std::string     _document_root;
    std::string     _index;

    void loadConfig();

public:
    ConfigParser(std::string const filePath);
    ~ConfigParser();
    std::string getHost();
    int getPort();
    std::string getDocumentRoot();
    std::string getIndex();
};


#endif //CONFIGPARSER_H