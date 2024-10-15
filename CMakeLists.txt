cmake_minimum_required(VERSION 3.10)

project(Handsome VERSION 1.0) 

#Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set the output directory for the built library
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)

# Add the library target
add_library(handsomelib STATIC
    src/httpServerCpp/allContentTypeHeader.cpp
    src/httpServerCpp/httpServer.cpp
    src/httpServerCpp/headers.cpp
    src/handSomeCpp/handSome.cpp
)

# Specify include directories for the library
target_include_directories(handsomelib PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

