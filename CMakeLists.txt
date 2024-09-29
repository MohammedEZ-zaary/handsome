# Minimum CMake version required
cmake_minimum_required(VERSION 3.10)

# Project name and language
project(HttpServer CXX)

# Set the C++ standard to C++17 (or higher if required)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the include directories (adjust the path to your actual MongoDB installation)
include_directories(/usr/local/include/mongocxx/v_noabi)
include_directories(/usr/local/include/bsoncxx/v_noabi)
include_directories(/home/dev/Downloads/mongo-cxx-driver-r3.10.1/build/src/bsoncxx/third_party/_deps/core-install/include)

# Add the executable target (replace 'http_server.cpp' with your actual source files)
add_executable(http_server 
    src/main.cpp
    src/httpServer.cpp
    src/handSome.cpp
    src/allContentTypeHeader.cpp
)

# Link directories and libraries
link_directories(/usr/local/lib)
target_link_libraries(http_server
    PRIVATE
    mongocxx
    bsoncxx
)
