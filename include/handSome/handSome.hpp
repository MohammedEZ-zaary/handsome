#ifndef HANDSOMESERVER_HPP
#define HANDSOMESERVER_HPP

#include <string>
#include <functional>
#include "../httpServer/httpServer.hpp"
#include "../httpServer/route.hpp"
#include "../httpServer/headers.hpp"
#include "../httpServer/allContentTypeHeader.hpp"

class HandsomeServer {
    private:
        std::string staticRootFolderPath = "src/templates"; // default value 
    public:
        HandsomeServer();

        void serveStaticFile(const std::string& p) ; 
        // Sends a file's content in response to a request
        void sendFile(const std::string& filePath);
        // create new route using route method
        void route(const std::string& routeName, const std::function<void(const requestHeader&, const responseHeader&)>& lambdaExcuter) ; 
        void sendText(const std::string& text) ;
        // edit static root folder
        void setStaticRouteFolder(const std::string& folderPath) ;
        void sendJson(const std::string& json) ;
        // Sets the port number for the server and enables method chaining
        HandsomeServer& listen(int port);
        // Starts the server
        HandsomeServer& startServer();
};

#endif // HANDSOMESERVER_HPP
