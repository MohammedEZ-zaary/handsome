#ifndef REQUEST_HEADER
#define REQUEST_HEADER
#include "header.hpp"
#include <map>
#include <string>

// Request Header
class requestHeader : public Header {
public:
  std::string method;  // GET POST DELETE PUT
  std::string uri;     // path
  std::string version; // HTTP/1.1
  std::map<std::string, std::string>
      queryParams;                              // req.queryParams["id"] = "123"
  std::map<std::string, std::string> queryBody; // req.queryParams["id"] = "123"
  std::string body;                 // req.body = "{\"name\": \"John\"}";
  std::string ipAddress;            // 192.168.11
  unsigned long long contentLength; // 300 bytes
  // init
  requestHeader();
  void cleanUpfunction();

private:
  void setDefaultValuesOfHeader();
};
#endif
