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
#include "headers/headers.hpp"

using std::string;
using std::stringstream;
using std::cout;
using std::endl;

// creating inctance of of response and request HTTP  header
requestHeader req ;
responseHeader res ; 
//
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
    // must edit 10  to somthing much more heigher
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

        for (const auto& pair : parseHttpHeaderRequest(buffer)) {
            // set response Header 
            req.setHeader(pair.first , pair.second) ; 
        } 

        // Check client request
        for (const auto& pair : parseHttpHeaderRequest(buffer)) {
            if(pair.first == "Request-Line"){
                Route routeClone =   getRoute(extractRoute(pair.second)) ;
                routeClone.executor(req  , res) ;
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
    res.setHeader("Content-Type" , contentType ) ;
    res.setHeader("Content-Length" ,  std::to_string(content.size())) ;
    res.setHeader("Connection" ,  "close") ;
    std::string resAsString = res.getResponseString("200 OK" , content) ; 
    send(clientSocket,resAsString.c_str() , resAsString.size(), 0);
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
    // infinte route
    routes.push_back(route);
    routeCount++;
}
// Function to get a route by name
Route httpServer::getRoute(const std::string& routeName)  {
    for(const Route& route : routes){
        if(route.routeName == routeName) {
            return route ;
        }
    }
    return Route("404", [this](requestHeader req , responseHeader res) {
        sendResponse(getClientSocketClone(), "<h1>Not Found Error 404</h1>", "<h1> 404 Not Found</h1>", "text/html");
    }); 
    // If the route is not found, return a default route
    // return Route("404", "<h1>404 - Route Not Found</h1>" , );
}
