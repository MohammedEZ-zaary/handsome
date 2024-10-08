#include <iostream>
#include <unistd.h> // For close()
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sstream>
#include "../../include/httpServer/httpServer.hpp"
#include "../../include/httpServer/allContentTypeHeader.hpp"



// Header Class
void Header::setHeader(const std::string& key, const std::string& value) {
        headers[key] = value;
}

std::string Header::getHeader(const std::string& key) const {
    auto it = headers.find(key);
    if (it != headers.end()) {
        return it->second;
    }
    return ""; // Return empty string if the header is not found
}

void Header::printHeaders() const {
    for (const auto& pair : headers) {
        std::cout << pair.first << ": " << pair.second << std::endl;
    }

}

// request Header class
// init
requestHeader::requestHeader() {
    setDefaultValuesOfHeader();
}

void requestHeader::cleanUpfunction() {
        this->method = "";
        this->uri = "";
        this->version = "";
        this->headers.clear();
        this->queryParams.clear();
        this->queryBody.clear();
        this->body = "";
        this->ipAddress = "";
        requestHeader::setDefaultValuesOfHeader();
};

void requestHeader::setDefaultValuesOfHeader(){
        headers["Accept"] = "*/*";
        headers["Accept-Encoding"] = "gzip, deflate, br";
        headers["Body"] = "";
        headers["Content-Length"] = "";
        headers["Connection"] = "keep-alive";
        headers["Content-Type"] = "application/json";
        headers["Host"] = "localhost:9000";
        headers["User-Agent"] = "Mozilla/5.0";
}

// Response Header Class
// Constructor that accepts a reference to the server
responseHeader::responseHeader(httpServer* server) : serverRef(server) {
        headers["Content-Type"] = "text/html";
        headers["Content-Length"] = "";
        headers["Connection"] = "keep-alive";
        headers["Server"] = "HandsomeServer/1.0";
    }

void responseHeader::setStatusCode(std::string status) {
    this->statusCode = status;
}

void responseHeader::setBody(const std::string& body) {
    this->body = body;
}

void responseHeader::sendData(const std::string& data) {
    setBody(data);
    setHeader("Content-Length", std::to_string(data.size()));
    std::string resAsString = getResponseString();
    
    send(getServerRef()->getClientSocketClone(), resAsString.c_str(), resAsString.size(), 0);
}

void responseHeader::sendFile(const std::string& filePath) {
    // add Error Handler
    ContentTypeMapper contentType ;
    std::string CONTENT_TYPE_SELECTOR = contentType.getContentTypeHeader(filePath); // return text/html or application/json it deppend on file format  
    std::string data = getServerRef()->readFileContent(filePath) ;
    setHeader("Content-Type" , CONTENT_TYPE_SELECTOR ) ;
    setHeader("Connection" ,  "close") ;
    setHeader("Content-Length", std::to_string(data.size()));
    setStatusCode("200");
    // read content of file and put it on body Header
    setBody(data);
    std::string resAsString = getResponseString();
    send(getServerRef()->getClientSocketClone(), resAsString.c_str(), resAsString.size(), 0);
}

std::string  responseHeader::getResponseString() {
    std::ostringstream responseStream;

    responseStream << "HTTP/1.1 " << this->statusCode << "\r\n";

    for (const auto& pair : headers) {
        responseStream << pair.first << ": " << pair.second << "\r\n";
    }

    responseStream << "\r\n"; // End of headers

    if (!this->body.empty()) {
        responseStream << this->body;
    }
    return responseStream.str();
}

void responseHeader::setServerRef(httpServer& server) {
    serverRef = &server ;
}

httpServer* responseHeader::getServerRef() {
    return serverRef ;
}