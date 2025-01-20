#include "../../include/handSome/handSome.hpp"
#include "../../include/httpServer/allContentTypeHeader.hpp"
#include "../../include/httpServer/fileManager/fileHandlerUtils.hpp"
#include "../../include/httpServer/headerParsing/form-data.hpp"
#include "../../include/httpServer/httpServer.hpp"
#include "../../include/httpServer/requestHeader.hpp"
#include "../../include/httpServer/responseHeader.hpp"
#include "../../include/httpServer/route.hpp"

#include <filesystem>
#include <functional>
#include <iostream>
#include <string>

using std::string;
namespace fs = std::filesystem;
// craet content type header of file base on file end extantion
ContentTypeMapper contentType; // create instance

httpServer HandsomeServer::httpserver; // Adjust this based on your design
HandsomeServer::HandsomeServer() {
  // Call the serveStaticFile method inside the constructor body
  serveStaticFile(staticRootFolderPath);
  // Create a global or static instance of the httpServer
};
HandsomeServer &HandsomeServer::listen(int port) {
  // Set port number
  httpserver.portListen(port);
  return *this; // Enable method chaining
};
HandsomeServer &HandsomeServer::startServer(bool multi_thread_status) {
  // Start the server
  if (multi_thread_status) {
    // Which is the default value
    httpserver.MULTI_THREAD = true;
  } else {
    httpserver.MULTI_THREAD = false;
  }
  httpserver.run();
  return *this; // Enable method chaining
};
void HandsomeServer::route(
    const std::string &routeName,
    const std::function<void(const requestHeader &, responseHeader &)>
        &lambdaExcuter) {
  // Create new HTTP route
  httpserver.setRoute(
      Route(routeName, [this, lambdaExcuter](const requestHeader &req) {
        responseHeader res(&httpserver);
        lambdaExcuter(req, res);
      }));
}
void HandsomeServer::serveStaticFile(const std::string &staticFolderName) {
  fs::path directoryPath = staticFolderName;

  // Ensure the directory exists and is indeed a directory
  if (fs::exists(directoryPath) && fs::is_directory(directoryPath)) {
    for (const auto &entry : fs::directory_iterator(directoryPath)) {
      // Recursively call only if it is a directory
      if (fs::is_directory(entry.path())) {
        // Call recursively for nested directories
        serveStaticFile(entry.path().string());
      } else {
        // Get the file path as string
        std::string clonePath = entry.path().string();

        // Find the index of "/templates" and adjust substring
        size_t index = clonePath.find("/templates");
        if (index != std::string::npos) {
          std::string extractRoutName =
              clonePath.substr(index + std::string("/templates").length());

          // Full path of the static file
          std::string extractPathOfStaticFile =
              fs::absolute(entry.path()).string();
          // Lambda to serve the file
          route(extractRoutName,
                [this, extractPathOfStaticFile](const requestHeader &req,
                                                responseHeader &res) {
                  // Send the file as a response
                  res.sendFile(extractPathOfStaticFile);
                });
        }
      }
    }
  } else {
    if (getStaticRootFolderPath() != "") {
      std::cerr << "\033[1;31m[!] Please check the path of the static folders. "
                   "the folder "
                << directoryPath << " Not Found" << "\033[0m\n";
    }
  }
}
void HandsomeServer::setStaticRouteFolder(const std::string &folderPath) {
  // insert new static folder path
  staticRootFolderPath = folderPath;
  // add all files and folders to new Route
  HandsomeServer::serveStaticFile(getStaticRootFolderPath());
}
std::string HandsomeServer::getStaticRootFolderPath() {
  return staticRootFolderPath;
}
std::string HandsomeServer::readFileContent(const std::string &filePath) {
  //  read files like javascript , html , css ...
  return FileManager::readFileContent(filePath);
}

vector<HandsomeServer::MultipartFormData::clientFinelFile>
HandsomeServer::MultipartFormData::saveMultiPartFile(
    requestHeader request, std::string path, int memoryAlloc,
    std::function<void(MultipartFormData::fileProgress)> per) {
  int multiPartSockets =
      httpserver.getRoute(request, request.uri).multipartFormDataClientSocket;

  vector<HandsomeServer::MultipartFormData::clientFinelFile> results =
      Multipart_FormData::handleMultipartRequest(multiPartSockets, request,
                                                 path, memoryAlloc, per);

  return results;
};
