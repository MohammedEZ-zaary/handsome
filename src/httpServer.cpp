#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <string>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <map>
#include "headers/httpServer.hpp"
#include "headers/route.hpp"

using std::string;
using std::stringstream;
using std::cout;
using std::endl;

string httpServer::trim(const std::string &str) {
    // Function to trim whitespace from strings
    size_t first = str.find_first_not_of(' ');
    if (std::string::npos == first) return str; // If all whitespace
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
} ;

string httpServer::extractRoute(const string& requestLine) {
    // Function to extract the route from the request line
    std::istringstream stream(requestLine);
    string method, route, httpVersion;

    // Parse the request line (method, route, version)
    stream >> method >> route >> httpVersion;

    // Return the extracted route
    return route;
};
// Function to parse the HTTP request and return key-value pairs
std::map<std::string, std::string> httpServer::parseHTTPRequest(const std::string &httpRequest) {
    std::map<std::string, std::string> headers;
    std::istringstream stream(httpRequest);
    std::string line;

    // First line (request line)
    if (std::getline(stream, line)) {
        headers["Request-Line"] = line;
    }

    // Following lines (headers)
    while (std::getline(stream, line) && line != "\r" && !line.empty()) {
        size_t delimiterPos = line.find(':');
        if (delimiterPos != std::string::npos) {
            std::string key = trim(line.substr(0, delimiterPos));
            std::string value = trim(line.substr(delimiterPos + 1));
            headers[key] = value;
        }
    }

    // Read the body (if present)
    std::string body;
    if (std::getline(stream, body, '\0')) {
        body = trim(body);
        if (!body.empty()) {
            headers["Body"] = body;
        }
    }

    return headers;
}

void httpServer::run() {
    // Create server socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Failed to create socket" << endl;
        return; // Early exit on error
    }

    serverSocketClone = serverSocket;

    // Defining server
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port); // Use the member variable directly
    serverAddress.sin_addr.s_addr = INADDR_ANY; // Listen on all interfaces

    // Bind server socket
    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        std::cerr << "Failed to bind socket" << endl;
        close(serverSocket);
        return; // Early exit on error
    }

    // Listen for connections
    if (listen(serverSocket, 10) < 0) {
        std::cerr << "Failed to listen on socket" << endl;
        close(serverSocket);
        return; // Early exit on error
    }

    cout << "Server is listening on port " << port << endl;

    // Accepting client connections
    while (true) {
        int clientSocket = accept(serverSocket, nullptr, nullptr);
        if (clientSocket < 0) {
            std::cerr << "Failed to accept client connection" << endl;
            continue; // Try to accept the next connection
        }

        clientSocketClone = clientSocket;

        // Receiving data from client
        char buffer[1024] = {0};
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived < 0) {
            std::cerr << "Error receiving data from client" << endl;
            close(clientSocket);
            continue; // Try to accept the next connection
        }

        // Check client request
        for (const auto& pair : parseHttpHeaderRequest(buffer)) {
            cout << pair.first + ": " + pair.second << endl; 
            if(pair.first == "Request-Line"){
                Route routeClone =   getRoute(extractRoute(pair.second)) ;
                if(getRoute(extractRoute(pair.second)).content != "") {
                    // switch (routeClone.fileType)
                    // {
                    // case "html":
                    //     sendResponse(getClientSocketClone() , routeClone.content , "200 OK" , "text/html") ;
                    //     break;
                    // case "js":
                    //     sendResponse(getClientSocketClone() , routeClone.content , "200 OK" , "text/js") ;
                    // case "css":
                    //     sendResponse(getClientSocketClone() , routeClone.content , "200 OK" , "text/css") ;
                    // default:
                    //     sendResponse(getClientSocketClone() , routeClone.content , "200 OK" , "text/html") ;
                    //     break;
                    // }
                    if(routeClone.fileType == "html"){
                        sendResponse(getClientSocketClone() , routeClone.content , "200 OK" , "text/html") ;
                    }else if (routeClone.fileType == "js")
                    {
                        sendResponse(getClientSocketClone() , routeClone.content , "200 OK" , "text/js") ;
                    }
                    else if (routeClone.fileType == "css")
                    {
                        sendResponse(getClientSocketClone() , routeClone.content , "200 OK" , "text/css") ;
                    }
                    else 
                    {
                        /* code */

                        sendResponse(getClientSocketClone() , routeClone.content , "200 OK" , "text/html") ;
                    }


                    
                }
            }
        }

        // Close client connection
        close(clientSocket);
    }

    // Close server socket (this line will never be reached in the current loop)
    close(serverSocket);
}

std::string httpServer::readFileContent(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file) {
        std::cerr << "Unable to open the file!" << endl;
        return "";
    }
    
    stringstream buffer;
    buffer << file.rdbuf(); // Read file contents into the stringstream
    return buffer.str(); // Return the file content
}

void httpServer::sendResponse(int clientSocket, const std::string& content, const std::string& status, const std::string& contentType) {
    std::string response = "HTTP/1.1 " + status + "\r\n";
    response += "Content-Type: " + contentType + "\r\n";
    response += "Content-Length: " + std::to_string(content.size()) + "\r\n";
    response += "Connection: close\r\n\r\n";
    response += content;

    send(clientSocket, response.c_str(), response.size(), 0);
}

void httpServer::portListen(int port) {
    this->port = port; // Use 'this' pointer for clarity
}

std::map<std::string, std::string> httpServer::parseHttpHeaderRequest(const std::string& request) {
    return parseHTTPRequest(request); // Forward to the other method
}

int httpServer::getClientSocketClone() const{
    return clientSocketClone;
}

int httpServer::getServerSocketClone() const  {
    return serverSocketClone;
}


// Function to add a route
void httpServer::setRoute(const Route& route) {
    if (routeCount < 100) {  // Limit to 100 routes
        routes.push_back(route);
        routeCount++;
    }
}

// Function to get a route by name
Route httpServer::getRoute(const std::string& routeName) const {
    for(const Route& route : routes){
        if(route.routeName == routeName) {
            return route ;
        }
    }

    // If the route is not found, return a default route
    return Route("404", "<h1>404 - Route Not Found</h1>" , "html");
}
