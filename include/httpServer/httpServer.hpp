#ifndef HTTP_SERVER_HPP
#define HTTP_SERVER_HPP

#include <map>
#include <vector>
#include <iostream>
#include <functional>
// Base class for headers
class Header {
protected:
    std::map<std::string, std::string> headers; // Store headers as key-value pairs

public:
    void setHeader(const std::string& key, const std::string& value) ;
    std::string getHeader(const std::string& key) const ; 
    void printHeaders() const ;
};
// Request Header
class requestHeader : public Header {
public:
    std::string method; // GET POST DELETE PUT
    std::string uri;    // path 
    std::string version; // HTTP/1.1
    std::map<std::string, std::string> queryParams; // req.queryParams["id"] = "123" 
    std::map<std::string, std::string> queryBody; // req.queryParams["id"] = "123" 
    std::string body;   // req.body = "{\"name\": \"John\"}";
    std::string ipAddress; // 192.168.11
    // init
    requestHeader()  ;
    void cleanUpfunction() ;
private:
    void setDefaultValuesOfHeader() ;
};
// Route Class
class Route {
public:
    std::string routeName;
    std::function<void(const requestHeader&)> executor;

    Route(const std::string& routeName, const std::function<void(const requestHeader&)>& executor)
        : routeName(routeName), executor(executor) {}
};

class httpServer {
private:
    int port;                       // Port number
    int clientSocketClone;          // Client socket
    int serverSocketClone;          // Server socket
    int routeCount = 0;
    Route Error404Page = Route("/404" , [this](requestHeader req){});
    std::vector<Route> routes;      // Use a vector to store routes

    // Methods
    std::map<std::string, std::string> parseHttpHeaderRequest(const std::string& request);
    std::map<std::string, std::string> parseHTTPRequest(const std::string& httpRequest);
    std::string trim(const std::string& str);
    std::string extractRoute(const std::string& requestLine);
    Route getRoute(requestHeader& req , const std::string& routeName);
    bool createSocket();             // Create the server socket
    bool bindSocket();               // Bind the server socket to an address and port
    bool listenForConnections();      // Listen for incoming client connections
    void acceptConnections();         // Accept and process client connections
    void processClientRequest(int clientSocket , requestHeader& req); // Process each client request
    void handleRequestBody(requestHeader& req ,const std::string& value); 
    void handleQueryParams(requestHeader& req ,const std::string& value) ;
    void handleQueryBody(requestHeader& req ,const std::string& value ) ; 
    std::string urlDecoded(const std::string &encoded);

public:
    httpServer() : port(0), clientSocketClone(-1), serverSocketClone(-1){} // Constructor for initialization
    bool MULTI_THREAD = true;
    void run();                     // Starts the server
    int getClientSocketClone() const; // Returns the client socket clone
    int getServerSocketClone() const; // Returns the server socket clone
    std::string readFileContent(const std::string& filePath); // Reads file content
    void sendResponse(int clientSocket, const std::string& content, const std::string& status = "200 OK", const std::string& contentType = "text/html");
    void portListen(int port);      // Sets the port to listen on
    void setRoute(const Route& route);
    void serveStaticFile(const std::string& staticRootFolder, const std::function<void(const std::string&, const std::string&)>& callback);
    void handleRequestHeader(requestHeader& req ,  const std::string&  , const std::string& ) ;
};

// Response Header Class
class responseHeader : public Header {
private:
    httpServer* serverRef; // Pointer to the httpServer
    // Private Method
    void setBody(const std::string& body); 
    httpServer* getServerRef() ;
public:
    std::string statusCode = "200 OK"; // Default status code
    std::string body; // Response body

    // Constructor that accepts a reference to the server
    responseHeader(httpServer* server)   ;
    void setStatusCode(std::string status); 
    void sendData(const std::string& data) ; 
    std::string getResponseString(); 
    void sendFile(const std::string& filePath) ;
};
#endif
