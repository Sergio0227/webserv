cmake_minimum_required(VERSION 3.30)
project(webserv)

include_directories(
        ${CMAKE_SOURCE_DIR}/webserv/Client
        ${CMAKE_SOURCE_DIR}/webserv/Server
        ${CMAKE_SOURCE_DIR}/webserv/Socket
        ${CMAKE_SOURCE_DIR}/webserv/Config
        ${CMAKE_SOURCE_DIR}/webserv
)

set(CMAKE_CXX_STANDARD 98)

add_executable(webserv webserv/main_server.cpp
                        webserv/Client/Client.cpp
                        webserv/Server/HttpServer.cpp
                        webserv/Socket/Socket.cpp
                        webserv/utils.cpp)
