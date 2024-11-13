#include <iostream>
#include <map>
#include <string>
#include "../../include/httpServer/allContentTypeHeader.hpp" 

// Function to get content type header based on the file path
std::string ContentTypeMapper::getContentTypeHeader(const std::string& filePath) {
    std::string extension = extractFileExtension(filePath); // Extract the extension
    auto it = contentTypeMap.find(extension);
    if (it != contentTypeMap.end()) {
        return it->second; // Return the corresponding content type
    } else {
        return "application/octet-stream"; // Default for unknown types
    }
}

// Method to extract file extension from the file path
std::string ContentTypeMapper::extractFileExtension(const std::string& filePath) {
    size_t pos = filePath.find_last_of(".");
    if (pos != std::string::npos) {
        return filePath.substr(pos); // Return the extension
    }
    return ""; // Return empty if no extension found
}