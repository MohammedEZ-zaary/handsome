#include "../../../include/httpServer/headerParsing/form-data.hpp"
#include "../../../include/httpServer/fileManager/fileHandlerUtils.hpp"

#include "httpServer/httpServer.hpp"
#include <iostream>
#include <string>
#include <vector>

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
