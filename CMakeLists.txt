cmake_minimum_required(VERSION 3.30)
project(webserv)

include_directories(
        ${CMAKE_SOURCE_DIR}/webserv/Server
        ${CMAKE_SOURCE_DIR}/webserv/Socket
        ${CMAKE_SOURCE_DIR}/webserv/ServerConfig
        ${CMAKE_SOURCE_DIR}/webserv/Brain
        ${CMAKE_SOURCE_DIR}/webserv/Location
        ${CMAKE_SOURCE_DIR}/webserv
)

set(CMAKE_CXX_STANDARD 98)

file(GLOB SOURCES
        "${CMAKE_SOURCE_DIR}/webserv/*.cpp"
        "${CMAKE_SOURCE_DIR}/webserv/Server/*.cpp"
        "${CMAKE_SOURCE_DIR}/webserv/Socket/*.cpp"
        "${CMAKE_SOURCE_DIR}/webserv/ServerConfig/*.cpp"
        "${CMAKE_SOURCE_DIR}/webserv/Brain/*.cpp"
        "${CMAKE_SOURCE_DIR}/webserv/Location/*.cpp"
)

add_executable(server ${SOURCES})