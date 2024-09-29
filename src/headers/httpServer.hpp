#ifndef HTTPSERVER_HPP
#define HTTPSERVER_HPP

#include <string>
#include <map>
#include "route.hpp"
#include <vector>
class httpServer {
private:
    int port;                       // Port number
    int clientSocketClone;          // Client socket
    int serverSocketClone;          // Server socket
    int routeCount = 0 ;
    std::vector<Route> routes;  // Use a vector to store routes
    std::map<std::string, std::string> parseHttpHeaderRequest(const std::string& request);
    std::map<std::string, std::string> parseHTTPRequest(const std::string& httpRequest);
    std::string trim(const std::string& str);
    std::string extractRoute(const std::string& requestLine);
    Route getRoute(const std::string& routeName);
public:
    httpServer() : port(0), clientSocketClone(-1), serverSocketClone(-1) {} // Constructor for initialization
    int getClientSocketClone() const; // Returns the client socket clone
    int getServerSocketClone() const; // Returns the server socket clone
    void run();                     // Starts the server
    std::string readFileContent(const std::string& filePath); // Reads file content
    void sendResponse(int clientSocket, const std::string& content, const std::string& status = "200 OK", const std::string& contentType = "text/html");
    void portListen(int port);      // Sets the port to listen on
    void setRoute(const Route& route);
    void serveStaticFile(const std::string& staticRootFolder ,const std::function<void(const std::string& ,const std::string& )>& callback); 
};

#endif // HTTPSERVER_HPP