# Minimum CMake version required
cmake_minimum_required(VERSION 3.10)

# Project name and language
project(HttpServer CXX)

# Set the C++ standard to C++11 (or higher if required)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the executable target (replace 'http_server.cpp' with your source file name)
add_executable(http_server 
    src/httpServer.cpp
    src/handSome.cpp
    src/allContentTypeHeader.cpp
    src/main.cpp
    )

# Link any necessary libraries (on Linux, the socket and networking functionality is part of libc)
# No need to explicitly link libraries like -lnet or -lsocket on Linux
