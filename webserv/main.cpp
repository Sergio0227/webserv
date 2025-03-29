#include <cstdlib>
#include <iostream>
#include "ConfigParser.hpp"

int main(int argc, char *argv[])
{
    if (argc > 2)
    {
        std::cerr << "Error: Too many arguments \n" << "Usage -> ./webserv [configfile]" <<std::endl;
        return EXIT_FAILURE;
    }
    std::string config_file_path = "configs/default.conf";
    if (argc == 2)
        config_file_path = argv[1];
    try
    {
        ConfigParser config(config_file_path);
    }
    catch (const std::runtime_error &e)
    {
        std::cout << "Error: " << e.what() << std::endl;
    }
}
