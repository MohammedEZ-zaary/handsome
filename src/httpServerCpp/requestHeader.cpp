#include "../../include/httpServer/requestHeader.hpp"

requestHeader::requestHeader() { setDefaultValuesOfHeader(); }

void requestHeader::cleanUpfunction() {
  this->method = "";
  this->uri = "";
  this->version = "";
  this->headers.clear();
  this->queryParams.clear();
  this->queryBody.clear();
  this->body = "";
  this->contentLength = 0;
  this->ipAddress = "";
  requestHeader::setDefaultValuesOfHeader();
};

void requestHeader::setDefaultValuesOfHeader() {
  headers["Accept"] = "*/*";
  headers["Accept-Encoding"] = "gzip, deflate, br";
  headers["Body"] = "";
  headers["Content-Length"] = "";
  headers["Connection"] = "keep-alive";
  headers["Content-Type"] = "application/json";
  headers["Host"] = "localhost:9000";
  headers["User-Agent"] = "Mozilla/5.0";
}
