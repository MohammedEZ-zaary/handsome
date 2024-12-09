#ifndef HEADER_BASE
#define HEADER_BASE

#include "httpServer.hpp"
// Response Header Class
class responseHeader : public Header {
private:
  httpServer *serverRef; // Pointer to the httpServer
  // Private Method
  void setBody(const std::string &body);
  httpServer *getServerRef();

public:
  std::string statusCode = "200 OK"; // Default status code
  std::string body;                  // Response body

  // Constructor that accepts a reference to the server
  responseHeader(httpServer *server);
  void setStatusCode(std::string status);
  void sendData(const std::string &data);
  std::string getResponseString();
  void sendFile(const std::string &filePath);
};
#endif
