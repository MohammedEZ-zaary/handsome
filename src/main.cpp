#include <iostream>
#include "headers/handSome.hpp"
#include "headers/httpServer.hpp"
#include <vector>
#include <filesystem>
namespace fs =  std::filesystem ;
int main() {
    HandsomeServer server ;
    server.setStaticRouteFolder("/handsome/src/templates");
    server.route("/" , [&server](){
        server.sendFile("src/templates/index.html");
    });
    // listen on port  9000
    server.listen(9000)
    .startServer();


    return 0 ; 
}