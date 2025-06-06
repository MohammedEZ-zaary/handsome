#include <map>
#include <string>

// Base class for headers
class Header {
protected:
  std::map<std::string, std::string>
      headers; // Store headers as key-value pairs

public:
  void setHeader(const std::string &key, const std::string &value);
  std::string getHeader(const std::string &key) const;
  void printHeaders() const;
};
