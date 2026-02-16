cmake_minimum_required(VERSION 3.31)
project(Server)

set(CMAKE_CXX_STANDARD 20)

# Specify the server source file
add_executable(Server server.cpp)

# Link libraries based on platform
if (WIN32)
    target_link_libraries(Server ws2_32)
endif()