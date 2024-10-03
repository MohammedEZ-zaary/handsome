#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <string>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <map>
#include "../../include/httpServer/httpServer.hpp"

// test multithreading 
#include <thread>
#include <mutex>
std::mutex socketMutex;

using std::string;
using std::stringstream;
using std::cout;
using std::endl;

// // creating inctance of of response and request HTTP  header
// // #bug must  fix
requestHeader req ;
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
    if (!createSocket()) {
        return; // Early exit on error
    }

    if (!bindSocket()) {
        return; // Early exit on error
    }

    if (!listenForConnections()) {
        return; // Early exit on error
    }

    cout << "Server is listening on port " << port << endl;

    acceptConnections(); // Accept client connections
}

bool httpServer::createSocket() {
    serverSocketClone = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocketClone == -1) {
        std::cerr << "Failed to create socket" << endl;
        return false; // Indicate failure
    }
    return true; // Indicate success
}

bool httpServer::bindSocket() {
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port); // Use the member variable directly
    serverAddress.sin_addr.s_addr = INADDR_ANY; // Listen on all interfaces

    if (bind(serverSocketClone, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        std::cerr << "Failed to bind socket" << endl;
        close(serverSocketClone);
        return false; // Indicate failure
    }
    return true; // Indicate success
}

bool httpServer::listenForConnections() {
    // Change 100 to a more suitable backlog size if necessary
    if (listen(serverSocketClone, 100) < 0) {
        std::cerr << "Failed to listen on socket" << endl;
        close(serverSocketClone);
        return false; // Indicate failure
    }
    return true; // Indicate success
}

void httpServer::acceptConnections() {
    while (true) {
        int clientSocket = accept(serverSocketClone, nullptr, nullptr);
        if (clientSocket < 0) {
            std::cerr << "Failed to accept client connection" << endl;
            continue; // Try to accept the next connection
        }
        

        clientSocketClone = clientSocket; 
        
        processClientRequest(clientSocket); // Create a new thread to handle the client connection
        close(clientSocket); // Close client connection when done
        // multithreading Bug #3
        // std::thread handleClient([this , clientSocket](){
            // processClientRequest(clientSocket); // Create a new thread to handle the client connection
        // }) ;
        // handleClient.detach();
    }
}

void httpServer::processClientRequest(int clientSocket) {
    // std::lock_guard<std::mutex> lock(socketMutex);
    char buffer[1024] = {0};
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesReceived < 0) {
        std::cerr << "Error receiving data from client" << endl;
        return; 
    }

    auto headers = parseHttpHeaderRequest(buffer);

    // std::lock_guard<std::mutex> lock(routesMutex); // Lock the mutex
    // std::lock_guard<std::mutex> routesLock(routesMutex) ;
    for (const auto& pair : headers) {

        handleHeader(pair.first, pair.second);
        req.setHeader(pair.first, pair.second);
    }
    // Check client request
    for (const auto& pair : headers) {
        if (pair.first == "Request-Line") {
            // search for Route for example: /login
            Route routeClone = getRoute(extractRoute(pair.second));
            // excute the the content of the route
            routeClone.executor(req);
            // clean Header
            req.cleanUpfunction();
        }
    }

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


void httpServer::portListen(int port) {
    this->port = port; // Use 'this' pointer for clarity
}
std::map<std::string, std::string> httpServer::parseHttpHeaderRequest(const std::string& request) {
    return parseHTTPRequest(request); // Forward to the other method
}
int httpServer::getClientSocketClone() const{
    // std::lock_guard<std::mutex> lock(socketMutex);            
    return clientSocketClone;
}
int httpServer::getServerSocketClone() const  {
    // std::lock_guard<std::mutex> lock(socketMutex);            
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
    return Route("404", [this](const requestHeader req) {
        // fix bug #2 
    }); 
    
}
void httpServer::handleHeader(const std::string& key ,const std::string& value) {
    // add Other 
    if(key == "Body"){
        req.body = trim(value) ; 
    }
}
void httpServer::closeSocket() {
    if (serverSocketClone != -1) {
        close(serverSocketClone);  // Close the server socket
        serverSocketClone = -1;    // Reset the socket descriptor
        std::cout << "Socket closed successfully." << std::endl;
    }
}