#ifndef ROUTE_HPP
#define ROUTE_HPP

#include <string>
#include <functional>
#include <map>
#include "headers.hpp"
#include "httpServer.hpp"

class Route {
public:
    std::string routeName;
    std::function<void(const requestHeader& , const responseHeader&)> executor;  // Change to take httpServer& as an argument

    Route(const std::string& routeName, const std::function<void(const requestHeader& , const responseHeader&)>& executor) {
        this->routeName = routeName ; 
        this->executor = executor ; 
    }
};

#endif
