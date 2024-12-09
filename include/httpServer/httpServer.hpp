// global
#ifndef HTTP_SERVER_HPP
#define HTTP_SERVER_HPP
#include "requestHeader.hpp"
#include "route.hpp"
#include <functional>
#include <string>
#include <vector>

// windows
#ifdef _WIN32
#include <winsock2.h>
#endif

class httpServer {
private:
  // global = work both on win and linux
  int port;              // Port number
  int clientSocketClone; // Client socket
  int serverSocketClone; // Server socket
  int routeCount = 0;
  Route Error404Page = Route("/404", [this](requestHeader req) {});

  std::vector<Route> routes; // Use a vector to store routes
  std::string extractRoute(const std::string &requestLine);

// linux Methods
#ifdef __linux__
  bool createSocket();         // Create the server socket
  bool bindSocket();           // Bind the server socket to an address and port
  bool listenForConnections(); // Listen for incoming client connections
  void acceptConnections();    // Accept and process client connections
  void processClientRequest(int clientSocket,
                            requestHeader &req); // Process each client request
#endif
  // windows Method
#ifdef _WIN32
  bool createSocketWin(); // Create the server socket
  bool bindSocket();      // Bind the server socket to an address and port
  bool listenForConnectionsWin(); // Listen for incoming client connections
  void acceptConnectionsWin();    // Accept and process client connections
  void processClientRequest(int clientSocket,
                            requestHeader &req); // Process each client request
  //
  // windows socket propprties
  struct sockaddr_in clientAddrClone;
  int clientAddrSizeClone;
#endif

public:
  httpServer()
      : port(0), clientSocketClone(-1), serverSocketClone(-1) {
  } // Constructor for initialization
  bool MULTI_THREAD = true;
  void run();                       // Starts the server
  int getClientSocketClone() const; // Returns the client socket clone
  int getServerSocketClone() const; // Returns the server socket clone

  void sendResponse(int clientSocket, const std::string &content,
                    const std::string &status = "200 OK",
                    const std::string &contentType = "text/html");
  void portListen(int port); // Sets the port to listen on
  void setRoute(const Route &route);
  void serveStaticFile(
      const std::string &staticRootFolder,
      const std::function<void(const std::string &, const std::string &)>
          &callback);

  Route &getRoute(requestHeader &req, const std::string &routeName);
};

#endif
