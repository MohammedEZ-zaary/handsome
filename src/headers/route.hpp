#ifndef ROUTE_HPP
#define ROUTE_HPP

#include <string>
#include <functional>
#include <map>
#include "headers.hpp"
#include "httpServer.hpp"
#include "headers.hpp"


class Route {
public:
    std::string routeName;
    std::function<void(requestHeader , responseHeader)> executor;  // Change to take httpServer& as an argument

    Route(const std::string& routeName, const std::function<void(requestHeader , responseHeader)>& executor) {
        this->routeName = routeName ; 
        this->executor = executor ; 
    }
};

#endif
