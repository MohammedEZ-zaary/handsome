#include <iostream>
#include "headers/handSome.hpp"

int main() {
    // create web http server
    HandsomeServer server ;
    server.route("/", "src/templates/index.html" , "html");
    server.route("/static/js/main.82c7c605.js", "src/templates/static/js/main.82c7c605.js" , "js");
    server.route("/static/css/main.d2623b5b.css", "src/templates/static/css/main.d2623b5b.css" , "css");
    // run server
    server.listen(9000)
    .startServer();

    return 0 ; 
}