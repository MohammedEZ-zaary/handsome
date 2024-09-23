#ifndef ROUTE_HPP
#define ROUTE_HPP

#include <string>

class Route {
public:
    std::string routeName;
    std::string content;
    std::string fileType ;



    // Parameterized constructor
    Route(const std::string& routeName, const std::string& content  , const std::string& fileType) {
        this-> routeName = routeName ;
        this->content = content ; 
        this->fileType =  fileType ;
    } 
        
};

#endif
