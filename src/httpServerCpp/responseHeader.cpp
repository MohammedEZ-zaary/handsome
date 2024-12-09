#include "../../include/httpServer/responseHeader.hpp"
#include "../../include/httpServer/allContentTypeHeader.hpp"
#include "../../include/httpServer/fileManager/fileHandlerUtils.hpp"
#include <sstream>
// win32
#ifdef _WIN32
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#endif

// linux
#ifdef __linux__
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h> // For close()
#endif

responseHeader::responseHeader(httpServer *server) : serverRef(server) {
  headers["Content-Type"] = "text/html";
  headers["Content-Length"] = "";
  headers["Connection"] = "keep-alive";
  headers["Server"] = "HandsomeServer/1.0";
}

void responseHeader::setStatusCode(std::string status) {
  this->statusCode = status;
}

void responseHeader::setBody(const std::string &body) { this->body = body; }

void responseHeader::sendData(const std::string &data) {
  setBody(data);
  setHeader("Content-Length", std::to_string(data.size()));
  std::string resAsString = getResponseString();

  send(getServerRef()->getClientSocketClone(), resAsString.c_str(),
       resAsString.size(), 0);
}

void responseHeader::sendFile(const std::string &filePath) {
  // add Error Handler
  ContentTypeMapper contentType;
  std::string CONTENT_TYPE_SELECTOR = contentType.getContentTypeHeader(
      filePath); // return text/html or application/json it deppend on file
                 // format
  std::string data = FileManager::readFileContent(filePath);
  setHeader("Content-Type", CONTENT_TYPE_SELECTOR);
  setHeader("Connection", "close");
  setHeader("Content-Length", std::to_string(data.size()));
  setStatusCode("200");
  // read content of file and put it on body Header
  setBody(data);
  std::string resAsString = getResponseString();
  send(getServerRef()->getClientSocketClone(), resAsString.c_str(),
       resAsString.size(), 0);
}

std::string responseHeader::getResponseString() {
  std::ostringstream responseStream;

  responseStream << "HTTP/1.1 " << this->statusCode << "\r\n";

  for (const auto &pair : headers) {
    responseStream << pair.first << ": " << pair.second << "\r\n";
  }

  responseStream << "\r\n"; // End of headers

  if (!this->body.empty()) {
    responseStream << this->body;
  }
  return responseStream.str();
}
httpServer *responseHeader::getServerRef() { return serverRef; }
