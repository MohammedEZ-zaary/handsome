#ifndef ROUTE
#define ROUTE

#include "requestHeader.hpp"
#include <functional>
#include <string>

// Route Class
class Route {
public:
  std::string routeName;
  std::function<void(const requestHeader &)> executor;
  int multipartFormDataClientSocket = 0; // we store within it the
                                         // clientsocket of the client who want
                                         // to upload files

  Route(const std::string &routeName,
        const std::function<void(const requestHeader &)> &executor)
      : routeName(routeName), executor(executor) {}
};
#endif
