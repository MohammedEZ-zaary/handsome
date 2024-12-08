# HandsomeServer Documentation

**HandsomeServer** is a simple, lightweight C++ library designed to help you quickly create backend web servers. It handles HTTP requests and responses, and serves static files like HTML, CSS, and JavaScript. The library is written in modern C++ (C++17) and provides a clean and simple API for building web applications.

## Features

- Easy-to-use API to define HTTP routes.
- Serve static files like `index.html`, `style.css`, etc.
- Built-in support for handling various content types (HTML, CSS, JavaScript, etc.).
- Written in modern C++ (C++17).

## Table of Contents
- [Introduction](#handsomeServer-documentation)
- [Installation](#installtion)
- [Usage](#handsomeServer-example)
  - [Quick Start](#quick-start)
  - [Route Method](#code-explanation)
  - [Request Methods](#request-methods)
  - [Response Methods](#response-methods)
  - [Static Files](#serving-static)


## Installtion 

To get started with **HandsomeServer**, follow these steps to build and link the library into your own project.

### Step 1: Clone and Build HandsomeServer

#### 1. Clone the HandsomeServer repository:

You can clone the repository from GitHub using the following command:

```bash
git clone https://github.com/MohammedEZ-zaary/handsome.git
```

#### 2. Build the HandsomeServer library using CMake:

Navigate into the cloned repository, create a `build` directory, and configure the project with CMake:

```bash
cd handsome
mkdir build
cd build
cmake ..
```

#### 3. Build the project:

```bash
cmake --build .
```

#### 4. Copy the necessary files:

Once the library has been built, you need to copy the `include` and `lib` directories into your own project. Below is an example of what the structure will look like in your project:

```
web
├── CMakeLists.txt
├── include
│   ├── handSome
│   │   ├── handSome.hpp
│   │   └── json.hpp
│   └── httpServer
│       ├── allContentTypeHeader.hpp
│       └── httpServer.hpp
├── lib
│   └── libhandsomelib.a
├── main.cpp
```

### Step 2: Link HandsomeServer to Your Project

In your project’s `CMakeLists.txt`, you need to include the HandsomeServer library. Here's an example configuration:

```cmake
cmake_minimum_required(VERSION 3.10)
project(web_server VERSION 1.0)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add an executable
add_executable(webapp main.cpp)

# Include directories
target_include_directories(webapp PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

# Link the HandsomeServer static library
target_link_libraries(webapp PRIVATE ${PROJECT_SOURCE_DIR}/lib/libhandsomelib.a)
```

### Step 3: Build and Run Your Project

After linking HandsomeServer to your project, you can now build and run your project using CMake:

```bash
cmake -S . -B build
cmake --build build
./build/webapp
```

Once the project is built, the web server will be running, and you can test it by visiting `http://localhost:8080`.

## Quick Start

The following C++ code demonstrates how to create a basic web server that responds with an HTML message when a user accesses the root URL (`/`).

```cpp
#include <iostream>
#include "include/handSome/handSome.hpp"

int main(){
    HandsomeServer app;

    // Define a route for the root URL
    app.route("/", [&app](const requestHeader& request, responseHeader& response) {
        // Set the content type to HTML
        response.setHeader("Content-Type", "text/html");
        
        // Send a simple HTML response
        response.sendData("<h1>Hello World</h1>");
    });

    // Set up the server to listen on port 8080 and start it
    app.listen(8080).startServer();
    return 0;
}
```

### Code Explanation

1. **Create a HandsomeServer Instance**:  
   The `HandsomeServer` instance is created to handle routing and HTTP requests:
   ```cpp
   HandsomeServer app;
   ```

2. **Define a Route**:  
   The `app.route()` function is used to define a handler for the root URL `/`. It takes two arguments: the URL path and a lambda function that will handle the request:
   ```cpp
   app.route("/", [&app](const requestHeader& request, responseHeader& response) {
       response.setHeader("Content-Type", "text/html");
       response.sendData("<h1>Hello World</h1>");
   });
   ```
   - **requestHeader**: Provides access to the incoming HTTP request's data (method, URL, headers, etc.).
   - **responseHeader**: Provides methods to set response headers and send data back to the client.

3. **Start the Server**:  
   The `listen()` method binds the server to port `8080`, and `startServer()` starts accepting requests:
   ```cpp
   app.listen(8080).startServer();
   ```

### What Happens When You Run the Server?

- When a user accesses `http://localhost:8080`, the server responds with an HTML message:
  ```html
  <h1>Hello World</h1>
  ```

The `app.route()` method accepts two parameters:
1. **Route Path**: The URL path to handle (e.g., `/`, `/login`, `/status`).
2. **Lambda Function**: A lambda function that processes the incoming `request` and prepares the outgoing `response`.

### Example

Here is a basic route that handles requests to the root (`/`) and responds with HTML content:

- route method accepts two parameters: 
- route name (URL path), 
- lambda function (request, response) to process the request and send a response.

Inside the lambda function, you can handle client requests and create responses. The `request` parameter 
provides methods to access the request data, and the `response` parameter allows you to set up the response.

## Request Methods:
-----------------
1. `request.printHeader()`: 
    - Shows the full HTTP header of the client's request.
    ```c++ 
    app.route("/", [](const requestHeader& request, responseHeader& response) {

      request.printHeader(); // Prints all the HTTP headers of the request

      //response
      response.setHeader("Content-Type", "text/plain");
      response.sendData("Check console for request headers.");
    });

    ```
    -  Output. 
    ```text 

      Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8
      Accept-Encoding: gzip, deflate, br, zstd
      Accept-Language: en-US,en
      Body: 
      Connection: keep-alive
      Content-Length: 
      Content-Type: application/json
      Host: localhost:8080
      Request-Line: GET / HTTP/1.1
      Sec-Fetch-Dest: document
      Sec-Fetch-Mode: navigate
      Sec-Fetch-Site: none
      Sec-Fetch-User: ?1
      Sec-GPC: 1
      Upgrade-Insecure-Requests: 1
      User-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/129.0.0.0 Safari/537.36
      sec-ch-ua: "Brave";v="129", "Not=A?Brand";v="8", "Chromium";v="129"
      sec-ch-ua-mobile: ?0
      sec-ch-ua-platform: "Linux"
     ```
2. `request.getHeader(headerName)`: 
    - Gets the value of a specific header. Example:
      `request.getHeader("Content-Type")` could return `"text/html"`.
    ```c++ 
        app.route("/", [](const requestHeader& request, responseHeader& response) {
          std::string contentType = request.getHeader("Content-Type"); // Retrieves Content-Type from header
          std::cout << "Content-Type: " << contentType << std::endl;
          response.setHeader("Content-Type", "text/plain");
          response.sendData("Content-Type received: " + contentType);
      });
    ``` 
    - Output.
    ```text 
      Content-Type: application/json
    ```
3. `request.queryBody.at("key")`: 
    - Accesses POST request body data. Example:
      - POST body: `username=handsome&password=221122`
      - `request.queryBody.at("username")` will return `"handsome"`.
      ```c++ 
        app.route("/post-body", [](const requestHeader& request, responseHeader& response) {
          if (!request.body.empty()) {
              std::string username = request.queryBody.at("username"); // Extracts 'username' from the POST body
              std::cout << "Username: " << username << std::endl;

              response.setHeader("Content-Type", "text/plain");
              response.sendData("Received Username: " + username);
          }
        });
      ```
      - Output.
      ```text 
        Received Username: handsome
      ```

4. `request.queryParams.at("key")`: 
    - Accesses GET request query parameters. Example:
      - URL: `http://localhost/index.html?id=22&theme=black`
      - `request.queryParams.at("id")` will return `"22"`.
      ```c++ 
        app.route("/query-param", [](const requestHeader& request, responseHeader& response) {
            std::string id = request.queryParams.at("id"); // Extracts 'id' query parameter from URL
            std::cout << "ID: " << id << std::endl;

            response.setHeader("Content-Type", "text/plain");
            response.sendData("Received ID: " + id);
      });
      ```
      - Output.
      ```text 
        Received ID: 22
      ```
Request Properties:
-------------------
- `request.body`: The body of the HTTP request (e.g., JSON data or URL-encoded parameters).
- `request.method`: The HTTP method, such as `POST`, `GET`, `DELETE`.
- `request.uri`: The requested URI, such as `/login/api`.
- `request.version`: The HTTP version, e.g., `HTTP/1.1` or `HTTP/2.0`.

## Response Methods:
-----------------
1. `response.setHeader("name", "value")`: 
    - Sets a specific header. Example:
      `response.setHeader("Content-Type", "text/html")`.
    ```c++ 
      app.route("/", [](const requestHeader& request, responseHeader& response) {
        response.setHeader("Content-Type", "application/json");
        response.sendData("{\"message\":\"Header set to application/json\"}");
      });
    ```  
    - Output.
    ```json 
      {
        "message": "Header set to application/json"
      }
     ```
2. `response.sendData("string")`: 
    - Sends a string as the response body. Example:
      - `response.sendData("<html><body><h1>Hello, World!</h1></body></html>")`.
      ```c++ 
        app.route("/sendData", [](const requestHeader& request, responseHeader& response) {
          response.setHeader("Content-Type", "text/html");
          response.sendData("<html><body><h1>Hello, World!</h1></body></html>");
        });
      ```
      - Output.
      ```html 
        <html><body><h1>Hello, World!</h1></body></html>
      ```
3. `response.sendFile("path_to_your_file")`: 
    - Sends a file as the response body. The `Content-Type` is set automatically.
      ```c++ 
        app.route("/", [](const requestHeader& request, responseHeader& response) {
            response.sendFile("path/to/your/file.html/json/jpg/js/bin/txt..."); // Sends an HTML file from the templates directory
        }); 
      ```
4. `response.getHeader("name")`: 
    - Gets the value of a specific response header.
    ```c++ 
        app.route("/getResponseHeader", [](const requestHeader& request, responseHeader& response) {
          response.setHeader("Custom-Header", "HeaderValue");
          std::string headerValue = response.getHeader("Custom-Header");
          response.sendData("Custom-Header value: " + headerValue);
      });
     ```
5. `response.printHeaders()`: 
    - Prints all the response headers.
      ```c++ 
        app.route("/", [](const requestHeader& request, responseHeader& response) {
          response.printHeaders();
          // you logical code ...
          response.sendData("....");
        });
     ```
     
6. `response.setStatusCode("status_code")`: 
    - Sets the HTTP status code, such as `200`, `404`, `500`.
    ```c++ 
        app.route("/", [](const requestHeader& request, responseHeader& response) {
          // you logical code ...
          response.setStatusCode("404") ; // page not found
          // seccess response
          response.setStatusCode("200") ; // page not found
          response.sendData("....");
        });
     ```
Response Properties (Read-Only):
--------------------------------
- `response.body`: The response body set by `sendData()` or `sendFile()`.
- `response.statusCode`: The HTTP status code, such as `200`, `404`.
  
## Enhancements and Features

- **Error Handling**:  
  You can add error handling for 404 pages or 500 internal errors:
  
  ```cpp
  app.route("/404", [&app](const requestHeader& request, responseHeader& response) {
      response.setHeader("Content-Type", "text/html");
      response.sendData("<h1>Internal Server Error</h1>");
  });
  ```
# Serving Static
> ⚠️ **Important:** Your ```/templates``` folder must be on the same path of the folder ```/lib```
``` tree
web
├── include
├── lib
│   └── libhandsomelib.a
├── main.cpp
└── templates
    └── file.txt
```
> **Note:** The `/templates` folder must be in the same directory as `/lib` to ensure the **Handsome** library can access it correctly. This is crucial for serving static files, as the library depends on this structure to find the files you wish to serve.
### `setStaticRouteFolder` Method
``` 
web
├── CMakeLists.txt
├── include
│   ├── handSome
│   │   ├── handSome.hpp
│   │   └── json.hpp
│   └── httpServer
│       ├── allContentTypeHeader.hpp
│       └── httpServer.hpp
├── lib
│   └── libhandsomelib.a
├── main.cpp
├── README1.md
├── README.md
└── templates
    └── file.txt

```

The `setStaticRouteFolder()` method is used to define a directory from which your server will serve static files (such as HTML, CSS, JavaScript, images, etc.). This allows you to easily serve static content without having to manually define routes for each individual file.

### Example Code:

```cpp
#include "handSome/handSome.hpp"
#include "httpServer/httpServer.hpp"

int main() {
    // Create an instance of HandsomeServer
    HandsomeServer app;

    // Set the folder to serve static files from
    app.setStaticRouteFolder("../templates");

    app.route("/" , [](const requestHeader& request , responseHeader& response){
        // you logical code 
        response.sendData("content ") ;
    }) ;
    // Start the server, listening on port 8080
    app.listen(8080).startServer();

    return 0;
}
```
### Explanation

#### `app.setStaticRouteFolder("../templates");`

- **Purpose**: This method sets the folder where static files are located. In this example, the `../templates` directory is specified.

- **How It Works**: When a client makes a request for a file (e.g., `http://localhost:8080/index.html`), the server will look for that file in the `../templates` folder. If the file exists, the server will automatically serve it to the client.

- **Use Case**: This method is typically used to serve static content such as:
  - HTML pages (`index.html`)
  - CSS files (`style.css`)
  - JavaScript files (`script.js`)
  - Images (`logo.png`)

---

### Example of a Static Request:

- **Request**: `http://localhost:8080/index.html`
- **Server Action**: The server will look for `index.html` in the `../templates` folder and serve it to the client if found.

## License

HandsomeServer is open-source software released under the MIT License. See the [LICENSE](LICENSE) file for more details.
# Fastscan 1.1v
## How to install fastscan 
### Type this commend : python install.py 

## How to Using fastscan 

###  If you want scan famous port Type this commend : python fastscan.py -f "type ip address"
### If you want to scann all port Type this commend : python fastscan.py -a " type ip address"
### To know more information Write : python fastscan.py -h -
