#ifndef HEADERS_HPP // Change this to match your header file name
#define HEADERS_HPP
#include <string>
#include <map>
#include <iostream>
#include <map>
#include <string>
#include <sstream>

// Base class for headers
class Header {
    protected:
        std::map<std::string, std::string> headers; // Store headers as key-value pairs

    public:
        // Set a specific header
        void setHeader(const std::string& key, const std::string& value) {
            headers[key] = value;
        }

        // Get a specific header
        std::string getHeader(const std::string& key) const {
            auto it = headers.find(key);
            if (it != headers.end()) {
                return it->second;
            }
            return ""; // Return empty string if the header is not found
        }

        // Print all headers
        void printHeaders() const {
            for (const auto& pair : headers) {
                std::cout << pair.first << ": " << pair.second << std::endl;
            }
        }
};

class requestHeader : public Header {
    private:
        std::map<std::string, std::string> headers; // Store headers as key-value pairs

    public:
    
    requestHeader(){
        // defautl  values 
        headers["Accept"] = "*/*";
        headers["Accept-Encoding"] = "gzip, deflate, br";
        headers["Body"] =  "" ;
        headers["Content-Length"] = "" ;
        headers["Connection"] = "keep-alive";
        headers["Content-Type"] = "application/json";
        headers["Host"] = "localhost:9000";
        headers["User-Agent"] = "Mozilla/5.0";
    } 

   

} ;

class responseHeader: public  Header {
    public:
        responseHeader() {
        // Set default response headers
        headers["Content-Type"] = "text/html";
        headers["Content-Length"] = "";
        headers["Connection"] = "keep-alive";
        headers["Server"] = "HandsomeServer/1.0";
    } ;

    // Method to get the complete HTTP response as a string
    std::string getResponseString(const std::string& statusCode = "200 OK", const std::string& body = "") {
        std::ostringstream responseStream;

        // Start with the status line
        responseStream << "HTTP/1.1 " << statusCode << "\r\n";

        // Append headers
        for (const auto& pair : headers) {
            responseStream << pair.first << ": " << pair.second << "\r\n";
        }

        // Add Content-Length header if a body is provided
        // if (!body.empty()) {
        //     responseStream << "Content-Length: " << headers["Content-Length"] << "\r\n";
        // }

        // End headers section
        responseStream << "\r\n"; // End of headers

        // Append body if provider
        if (!body.empty()) {
            responseStream << body;
        }

        return responseStream.str(); // Return the entire response as a string
    }

};

#endif