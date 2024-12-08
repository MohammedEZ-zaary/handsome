#ifndef FORM_DATA_HEADER
#define FORM_DATA_HEADER
#include "httpServer/httpServer.hpp"
#include <functional>
#include <string>

using std::string;

namespace Multipart_FormData {

;
struct FileInfo {
  string fileName;
  string fullBoundary;
  string boundaryId;
  string boundaryEnd;
  string filePath;
  bool isFileComplete = false;
  bool status = false;
};
string trim(const string &str);

string extractFullBoundary(string contentType);

FileInfo handleMultipartRequest(int clientSocket, const requestHeader &,
                                std::function<void(double)> progress);

bool isContentTypeFormData(const string &contentType);

string extractBoundaryNumber(string boundary);

string extractFileName(const string &buffer);

// void removeHttpHeaderFromFile(std::vector<char> proccessedBufferStore,
//                               char buffer);
void setFileBoundary(FileInfo &store, const requestHeader &request);

void removeHttpHeaderFromFile(std::vector<char> &bufferForProccess,
                              FileInfo &fileInfo);

void searchForBoundaryAndRemove(string filePath);
} // namespace Multipart_FormData
#endif
