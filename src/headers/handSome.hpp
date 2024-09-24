#ifndef HANDSOMESERVER_HPP
#define HANDSOMESERVER_HPP

#include <string>
#include <functional>
#include "httpServer.hpp"
#include "route.hpp"
#include "allContentTypeHeader.hpp"

class HandsomeServer {
    private:
        std::string staticRootFolderPath = "src/templates"; // default value 
    public:
        HandsomeServer();
        void serveStaticFile(const std::string& p) ; 
        // Sends a file's content in response to a request
        void sendFile(const std::string& filePath);
        // create new route using route method
        void route(const std::string& routeName, const std::function<void()>& lambdaExcuter) ; 
        // edit static root folder
        void setStaticRouteFolder(const std::string& folderPath) ;
        // Sets the port number for the server and enables method chaining
        HandsomeServer& listen(int port);
        // Starts the server
        HandsomeServer& startServer();
};

#endif // HANDSOMESERVER_HPP
