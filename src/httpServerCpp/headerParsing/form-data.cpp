#include "../../../include/httpServer/headerParsing/form-data.hpp"
#include "httpServer/fileManager/fileHandlerUtils.hpp"
#include <cstdio>
#include <cstring>
#include <functional>
#include <iostream>
#include <random>
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

using namespace std;

namespace Multipart_FormData {

string trim(const string &str) {
  // Find the position of the first non-whitespace character
  size_t start = str.find_first_not_of(" \t\n\r");

  // If the string is all whitespace, return an empty string
  if (start == string::npos) {
    return "";
  }

  // Find the position of the last non-whitespace character
  size_t end = str.find_last_not_of(" \t\n\r");

  // Return the trimmed string
  return str.substr(start, end - start + 1);
}
string extractBoundaryNumber(string boundary) {
  // If the boundary starts with '--', remove the leading '--'

  if (boundary.find("--", 0) == 0) {
    boundary = boundary.substr(4); // Remove the leading '--'
  }
  return boundary;
}

string extractFullBoundary(string contentType) {
  string boundaryKey = "boundary=";
  size_t start = contentType.find(boundaryKey) + boundaryKey.length();
  return trim(contentType.substr(start));
}

std::string generateRandomString(size_t length) {
  const std::string characters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                 "abcdefghijklmnopqrstuvwxyz"
                                 "0123456789";

  std::random_device rd;        // Seed generator
  std::mt19937 generator(rd()); // Random number generator
  std::uniform_int_distribution<> distribution(0, characters.size() - 1);

  std::string randomString;
  for (size_t i = 0; i < length; ++i) {
    randomString += characters[distribution(generator)];
  }

  return randomString;
}

std::vector<clientFinelFile>
handleMultipartRequest(int clientSocket, const requestHeader &req,
                       int memoryAlloc, std::function<void(double)> progress) {
  // handleMultipleFiles varibles
  vector<clientFinelFile> files;
  string textFaild;
  // End of handleMultipleFiles varibles

  //  main varibles
  vector<char> body;
  // every bytes received  from socket coutered by this varible
  unsigned long long bytesToRead = 0;
  unsigned long long bytesReceived = 0;
  // check if we received the first chunk of file buffer. if yes then :
  // tempSaveBufferOnFileCounter must be  > 1
  int tempSaveBufferOnFileCounter = 0;

  const size_t bufferSize = 8000; // 8KB buffer size
  char buffer[bufferSize] = {0};
  // content-Length
  unsigned long long contentLengthOfTheFile =
      static_cast<unsigned long long>(req.contentLength);
  // the progress of the upload file example start from
  // 0%.....100% file complete received
  double percentage = 0;
  // in this struct we store all data that belong the file;
  FileInfo fileInformation;
  // isDataComplete true only when we received full buffer of the file
  bool isDataComplete = false;
  string requestBody = req.getHeader("Body");
  // set capacity of vector<char> that hold the buffer
  body.reserve(25 * 1024 * 1024);
  if (memoryAlloc < 1) {
    memoryAlloc = 1;
  }
  if (memoryAlloc > bufferSize) {
    memoryAlloc = bufferSize;
  }
  if (req.method != "POST") {
    return files;
  }

  if (!requestBody.empty()) {
    contentLengthOfTheFile = (req.contentLength - requestBody.length());
  }

  if (contentLengthOfTheFile <= 0) {
    std::cerr << "[Warning] No file provided.\n"
              << "This route expects a file to be uploaded.\n"
              << "If file upload is not required, please remove the function "
                 "call'saveMultiPartFile()' from route :"
              << req.uri << std::endl;
    return files;
  }

  try {
    // start get the rest of the buffer
    if (!requestBody.empty()) {
      // this is more faster then body.push_back();
      body.insert(body.end(), requestBody.begin(), requestBody.end());
    }
    while (bytesToRead != contentLengthOfTheFile) {
      bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
      if (bytesReceived <= 0) {
        if (bytesReceived == 0) {
          FileManager::removeFile(fileInformation.fileName);
          cerr << "Client disconnected" << endl;
          break;
        } else {
          perror("recv failed");
        }
        break;
      }

      if (bytesReceived > 0) {
        // this is more faster then body.push_back();
        try {

          body.insert(body.end(), buffer, buffer + bytesReceived);

        } catch (const runtime_error &e) {
          std::cerr << "Error Here : " << &e << endl;
        }

        bytesToRead += bytesReceived;

        if (bytesToRead % (sizeof(buffer) * memoryAlloc) == 0 ||
            // cout << "MemeAlloc : " << (sizeof(buffer) * memoryAlloc)
            // << endl;
            bytesToRead == contentLengthOfTheFile) {
          percentage = (static_cast<double>(bytesToRead) * 100.0) /
                       contentLengthOfTheFile;

          progress(percentage);

          handleMultipleFiles(req, fileInformation, files, body);
          body.clear();
        }
      }
    }
    return files;
  } catch (const runtime_error &e) {
    bytesToRead = 0;
    body.clear();
    cout << "Error : " << &e << endl;
  }
  return files;
};
void handleMultipleFiles(const requestHeader &request, FileInfo &fileInfo,
                         vector<clientFinelFile> &files, vector<char> body) {

  vector<int> boundaries;
  setFileBoundary(fileInfo, request);
  string bodyAsString(body.begin(), body.end());

  int fileContent = bodyAsString.find("--" + fileInfo.fullBoundary);
  while (fileContent != string::npos) {
    // check if the boundary is not the end boundary
    if (fileContent != bodyAsString.find(fileInfo.boundaryEnd)) {

      boundaries.push_back(fileContent);

      fileContent =
          bodyAsString.find("--" + fileInfo.fullBoundary, fileContent + 1);
    } else {
      // If we found the boundary end that mean we have received the last chunk
      // of our data
      break;
    }
  }
  if (!boundaries.empty()) {
    string fileContent;

    for (int i = 0; i < boundaries.size(); i++) {
      if (boundaries.size() < 2) {
        fileContent = bodyAsString.substr(boundaries[i]);
      } else {
        if (i + 1 < boundaries.size()) {
          fileContent = bodyAsString.substr(boundaries[i],
                                            boundaries[i + 1] - boundaries[i]);
        } else {

          fileContent = bodyAsString.substr(boundaries[i]);
        }
      }
      try {

        fileInfo.fileName = extractFileName(fileContent);
        if (fileInfo.fileName != "No fileName provided") {
          clientFinelFile newFile;
          newFile.fileName = fileInfo.fileName;
          newFile.status = true;
          newFile.filePath = "";
          files.push_back(newFile);
          removeMetaDataFromBuffer(fileContent);
        } else {
          // if there no filename in meta data in the header then we are
          // handling text return;
          // clientFinelFile textFile;
          // string fialdName = extractFialdName(fileContent);
          // removeMetaDataFromBuffer(fileContent);
          // textFile.text.insert({fialdName, fileContent});
          // files.push_back(textFile);
          // return;
        }

      } catch (...) {
        cout << "Errror file name not found" << endl;
      }
      // check if there any end boundary mess
      size_t eBoundary = fileContent.find(fileInfo.boundaryEnd);
      if (eBoundary != string::npos) {
        FileManager::saveFileBuffer(fileContent.substr(0, eBoundary),
                                    fileInfo.fileName);
        return;
      } else {

        FileManager::saveFileBuffer(fileContent, fileInfo.fileName);
      }
    }
  }

  if (boundaries.empty()) {
    int boundaryEnd = bodyAsString.find(fileInfo.boundaryEnd);
    if (boundaryEnd != string::npos) {
      FileManager::saveFileBuffer(bodyAsString.substr(0, boundaryEnd),
                                  fileInfo.fileName);
      return;
    }
    FileManager::saveFileBuffer(bodyAsString, fileInfo.fileName);
  }
};

bool isContentTypeFormData(const string &contentType) {
  return contentType.find("multipart/form-data") == 0;
}
bool isThereContentTypeInBuffer(const string &buffer) {
  if (buffer.find("Content-Type:") != string::npos) {
    return true;
  }

  return false;
}
string extractFileName(const string &buffer) {
  size_t contentDispositionIndex = buffer.find("Content-Disposition");
  size_t lineEnd = buffer.find("\n", contentDispositionIndex);
  string contentDisposition =
      buffer.substr(contentDispositionIndex, lineEnd - contentDispositionIndex);
  try {
    string fileNameWithKeyAndValue =
        contentDisposition.substr(contentDisposition.find("filename=\""));
    string fileName =
        (fileNameWithKeyAndValue.substr(fileNameWithKeyAndValue.find("\"") + 1,
                                        fileNameWithKeyAndValue.length() - 12));
    if (fileName.empty()) {
      return "No fileName provided";
    }
    return (generateRandomString(10) + fileName);
  } catch (...) {

    return "No fileName provided";
  }
};

void removeMetaDataFromBuffer(string &bufferForProccess) {
  string buffAsString = bufferForProccess;
  string fileBody = buffAsString.substr(buffAsString.find("\r\n\r\n") + 4);
  bufferForProccess.clear();
  bufferForProccess = fileBody;
};

void setFileBoundary(FileInfo &store, const requestHeader &request) {
  // extract file information
  store.fullBoundary = extractFullBoundary(request.getHeader("Content-Type"));
  store.boundaryId = extractBoundaryNumber(store.fullBoundary);
  store.boundaryEnd = "--" + store.fullBoundary + "--";
}
string extractFialdName(std::string &buffer) {
  size_t contentDispositionIndex = buffer.find("Content-Disposition");
  size_t lineEnd = buffer.find("\n", contentDispositionIndex);
  string contentDisposition =
      buffer.substr(contentDispositionIndex, lineEnd - contentDispositionIndex);
  try {
    string fialdNameWithKeyAndValue =
        contentDisposition.substr(contentDisposition.find("name="));
    string fialdName = fialdNameWithKeyAndValue.substr(
        fialdNameWithKeyAndValue.find("\"") + 1,
        fialdNameWithKeyAndValue.size() - fialdNameWithKeyAndValue.find("\"") -
            3);
    return fialdName;
  } catch (...) {

    return "";
  }
  return "";
}
} // namespace Multipart_FormData
