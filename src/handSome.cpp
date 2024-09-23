#include <iostream>
#include <string>
#include "headers/handSome.hpp"
#include "headers/route.hpp"
#include "headers/httpServer.hpp"
using std::string;

// Create a global or static instance of the httpServer
httpServer httpserver; // Adjust this based on your design

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

void HandsomeServer::route(const string& routeName ,const string& filePath  , const string& fileType) {
    httpserver.setRoute(Route(routeName  , httpserver.readFileContent(filePath) , fileType ));
}
