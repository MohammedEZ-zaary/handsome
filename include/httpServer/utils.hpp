#ifndef HTTPHEADER_UTILS
#define HTTPHEADER_UTILS

#include "httpServer.hpp"
#include <iostream>
#include <sstream>
#include <string>

using std::string;
/*
  httpUtilsString
    -> trim()
    -> extractRoute()
  httpParsingString
    -> parseHTTPRequest
    -> parseHttpHeaderRequest
  requestHandlerUtil
    -> handleRequestHeader()
    -> handleRequestBody()
    -> handleQueryParams()
    -> handleQueryBody()
    -> urlDecoded()
 */
namespace httpUtilsString {

string trim(const std::string &str) {
  // Function to trim whitespace from strings
  size_t first = str.find_first_not_of(' ');
  if (std::string::npos == first)
    return str; // If all whitespace
  size_t last = str.find_last_not_of(' ');
  return str.substr(first, (last - first + 1));
};

string extractRoute(const string &requestLine) {
  // Function to extract the route from the request line
  std::istringstream stream(requestLine);
  string method, route, httpVersion;

  // Parse the request line (method, route, version)
  stream >> method >> route >> httpVersion;

  // Return the extracted route
  return route;
};

}; // namespace httpUtilsString

namespace httpParsingString {
std::map<std::string, std::string>
parseHTTPRequest(const std::string &httpRequest) {
  // Function to parse the HTTP request and return key-value pairs
  std::map<std::string, std::string> headers;
  std::istringstream stream(httpRequest);
  std::string line;

  // First line (request line)
  if (std::getline(stream, line)) {
    headers["Request-Line"] = line;
  }

  // Following lines (headers)
  while (std::getline(stream, line) && line != "\r" && !line.empty()) {
    size_t delimiterPos = line.find(':');
    if (delimiterPos != std::string::npos) {
      std::string key = httpUtilsString::trim(line.substr(0, delimiterPos));
      std::string value = httpUtilsString::trim(line.substr(delimiterPos + 1));
      headers[key] = value;
    }
  }

  // Read the body (if present)
  std::string body;
  if (std::getline(stream, body, '\0')) {
    body = httpUtilsString::trim(body);
    if (!body.empty()) {
      headers["Body"] = body;
    }
  }

  return headers;
}
std::map<std::string, std::string>
parseHttpHeaderRequest(const std::string &request) {
  return parseHTTPRequest(request); // Forward to the other method
}
}; // namespace httpParsingString

namespace requestHandlerUtil {

string urlDecoded(const std::string &encoded);
void handleQueryBody(requestHeader &req, const std::string &value);
void handleQueryParams(requestHeader &req, const std::string &value);

void handleRequestHeader(requestHeader &req, const std::string &key,
                         const std::string &value) {

  // handle Request Header
  // arr[0]  , arr[1], arr[2]
  // [Method , Path or URI , http Version]
  std::string arr[3];
  if (key == "Request-Line") {
    size_t firstSpace = value.find(' ');
    size_t lastSpace = value.rfind(' ');
    size_t questionMark = value.find('?'); // Find the '?'

    arr[0] = value.substr(0, firstSpace);
    arr[2] = value.substr(lastSpace + 1);

    if (questionMark != std::string::npos) {
      arr[1] = value.substr(
          firstSpace + 1,
          questionMark -
              (firstSpace + 1)); // Get the substring between first space and ?
      // As we know the params are only accept it on GET PUT HEAD AND DELET
      if (arr[0] != "POST") {
        handleQueryParams(req, value);
      } else {
        std::cout << "\033[1;31m[!] Warning you cannot pass param in POST "
                     "Method. \033[0m\n";
        std::cout << "\033[1;34m[+] Try to Edit Route : " << arr[1]
                  << "\033[0m\n";
      }
    } else {
      arr[1] = value.substr(firstSpace + 1,
                            value.find(' ', firstSpace + 1) -
                                (firstSpace + 1)); // Handle case without '?'
    }

    req.method = arr[0];
    req.uri = arr[1];
    req.version = arr[2];
  }

  // Handle Content-Type : application/x-www-form-urlencoded Body Data
  if (key == "Body") {
    // arr[0] = Method POST , PUT ...
    if (!value.empty()) {
      handleQueryBody(req, value);
    }
  }
}

void handleRequestBody(requestHeader &req, const std::string &value) {
  // Those the only method that allow to accept body
  if (req.method == "POST" || req.method == "PUT" || req.method == "PATCH") {
    req.body = httpUtilsString::trim(value);
  } else {
    std::cout << "\033[1;31m[!] Warning your trying to send data in Body "
                 "header using GET method. \033[0m\n";
    std::cout << "\033[1;34m[+] Try to use POST method in Route " << req.uri
              << "\033[0m\n";
  }
}
void handleQueryParams(requestHeader &req, const std::string &value) {
  // handle query params if only method is not a POST request
  size_t questionMark = value.find('?'); // Find the '?'
  if (questionMark != std::string::npos) {
    // Get the substring containing the parameters
    std::string paramString = value.substr(
        questionMark + 1, value.find(' ', questionMark) - (questionMark + 1));

    // Split parameters by '&'
    std::stringstream ss(paramString);
    std::string item;
    while (std::getline(ss, item, '&')) {
      // Split key and value by '='
      size_t equalsPos = item.find('=');
      if (equalsPos != std::string::npos) {
        std::string key = item.substr(0, equalsPos);
        std::string value = item.substr(equalsPos + 1);
        req.queryParams[key] = urlDecoded(value); // Store in the map
      }
    }
  }
}
void handleQueryBody(requestHeader &req, const std::string &value) {
  // if the value start with json character we will not handle it as Query Body
  // like : username=handsome&password=Handsome2024
  // you can access json Directliy by using req.body
  if (value[0] == '{' || value[0] == '}') {
    return;
  }
  // Handle Query Body like : username=handsome&password=Handsome2024
  // Split parameters by '&'
  std::stringstream ss(value);
  std::string item;
  while (std::getline(ss, item, '&')) {
    // Split key and value by '='
    size_t equalsPos = item.find('=');
    if (equalsPos != std::string::npos) {
      std::string key = item.substr(0, equalsPos);
      std::string value = item.substr(equalsPos + 1);
      req.queryBody[key] = urlDecoded(value); // Store in the map
    }
  }
}
string urlDecoded(const std::string &encoded) {
  std::ostringstream decoded;
  for (size_t i = 0; i < encoded.length(); ++i) {
    if (encoded[i] == '+') {
      decoded << ' '; // Convert + to space
    } else if (encoded[i] == '%' && i + 2 < encoded.length()) {
      // Convert %XX to the corresponding character
      std::string hex = encoded.substr(i + 1, 2);
      decoded << static_cast<char>(std::stoi(hex, nullptr, 16));
      i += 2; // Skip past the two hex digits
    } else {
      decoded << encoded[i];
    }
  }
  return decoded.str();
}
}; // namespace requestHandlerUtil
#endif
