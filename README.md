# HandsomeServer

## Overview

The HandsomeServer library is a simple HTTP server implementation that allows you to serve static files, handle JSON and text responses, and define custom routes using lambda functions.

## Class: HandsomeServer

### Constructor

```cpp
HandsomeServer();
```
- **Description**: Initializes the HandsomeServer instance and calls `serveStaticFile` to set up static file serving.

### Method: `listen`

```cpp
HandsomeServer& listen(int port);
```
- **Parameters**: 
  - `port` (int): The port number on which the server will listen for incoming connections.
- **Returns**: A reference to the current `HandsomeServer` instance (for method chaining).
- **Description**: Sets the server to listen on the specified port.

### Method: `startServer`

```cpp
HandsomeServer& startServer();
```
- **Returns**: A reference to the current `HandsomeServer` instance (for method chaining).
- **Description**: Starts the server to accept incoming connections.

### Method: `route`

```cpp
void route(const std::string& routeName, const std::function<void(const requestHeader&, const responseHeader&)>& lambdaExecutor);
```
- **Parameters**:
  - `routeName` (const std::string&): The route name for which the lambda function will be executed.
  - `lambdaExecutor` (std::function): A lambda function that takes `requestHeader` and `responseHeader` as parameters to handle requests for the specified route.
- **Description**: Defines a new HTTP route.

### Method: `sendFile`

```cpp
void sendFile(const std::string& filePath);
```
- **Parameters**:
  - `filePath` (const std::string&): The path to the file to be sent in response to a client request.
- **Description**: Sends the specified file to the client using the HTTP server.

### Method: `serveStaticFile`

```cpp
void serveStaticFile(const std::string& staticFolderName);
```
- **Parameters**:
  - `staticFolderName` (const std::string&): The path to the directory containing static files.
- **Description**: Sets up routes for all files in the specified directory and its subdirectories.

### Method: `sendText`

```cpp
void sendText(const std::string& text);
```
- **Parameters**:
  - `text` (const std::string&): The text content to be sent in response to a client request.
- **Description**: Sends plain text content to the client.

### Method: `sendJson`

```cpp
void sendJson(const std::string& json);
```
- **Parameters**:
  - `json` (const std::string&): The JSON content to be sent in response to a client request.
- **Description**: Sends JSON content to the client.

### Method: `setStaticRouteFolder`

```cpp
void setStaticRouteFolder(const std::string& folderPath);
```
- **Parameters**:
  - `folderPath` (const std::string&): The path to the static folder from which files will be served.
- **Description**: Sets the folder path for serving static files.

## Example Usage

Here’s a simple example of how to use the HandsomeServer:

```cpp
#include "HandsomeServer.hpp"

int main() {
    HandsomeServer server;

    // Set the folder for static files
    server.setStaticRouteFolder("path/to/static/files");

    // Define a route for the root path
    server.route("/", [](const requestHeader& req, const responseHeader& res) {
        res.sendText("<h1>Welcome to Handsome Server!</h1>");
    });

    // Start listening on port 9000
    server.listen(9000).startServer();

    return 0;
}
```

## Conclusion

The HandsomeServer library provides a flexible way to handle HTTP requests and responses, making it easy to serve static content and create custom routes. This documentation should help you get started with using the library effectively.
