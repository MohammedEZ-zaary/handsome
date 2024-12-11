#ifndef FORM_DATA_HEADER
#define FORM_DATA_HEADER
#include "httpServer/requestHeader.hpp"
#include <functional>
#include <map>
#include <string>
#include <vector>

using std::string;
using std::vector;

namespace Multipart_FormData {

;
struct FileInfo {
  string fileName;
  string fullBoundary;
  string boundaryId;
  string boundaryEnd;
  string filePath;
  std::map<string, string> text;
  unsigned long long fileSize = 0;
  bool isFileComplete = false;
  bool status = false;
};

struct clientFinelFile {
  string fileName;
  string filePath;
  unsigned long long fileSize = 0;
  bool status = false;
  std::map<string, string> text;
};
string trim(const string &str);

std::string generateRandomString(size_t length);

string extractFullBoundary(string contentType);

std::vector<clientFinelFile>
handleMultipartRequest(int clientSocket, const requestHeader &, int memoryAlloc,
                       std::function<void(double)> progress);

bool isContentTypeFormData(const string &contentType);

string extractBoundaryNumber(string boundary);

string extractFileName(const string &buffer);

void setFileBoundary(FileInfo &store, const requestHeader &request);

void removeMetaDataFromBuffer(string &bufferForProccess);

void handleMultipleFiles(const requestHeader &request, FileInfo &fileInfo,
                         std::vector<clientFinelFile> &files,
                         vector<char> body);
// extract the fiald Name from Header Meta data
string extractFialdName(std::string &buffer);

bool isThereContentTypeInBuffer(const string &buffer);
} // namespace Multipart_FormData
#endif
