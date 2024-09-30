#include <iostream>
#include <string>
#include <functional> 
#include <filesystem>
#include <typeinfo> // Required for typeid
#include "../../include/handSome/handSome.hpp"
#include "../../include/httpServer/route.hpp"
#include "../../include/httpServer/httpServer.hpp"
#include "../../include/httpServer/allContentTypeHeader.hpp"

using std::string;
namespace fs =  std::filesystem ;
// Create a global or static instance of the httpServer
httpServer httpserver; // Adjust this based on your design
// Create instence of 
// craet content type header of file base on file end extantion
ContentTypeMapper contentType ; // create instance
HandsomeServer::HandsomeServer() {
    // Call the serveStaticFile method inside the constructor body
    serveStaticFile(staticRootFolderPath); 
};
HandsomeServer& HandsomeServer::listen(int port) {
    // Set port number
    httpserver.portListen(port);
    return *this; // Enable method chaining
};
HandsomeServer& HandsomeServer::startServer() {
    // Start the server
    httpserver.run();
    return *this; // Enable method chaining
};
void HandsomeServer::route(const std::string& routeName, const std::function<void(const requestHeader& , const responseHeader&)>& lambdaExcuter){
    // Create new HTTP route 
    httpserver.setRoute(Route(routeName , lambdaExcuter)) ;
} 
void HandsomeServer::sendFile( const std::string& filePath) {
    // send file using HTTP SERVER
    const std::string& CONTENT_TYPE = contentType.getContentTypeHeader(filePath) ; 
    httpserver.sendResponse(httpserver.getClientSocketClone() , httpserver.readFileContent(filePath), "200 OK" , CONTENT_TYPE );
}
void HandsomeServer::serveStaticFile(const std::string& staticFolderName) {
    fs::path directoryPath = staticFolderName;

    // Ensure the directory exists and is indeed a directory
    if (fs::exists(directoryPath) && fs::is_directory(directoryPath)) {
        for (const auto& entry : fs::directory_iterator(directoryPath)) {
            // Recursively call only if it is a directory
            if (fs::is_directory(entry.path())) {
                // go to nested folder
                serveStaticFile(entry.path()); 
            } else {
                std::string clonePath = entry.path(); // Get the file path as string
                size_t index = clonePath.find("/templates");
                    std::string extractRoutName = clonePath.substr(index + sizeof("templates")); // Adjust the index calculation
                    std::string extractPathOfStaticFile = fs::current_path().string() + "/" + clonePath; // get the full path of the file
                    // Use a copy of tempText to capture in the lambda
                    route(extractRoutName, [this, extractPathOfStaticFile](requestHeader , responseHeader) {
                        sendFile(extractPathOfStaticFile);
                    });
            }
        }
    } else {
        std::cerr << "Directory does not exist or is not a directory: " << directoryPath << std::endl;
    }
}

void HandsomeServer::sendText(const std::string& text){

    httpserver.sendResponse(httpserver.getClientSocketClone() , text, "200 OK" , "text/html" );
} ;
void HandsomeServer::sendJson(const std::string& json){

    httpserver.sendResponse(httpserver.getClientSocketClone() ,json , "200 OK" , "application/json" );
} ; 

void HandsomeServer::setStaticRouteFolder(const std::string& folderPath) {
    staticRootFolderPath = folderPath ;
}