#ifndef HANDSOMESERVER_HPP
#define HANDSOMESERVER_HPP

#include <string>
#include <functional>
#include "../httpServer/httpServer.hpp"

class HandsomeServer {
private:
    httpServer httpserver; // Instance of the httpServer class
    std::string staticRootFolderPath  ; // Path to the static files
    // Serve static files from a directory
    void serveStaticFile(const std::string& staticFolderName);
    
public:
    HandsomeServer(); // Constructor
    HandsomeServer& listen(int port); // Set port and enable method chaining
    HandsomeServer& startServer(bool multi_thread_status = true); // Start the server and enable method chaining
    // Define a route with a lambda function executor
    void route(const std::string& routeName, const std::function<void(const requestHeader&,  responseHeader&)>& lambdaExecutor);
    // Set the static route folder path
    void setStaticRouteFolder(const std::string& folderPath);
    // return path of static folder
    std::string getStaticRootFolderPath()  ;
    std::string readFileContent(const std::string& folderPath) ;
};

#endif
