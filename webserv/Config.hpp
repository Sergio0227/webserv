#pragma once

#ifndef CONFIGPARSER_H
#define CONFIGPARSER_H

#include <string>
#include <fstream>

class Config {
private:
    std::string     _host;
    int             _port;
    std::string     _document_root;
    std::string     _index;

    void loadConfig();

public:
    Config(std::string const filePath);
    ~Config();
    std::string getHost();
    int getPort();
    std::string getDocumentRoot();
    std::string getIndex();
};


#endif //CONFIGPARSER_H