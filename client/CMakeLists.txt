cmake_minimum_required(VERSION 3.31)
project(Client)

set(CMAKE_CXX_STANDARD 20)

# Specify the client source file
add_executable(Client client.cpp)

# Link libraries based on platform
if (WIN32)
    target_link_libraries(Client ws2_32)
endif()