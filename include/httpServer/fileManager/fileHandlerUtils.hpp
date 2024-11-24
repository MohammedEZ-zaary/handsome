#ifndef FILE_MANAGER
#define FILE_MANAGER
#include <string>
#include <vector>

namespace FileManager {
void saveImageToFile(const std::vector<char> &body,
                     const std::string &fileName);

std::string readFileContent(const std::string &filePath);

} // namespace FileManager

#endif
