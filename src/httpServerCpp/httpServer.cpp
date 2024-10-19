#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <string>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <map>
#include "../../include/httpServer/httpServer.hpp"
#include <thread>

using std::string;
using std::stringstream;
using std::endl;

// creating inctance of of response and request HTTP  header
// #bug must  fix
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
// Good at multiThreading 
std::map<std::string, std::string> httpServer::parseHTTPRequest(const std::string &httpRequest) {
// Function to parse the HTTP request and return key-value pairs
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
    if(httpServer::MULTI_THREAD){

        std::cout << "\033[1;32m[+] Server is listening on port " << port << " Using Multi Threads" << "\033[0m\n";
    }else {

        std::cout << "\033[1;32m[+] Server is listening on port " << port << " Using Single Thread" << "\033[0m\n";
    }
    acceptConnections(); // Accept client connections
}
bool httpServer::createSocket() {
    serverSocketClone = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocketClone == -1) {
        std::cerr << "Failed to create socket" << endl;
        exit(EXIT_FAILURE);
        return false; // Indicate failure
    }
    // Set the SO_REUSEADDR option to allow reusing the port
    int opt = 1;
    if (setsockopt(serverSocketClone, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        std::cerr << "Failed to set socket options" << endl;
        close(serverSocketClone);
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
    if (listen(serverSocketClone, 50) < 0) {
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
        
        // Clone clientSocket to use it in responseHeader 
        clientSocketClone = clientSocket; 
        // MultiThreads
        if(httpServer::MULTI_THREAD){
            std::thread clinet_request_thread([this](int clientSocket){
                requestHeader req ;
                processClientRequest(clientSocket , req); 
                close(clientSocket); // Close client connection when done
            } , clientSocket);
            clinet_request_thread.join();
        }else {
            // Single Thread 
            requestHeader req ;
            processClientRequest(clientSocket , req); 
            close(clientSocket); 
        }

    }
}
// Not Good at multiThreading -> Race condition at req/std::cerr bug #06 read/write req
// For more saftiy pass const clientSocket
void httpServer::processClientRequest(int clientSocket , requestHeader& req ) {
    char buffer[1024] = {0};
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesReceived < 0) {
        // std::mutx insted of std::cerr
        std::cerr << "Error receiving data from client" << endl;
        return; 
    }
    // Safe
    auto headers = parseHttpHeaderRequest(buffer);

    for (const auto& pair : headers) {
        // Not Safe
        handleRequestHeader(req,pair.first , pair.second);
        req.setHeader(pair.first, pair.second);
    }
    // give value  to Request  Body if the method is POST request
    for (const auto& pair : headers) {
            if(pair.first == "Body") {
                handleRequestBody(req  ,pair.second );
            }
    }
    // Check client request
    for (const auto& pair : headers) {
        if (pair.first == "Request-Line") {
            // search for Route for example: /login
            Route routeClone = getRoute(req,extractRoute(pair.second));
            // excute the the content of the route
            routeClone.executor(req);
            // clean Header
            req.cleanUpfunction();
        }
    }

}
// Good at multiThreading 
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
// Good at multiThreading 
std::map<std::string, std::string> httpServer::parseHttpHeaderRequest(const std::string& request) {
    return parseHTTPRequest(request); // Forward to the other method
}
// Good at multiThreading 
int httpServer::getClientSocketClone() const{
    return clientSocketClone;
}
// Good at multiThreading 
int httpServer::getServerSocketClone() const  {
    return serverSocketClone;
}
// Good at multiThreading 
void httpServer::setRoute(const Route& route) {
    // Function to add a route
    if(route.routeName == "/404"){
        Error404Page = route ;
        routeCount++;
        return ;
    }

    routes.push_back(route);
    routeCount++;
}
Route httpServer::getRoute(requestHeader& req , const std::string& routeName)  {
    // Function to get a route by name
    // search for Name of The client Route
    for(const Route& route : routes ){
        if(route.routeName == req.uri && req.uri != "/404") {
            return route ;
        }
    }

    return Error404Page ;
    
};
void httpServer::handleRequestHeader(requestHeader& req ,const std::string& key , const std::string& value  ) { 

    // handle Request Header 
    // arr[0]  , arr[1], arr[2]
    // [Method , Path or URI , http Version]
    std::string arr[3] ;
    if(key == "Request-Line"){
        size_t firstSpace =  value.find(' ') ;
        size_t lastSpace =  value.rfind(' ') ;
        size_t questionMark = value.find('?'); // Find the '?'

        arr[0] = value.substr(0 , firstSpace);
        arr[2] = value.substr(lastSpace + 1  );
        
        if (questionMark != std::string::npos ) {
            arr[1] = value.substr(firstSpace + 1, questionMark - (firstSpace + 1)); // Get the substring between first space and ?
            // As we know the params are only accept it on GET PUT HEAD AND DELET
            if(arr[0] != "POST") {
                handleQueryParams(req,value) ;
            }else {
                std::cout << "\033[1;31m[!] Warning you cannot pass param in POST Method. \033[0m\n";
                std::cout << "\033[1;34m[+] Try to Edit Route : " << arr[1] << "\033[0m\n";

            }
        } else {
            arr[1] = value.substr(firstSpace + 1, value.find(' ', firstSpace + 1) - (firstSpace + 1)); // Handle case without '?'
        }


        req.method = arr[0];
        req.uri  = arr[1] ;
        req.version = arr[2]  ;
    }

    // Handle Content-Type : application/x-www-form-urlencoded Body Data
    if(key == "Body"){
        // arr[0] = Method POST , PUT ...
        if(!value.empty()) {
            handleQueryBody(req,value) ;
        }
    }

}
void httpServer::handleRequestBody(requestHeader& req ,const std::string& value) {
        // Those the only method that allow to accept body 
        if(req.method == "POST" || req.method == "PUT" || req.method == "PATCH") {
            req.body =  trim(value);
        }else {
            std::cout << "\033[1;31m[!] Warning your trying to send data in Body header using GET method. \033[0m\n";
            std::cout << "\033[1;34m[+] Try to use POST method in Route " << req.uri << "\033[0m\n";
        }
}
void httpServer::handleQueryParams(requestHeader& req ,const std::string& value ) {
    // handle query params if only method is not a POST request
    size_t questionMark = value.find('?'); // Find the '?'
    if (questionMark != std::string::npos) {
        // Get the substring containing the parameters
        std::string paramString = value.substr(questionMark + 1, value.find(' ', questionMark) - (questionMark + 1));
        
        // Split parameters by '&'
        std::stringstream ss(paramString);
        std::string item;
        while (std::getline(ss, item, '&')) {
            // Split key and value by '='
            size_t equalsPos = item.find('=');
            if (equalsPos != std::string::npos) {
                std::string key = item.substr(0, equalsPos);
                std::string value = item.substr(equalsPos + 1);
                req.queryParams[key] = httpServer::urlDecoded(value); // Store in the map
            }
        }
    }
}
void httpServer::handleQueryBody(requestHeader& req ,const std::string& value) {
        // if the value start with json character we will not handle it as Query Body 
        // like : username=handsome&password=Handsome2024
        // you can access json Directliy by using req.body
        if(value[0] == '{' || value[0] == '}'){
            return ;
        }
        // Handle Query Body like : username=handsome&password=Handsome2024  
        // Split parameters by '&'
        std::stringstream ss(value);
        std::string item;
        while (std::getline(ss, item, '&')) {
            // Split key and value by '='
            size_t equalsPos = item.find('=');
            if (equalsPos != std::string::npos) {
                std::string key = item.substr(0, equalsPos);
                std::string value = item.substr(equalsPos + 1);
                req.queryBody[key] = httpServer::urlDecoded(value); // Store in the map
            }
        }
}
// Good at multiThreading no shared data
std::string httpServer::urlDecoded(const std::string &encoded) {
    std::ostringstream decoded;
    for (size_t i = 0; i < encoded.length(); ++i) {
        if (encoded[i] == '+') {
            decoded << ' ';  // Convert + to space
        } else if (encoded[i] == '%' && i + 2 < encoded.length()) {
            // Convert %XX to the corresponding character
            std::string hex = encoded.substr(i + 1, 2);
            decoded << static_cast<char>(std::stoi(hex, nullptr, 16));
            i += 2;  // Skip past the two hex digits
        } else {
            decoded << encoded[i];
        }
    }
    return decoded.str();
}
