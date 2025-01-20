#ifndef HANDSOMESERVER_HPP
#define HANDSOMESERVER_HPP

#include "../httpServer/headerParsing/form-data.hpp"
#include "../httpServer/httpServer.hpp"
#include "../httpServer/responseHeader.hpp"
#include <functional>
#include <string>
#include <vector>

class HandsomeServer {
private:
  static httpServer httpserver;     // Instance of the httpServer class
  std::string staticRootFolderPath; // Path to the static files
  // Serve static files from a directory
  void serveStaticFile(const std::string &staticFolderName);

public:
  HandsomeServer();                 // Constructor
  HandsomeServer &listen(int port); // Set port and enable method chaining
  // Start the server and enable method chaining
  HandsomeServer &startServer(bool multi_thread_status = true);

  // content-type props we will use it as a type of varibles
  struct MultipartFormData {
    using clientFinelFile = ::Multipart_FormData::clientFinelFile;
    using fileProgress = ::Multipart_FormData::fileProgress;
    std::vector<Multipart_FormData::clientFinelFile> saveMultiPartFile(
        requestHeader, std::string path, int memoryAlloc,
        std::function<void(Multipart_FormData::fileProgress)> per);
  };
  // Define a route with a lambda function executor
  void route(const std::string &routeName,
             const std::function<void(const requestHeader &, responseHeader &)>
                 &lambdaExecutor);
  // Set the static route folder path
  void setStaticRouteFolder(const std::string &folderPath);
  // return path of static folder
  std::string getStaticRootFolderPath();
  std::string readFileContent(const std::string &folderPath);
};

#endif
