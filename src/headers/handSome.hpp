#ifndef HANDSOMESERVER_HPP
#define HANDSOMESERVER_HPP

#include <string>
#include "httpServer.hpp" // Ensure you have the appropriate header for httpServer

class HandsomeServer {
private:
    httpServer server; // Instance of the httpServer to handle the underlying logic

public:
    // Routes a given route name to the appropriate handler
    void route(const std::string& routeName);

    // Sends a file's content in response to a request
    void sendFile(const std::string& filePath);

    void route(const std::string& routeName ,const std::string& filePath , const std::string& fileType ); 
    // Sets the port number for the server and enables method chaining
    HandsomeServer& listen(int port);

    // Starts the server
    HandsomeServer& startServer();
};

#endif // HANDSOMESERVER_HPP
