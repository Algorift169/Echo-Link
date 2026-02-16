cmake_minimum_required(VERSION 3.31)
project(Client)

set(CMAKE_CXX_STANDARD 20)

# Specify the client source file
add_executable(Client main.cpp)

# Link Winsock2 library for networking on Windows
target_link_libraries(Client ws2_32)

