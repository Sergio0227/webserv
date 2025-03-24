#include <cstdlib>
#include <iostream>
#include <fstream>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    if (argc > 2)
    {
        std::cerr << "Error: Too many arguments \n" << "Usage -> ./webserv [configfile]" <<std::endl;
        return EXIT_FAILURE;
    }
    std::string config_file = "configs/default";
    if (argc == 2)
        config_file = argv[1];
    std::ifstream file(config_file.c_str());
}
