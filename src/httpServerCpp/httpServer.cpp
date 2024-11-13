// global
#include "../../include/httpServer/httpServer.hpp"
#include "../../include/httpServer/utils.hpp"
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <thread>
// windows
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#endif

// linux
#ifdef __linux__
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

// namespace
using std::cout;
using std::endl;
using std::string;
using std::stringstream;


// creating inctance of of response and request HTTP  header

//////////////////////////////////////////////////////// Start
///////////////////////////////////
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
  if (httpServer::MULTI_THREAD) {

    std::cout << "\033[1;32m[+] Server is listening on port " << port
              << " Using Multi Threads" << "\033[0m\n";
  } else {

    std::cout << "\033[1;32m[+] Server is listening on port " << port
              << " Using Single Thread" << "\033[0m\n";
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
  if (setsockopt(serverSocketClone, SOL_SOCKET, SO_REUSEADDR, &opt,
                 sizeof(opt)) < 0) {
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

  if (bind(serverSocketClone, (struct sockaddr *)&serverAddress,
           sizeof(serverAddress)) < 0) {
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
    if (httpServer::MULTI_THREAD) {
      std::thread client_request_thread(
          [this](int clientSocket) {
            requestHeader req;
            processClientRequest(clientSocket, req);
            close(clientSocket); // Close client connection when done
          },
          clientSocket);
      client_request_thread.join();
    } else {
      // Single Thread
      requestHeader req;
      processClientRequest(clientSocket, req);
      close(clientSocket);
    }
  }
}
void httpServer::processClientRequest(int clientSocket, requestHeader &req) {
  char buffer[1024] = {0};
  int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
  if (bytesReceived < 0) {
    // std::mutx insted of std::cerr
    std::cerr << "Error receiving data from client" << endl;
    return;
  }
  // Safe
  auto headers =  httpParsingString::parseHttpHeaderRequest(buffer)  ;

  for (const auto &pair : headers) {
    // Not Safe
    requestHandlerUtil::handleRequestHeader(req , pair.first , pair.second);
    req.setHeader(pair.first, pair.second);
  }
  // give value  to Request  Body if the method is POST request
  for (const auto &pair : headers) {
    if (pair.first == "Body") {
      requestHandlerUtil::handleRequestBody(req ,pair.second);
    }
  }
  // Check client request
  for (const auto &pair : headers) {
    if (pair.first == "Request-Line") {
      // search for Route for example: /login
      Route routeClone = getRoute(req, httpUtilsString::extractRoute(pair.second));
      // excute the the content of the route
      routeClone.executor(req);
      // clean Header
      req.cleanUpfunction();
    }
  }
}

std::string httpServer::readFileContent(const std::string &filePath) {
  std::ifstream file(filePath);
  if (!file) {
    std::cerr << "Unable to open the file!" << endl;
    return "";
  }

  stringstream buffer;
  buffer << file.rdbuf(); // Read file contents into the stringstream
  return buffer.str();    // Return the file content
}
void httpServer::portListen(int port) {
  this->port = port; // Use 'this' pointer for clarity
}

// Good at multiThreading
int httpServer::getClientSocketClone() const { return clientSocketClone; }
// Good at multiThreading
int httpServer::getServerSocketClone() const { return serverSocketClone; }
// Good at multiThreading
void httpServer::setRoute(const Route &route) {
  // Function to add a route
  if (route.routeName == "/404") {
    Error404Page = route;
    routeCount++;
    return;
  }

  routes.push_back(route);
  routeCount++;
}
Route httpServer::getRoute(requestHeader &req, const std::string &routeName) {
  // Function to get a route by name
  // search for Name of The client Route
  for (const Route &route : routes) {
    if (route.routeName == req.uri && req.uri != "/404") {
      return route;
    }
  }

  return Error404Page;
};
