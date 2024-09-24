#ifndef ROUTE_HPP
#define ROUTE_HPP
#include "httpServer.hpp"
#include <string>
#include <functional>

class Route {
public:
    std::string routeName;
    std::function<void()> excuter;  // Change to take httpServer& as an argument

    // Parameterized constructor
    Route(const std::string& routeName, const std::function<void()>& excuter) {
        this->routeName = routeName;
        this->excuter = excuter;
    }
};

#endif
