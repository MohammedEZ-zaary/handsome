#ifndef FORM_DATA_HEADER
#define FORM_DATA_HEADER
#include "httpServer/httpServer.hpp"
#include <string>
#include <vector>

using std::string;

namespace Multipart_FormData {

struct FileInfo;
string handleMultipartRequest(int clientSocket, const requestHeader &);
void parsingMultipartBody(const requestHeader &request,
                          const std::vector<char> &body);

bool isContentTypeFormData(const string &contentType);

int calculateFileLength(const string &buffer);
string extractBoundaryNumber(string boundary);

string extractFileName(const string &buffer);
} // namespace Multipart_FormData
#endif
