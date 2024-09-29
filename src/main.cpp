#include <iostream>
#include <string>
#include <vector>
#include "headers/handSome.hpp"
#include "headers/headers.hpp"



int main() {
    // Create server
    HandsomeServer app;

    // Route to send all user data in JSON format
    app.route("/", [&app](requestHeader req, responseHeader res) {
        app.sendFile("src/templates/index.html");
    });

    // Start the server on port 9000
    app.listen(9000)
       .startServer();

    return 0;
}
