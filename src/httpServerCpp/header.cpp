#include "../../include/httpServer/header.hpp"
#include <iostream>
// Header Class
void Header::setHeader(const std::string &key, const std::string &value) {
  headers[key] = value;
}

std::string Header::getHeader(const std::string &key) const {
  auto it = headers.find(key);
  if (it != headers.end()) {
    return it->second;
  }
  return ""; // Return empty string if the header is not found
}

void Header::printHeaders() const {
  for (const auto &pair : headers) {
    std::cout << pair.first << ": " << pair.second << std::endl;
  }
}
