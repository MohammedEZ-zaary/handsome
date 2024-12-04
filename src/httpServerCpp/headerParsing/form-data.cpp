#include "../../../include/httpServer/headerParsing/form-data.hpp"
#include "../../../include/httpServer/fileManager/fileHandlerUtils.hpp"

#include "httpServer/httpServer.hpp"
#include <cstring>
#include <iostream>
#include <string>
#include <vector>
// windows
#ifdef _WIN32
#include <winsock2.h>
#endif

// linux
#ifdef __linux__
#include <sys/socket.h>
#endif

using std::string;

namespace Multipart_FormData {
struct FileInfo {
  string fileName;
  int fileSize;
  string body;
  string fullBoundary;
  string boundaryId;
};

void parsingMultipartBody(const requestHeader &request,
                          const std::vector<char> &body) {
  FileInfo file;
  string bodyData = request.getHeader("Body");
  // get the Boundary form Content-Type header key
  string contentType = request.getHeader("Content-Type");
  string boundaryKey = "boundary=";
  file.fullBoundary =
      contentType.substr(contentType.find(boundaryKey) + boundaryKey.length());

  file.boundaryId = extractBoundaryNumber(file.fullBoundary);

  string bufferAsString(body.begin(), body.end());
  // std::cout << "boooday Data : " << bufferAsString << std::endl;
  // file.fileSize =
  //     bufferAsString.substr(0, bufferAsString.find(file.boundary + "--"))
  //         .length();
  // std::cout << "body :" << bufferAsString.rfind(file.boundaryId + "--")
  //           << std::endl;
  // std::cout << "Boundary id :" << file.boundaryId << std::endl;
  // std::cout << "Boundary full :" << file.fullBoundary << std::endl;

  // std::cout << "Here :"
  //           << bufferAsString.substr((bufferAsString.find("\r\n\r\n") + 2))
  //           << std::endl;
  if (!bodyData.empty()) {
    FileManager::saveFileBuffer(bufferAsString, extractFileName(bodyData));
  } else {
    FileManager::saveFileBuffer(
        bufferAsString.substr((bufferAsString.find("\r\n\r\n") + 4)),
        extractFileName(bufferAsString));
  }
}

string handleMultipartRequest(int clientSocket, const requestHeader &req) {
  std::vector<char> body;
  int bytesToRead = 0;
  bool isBufferFinsh = true;
  int bytesReceived = 0;
  char buffer[8192] = {0};
  // std::cout << req.getHeader("Body") << std::endl;
  // req.printHeaders();
  try {
    // start get the rest of the buffer
    // bytesReceived >= sizeof(buffer) || counter < bytesReceived
    //
    while (bytesToRead != std::stoi(req.getHeader("Content-Length"))) {

      bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);

      for (char c : buffer) {
        body.push_back(c);
      }

      if (bytesReceived <= 0) {
        std::cerr << "Error receiving data from client" << std::endl;
        std::string b(body.begin(), body.end());
        std::cout << b << std::endl;
        // clean and return varibles to default
        isBufferFinsh = false;
        bytesToRead = 0;
        // body.clear();
        std::memset(buffer, 0, sizeof(buffer));
        return "";
      }

      bytesToRead += bytesReceived;
      std::cout << "Read : " << bytesToRead << std::endl;
    }

    std::cout << "Bytes received: " << bytesReceived
              << ", Total bytes read: " << bytesToRead << std::endl;

    // clean and return varibles to default
    bytesToRead = 0;
    // body.clear();
    std::memset(buffer, 0, sizeof(buffer));

  } catch (const std::runtime_error &e) {
    bytesToRead = 0;
    // body.clear();
    std::memset(buffer, 0, sizeof(buffer));
    std::cout << "Error : " << &e << std::endl;
  }
  std::string bodyAsString(body.begin(), body.end());
  return bodyAsString;
};
int calculateFileLength(const string &buffer) {

  int calculatedSize;
  if (buffer.empty()) {
    return 0;
  }
  return calculatedSize;
}

bool isContentTypeFormData(const string &contentType) {
  return contentType.find("multipart/form-data") == 0;
}

string extractBoundaryNumber(string boundary) {
  /*
   * This function take boundary of file form header and return only the id of
   * the boundary example : input : ---------456545522154885212155 output:
   * 456545522154885212155
   * */
  return boundary.substr(boundary.rfind("-") + 1);
}

string extractFileName(const string &buffer) {
  FileInfo newFile;
  size_t contentDispositionIndex = buffer.find("Content-Disposition");
  size_t lineEnd = buffer.find("\n", contentDispositionIndex);
  // filename="hello.JPG"
  string contentDisposition =
      buffer.substr(contentDispositionIndex, lineEnd - contentDispositionIndex);
  string fileNameWithKeyAndValue =
      contentDisposition.substr(contentDisposition.find("filename=\""));
  string fileName =
      (fileNameWithKeyAndValue.substr(fileNameWithKeyAndValue.find("\"") + 1,
                                      fileNameWithKeyAndValue.length() - 12));
  return fileName;
}

} // namespace Multipart_FormData
