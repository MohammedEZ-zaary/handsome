#include "../../../include/httpServer/headerParsing/form-data.hpp"
#include "httpServer/fileManager/fileHandlerUtils.hpp"
#include "httpServer/httpServer.hpp"
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

FileInfo handleMultipartRequest(int clientSocket, const requestHeader &req,
                                std::function<void(double)> progress) {
  cout << "hello one time" << endl;
  vector<char> body;
  int bytesToRead =
      0; // every bytes received  from socket coutered by this varible
  int bytesReceived = 0; //
  int tempSaveBufferOnFileCounter =
      0; // check if we received the first chunk of file buffer. if yes then :
         // tempSaveBufferOnFileCounter must be  > 1
  char buffer[2096] = {0};
  long long contentLengthOfTheFile =
      static_cast<int>(req.contentLength); // content-Length
  double percentage = 0; // the progress of the upload file example start from
                         // 0%.....100% file complete received
  FileInfo
      fileInformation; // in this struct we store all data that belong the file;
  bool isDataComplete = false;
  string requestBody = req.getHeader("Body");

  if (!requestBody.empty()) {
    contentLengthOfTheFile = (req.contentLength - requestBody.length());
  }
  try {

    // start get the rest of the buffer
    if (!requestBody.empty()) {
      for (char c : requestBody) {
        body.push_back(c);
      }
    }
    // cout << "file Length : " << contentLengthOfTheFile << endl;
    // bytesToRead != contentLengthOfTheFile

    string r = generateRandomString(5);
    while (bytesToRead != contentLengthOfTheFile || isDataComplete) {
      bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
      for (char c : buffer) {
        body.push_back(c);
      }

      if (bytesReceived <= 0) {
        cerr << "Error receiving data from client" << endl;
        return fileInformation;
      }

      if (bytesReceived > 0) {
        bytesToRead += bytesReceived;
        // cout << "Read : " << bytesToRead << endl;

        tempSaveBufferOnFileCounter += 1;
        if (tempSaveBufferOnFileCounter == 1) {
          // varibles
          string bodyBuffer(body.begin(), body.end());
          // set the file name
          fileInformation.fileName = extractFileName(bodyBuffer);
          // extract boundary
          setFileBoundary(fileInformation, req);

          removeHttpHeaderFromFile(body, fileInformation);
        }

        // add comment
        //
        if (bytesToRead % (sizeof(buffer) * 5) == 0 ||
            bytesToRead == contentLengthOfTheFile) {

          percentage = (static_cast<double>(bytesToRead) * 100.0) /
                       contentLengthOfTheFile;
          // call callback progress function to send the progress of the upload
          progress(percentage);

          string bodyAsString(body.begin(), body.end());
          // start filer
          size_t targetBoundary =
              bodyAsString.find(fileInformation.boundaryEnd);
          if (targetBoundary != std::string::npos) {
            FileManager::saveFileBuffer(bodyAsString.substr(0, targetBoundary),
                                        (r + fileInformation.fileName));
            isDataComplete = true;
          } else {
            // cout << bodyAsString << endl;
            FileManager::saveFileBuffer(bodyAsString,
                                        (r + fileInformation.fileName));
          }
          // after proccessing the buffer on memeroy aka(char buffer) and save
          // it in file we delet that part from buffer[***] for allowing to next
          // new buffer to proccess
          body.clear();
        }
        if (bytesToRead == contentLengthOfTheFile) {
          fileInformation.status = true;
          bytesToRead = 0;
          body.clear();
          memset(buffer, 0, sizeof(buffer));

          cout << "......Upload Completed ....." << endl;
          return fileInformation;
        }
      }
    }
  } catch (const runtime_error &e) {
    bytesToRead = 0;
    body.clear();
    memset(buffer, 0, sizeof(buffer));
    cout << "Error : " << &e << endl;
  }
  return fileInformation;
};

bool isContentTypeFormData(const string &contentType) {
  return contentType.find("multipart/form-data") == 0;
}

string extractFileName(const string &buffer) {
  FileInfo newFile;
  size_t contentDispositionIndex = buffer.find("Content-Disposition");
  size_t lineEnd = buffer.find("\n", contentDispositionIndex);
  string contentDisposition =
      buffer.substr(contentDispositionIndex, lineEnd - contentDispositionIndex);

  string fileNameWithKeyAndValue =
      contentDisposition.substr(contentDisposition.find("filename=\""));
  string fileName =
      (fileNameWithKeyAndValue.substr(fileNameWithKeyAndValue.find("\"") + 1,
                                      fileNameWithKeyAndValue.length() - 12));
  return fileName;
};
void removeHttpHeaderFromFile(vector<char> &bufferForProccess,
                              FileInfo &fileInfo) {
  string buffAsString(bufferForProccess.begin(), bufferForProccess.end());
  string fileBody = buffAsString.substr(buffAsString.find("\r\n\r\n") + 4);
  bufferForProccess.clear();
  for (char c : fileBody) {
    bufferForProccess.push_back(c);
  }
};

void setFileBoundary(FileInfo &store, const requestHeader &request) {
  // extract file information
  store.fullBoundary = extractFullBoundary(request.getHeader("Content-Type"));
  store.boundaryId = extractBoundaryNumber(store.fullBoundary);
  store.boundaryEnd = "--" + store.fullBoundary + "--";
}
} // namespace Multipart_FormData
